#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "globalfunction.h"
#pragma warning(disable : 4996)
#pragma comment(lib, "Ws2_32.lib")
errno_t err;

int main(int argc, char* argv[])
{
	char filename[81] = {0};
	int filenamelength=0;
	char ip[81];
	char* cmds = "s";
	char* cmdg = "g";
	printf("USAGE: exefilename serverip port\n");
	printf("After run,input the filename you want to upload or download\n");
	/*判断输入的参数，如果不对，输出提示信息*/
	// Validate the parameters
	if (argc != 3)
	{
		printf("USAGE: %s serverip port \n", argv[0]);
		return 1;
	}
	//输入的参数argv[1]，IP地址
	int templenargv1 = strlen(argv[1])+1;
	strcpy_s(ip, templenargv1,argv[1]);
	//输入的参数argv[3]，要下载或者上传的文件名
	//int templenargv2 = strlen(argv[3])+1;
	//strcpy_s(filename,templenargv2, argv[3]);
	WSADATA wsaData;
	SOCKET fileclientsocket;
	FILE *fp;
	struct sockaddr_in serveraddr;
	char data[MAX_SIZE];
	int i;
	int iResult;
	int ret;
	char choicse='0';

	//// Initialize Winsock
	//iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	//if (iResult != 0) {
	//	printf("WSAStartup failed with error: %d\n", iResult);
	//	return 1;
	//}

	//fileclientsocket = socket(AF_INET, SOCK_STREAM, 0);
	//memset((void*)&serveraddr, 0, sizeof(serveraddr));
	//serveraddr.sin_family = AF_INET;
	//serveraddr.sin_addr.s_addr = inet_addr(ip);//inet_pton
	////serveraddr.sin_addr.s_addr = inet_pton(ip);
	//serveraddr.sin_port = htons(atoi(argv[2]));

	//if (connect(fileclientsocket, (struct sockaddr*)&serveraddr,sizeof(struct sockaddr_in)) ==SOCKET_ERROR)
	//{
	//	printf("connect fileserver error\n");
	//	closesocket(fileclientsocket);
	//	WSACleanup();
	//	exit(1);
	//};
	//先输入文件名放在这里没问题，已经解决
	//filenamelength = getFileName(filename);
	//发送给服务器选择，s是发送，g是下载

	printf("\nPlease input choice\n");
	printf("s =send %s to server\n", filename);
	printf("g=download %s from server\n",filename);
	printf("q=EXIT\n");
	while (choicse != 'q')
	{
		choicse = getchar();
		switch (choicse)
		{
		case 's':  //发送给服务器
			//1:首先获取命令字
			filenamelength = getFileName(filename);
			//2:连接server服务器
				// Initialize Winsock
			iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
			if (iResult != 0) {
				printf("WSAStartup failed with error: %d\n", iResult);
				return 1;
			}

			fileclientsocket = socket(AF_INET, SOCK_STREAM, 0);
			memset((void*)&serveraddr, 0, sizeof(serveraddr));
			serveraddr.sin_family = AF_INET;
			serveraddr.sin_addr.s_addr = inet_addr(ip);//inet_pton
			//serveraddr.sin_addr.s_addr = inet_pton(ip);
			serveraddr.sin_port = htons(atoi(argv[2]));

			if (connect(fileclientsocket, (struct sockaddr*) & serveraddr, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
			{
				//printf("connect fileserver error\n");
				printf("socket failed with error: %ld\n", WSAGetLastError());
				closesocket(fileclientsocket);
				WSACleanup();
				exit(1);
			};

			//3:发送命令字符给server	
			sendCmdToserver(fileclientsocket, cmds);
			/*open the file*/
			//4:打开文件，获取文件描述符
			if ((err = fopen_s(&fp, filename, "rb")) != 0)
			{
				printf("The file filename %s was not opened\n", filename);
				printf("fopen: %s\n", strerror(errno));
				return -1;
			}
			else
				printf("The file filename %s was opened\n", filename);
			//5:发送文件名称，发送文件
			sendFileName(fileclientsocket, filename);
			sendFile(fileclientsocket, fp);
			//6:关闭socket,等待其他命令
			closesocket(fileclientsocket);
			WSACleanup();
			break;

		case 'g':
			//1:首先获取命令字
			//输入g之后，不会等待输入文件名，需要查找原因
			//先输入文件名
			filenamelength = getFileName(filename);
			//2:连接server服务器
				// Initialize Winsock
			iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
			if (iResult != 0) {
				printf("WSAStartup failed with error: %d\n", iResult);
				return 1;
			}

			fileclientsocket = socket(AF_INET, SOCK_STREAM, 0);
			memset((void*)&serveraddr, 0, sizeof(serveraddr));
			serveraddr.sin_family = AF_INET;
			serveraddr.sin_addr.s_addr = inet_addr(ip);//inet_pton
			//serveraddr.sin_addr.s_addr = inet_pton(ip);
			serveraddr.sin_port = htons(atoi(argv[2]));

			if (connect(fileclientsocket, (struct sockaddr*) & serveraddr, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
			{
				//printf("connect fileserver error\n");
				printf("socket failed with error: %ld\n", WSAGetLastError());
				closesocket(fileclientsocket);
				WSACleanup();
				exit(1);
			};
			//3:发送命令字符给server	
			sendCmdToserver(fileclientsocket, cmdg);
			printf("----DOWNLOAD FILE FROM SERVER----\n");
			//4:打开文件，获取文件描述符
			/*open the file*/
			//errno_t err;
			if ((err = fopen_s(&fp, filename, "wb")) != 0)
			{
				printf("The file filename %s was not opened with write mode\n", filename);
				printf("fopen: %s\n", strerror(errno));
				return -1;
			}
			else
				printf("The file filename %s was opened\n", filename);
			//5:发送文件名称，获取文件下载
			sendFileName(fileclientsocket, filename);
			receiveFile(fileclientsocket, fp);
			//6:关闭socket,等待其他命令
			closesocket(fileclientsocket);
			WSACleanup();
			break;
		case 'q':
			closesocket(fileclientsocket);
			WSACleanup();
			printf("EXIT ........\n");
			exit(1);
			break;
		}
	}
}

