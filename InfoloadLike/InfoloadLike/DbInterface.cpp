#include "stdafx.h"
using namespace oracle::occi;
using namespace std;

inline int strncasecmp(const char* s1, const char* s2, size_t n)
{

	int diff = 0;
	for ( ; ; ++s1, ++s2, --n) {
		char c1 = *s1;
		// calculate difference
		diff = toupper(c1) - toupper(*s2);
		// if end of string or different
		if (!c1  ||  diff)
			break; // return difference
		if (n == 0)
			return 0;
	}
	return diff;

}


DbInterface::DbInterface(const Environment::Mode &mode)
{
	m_env = Environment::createEnvironment(mode);
	m_conn = (Connection *)NULL;
	m_stmt = (Statement *)NULL;
	m_rs = (ResultSet *)NULL;
	
	m_transflag = 0;
}


DbInterface::~DbInterface()
{
	disconnect();
	
	//�ͷ�ϵͳ����ռ�õ���Դ
	if (m_env)
	{
		Environment::terminateEnvironment(m_env);
		m_env = (Environment *)NULL;
	}
}


void DbInterface::connect(const string& username, const string& password, const string& servname)
{
	//��ֹ�����Ӵ���,�ȶϿ�
	disconnect();
	
	//�������ݿ�����
	try
	{
		m_conn = m_env->createConnection(username, password, servname);
	}
	catch(SQLException e)
	{
		cout<<"ORA :"<<e.getErrorCode()<<" "<<e.what()<<endl;
		throw CException(__FILE__, __LINE__, "�������ݿ����ӳ���,������Ϣ:=%s", e.getMessage().c_str());
	}
}


void DbInterface::disconnect()
{
	//�������������,��������лع�
	if (m_transflag == 1)
	{
		rollbackTrans();
	}
	
	//�ͷ�ϵͳ����ռ�õ���Դ
	try
	{
		if (m_conn)
		{
			if (m_stmt)
			{
				if (m_rs)
				{
					m_stmt->closeResultSet(m_rs);
				}
					
				m_conn->terminateStatement(m_stmt);
			}
				
			m_env->terminateConnection(m_conn);
		}
	}
	catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "�Ͽ����ݿ����ӳ���,������Ϣ:=%s", e.getMessage().c_str());
	}
		
	//ϵͳ������NULLֵ
	m_rs = (ResultSet *)NULL;
	m_stmt = (Statement *)NULL;
	m_conn = (Connection *)NULL;
}


void DbInterface::beginTrans()
{
	if (m_transflag == 0)
	{
		m_transflag = 1;
	}
}


void DbInterface::commitTrans()
{
	//������,�ύ
	if (m_transflag==1 && m_conn)
	{
		try
		{
			m_conn->commit();
		}
		catch(SQLException e)
		{
			throw CException(__FILE__, __LINE__, "���ݿ������ύʧ��,������Ϣ:=%s", e.getMessage().c_str());
		}
		m_transflag = 0;
	}
}
		

void DbInterface::rollbackTrans()
{
	//������,�ع�
	if (m_transflag==1 && m_conn)
	{
		try
		{
			m_conn->rollback();
		}
		catch(SQLException e)
		{
			throw CException(__FILE__, __LINE__, "���ݿ�����ع�ʧ��,������Ϣ:=%s", e.getMessage().c_str());
		}		
		m_transflag = 0;
	}
}


