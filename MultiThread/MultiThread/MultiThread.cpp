// MultiThread.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
using namespace std;

DWORD WINAPI proc1(LPVOID lpParameter );
DWORD WINAPI proc2(LPVOID lpParameter );

int tickets=100;
//HANDLE mutex;
//HANDLE hEvent;
CRITICAL_SECTION section;
int _tmain(int argc, _TCHAR* argv[])
{
	
	HANDLE handle1=CreateThread(NULL,0,proc1,NULL,0,NULL);
	HANDLE handle2=CreateThread(NULL,0,proc2,NULL,0,NULL);
	CloseHandle(handle1);
	CloseHandle(handle2);
	
	/*
	mutex=CreateMutex(NULL,FALSE,_T("123"));
	if(NULL!=mutex&&ERROR_ALREADY_EXISTS==GetLastError()){
		cout<<"Instance exists !"<<endl;
		return 1;
	}
	*/
	/*
	hEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
	SetEvent(hEvent);
	*/
	InitializeCriticalSection(&section);
	cout<<"Main Thread is running !"<<endl;
	Sleep(10000);
	//CloseHandle(hEvent);
	DeleteCriticalSection(&section);
	return 0;
}

DWORD WINAPI proc1(LPVOID lpParameter ){
	while(true){
		//WaitForSingleObject(hEvent,INFINITE);
		EnterCriticalSection(&section);
		if(tickets>0){
			Sleep(1);
			cout<<"Thread1 is selling :"<<tickets--<<endl;
		}else{
			break;
		}
		LeaveCriticalSection(&section);
		//SetEvent(hEvent);
	}
	
	return 0;
}
DWORD WINAPI proc2(LPVOID lpParameter ){
	while(true){
		//WaitForSingleObject(hEvent,INFINITE);
		EnterCriticalSection(&section);
		if(tickets>0){
			Sleep(1);
			cout<<"Thread2 is selling :"<<tickets--<<endl;
		}else{
			break;
		}
	//		ReleaseMutex(mutex);
		//SetEvent(hEvent);
		LeaveCriticalSection(&section);
	}
	return 0;
}


