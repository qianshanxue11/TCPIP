#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#pragma warning(disable : 4996)
#define MAX_SIZE 10240
#define PORT 8888
void clientprocessthread(void* s);//client�ϴ����߳�
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
		//if (recv(presentclientsocket, &cmdreceive, 1, 0) != 1)   //��ȡһ���ַ����ж��Ƿ�Ϊ1�����������1�����൱��û���ӻ��ȡʧ��
		//{
		//	printf("receive failed or client close connection\n");
		//	closesocket(presentclientsocket);
		//	return;
		//}

		//����һ���̴߳���һ���ͻ���
		_beginthread(clientprocessthread, 0, (void*)presentclientsocket);
		Sleep(1000);
		//���˫����IP��ַ�Ͷ˿���Ϣ
		reportconnectinfo(clientsocket);

	}

	closesocket(serversocket);
	WSACleanup();
	return 0;
}

//����ͻ����ļ�����(�ϴ�����)���߳�

void clientprocessthread(void* s)
{
	printf("\n\n�����ͻ����ϴ��̳߳ɹ�!\n\n\n");
	char filename[MAX_PATH];  //�ļ����MAX_PATH
	char data[MAX_SIZE];
	int i;
	char ch;
	char cmdreceive;
	FILE* fp;
	FILE* fsendp;
	//printf("Receive filename\n");
	//����ָ��
	//recv(*(SOCKET*)s, &cmdreceive, 1, 0);
	if (recv(*(SOCKET*)s, &cmdreceive, 1, 0) != 1)   //��ȡһ���ַ����ж϶�ȡ�������Ƿ�Ϊ1�����������1�����൱��û���ӻ��ȡʧ��
	{
		printf("receive cmd falied\n");
		closesocket(*(SOCKET*)s);
		return;
	}
	//��ȡ����s,g�ֱ���
	switch (cmdreceive)
	{
	case 's'://send from client to server
		printf("s Receive cmd is cmdreceive =%c\n", cmdreceive);
		memset((void*)filename, 0, sizeof(filename));
		for (i = 0; i < sizeof(filename); i++)
		{
			if (recv(*(SOCKET*)s, &ch, 1, 0) != 1)   //��ȡһ���ַ����ж϶�ȡ�������Ƿ�Ϊ1�����������1�����൱��û���ӻ��ȡʧ��
			{
				printf("filename receive failed or client close connection\n");
				closesocket(*(SOCKET*)s);
				return;
			}

			//if (ch == 0)  //�����ļ�����������Ϊ��ȷ���ļ���������ɣ�client�����'\0'��Ϊ���������յ�֮������
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
		//����client�������ļ���
		memset((void*)filename, 0, sizeof(filename));
		for (i = 0; i < sizeof(filename); i++)
		{
			if (recv(*(SOCKET*)s, &ch, 1, 0) != 1)   //��ȡһ���ַ����ж϶�ȡ�������Ƿ�Ϊ1�����������1�����൱��û���ӻ��ȡʧ��
			{
				printf("filename receive failed or client close connection\n");
				closesocket(*(SOCKET*)s);
				return;
			}
			//if (ch == 0)//"\0"��һ����0
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
			//ret = send(fileclientsocket, data, 1, 0);//1,ֻ����BUGֻ����һ��
			putchar('.');
			if (send(*(SOCKET*)s, data, i, 0) == SOCKET_ERROR)
			{
				printf("\n send failed,file maybe not complete\n");
				break;
			}
		}
		fclose(fsendp);
		closesocket(*(SOCKET*)s);  //��Ӳ����Ƿ���Ч
		reportconnectinfo(*(SOCKET*)s);
		break;
	default:
		printf("default Receive cmd is cmdreceive =%c\n", cmdreceive);
		break;
	}

	_endthread();
	//closesocket(*(SOCKET*)s);
}


//�������˫����IP��ַ�Ͷ˿���Ϣ
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
		if (recv(*(SOCKET*)s, &cmdreceive, 1, 0) != 1)   //��ȡһ���ַ����ж��Ƿ�Ϊ1�����������1�����൱��û���ӻ��ȡʧ��
		{
			printf("receive failed or client close connection\n");
			closesocket(*(SOCKET*)s);
			return;
		}
	return cmdreceive;
}