int DbInterface::execStaticSql(const string& staticsql)
{
	//����SQL������ѯ����
	m_columnnum = 0;
	
	char sqlstmt[MAX_DBSTRVALUE_LEN] = "";
	if (staticsql.length() < MAX_DBSTRVALUE_LEN)
	{
		strcpy(sqlstmt, staticsql.c_str());
	}
	else
	{
		throw CException(__FILE__, __LINE__, "���ݿ�ִ�о�̬SQL���ȳ�������%d,SQL���:[%s]", MAX_DBSTRVALUE_LEN, staticsql.c_str());
	}
	char *c = NULL;
	for (c=sqlstmt; c[0]==' '; c++);
	if(strncasecmp(c,"SELECT",6) == 0)
	{
		//��¼������'('����
		int signnum = 0;
		c = strcasestr(sqlstmt,"SELECT");
		while(c < strcasestr(sqlstmt,"FROM"))
		{
			c++;
			
			if (c[0] == '(')
				++signnum;
			if (c[0] == ')')
				--signnum;
				
			if(signnum==0 && c[0]==',')
				m_columnnum++;
			
		}
		m_columnnum++;
	}
	else
	{
		m_columnnum = 0;
	}
	
	
	//���ǰһ������ռ�õ���Դ
	try
	{
		if (m_stmt)
		{
			if (m_rs)
			{
				m_stmt->closeResultSet(m_rs);
				m_rs = (ResultSet *)NULL;
			}
			m_conn->terminateStatement(m_stmt);
			m_stmt = (Statement *)NULL;
		}
		
		//ִ�о�̬SQL
		m_stmt = m_conn->createStatement();
		//��ѯ����
		if (m_columnnum != 0)
		{
			m_stmt->setPrefetchRowCount(DEFAULT_RECORD_NUM);
		}
		m_stmt->setAutoCommit(false);

		m_stmt->setSQL(staticsql);	
		Statement::Status status = m_stmt->execute();
	
		//��ѯ
		if (status == Statement::RESULT_SET_AVAILABLE)
		{	
			m_rs = m_stmt->getResultSet(); 	
			return 1;
		}
		//����
		else if (status == Statement::UPDATE_COUNT_AVAILABLE)
		{
			return m_stmt->getUpdateCount(); 
		}
		//����
		else
		{	
			return 0;
		}
	}
	catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "���ݿ�ִ�о�̬SQLʧ��,������Ϣ:=%s", e.getMessage().c_str());
	}
}
		
		
int DbInterface::execDynamicSql(const string& dynamicsql, const vector<DbParam>& params)
{
  #ifdef FP_DEBUG
    FP_BEGIN(execDynamicSql)
  #endif	
	//(1)SQL������Ч���ж�
	char sqlstmt[MAX_DBSTRVALUE_LEN] = "";
	if (dynamicsql.length() < MAX_DBSTRVALUE_LEN)
	{
		strcpy(sqlstmt, dynamicsql.c_str());
	}
	else
	{
		throw CException(__FILE__, __LINE__, "��̬SQL���ȳ�������%d,SQL���:[%s]", MAX_DBSTRVALUE_LEN, dynamicsql.c_str());
	}

#ifdef _DEBUG_
/*
	printf("sqlstmt:=%s\n", sqlstmt);
	char currtime[15];
	time_t tNowTime;
	struct tm *stNowTime = NULL;
	time(&tNowTime);
	stNowTime=localtime(&tNowTime);
	
	sprintf(currtime, "%04d%02d%02d%02d%02d%02d",
  			stNowTime->tm_year+1900,stNowTime->tm_mon+1,stNowTime->tm_mday,stNowTime->tm_hour,stNowTime->tm_min,stNowTime->tm_sec);
  			
  printf("time:=%s\n", currtime);
  */
//	for (int i=0; i<params.size(); ++i)
//	{
//		printf("%d;",params[i].m_seq);
//		if (params[i].m_type == PARAM_TYPE_INT)
//		{
//			printf("%d;",params[i].m_intValue);
//		}
//		else if (params[i].m_type == PARAM_TYPE_LONG)
//		{
//			printf("%ld;",params[i].m_longValue);
//		}
//		else if (params[i].m_type == PARAM_TYPE_FLOAT)
//		{
//			printf("%f;",params[i].m_floatValue);
//		}
//		else if (params[i].m_type == PARAM_TYPE_DATE)
//		{
//			printf("%s;",params[i].m_dateValue);
//		}
//		else if (params[i].m_type == PARAM_TYPE_STR)
//		{
//			printf("%s;",params[i].m_strValue);
//		}
//		printf("\n");
//	}
#endif

	//(2)������ѯSQLѡȡ������
	m_columnnum = 0;
	char *c = NULL;
	for(c=sqlstmt;c[0]==' ';c++);
	if(strncasecmp(c,"SELECT",6) == 0)
	{
		//��¼������'('����
		int signnum = 0;
		c = strcasestr(sqlstmt,"SELECT");
		while(c < strcasestr(sqlstmt,"FROM"))
		{
			c++;
			
			if (c[0] == '(')
				++signnum;
			if (c[0] == ')')
				--signnum;
				
			if(signnum==0 && c[0]==',')
				m_columnnum++;
			
		}
		m_columnnum++;
	}
	else
	{
		m_columnnum = 0;
	}
	
	//(3)ִ��SQL����
	try
	{	
		//(3.1)���ǰһ������ռ�õ���Դ
		if (m_stmt)
		{
			if (m_rs)
			{
				m_stmt->closeResultSet(m_rs);
				m_rs = (ResultSet *)NULL;
			}
			m_conn->terminateStatement(m_stmt);
			m_stmt = (Statement *)NULL;
		}

		//(3.2)�����µ�m_stmt
		m_stmt = m_conn->createStatement(dynamicsql);
		//��ѯ����
		if (m_columnnum != 0)
		{
			m_stmt->setPrefetchRowCount(DEFAULT_RECORD_NUM);
		}
		m_stmt->setAutoCommit(false);
		
		//(3.3)ִ�в���׼��
		for (int i=0; i<params.size(); ++i)
		{
			//���ݲ�ͬ�����������ò�ͬ����
			if (params[i].m_type == PARAM_TYPE_INT)
			{
				m_stmt->setInt(params[i].m_seq, params[i].m_intValue);
			}
			else if (params[i].m_type == PARAM_TYPE_LONG)
			{
				char c[24]="\0";
				sprintf(c, "%ld", params[i].m_longValue);
				string s = c;
				m_stmt->setString(params[i].m_seq, s);
			}
			else if (params[i].m_type == PARAM_TYPE_FLOAT)
			{
				m_stmt->setFloat(params[i].m_seq, params[i].m_floatValue);
			}
			else if (params[i].m_type == PARAM_TYPE_DATE)
			{
				string s = params[i].m_dateValue;
				if (s.length() == 0)
				{	
					Date dateValue;
					m_stmt->setDate(params[i].m_seq, dateValue);
				}
				else
				{
					int iyear = atoi(s.substr(0,4).c_str());
					int imonth = atoi(s.substr(4,2).c_str());
					int iday = atoi(s.substr(6,2).c_str());
					int ihour = atoi(s.substr(8,2).c_str());
					int iminute = atoi(s.substr(10,2).c_str());
					int isecond = atoi(s.substr(12,2).c_str());
					Date dateValue(m_env, iyear, imonth, iday, ihour, iminute, isecond);
					m_stmt->setDate(params[i].m_seq, dateValue);
				}
			}
			else if (params[i].m_type == PARAM_TYPE_STR)
			{
				string s = params[i].m_strValue;
				m_stmt->setString(params[i].m_seq, s);
			}
		}
		
		//(3.4)ִ��SQL,���ؽ��
		Statement::Status status = m_stmt->execute();
  #ifdef FP_DEBUG
    FP_END(execDynamicSql)
  #endif			  
		//��ѯ
		if (status == Statement::RESULT_SET_AVAILABLE)
		{	
			m_rs = m_stmt->getResultSet();
			return 1;
		}
		//����
		else if (status == Statement::UPDATE_COUNT_AVAILABLE)
		{
			return m_stmt->getUpdateCount();
		}
		//����
		else
		{
			return 0;
		}
	}
	catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "��̬SQLִ��ʧ��,������Ϣ:=%s", e.getMessage().c_str());
	}
	/*
	char currtime1[15];
	///time_t tNowTime;
	//struct tm *stNowTime = NULL;
	time(&tNowTime);
	stNowTime=localtime(&tNowTime);
	
	sprintf(currtime1, "%04d%02d%02d%02d%02d%02d",
  			stNowTime->tm_year+1900,stNowTime->tm_mon+1,stNowTime->tm_mday,stNowTime->tm_hour,stNowTime->tm_min,stNowTime->tm_sec);
  			
  printf("time:=%s\n", currtime1);
  */
}


