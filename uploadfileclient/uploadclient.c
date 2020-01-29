#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma warning(disable : 4996)
#define MAX_SIZE 10240
//#define PORT 8888

//#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char* argv[])
{
	char filename[81];
	char ip[81];
	char* cmds = "s";
	char* cmdg = "g";
	if (argc != 4)
	{
		printf("USAGE: serverip port filename \n");
		exit(1);
	}
	//输入的参数argv[1]，IP地址
	int templenargv1 = strlen(argv[1])+1;
	strcpy_s(ip, templenargv1,argv[1]);
	//输入的参数argv[3]，要下载或者上传的文件名
	int templenargv2 = strlen(argv[3])+1;
	strcpy_s(filename,templenargv1, argv[3]);

	WSADATA wsaData;
	SOCKET fileclientsocket;
	FILE *fp;
	FILE* fpreceive;
	struct sockaddr_in serveraddr;
	char data[MAX_SIZE];
	int i;
	int ret;

	//if ((fp = fopen(filename, "rb")) == NULL)
	//{
	//	printf("can't open file %s\n", filename);
	//}



	WSAStartup(0x202, &wsaData);

	fileclientsocket = socket(AF_INET, SOCK_STREAM, 0);
	memset((void*)&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(ip);//inet_pton
	//serveraddr.sin_addr.s_addr = inet_pton(ip);
	serveraddr.sin_port = htons(atoi(argv[2]));

	if (connect(fileclientsocket, (struct sockaddr*)&serveraddr,sizeof(struct sockaddr_in)) ==SOCKET_ERROR)
	{
		printf("connect fileserver error\n");
		//fclose(fp);
		closesocket(fileclientsocket);
		WSACleanup();
		exit(1);
	};

	//发送给服务器选择，s是发送，g是下载
	printf("Please input choice ,s =send %s to server...g=download %s from server\n", filename,filename);
	char choicse = getchar();
	switch (choicse)
	{
	case 's':  //发送给服务器
		//发送s字符给server	

		printf("send the cmd 's' to server...\n");
		if (send(fileclientsocket, cmds, (int)(strlen(cmds)), 0) == SOCKET_ERROR)
		{
			wprintf(L"send failed with error: %d\n", WSAGetLastError());
			closesocket(fileclientsocket);
			WSACleanup();
			return 1;

		}
		//send(fileclientsocket, "\0", 1, 0);  //filename 字符串发送的结束

		//发送给服务器，则打开文件类型，rb,以二进制read模式打开，文件必须存在
		printf("----SEND FILE TO SERVER----\n");
		errno_t err;
		if ((err = fopen_s(&fp, filename, "rb")) != 0)
			printf("The file filename %s was not opened\n", filename);
		else
			printf("The file filename %s was opened\n", filename);
		//发送文件名给服务器端
		printf("send the name of sendfile to server...\n");
		send(fileclientsocket, filename, strlen(filename), 0);
		send(fileclientsocket, "\0", 1, 0);  //filename 字符串发送的结束
		printf("send file stream...\n");

		while (1)
		{
			memset((void*)data, 0, sizeof(data));
			i = fread(data, 1, sizeof(data), fp);
			//第一次执行，i=读到的数据
			//第二次执行时候，i=0所以break
			if (i == 0)
			{
				printf("\n send success\n");
				break;
			}
			//ret = send(fileclientsocket, data, 1, 0);//1,只出现BUG只发送一个
			//ret返回发送的数据数目
			ret = send(fileclientsocket, data, i, 0);
			putchar('.');
			if (ret == SOCKET_ERROR)
			{
				printf("\n send failed,file maybe not complete\n");
				break;
			}
		}
		fclose(fp);
		break;

	case 'g':
		printf("send the cmd 'g' to server...\n");
		if (send(fileclientsocket, cmdg, (int)(strlen(cmdg)), 0) == SOCKET_ERROR)
		{
			wprintf(L"send failed with error: %d\n", WSAGetLastError());
			closesocket(fileclientsocket);
			WSACleanup();
			return 1;
		}
		printf("----DOWNLOAD FILE FROM SERVER----\n");
		//发送文件名给服务器端
		printf("send the name of download file to server...\n");
		send(fileclientsocket, filename, strlen(filename), 0);
		//一定要发送个HEX的0服务器才能判断接收完成。？？？？
		send(fileclientsocket, "\0", 1, 0);  //filename 字符串发送的结束，"\0"是字符串，发送一个

		printf(" Receive Filename is =%s\n", filename);
		fpreceive = fopen(filename, "wb");
		if (fpreceive == NULL)
		{
			printf("Can't open the file with WRITEMODE\n");
			closesocket(fileclientsocket);
			return;
		}

		printf("-----The content from server------\n");
		memset((void*)data, 0, sizeof(data));
		//while (1)
		//{
		//	i = recv(fileclientsocket, data, sizeof(data), 0);
		//	//putchar('.');
		//	if (i == SOCKET_ERROR)
		//	{
		//		printf("Receive failed,file may be incomplete\n");
		//		break;
		//	}
		//	else if (i == 0)
		//	{
		//		printf("--RECEIVE SUCCESS---SHOWN BELOW-\n");
		//		break;
		//	}
		//	else
		//	{
		//		fwrite((void*)data, 1, i, fpreceive);
		//	}
		//}

		// Receive until the peer closes the connection，
		//so after server transfer ,server nee dto close the socket
		do {
			i = recv(fileclientsocket, data, sizeof(data), 0);
			if (i > 0)
			{
				wprintf(L"Bytes received: %d\n", i);
				if (fwrite((void*)data, 1, i, fpreceive) == i)
				{
					wprintf(L"Write to file successfully\n");
				}
				//fwrite((void*)data, 1, i, fpreceive);
			}
			else if (i == 0)
				wprintf(L"Connection closed\n");
			else
				wprintf(L"recv failed with error: %d\n", WSAGetLastError());

		} while (i > 0);

		printf("%s\n", data);
		printf("-----RECEIVE FINISH--------------\n");
		fclose(fpreceive);//
		break;
	default:
		break;
	}

	closesocket(fileclientsocket);
	WSACleanup();
}