// Md5Demo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{

	MD5 md5;
	md5.update("abc");


	cout<< "MD5(\"" << "" << "\") = " << md5.toString() << endl;
	
	return 0;
}