void DbInterface::execProcedure(const string& procedurename, vector<DbParam>& params)
{
	//׼�����ô洢���̵�SQL
	string strSql = "";
	char sql[200];
	memset(sql, 0x0, 200);
	sprintf(sql, "BEGIN %s(", procedurename.c_str());
	strSql = sql;
	for (int i=1; i<=params.size(); i++)
	{
		if (i == params.size())
		{
			sprintf(sql, ":%d", i);
		}
		else
		{	
			sprintf(sql, ":%d,", i);
		}
		strSql = strSql + sql;
	}
	sprintf(sql, "); END;");
	strSql = strSql + sql;
	
	//���ǰһ������ռ�õ���Դ
	try
	{
		if (m_stmt)
		{
			if (m_rs)
			{
				m_stmt->closeResultSet(m_rs);
				m_rs = (ResultSet *)NULL;
			}
			m_conn->terminateStatement(m_stmt);
			m_stmt = (Statement *)NULL;
		}
	
		//ִ�ж�̬SQL
		m_stmt = m_conn->createStatement();
//		m_stmt->setPrefetchRowCount(DEFAULT_RECORD_NUM);
		m_stmt->setAutoCommit(false);
		
		m_stmt->setSQL(strSql);
		for (int i=0; i<params.size(); ++i)
		{
			//���ݲ�ͬ�����������ò�ͬ����
			if (params[i].m_type == PARAM_TYPE_INT)
			{
				if (params[i].m_action == PARAM_ACT_OUT)
				{
					m_stmt->registerOutParam(params[i].m_seq, OCCIINT, sizeof(int));
				}
				else
				{
					m_stmt->setInt(params[i].m_seq, params[i].m_intValue);
				}
			}
			else if (params[i].m_type == PARAM_TYPE_LONG)
			{
				if (params[i].m_action == PARAM_ACT_OUT)
				{
					m_stmt->registerOutParam(params[i].m_seq, OCCINUMBER, sizeof(Number));
				}
				else
				{
					char c[24]="\0";
					sprintf(c, "%ld", params[i].m_longValue);
					string s = c;
					m_stmt->setString(params[i].m_seq, s);
				}
			}
			else if (params[i].m_type == PARAM_TYPE_FLOAT)
			{
				if (params[i].m_action == PARAM_ACT_OUT)
				{
					m_stmt->registerOutParam(params[i].m_seq, OCCIFLOAT, sizeof(float));
				}
				else
				{
					m_stmt->setFloat(params[i].m_seq, params[i].m_floatValue);
				}
			}
			else if (params[i].m_type == PARAM_TYPE_DATE)
			{
				if (params[i].m_action == PARAM_ACT_OUT)
				{
					m_stmt->registerOutParam(params[i].m_seq, OCCIDATE, sizeof(Date));
				}
				else
				{
					string s = params[i].m_dateValue;
					if (s.length() == 0)
					{	
						Date dateValue;
						m_stmt->setDate(params[i].m_seq, dateValue);
					}
					else
					{
						int iyear = atoi(s.substr(0,4).c_str());
						int imonth = atoi(s.substr(4,2).c_str());
						int iday = atoi(s.substr(6,2).c_str());
						int ihour = atoi(s.substr(8,2).c_str());
						int iminute = atoi(s.substr(10,2).c_str());
						int isecond = atoi(s.substr(12,2).c_str());
						Date dateValue(m_env, iyear, imonth, iday, ihour, iminute, isecond);
						m_stmt->setDate(params[i].m_seq, dateValue);
					}
				}
			}
			else if (params[i].m_type == PARAM_TYPE_STR)
			{
				if (params[i].m_action == PARAM_ACT_OUT)
				{
					m_stmt->registerOutParam(params[i].m_seq, OCCISTRING, MAX_DBSTRVALUE_LEN);
				}
				else
				{
					string s = params[i].m_strValue;
					m_stmt->setString(params[i].m_seq, s);
				}
			}
		}
		m_stmt->executeUpdate();
	}
	catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "[%s]���ݿ�ִ�д洢����ʧ��,������Ϣ:=%s", m_objalisename.c_str(),e.getMessage().c_str());
	}
		
	for(int i=0; i<params.size(); i++)
	{
		if (params[i].m_action==PARAM_ACT_OUT)
		{
			//���ݲ�ͬ�����������ò�ͬ����
			if (params[i].m_type == PARAM_TYPE_INT)
			{
				params[i].m_intValue = m_stmt->getInt(i+1);
			}
			else if (params[i].m_type == PARAM_TYPE_LONG)
			{
				params[i].m_longValue = long(m_stmt->getNumber(i+1));
			}
			else if (params[i].m_type == PARAM_TYPE_FLOAT)
			{
				params[i].m_floatValue = m_stmt->getFloat(i+1);
			}
			else if (params[i].m_type == PARAM_TYPE_DATE)
			{
				Date date = m_stmt->getDate(i+1);
				strcpy(params[i].m_dateValue, (date.toText("yyyymmddhh24miss", "")).c_str());
			}
			else if (params[i].m_type == PARAM_TYPE_STR)
			{
				strcpy(params[i].m_strValue, (m_stmt->getString(i+1)).c_str());
			}
		}
	}
}


