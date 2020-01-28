#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#pragma warning(disable : 4996)
#define MAX_SIZE 10240
#define PORT 8888
void serveoneclient(void* s);
void reportconnectinfo(SOCKET s);

int main(int argc, char* argv[])
{
	WSADATA wsadata;
	SOCKET listensocket, acceptsocket;
	static SOCKET* psocket;
	struct sockaddr_in serveraddr;
	struct sockaddr_in clientaddr;
	int nclientaddrlen;

	WSAStartup(0x202, &wsadata);

	listensocket = socket(AF_INET, SOCK_STREAM, 0);

	memset((void*)&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr("0.0.0.0");
	serveraddr.sin_port = htons(PORT);

	bind(listensocket, (struct sockaddr*) & serveraddr, sizeof(serveraddr));
	listen(listensocket, SOMAXCONN);
	printf("server is run now ,listen on PORT %d!\n", PORT);

	while (1)
	{
		nclientaddrlen = sizeof(clientaddr);

		memset((void*)&clientaddr, 0, nclientaddrlen);
		acceptsocket = accept(listensocket, (struct sockaddr*) & clientaddr, &nclientaddrlen);
		psocket = &acceptsocket;

		printf("client(%s:%d)has connect!\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		//启动一个线程处理一个客户端上传
		_beginthread(serveoneclient,0,(void *)psocket);
		Sleep(1000);

		//输出双方的IP地址和端口信息
		reportconnectinfo(acceptsocket);
	}

	closesocket(listensocket);
	WSACleanup();
	return 0;
}

//处理客户端文件上传的线程

void serveoneclient(void* s)
{
	printf("\n\n创建线程成功!\n\n\n");
	char filename[MAX_PATH];
	char data[MAX_SIZE];
	int i;
	char ch;
	FILE* fp;
	printf("receive filename\n");

	memset((void*)filename, 0, sizeof(filename));
	for (i = 0; i < sizeof(filename); i++)
	{
		if (recv(*(SOCKET*)s, &ch, 1, 0) != 1)
		{
			printf("receive failed or client close connection\n");
			closesocket(*(SOCKET*)s);
			return;
		}

		if (ch == 0)
		{
			break;
		}

		filename[i] = ch;
	}

	if (i == sizeof(filename))
	{
		printf("filename too long\n");
		closesocket(*(SOCKET*)s);
		return;
	}
	printf("filename =%s\n", filename);
	fp = fopen(filename, "wb");
	if (fp == NULL)
	{
		printf("can't open the file with WRITEMODE\n");
		closesocket(*(SOCKET*)s);
		return;
	}
	printf("the content of receive");
	memset((void*)data, 0, sizeof(data));
	while (1)
	{
		i = recv(*(SOCKET*)s, data, sizeof(data),0);
		putchar('.');
		if (i == SOCKET_ERROR)
		{
			printf("\n receive failed,file may be incomplete\n");
			break;
		}
		else if (i == 0)
		{
			printf("\n receive successs\n");
			break;
		}
		else
		{
			fwrite((void*)data, 1, i, fp);
		}
	}
	printf("%s", data);
	fclose(fp);
	_endthread();
	closesocket(*(SOCKET*)s);
}


//输出连接双方的IP地址和端口信息
void reportconnectinfo(SOCKET s)
{
	struct sockaddr_in addr;
	int naddrlen;

	naddrlen = sizeof(addr);
	memset(&addr, 0, naddrlen);
	getsockname(s, (struct sockaddr*) & addr, &naddrlen);
	printf("server (%s:%d)has connect!\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
	naddrlen = sizeof(addr);
	memset(&addr, 0, naddrlen);
	getpeername(s, (struct sockaddr*) & addr, &naddrlen);
	printf("client (%s:%d)has connect!\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
}


