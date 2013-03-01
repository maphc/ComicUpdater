#ifndef __CUSTSERVPUBLIC_H_
#define __CUSTSERVPUBLIC_H_

#include <sstream>
#include <string>
#include <algorithm>
#include <functional>


using namespace std;

template <typename T>
const string to_string(T value, bool sign = false) {
	ostringstream buffer; 
	if(sign)buffer<<showpos;
	buffer<<fixed<<value;
	return buffer.str();
}

#define CLASS_FUNC(theFunc) void theFunc(CFmlBuf &inBuf,CFmlBuf &outBuf)

//string& ltrim(string &ss, int (*pf)(int)=isspace)
//{
//	string::iterator p=find_if(ss.begin(),ss.end(),not1(ptr_fun(pf)));
//	ss.erase(ss.begin(),p);
//	return ss;
//}
//
//string& rtrim(string &ss, int (*pf)(int)=isspace)
//{
//	string::reverse_iterator p=find_if(ss.rbegin(),ss.rend(),not1(ptr_fun(pf)));
//	ss.erase(p.base(),ss.end());
//	return ss;
//}
//
//string& trim(string &st)
//{
//	ltrim(rtrim(st));
//	return st;
//}

#endif