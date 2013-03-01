// CException.cpp: implementation of the CException class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CException::CException()
{}




CException::~CException()
{}




CException::CException( char * errstr)
{
	m_msg.push_back(CExceptionMsg(0,"__FILE__",errstr));
}

CException::CException(char * filename, int linenum , const char * format ,...)
{
	char *ls_str = new char[1024 * 10];
	va_list  pvar;
	va_start (pvar, format);
	vsprintf(ls_str,format,pvar);
	va_end(pvar);
	//CExceptionMsg e(filename,linenum,"%s",ls_str);
	CExceptionMsg e(linenum,filename,ls_str);
	m_msg.push_back(e);
	delete[] ls_str;
}

void CException::toString()
{
	vector<CExceptionMsg>::iterator it;
	for ( it = m_msg.begin(); it != m_msg.end(); it++)
		printf("File[%12s:%4d] Msg[ %s ]\n",it->m_filename,it->m_linenum,it->m_msg);
}

void CException::Push(CExceptionMsg msg)
{
	m_msg.push_back(msg);
}

char* CException::GetString()
{
        vector<CExceptionMsg>::iterator it;
        char *pstr = new char[1024];
        int curpos = 0;
        int leftsize = sizeof(m_errstr) -1;
        int length = 0;
	      for ( it = m_msg.begin(); it != m_msg.end(); it++)
        {
                sprintf(pstr,"File[%12s:%4d] Msg[ %s ]\n",it->m_filename,it->m_linenum,it->m_msg);
                length = strlen(pstr);
                leftsize -= length;
                if (leftsize < 0)
                        break;
                strcpy(&m_errstr[curpos], pstr);
                curpos += length;
        }
        delete [] pstr;//
        return m_errstr;
}

CExceptionMsg::CExceptionMsg(char * filename, int linenum , char * format, ...)
{
	strcpy(m_filename,filename);
	m_linenum = linenum;
	char *msg = new char[1024*2];
	va_list  pvar;
	va_start (pvar,format);
	vsprintf(msg,format,pvar);
	va_end (pvar);
	strncpy(m_msg,msg,sizeof(m_msg)-20);
	if (strlen(msg) > sizeof(m_msg)-20)
	{
		strcat(m_msg,"[[msg³¬³¤]]");
	}
	delete [] msg;
}



