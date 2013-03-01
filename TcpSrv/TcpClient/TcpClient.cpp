// TcpClient.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <WinSock2.h>

int _tmain(int argc, _TCHAR* argv[])
{
	WORD wVersionRequested=MAKEWORD(2,2);
	WSADATA wsaData;
	WSAStartup(wVersionRequested,&wsaData);
	if(LOBYTE(wsaData.wVersion)!=2||HIBYTE(wsaData.wVersion)!=2){
		puts("Error startup !");
		return -1;
	}

	SOCKET s=socket(AF_INET,SOCK_STREAM,0);
	if(INVALID_SOCKET==s){
		puts("Error socket !");
		return -1;
	}
	
	SOCKADDR_IN clientAddr;
	clientAddr.sin_family=AF_INET;
	clientAddr.sin_port=htons(1210);
	clientAddr.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");

	int n=connect(s,(SOCKADDR*)&clientAddr,sizeof(clientAddr));
	if(SOCKET_ERROR==n){
		puts("Error connect !");
		return -1;
	}

	char sendBuf[512]="Hello Server !";
	send(s,sendBuf,strlen(sendBuf)+1,0);

	closesocket(s);

	WSACleanup();

	return 0;
}

