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

	//! 参数加载，从数据库中读取参数数据并缓存
	//static void Load();

	//! 卸载缓存
	//static void UnLoad();

	//! 键值加一个value匹配参数
	static string getParam(const string& key, const string& value1);

	//! 键值加二个value匹配参数
	static string getParam(const string& key, const string& value1, const string& value2);

	//! 键值加三个value匹配参数
	//这个遇到再说
	//static string getParam(const string& key, const string& value1, const string& value2, const string& value3);

};
int ConvertCodeToName(CFmlBuf& buf, const string& skey, const char* fromFldName1, const char* fromFldName2, const char* toFldName);
int ConvertCodeToName(CFmlBuf& buf, const string& skey, const char* fromFldName, const char* toFldName);
