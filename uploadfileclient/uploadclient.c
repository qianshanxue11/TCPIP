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
	/*�ж�����Ĳ�����������ԣ������ʾ��Ϣ*/
	// Validate the parameters
	if (argc != 3)
	{
		printf("USAGE: %s serverip port \n", argv[0]);
		return 1;
	}
	//����Ĳ���argv[1]��IP��ַ
	int templenargv1 = strlen(argv[1])+1;
	strcpy_s(ip, templenargv1,argv[1]);
	//����Ĳ���argv[3]��Ҫ���ػ����ϴ����ļ���
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
	//�������ļ�����������û���⣬�Ѿ����
	//filenamelength = getFileName(filename);
	//���͸�������ѡ��s�Ƿ��ͣ�g������

	printf("\nPlease input choice\n");
	printf("s =send %s to server\n", filename);
	printf("g=download %s from server\n",filename);
	printf("q=EXIT\n");
	while (choicse != 'q')
	{
		choicse = getchar();
		switch (choicse)
		{
		case 's':  //���͸�������
			//1:���Ȼ�ȡ������
			filenamelength = getFileName(filename);
			//2:����server������
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

			//3:���������ַ���server	
			sendCmdToserver(fileclientsocket, cmds);
			/*open the file*/
			//4:���ļ�����ȡ�ļ�������
			if ((err = fopen_s(&fp, filename, "rb")) != 0)
			{
				printf("The file filename %s was not opened\n", filename);
				printf("fopen: %s\n", strerror(errno));
				return -1;
			}
			else
				printf("The file filename %s was opened\n", filename);
			//5:�����ļ����ƣ������ļ�
			sendFileName(fileclientsocket, filename);
			sendFile(fileclientsocket, fp);
			//6:�ر�socket,�ȴ���������
			closesocket(fileclientsocket);
			WSACleanup();
			break;

		case 'g':
			//1:���Ȼ�ȡ������
			//����g֮�󣬲���ȴ������ļ�������Ҫ����ԭ��
			//�������ļ���
			filenamelength = getFileName(filename);
			//2:����server������
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
			//3:���������ַ���server	
			sendCmdToserver(fileclientsocket, cmdg);
			printf("----DOWNLOAD FILE FROM SERVER----\n");
			//4:���ļ�����ȡ�ļ�������
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
			//5:�����ļ����ƣ���ȡ�ļ�����
			sendFileName(fileclientsocket, filename);
			receiveFile(fileclientsocket, fp);
			//6:�ر�socket,�ȴ���������
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

