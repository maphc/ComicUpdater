#pragma once
#include <map>
#include <iostream>
#include <string>

#include "StaticDAO.h"
using namespace std;

class CParamDAO {
private:
	static StaticDAO& dao;
	static map<string, map<string,string> > cache;
	static const bool IS_SKIP;
public:

	//! �������أ������ݿ��ж�ȡ�������ݲ�����
	//static void Load();

	//! ж�ػ���
	//static void UnLoad();

	//! ��ֵ��һ��valueƥ�����
	static string getParam(const string& key, const string& value1);

	//! ��ֵ�Ӷ���valueƥ�����
	static string getParam(const string& key, const string& value1, const string& value2);

	//! ��ֵ������valueƥ�����
	//���������˵
	//static string getParam(const string& key, const string& value1, const string& value2, const string& value3);

};
int ConvertCodeToName(CFmlBuf& buf, const string& skey, const char* fromFldName1, const char* fromFldName2, const char* toFldName);
int ConvertCodeToName(CFmlBuf& buf, const string& skey, const char* fromFldName, const char* toFldName);
