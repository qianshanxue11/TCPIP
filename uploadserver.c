#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>

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
		//����һ���̴߳���һ���ͻ����ϴ�
		_beginthread(serveoneclient,0,(void *)psocket);
		Sleep(1000);

		//���˫����IP��ַ�Ͷ˿���Ϣ
		reportconnectinfo(acceptsocket);
	}

	closesocket(listensocket);
	WSACleanup();
	return 0;
}

//����ͻ����ļ��ϴ����߳�

void serveoneclient(void* s)
{
	printf("\n\n�����̳߳ɹ�!\n\n\n");
	char filename[MAX_PATH];
	char data[MAX_SIZE];

}


//�������˫����IP��ַ�Ͷ˿���Ϣ
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


