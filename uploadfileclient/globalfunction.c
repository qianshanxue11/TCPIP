#include "globalfunction.h"

int sendCmdToserver(SOCKET s, char* cmd)
{
	//����s�ַ���server	
	printf("send the cmd %s to server...\n", cmd);
	if (send(s, cmd, (int)(strlen(cmd)), 0) == SOCKET_ERROR)
	{
		wprintf(L"send failed with error: %d\n", WSAGetLastError());
		closesocket(s);
		WSACleanup();
		return 1;
	}
	return 0;
}

int getFileName(char* filename)
{
	
	int filenamelength = 0;
	printf("����Ҫ�������ļ���:\r\n");
	//����1����һ��getchar����
	//getchar();//����һ��getchar����
	//����2���������������õ����ַ����ǿմ����������,Ҳ��Ч��
	do {
	gets(filename);
	} while (!filename[0]);
	//flush(stdin);��Ч
	//gets(filename);
	for (filenamelength = 0; filenamelength < 81; filenamelength++)
	{
		if (filename[filenamelength] == '\0')
		{
			break;
		}
	}
	printf("�ļ�����: %s ����Ϊ:%d\n", filename, filenamelength);
	return filenamelength;
}

int sendFileName(SOCKET s, char* filename)
{
	int iResult;
	//���͸�������������ļ����ͣ�rb,�Զ�����readģʽ�򿪣��ļ��������
	printf("----SEND FILENAME TO SERVER----\n");
	//�����ļ�������������
	printf("Send the name of sendfile to server...\n");
	// Send an initial buffer
	iResult = send(s, filename, (int)strlen(filename), 0);
	if (iResult == SOCKET_ERROR) {
		wprintf(L"send filename failed with error: %d\n", WSAGetLastError());
		closesocket(s);
		WSACleanup();
		return 1;
	}
	iResult = send(s, "\0", 1, 0);  //filename �ַ������͵Ľ���
	if (iResult == SOCKET_ERROR) {
		wprintf(L"send filename failed with error: %d\n", WSAGetLastError());
		closesocket(s);
		WSACleanup();
		return 1;
	}
	return 0;
}

int sendFile(SOCKET s, FILE* fp)
{
	char data[MAX_SIZE];
	int iResult;
	int returnResult;
	while (1)
	{
		memset((void*)data, 0, sizeof(data));
		iResult = fread(data, 1, sizeof(data), fp);
		//��һ��ִ�У�i=����������
		//�ڶ���ִ��ʱ��i=0����break
		if (iResult == 0)
		{
			printf("\n send file data success\n");
			break;
		}
		//ret���ط��͵�������Ŀ
		returnResult = send(s, data, iResult, 0);
		putchar('.');
		if (returnResult == SOCKET_ERROR)
		{
			printf("\n send failed,file maybe not complete\n");
			break;
		}
	}
	fclose(fp);
	return 0;
}

int receiveFile(SOCKET s, FILE* fp)
{
	char data[MAX_SIZE];
	int iResult;
	int returnResult;
	printf("-----The file content from server------\n");
	memset((void*)data, 0, sizeof(data));
	// Receive until the peer closes the connection��
	//so after server transfer ,server nee dto close the socket
	do {
		iResult = recv(s, data, sizeof(data), 0);
		if (iResult > 0)
		{
			wprintf(L"Bytes received: %d\n", iResult);
			if (fwrite((void*)data, 1, iResult, fp) == iResult)
			{
				wprintf(L"Write to file successfully\n");
			}
			//fwrite((void*)data, 1, i, fpreceive);
		}
		else if (iResult == 0)
			wprintf(L"Connection closed\n");
		else
			wprintf(L"recv failed with error: %d\n", WSAGetLastError());

	} while (iResult > 0);
	printf("%s\n", data);
	printf("-----RECEIVE FINISH--------------\n");
	fclose(fp);//
	return 0;
}