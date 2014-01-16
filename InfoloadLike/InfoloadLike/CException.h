// CException.h: interface for the CException class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_CException_H__03E21B52_E5ED_451B_920A_ED406035BDB6__INCLUDED_)
#define AFX_CException_H__03E21B52_E5ED_451B_920A_ED406035BDB6__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <map>
#include <vector>

#ifdef WIN32
#pragma warning(disable:4786)
#else
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#endif
using namespace std;


//??异常信息类

class CExceptionMsg
{
public:
	//??异常发生的文件
	char m_filename[255];
	//??异常发生的行数
	int  m_linenum;
	//??异常信息
	char m_msg[1024];	
	
	//??构造函数
	//??参数：
	//??-filename 文件名
	//??-linenum 文件行号
	//??-format 记录格式
	//??-...  变量参数
	//??返回值：
	//??1 成功
	//??0 失败
	CExceptionMsg(char * filename, int linenum , char * format, ...);
	
	CExceptionMsg(int linenum, char * filename, char * msg)
	{
		strcpy(m_filename,filename);
		m_linenum = linenum;
		strncpy(m_msg,msg,sizeof(m_msg)-20);
		if (strlen(msg) > sizeof(m_msg)-20)
		{
			strcat(m_msg,"[[msg超长!]]");
		}
	};
};
//#define THROW throw CException(__FILE__,__LINE__,

//??异常处理类
class CException
{
public:
	//??信息打印
	//??参数：
	//??无
	//??返回值：
	//??1 成功
	//??0 失败
	void toString();
	
	//??信息转化成字符串
	//??参数：
	//??无
	//??返回值：
	//??错误信息
	char *GetString();
	
	//??构造函数
	//??参数：
	//??-filename 文件名
	//??-linenum 文件行号
	//??-format 记录格式
	//??-...  变量参数
	//??返回值：
	//??无
	CException(char * filename, int linenum , const char * format, ...);
	
	//??构造函数
	//??参数：
	//??-errstr 错误信息
	//??返回值：
	//??无
	CException(char *  errstr);
	
	//??构造函数
	CException();
	
	//??迭代异常信息
	//??参数：
	//??-msg 错误信息对象
	//??返回值：
	//??无
	void Push(CExceptionMsg msg);
	
	//??异常信息vector类型
	vector<CExceptionMsg> m_msg;
	
	//??异常信息
	char m_errstr[4*1024];
	
	//??析构函数
	~CException();

};


#endif // !defined(AFX_CException_H__03E21B52_E5ED_451B_920A_ED406035BDB6__INCLUDED_)