bool DbInterface::getNext()
{
	try
	{
		return m_rs->next();
	}
	catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "���ݿ��ȡ��¼�����ʧ��,������Ϣ:=%s", e.getMessage().c_str());
	}
}


int	 DbInterface::getColumnNum()
{
	return m_columnnum;
}


int DbInterface::getColumnIntValue(const unsigned int& pos)
{
	try
	{
		return m_rs->getInt(pos);
	}
	catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "���ݿ��ȡ��¼���������������ʧ��,������Ϣ:=%s", e.getMessage().c_str());
	}	
}


long DbInterface::getColumnLongValue(const unsigned int& pos)
{
	try
	{
		return long(m_rs->getNumber(pos));
	}
	catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "���ݿ��ȡ��¼������г���������ʧ��,������Ϣ:=%s", e.getMessage().c_str());
	}
}


float DbInterface::getColumnFloatValue(const unsigned int& pos)
{
	try
	{
		return m_rs->getFloat(pos);
	}
	catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "���ݿ��ȡ��¼������и���������ʧ��,������Ϣ:=%s", e.getMessage().c_str());
	}	
}


string DbInterface::getColumnDateValue(const unsigned int& pos)
{
	try
	{
		Date date = m_rs->getDate(pos);
		return date.toText("yyyymmddhh24miss", "");
	}
	catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "���ݿ��ȡ��¼���������������ʧ��,������Ϣ:=%s", e.getMessage().c_str());
	}
}


