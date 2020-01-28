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


