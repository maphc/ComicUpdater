#pragma once 


using namespace std;

static string GetSimpleCfgParam(string keyParam){
	char pathBuf[200]={0};
	GetCurrentDirectory(200,pathBuf);

	string cfgFile=string(pathBuf)+"\\cfg.cfg";

	const int VALUE_BUF_SIZE=50;
	char rtn[VALUE_BUF_SIZE]={0};
	GetPrivateProfileString("1",keyParam.c_str(),"",rtn,VALUE_BUF_SIZE-1,cfgFile.c_str());
	return string(rtn);
}


