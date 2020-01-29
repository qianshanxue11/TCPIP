#include "globalfunction.h"

int sendCmdToserver(SOCKET s, char* cmd)
{
	//发送s字符给server	
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
	printf("输入要操作的文件名:\r\n");
	//方法1增加一个getchar可以
	//getchar();//增加一个getchar可以
	//方法2检查输入结果，如果得到的字符串是空串，则继续读,也有效果
	do {
	gets(filename);
	} while (!filename[0]);
	//flush(stdin);无效
	//gets(filename);
	for (filenamelength = 0; filenamelength < 81; filenamelength++)
	{
		if (filename[filenamelength] == '\0')
		{
			break;
		}
	}
	printf("文件名是: %s 长度为:%d\n", filename, filenamelength);
	return filenamelength;
}

int sendFileName(SOCKET s, char* filename)
{
	int iResult;
	//发送给服务器，则打开文件类型，rb,以二进制read模式打开，文件必须存在
	printf("----SEND FILENAME TO SERVER----\n");
	//发送文件名给服务器端
	printf("Send the name of sendfile to server...\n");
	// Send an initial buffer
	iResult = send(s, filename, (int)strlen(filename), 0);
	if (iResult == SOCKET_ERROR) {
		wprintf(L"send filename failed with error: %d\n", WSAGetLastError());
		closesocket(s);
		WSACleanup();
		return 1;
	}
	iResult = send(s, "\0", 1, 0);  //filename 字符串发送的结束
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
		//第一次执行，i=读到的数据
		//第二次执行时候，i=0所以break
		if (iResult == 0)
		{
			printf("\n send file data success\n");
			break;
		}
		//ret返回发送的数据数目
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
	// Receive until the peer closes the connection，
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