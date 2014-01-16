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


//??�쳣��Ϣ��

class CExceptionMsg
{
public:
	//??�쳣�������ļ�
	char m_filename[255];
	//??�쳣����������
	int  m_linenum;
	//??�쳣��Ϣ
	char m_msg[1024];	
	
	//??���캯��
	//??������
	//??-filename �ļ���
	//??-linenum �ļ��к�
	//??-format ��¼��ʽ
	//??-...  ��������
	//??����ֵ��
	//??1 �ɹ�
	//??0 ʧ��
	CExceptionMsg(char * filename, int linenum , char * format, ...);
	
	CExceptionMsg(int linenum, char * filename, char * msg)
	{
		strcpy(m_filename,filename);
		m_linenum = linenum;
		strncpy(m_msg,msg,sizeof(m_msg)-20);
		if (strlen(msg) > sizeof(m_msg)-20)
		{
			strcat(m_msg,"[[msg����!]]");
		}
	};
};
//#define THROW throw CException(__FILE__,__LINE__,

//??�쳣������
class CException
{
public:
	//??��Ϣ��ӡ
	//??������
	//??��
	//??����ֵ��
	//??1 �ɹ�
	//??0 ʧ��
	void toString();
	
	//??��Ϣת�����ַ���
	//??������
	//??��
	//??����ֵ��
	//??������Ϣ
	char *GetString();
	
	//??���캯��
	//??������
	//??-filename �ļ���
	//??-linenum �ļ��к�
	//??-format ��¼��ʽ
	//??-...  ��������
	//??����ֵ��
	//??��
	CException(char * filename, int linenum , const char * format, ...);
	
	//??���캯��
	//??������
	//??-errstr ������Ϣ
	//??����ֵ��
	//??��
	CException(char *  errstr);
	
	//??���캯��
	CException();
	
	//??�����쳣��Ϣ
	//??������
	//??-msg ������Ϣ����
	//??����ֵ��
	//??��
	void Push(CExceptionMsg msg);
	
	//??�쳣��Ϣvector����
	vector<CExceptionMsg> m_msg;
	
	//??�쳣��Ϣ
	char m_errstr[4*1024];
	
	//??��������
	~CException();

};


#endif // !defined(AFX_CException_H__03E21B52_E5ED_451B_920A_ED406035BDB6__INCLUDED_)
