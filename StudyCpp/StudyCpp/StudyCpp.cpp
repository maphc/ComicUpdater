// StudyCpp.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Handler.h"
#include "TinyReplacer.h"


int _tmain(int argc, _TCHAR* argv[])
{
	ifstream f("z:\\a.txt");
	string context;
	context.reserve(16*1024);
	context.assign(istreambuf_iterator<char>(f.rdbuf()),istreambuf_iterator<char>());


	TinyReplacer np;
	np.init();
	np.setUser("crm1");
	np.setConn("10.124.0.3/crm1");
	string result=np.replace(context);
	printf("%s\n",result.c_str());


	return EXIT_SUCCESS;
}

