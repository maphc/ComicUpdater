// TcpSrv.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <WinSock2.h>
const int MAX_SIZE=512;

int _tmain(int argc, _TCHAR* argv[])
{
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested=MAKEWORD(2,2);
	if(WSAStartup(wVersionRequested,&wsaData)!=0){
		puts("Error !");
		return -1;
	}
	printf("High :%d Low :%d \n",HIBYTE(wsaData.wHighVersion),LOBYTE(wsaData.wHighVersion));
	printf("Descriptions :%s\n",wsaData.szDescription);

	if(LOBYTE(wsaData.wVersion)!=2||HIBYTE(wsaData.wVersion)!=2){
		puts("Error Version !");
		return -1;
	}
	
	//china-74c99b15d
	char hostName[256];
	if(SOCKET_ERROR==gethostname(hostName,sizeof(hostName))){
		puts("GETHOSTNAME ERROR !");
	}else{
		printf("Host name :%s\n",hostName);
	}
	
	SOCKET s=socket(AF_INET,SOCK_STREAM,0);
	if(INVALID_SOCKET==s){
		puts("Error socket !");
		WSACleanup();
		return -1;
	}
	puts("SOCKET Created !");

	SOCKADDR_IN addrSrv;

	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(1210);
	addrSrv.sin_addr.S_un.S_addr=INADDR_ANY;
	//addrSrv.sin_addr.s_addr = inet_addr("127.0.0.1");


	if(SOCKET_ERROR==bind(s,(SOCKADDR*)&addrSrv,sizeof(addrSrv))){
		puts("Error bind !");
		WSACleanup();
		return -1;
	
	}
	puts("bind ends ..");
	
	if(SOCKET_ERROR==listen(s,5)){
		puts("Error listen !");
		WSACleanup();
		return -1;
	}
	
	SOCKADDR_IN srvSockAddr;
	int len=sizeof(srvSockAddr);

	while(1){
		
		SOCKET srvSock=accept(s,(SOCKADDR*)&srvSockAddr,&len);
		puts("accept a socket ...");
		char recvBuf[MAX_SIZE];

		int iResult=recv(srvSock,recvBuf,strlen(recvBuf)+1,0);
		if(iResult>0){
			printf("Receive :%s\n",recvBuf);
		}
		closesocket(srvSock);
	}
	

	return 0;
}

