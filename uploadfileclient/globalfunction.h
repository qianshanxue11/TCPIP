#pragma once
//防止多次被包含
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma warning(disable : 4996)
#pragma comment(lib, "Ws2_32.lib")
#define MAX_SIZE 10240

int sendCmdToserver(SOCKET s, char* cmd);
int getFileName(char* filename);
int sendFileName(SOCKET s, char* filename);
int sendFile(SOCKET s, FILE* fp);
int receiveFile(SOCKET s, FILE* fp);