string DbInterface::getColumnStringValue(const unsigned int& pos)
{
	try
	{
		return m_rs->getString(pos);
	}
	catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "���ݿ��ȡ��¼��������ַ�����ʧ��,������Ϣ:=%s", e.getMessage().c_str());
	}
}


string DbInterface::getColumnToStringValue(const unsigned int& pos)
{
	char buf[MAX_DBSTRVALUE_LEN] = "\0";
	string tostring="";
	vector<MetaData> metaDataVector;
	metaDataVector.clear();
	try
	{
		int datatype = 0;
		datatype = ((m_rs->getColumnListMetaData())[pos-1]).getInt(MetaData::ATTR_DATA_TYPE);
		switch(datatype)
		{
			case OCCI_SQLT_CHR:
				tostring = getColumnStringValue(pos);
				break;
			case OCCI_SQLT_NUM:
				tostring = getColumnStringValue(pos);
				break;
			case OCCIINT:
				sprintf(buf, "%d", getColumnIntValue(pos));
				tostring = buf;
				break;
			case OCCIFLOAT:
				sprintf(buf, "%f", getColumnFloatValue(pos));
				tostring = buf;
				break;
			case OCCI_SQLT_STR:
				tostring = getColumnStringValue(pos);
				break;
			case OCCI_SQLT_LNG:
				sprintf(buf, "%ld", getColumnLongValue(pos));
				tostring = buf;
				break;
			case OCCI_SQLT_DAT: 
//				sprintf(buf, "%ld", getColumnLongValue(pos));
//				tostring = buf;
				tostring = getColumnDateValue(pos);
				break;
			case OCCI_SQLT_AFC:
				tostring = getColumnStringValue(pos);	
				break;
			default:
				tostring = getColumnStringValue(pos);
				break;
		}
		
		return tostring;
	}
	catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "���ݿ��ȡ��¼��������ַ�����ʧ��,������Ϣ:=%s", e.getMessage().c_str());
	}
}
//--------------------------��������
void  DbInterface::createStatement()
{
  try
  {
		if (m_stmt)
		{
			if (m_rs)
			{
				m_stmt->closeResultSet(m_rs);
				m_rs = (ResultSet *)NULL;
			}
			m_conn->terminateStatement(m_stmt);
			m_stmt = (Statement *)NULL;
		}
		m_stmt = m_conn->createStatement();
		m_stmt->setAutoCommit(false);   
  }
	catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "ִ��createStatementʧ��,������Ϣ:=%s", e.getMessage().c_str());
	}
}

