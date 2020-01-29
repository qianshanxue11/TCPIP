#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#pragma warning(disable : 4996)
#define MAX_SIZE 10240
#define PORT 8888
void clientprocessthread(void* s);//client上传的线程
void reportconnectinfo(SOCKET s);
char receivecmdfromclient(void* s);

int main(int argc, char* argv[])
{
	WSADATA wsadata;
	SOCKET serversocket, clientsocket;
	static SOCKET* presentclientsocket;
	struct sockaddr_in serveraddr;
	struct sockaddr_in clientaddr;
	int nclientaddrlen;

	WSAStartup(0x202, &wsadata);

	serversocket = socket(AF_INET, SOCK_STREAM, 0);

	memset((void*)&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr("0.0.0.0");
	serveraddr.sin_port = htons(PORT);

	bind(serversocket, (struct sockaddr*) & serveraddr, sizeof(serveraddr));
	listen(serversocket, SOMAXCONN);
	printf("Server is run now ,listen on PORT %d!\n", PORT);

	while (1)
	{
		nclientaddrlen = sizeof(clientaddr);

		memset((void*)&clientaddr, 0, nclientaddrlen);
		clientsocket = accept(serversocket, (struct sockaddr*) & clientaddr, &nclientaddrlen);
		presentclientsocket = &clientsocket;

		//printf("Client(%s:%d)has connected!\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		printf("Waiting for cmd from client\n");
		//char cmdreceive = receivecmdfromclient(presentclientsocket);
		char cmdreceive;
		//if (recv(presentclientsocket, &cmdreceive, 1, 0) != 1)   //读取一个字符，判断是否为1，如果不等于1，则相当于没连接或读取失败
		//{
		//	printf("receive failed or client close connection\n");
		//	closesocket(presentclientsocket);
		//	return;
		//}

		//启动一个线程处理一个客户端
		_beginthread(clientprocessthread, 0, (void*)presentclientsocket);
		Sleep(1000);
		//输出双方的IP地址和端口信息
		reportconnectinfo(clientsocket);

	}

	closesocket(serversocket);
	WSACleanup();
	return 0;
}

//处理客户端文件处理(上传下载)的线程

void clientprocessthread(void* s)
{
	printf("\n\n创建客户端上传线程成功!\n\n\n");
	char filename[MAX_PATH];  //文件名最长MAX_PATH
	char data[MAX_SIZE];
	int i;
	char ch;
	char cmdreceive;
	FILE* fp;
	FILE* fsendp;
	//printf("Receive filename\n");
	//接收指令
	//recv(*(SOCKET*)s, &cmdreceive, 1, 0);
	if (recv(*(SOCKET*)s, &cmdreceive, 1, 0) != 1)   //读取一个字符，判断读取的数量是否为1，如果不等于1，则相当于没连接或读取失败
	{
		printf("receive cmd falied\n");
		closesocket(*(SOCKET*)s);
		return;
	}
	//读取到的s,g分别处理
	switch (cmdreceive)
	{
	case 's'://send from client to server
		printf("s Receive cmd is cmdreceive =%c\n", cmdreceive);
		memset((void*)filename, 0, sizeof(filename));
		for (i = 0; i < sizeof(filename); i++)
		{
			if (recv(*(SOCKET*)s, &ch, 1, 0) != 1)   //读取一个字符，判断读取的数量是否为1，如果不等于1，则相当于没连接或读取失败
			{
				printf("filename receive failed or client close connection\n");
				closesocket(*(SOCKET*)s);
				return;
			}

			//if (ch == 0)  //由于文件名不定长，为了确定文件名发送完成，client最后发送'\0'作为结束，接收到之后跳出
			if (ch == '\0')
			{
				break;
			}

			filename[i] = ch;
		}

		if (i == sizeof(filename))
		{
			printf("Filename too long\n");
			closesocket(*(SOCKET*)s);
			return;
		}
		printf(" Receive Filename is =%s\n", filename);
		fp = fopen(filename, "wb");
		if (fp == NULL)
		{
			printf("Can't open the file with WRITEMODE\n");
			closesocket(*(SOCKET*)s);
			return;
		}
		printf("-----The content from client------\n");
		memset((void*)data, 0, sizeof(data));
		while (1)
		{
			i = recv(*(SOCKET*)s, data, sizeof(data), 0);
			//putchar('.');
			if (i == SOCKET_ERROR)
			{
				printf("Receive failed,file may be incomplete\n");
				break;
			}
			else if (i == 0)
			{
				printf("--RECEIVE SUCCESS---SHOWN BELOW-\n");
				break;
			}
			else
			{
				fwrite((void*)data, 1, i, fp);
			}
		}
		printf("%s\n", data);
		printf("-----RECEIVE FINISH--------------\n");
		fclose(fp);
		break;

	case 'g'://client download from server
		printf("g Receive cmd is cmdreceive =%c\n", cmdreceive);
		//接收client发来的文件名
		memset((void*)filename, 0, sizeof(filename));
		for (i = 0; i < sizeof(filename); i++)
		{
			if (recv(*(SOCKET*)s, &ch, 1, 0) != 1)   //读取一个字符，判断读取的数量是否为1，如果不等于1，则相当于没连接或读取失败
			{
				printf("filename receive failed or client close connection\n");
				closesocket(*(SOCKET*)s);
				return;
			}
			//if (ch == 0)//"\0"第一个是0
			if (ch == '\0')
			{
				break;
			}
			filename[i] = ch;
		}

		if (i == sizeof(filename))
		{
			printf("Filename too long\n");
			closesocket(*(SOCKET*)s);
			return;
		}
		printf(" Receive Filename is =%s\n", filename);
		printf(" Prepare to open the the Filename is =%s\n", filename);
		fsendp = fopen(filename, "rb");
		if (fsendp == NULL)
		{
			printf("Can't open the file with READMODE\n");
			closesocket(*(SOCKET*)s);
			return;
		}

		while (1)
		{
			memset((void*)data, 0, sizeof(data));
			i = fread(data, 1, sizeof(data), fsendp);
			if (i == 0)
			{
				printf("\n send successly\n");
				break;
			}
			//ret = send(fileclientsocket, data, 1, 0);//1,只出现BUG只发送一个
			putchar('.');
			if (send(*(SOCKET*)s, data, i, 0) == SOCKET_ERROR)
			{
				printf("\n send failed,file maybe not complete\n");
				break;
			}
		}
		fclose(fsendp);
		closesocket(*(SOCKET*)s);  //添加测试是否有效
		reportconnectinfo(*(SOCKET*)s);
		break;
	default:
		printf("default Receive cmd is cmdreceive =%c\n", cmdreceive);
		break;
	}

	_endthread();
	//closesocket(*(SOCKET*)s);
}


//输出连接双方的IP地址和端口信息
void reportconnectinfo(SOCKET s)
{
	struct sockaddr_in addr;
	int naddrlen;

	naddrlen = sizeof(addr);
	memset(&addr, 0, naddrlen);
	getsockname(s, (struct sockaddr*) & addr, &naddrlen);
	printf("--Server address (%s:%d)!--\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
	naddrlen = sizeof(addr);
	memset(&addr, 0, naddrlen);
	getpeername(s, (struct sockaddr*) & addr, &naddrlen);
	printf("--One Client (%s:%d)has connected to this server!--\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
}

char receivecmdfromclient(void* s)
{
	char cmdreceive='a';
		if (recv(*(SOCKET*)s, &cmdreceive, 1, 0) != 1)   //读取一个字符，判断是否为1，如果不等于1，则相当于没连接或读取失败
		{
			printf("receive failed or client close connection\n");
			closesocket(*(SOCKET*)s);
			return;
		}
	return cmdreceive;
}


