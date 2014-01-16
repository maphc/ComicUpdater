// pele.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <tlhelp32.h>
#include <shellapi.h>
#include <stdio.h>



void runas(TCHAR* pFileName){
	SHELLEXECUTEINFO sei = { sizeof(sei) };
	sei.lpVerb = _T("runas");
	sei.lpFile = pFileName;

	sei.nShow = SW_NORMAL;

	if (!ShellExecuteEx(&sei))
	{
		DWORD dwError = GetLastError();
		if (dwError == ERROR_CANCELLED)
		{
			printf("%s run as a administrator failed ",pFileName);
		}else{

			printf("%s run as a administrator success ",pFileName);
		}
	}
}

int WINAPI WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd ){
	runas(_T("c:\\windows\\system32\\cmd.exe"));


	return 0;
}