void  DbInterface::setSql(char* sSQL)
{
  try
  {
        m_stmt->setSQL(sSQL);
  }
  catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "ִ��setSqlʧ��,������Ϣ:=%s", e.getMessage().c_str());
	}
}

void  DbInterface::setMaxIteration(const int &iMaxIteration)
{
	try
  {
     m_stmt->setMaxIterations(iMaxIteration);
  }
  catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "ִ��setMaxIterationʧ��,������Ϣ:=%s", e.getMessage().c_str());
	}     
}

void  DbInterface::addIteration()
{
	try
  {      
      m_stmt->addIteration();
  }
  catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "ִ��addIterationʧ��,������Ϣ:=%s", e.getMessage().c_str());
	}         
}

void DbInterface::setMaxParamSize(const int &riPosition, const int &riSize)
{     
	try
  {     
		   m_stmt->setMaxParamSize(riPosition, riSize);
  }
  catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "ִ��setMaxParamSizeʧ��,������Ϣ:=%s", e.getMessage().c_str());
	}  		   
}
void  DbInterface::setParameter(const int &iPosition, const int & iValue)
{
	try
  {       
      m_stmt->setInt(iPosition, iValue);
  }
  catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "ִ��setParameter(int)ʧ��,������Ϣ:=%s", e.getMessage().c_str());
	} 
}

void  DbInterface::setParameter(const int &iPosition, const char *sValue)
{
	try
  {       
      m_stmt->setString(iPosition, sValue);
  }
  catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "ִ��setParameter(char*)ʧ��,������Ϣ:=%s", e.getMessage().c_str());
	} 
}  

void  DbInterface::setParameter(const int &iPosition, const string &sValue)
{
	try
  {       
     m_stmt->setString(iPosition, sValue);
  }
  catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "ִ��setParameter(string)ʧ��,������Ϣ:=%s", e.getMessage().c_str());
	} 
}  
  
void  DbInterface::setParameter(const int &iPosition, const long &lValue)
{
	try
  {       
       m_stmt->setNumber(iPosition, lValue);
  }
  catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "ִ��setParameter(long)ʧ��,������Ϣ:=%s", e.getMessage().c_str());
	} 
}

void  DbInterface::setParameter(const int &iPosition, const Date &dataValue)
{
  try
  {  
       m_stmt->setDate(iPosition, dataValue);
  }
  catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "ִ��setParameter(Date)ʧ��,������Ϣ:=%s", e.getMessage().c_str());
	}        
}

void  DbInterface::setBatchParameter(vector<BatchDbParam>& dbparams,int& rmaxrow)
{
   // 1.���ñ����δ���ļ�¼�ܻ���
  try
  {
   setMaxIteration( rmaxrow );
   // 2.����ÿ�еĴ�С
   int nColCount = dbparams.size();
   int iPosition = 0;
   int isize ;
   for( int i =0 ; i < nColCount ;++ i )
   {
       iPosition = dbparams[i].m_seq;
       isize     = dbparams[i].size ;
       setMaxParamSize( iPosition ,isize ); 
   }
   // 3.����ÿ������
   char *          p = NULL;
   PROCPARAM_TYPE  type ;
   for ( int i = 0 ; i < rmaxrow ;++ i )
   {
      for( int j = 0 ;j < nColCount;++j )
      {
          iPosition = dbparams[j].m_seq;
          isize     = dbparams[j].size ;
          type      = dbparams[j].m_type;
          if (  PARAM_TYPE_INT  == type )
          {
              setParameter( iPosition ,dbparams[j].m_intValue[i] );
          }else if (  PARAM_TYPE_LONG == type )
          {
              setParameter( iPosition ,dbparams[j].m_longValue[i] );
          }else if (  PARAM_TYPE_FLOAT == type )
          {
              setParameter( iPosition ,dbparams[j].m_floatValue[i] );
          }else if (  PARAM_TYPE_STR == type )
          {
              setParameter( iPosition ,&dbparams[j].m_strValue[ i * isize ] );
          
          }else if ( PARAM_TYPE_DATE == type )
          {
               //��������Ҫ�������⴦��Ҫ��ϵͳ����(���ж��ΰ�)
               //���iYear���ֶ�ȫΪ0�Ļ������򱨴�
               int iYear  = dbparams[j].m_dateValue._Param[i].iYear;
               int iMonth = dbparams[j].m_dateValue._Param[i].iMonth;
               int iDay   = dbparams[j].m_dateValue._Param[i].iDay;
               int iHour  = dbparams[j].m_dateValue._Param[i].iHour;
               int iMin   =  dbparams[j].m_dateValue._Param[i].iMin;
               int iSec   =  dbparams[j].m_dateValue._Param[i].iSec;
               //�������0�Ļ�����Ĭ��
               if (  ( iYear  == 0 ) && 
                     ( iMonth == 0 ) &&
                     ( iDay   == 0 ) &&
                     ( iHour  == 0 ) &&
                     ( iMin   == 0 ) &&
                     ( iSec   == 0 ) 
                  )
                {
                  Date dateValue ;
                  dbparams[j].m_dateValue._date[i] = dateValue ;   
                }
               else 
               {
                     Date dateValue( m_env,iYear,
                                     iMonth,
                                     iDay ,
                                     iHour ,
                                     iMin ,
                                     iSec 
                                   );
                 dbparams[j].m_dateValue._date[i] = dateValue ;   
               }
               setParameter( iPosition ,dbparams[j].m_dateValue._date[ i ] );                                                           
          }
      }
      // 4.�Ƿ���Ҫ���ӵ���
      if (i < rmaxrow - 1 )
      	addIteration(); 
    }
      }
  catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "setBatchParameterʧ��,������Ϣ:=%s", e.getMessage().c_str());
	}  
}


	
/*
void  DbInterface::setParameter(const int &iPosition, const Number &nValue)
{
  try
  {       
       m_stmt->setNumber(iPosition, nValue);     
  }
  catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "ִ��setParameter(Number)ʧ��,������Ϣ:=%s", e.getMessage().c_str());
	}       
}
*/
void  DbInterface::setParameter(const int &iPosition, const double &dValue)
{
  try
  {   
       m_stmt->setDouble(iPosition, dValue);
  }
  catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "ִ��setParameter(double)ʧ��,������Ϣ:=%s", e.getMessage().c_str());
	}       
}
    
void  DbInterface::setParameter(const int &iPosition, const float &fValue)
{
  try
  { 
       m_stmt->setFloat(iPosition, fValue);
  }
  catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "ִ��setParameter(float)ʧ��,������Ϣ:=%s", e.getMessage().c_str());
	}       
}

int  DbInterface::executeUpdate()
{
  int iProcRecord = 0;
  try
  { 
      iProcRecord = m_stmt->executeUpdate();
  }
  catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "ִ��executeUpdateʧ��,������Ϣ:=%s", e.getMessage().c_str());
	}    
	return  iProcRecord;  
}

void  DbInterface::executeUpdate(const string &str)
{
  
  try
  { 
   m_stmt->executeUpdate(str);
  }
  catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "ִ��executeUpdateʧ��,������Ϣ:=%s", e.getMessage().c_str());
	}    
}

void  DbInterface::executeQuery(const int rmaxrecordsize) 
{
   try
   { 
      m_rs     = m_stmt->executeQuery();//ִ�в�ѯ 
      m_stmt->setPrefetchRowCount(rmaxrecordsize);
   }
   catch(SQLException e)
	 {
		throw CException(__FILE__, __LINE__, "ִ��executeQueryʧ��,������Ϣ:=%s", e.getMessage().c_str());
	 }       
}

void  DbInterface::SetResultDataBuffer(const int& iPosition,void* buf ,const Type & coltype ,const unsigned int & size )
{
   try
   {   
    m_rs->setDataBuffer( iPosition ,buf,coltype,size );
   }
   catch(SQLException e)
	 {
		throw CException(__FILE__, __LINE__, "ִ��SetResultDataBufferʧ��,������Ϣ:=%s", e.getMessage().c_str());
	 }       
}


void  DbInterface::SetResultDataBuffer(const int& iPosition,void* buf ,const Type & coltype ,const unsigned int & size,
                                       ub2 *length,sb2 *ind, ub2 *rc )
{
   try
   {   
     m_rs->setDataBuffer( iPosition ,buf,coltype,size,length ,ind ,rc ) ;
   }
   catch(SQLException e)
	 {
		throw CException(__FILE__, __LINE__, "ִ��SetResultDataBufferʧ��,������Ϣ:=%s", e.getMessage().c_str());
	 }       
}

//add xueyf 20101230
void  DbInterface::ResultDataBuffer(const int& iPosition,void* buf ,const Type & coltype ,const unsigned int & size,
                                       ub2 *length,sb2 *ind, ub2 *rc )
{
   try
   {   
     m_stmt->setDataBuffer( iPosition ,buf,coltype,size,length ,ind ,rc ) ;
   }
   catch(SQLException e)
	 {
		throw CException(__FILE__, __LINE__, "ִ��ResultDataBufferʧ��,������Ϣ:=%s", e.getMessage().c_str());
	 }       
}

bool  DbInterface::BatchNext(int& rcount ,const int& rmaxrecordsize ) 
{
	try
	{
	  bool   bsuccess = false ;
		rcount = 0 ;
		bsuccess =  m_rs->next( rmaxrecordsize );
		if (bsuccess) rcount = m_rs->getNumArrayRows();
		return bsuccess ;
	}
	catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "���ݿ��ȡ������¼�����ʧ��,������Ϣ:=%s", e.getMessage().c_str());
	}

}
int   DbInterface::getColCount()
{
  try
  {
    return m_rs->getColumnListMetaData().size();
  }
	catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "��ȡ�е�����ʧ��,������Ϣ:=%s", e.getMessage().c_str());
	}
}

//add xueyf 20101230
int   DbInterface::getColumnType(const unsigned int& pos)
{
  try
  {
    return ((m_rs->getColumnListMetaData())[pos-1]).getInt(MetaData::ATTR_DATA_TYPE);
  }
	catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "��ȡ�е�����ʧ��,������Ϣ:=%s", e.getMessage().c_str());
	}
}


int   DbInterface::getColumnSize(const unsigned int& pos)
{
  try
  {
    return ((m_rs->getColumnListMetaData())[pos-1]).getInt(MetaData::ATTR_DATA_SIZE);
  }
	catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "��ȡ�еĴ�Сʧ��,������Ϣ:=%s", e.getMessage().c_str());
	}
}

//add xueyf 20101230
void DbInterface:: execArrayUpdate(const unsigned int& arry_num)
{
	try
  { 
   m_stmt->executeArrayUpdate(arry_num);
  }
  catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "ִ��executeArrayUpdateʧ��,������Ϣ:=%s", e.getMessage().c_str());
	}
}    

//add xueyf 20101230
void DbInterface::getUpdateNum()
{
	try
  { 
   m_stmt->getUpdateCount();
  }
  catch(SQLException e)
	{
		throw CException(__FILE__, __LINE__, "ִ��getUpdateCountʧ��,������Ϣ:=%s", e.getMessage().c_str());
	}
}
	
//--------------------------------------------end-----------------------------------------------------------------
char* DbInterface::strcasestr(const char *str1,const char *str2)
{
	char *str;
	int i;

	str=(char *)str1;

	while(str1+strlen(str1)>=str+strlen(str2))
	{
		for(i=0;i<strlen(str2);i++)
		{
			if(toupper(str[i])!=toupper(str2[i]))break;
		}
		if(i==strlen(str2))return str;
		str++;
	}
	return 0;
}

