#include "stdafx.h"


RecordProcess::RecordProcess()
{
	m_loadtable = NULL;
	
	m_objdbis = NULL;
	m_objmmis = NULL;
	m_objdbisPro = NULL;
	m_objmmisPro = NULL;
	m_batchIntoBilling = false;
	m_ifProvinceMode   = false;
	m_keySet.clear();
}

RecordProcess::RecordProcess(LoadTable* loadtable)
{
	m_loadtable = loadtable;
	
	m_objdbis = NULL;
	m_objmmis = NULL;
	m_objdbisPro = NULL;
	m_objmmisPro = NULL;
	
	m_keySet.clear();	
}


RecordProcess::~RecordProcess()
{
	m_keySet.clear();
}


void RecordProcess::setLoadTable(LoadTable* loadtable)
{
	m_loadtable = loadtable;
}


void RecordProcess::setLoadDatabase(vector<DbInterface*>* objdbis, vector<MmInterface*>* objmmis, vector<DbInterface*>* objdbisPro, vector<MmInterface*>* objmmisPro)
{
	m_objdbis = objdbis;
	m_objmmis = objmmis;
	
	m_objdbisPro = objdbisPro;
	m_objmmisPro = objmmisPro;
}


void RecordProcess::processRecord(Record* record)
{
	//(1)��ȡ��������
	int opertype = atoi((*record)[2]);
	
	//(2)���ڲ�����������Ӧ�Ĵ���
	switch(opertype)
	{
		//����
		case 0:
			try
			{
				insert(record);
			}
			catch(CException e)
			{
				throw e;
			}
			break;
		//����
		case 2:
			try
			{
				update(record);
			}
			catch(CException e)
			{
				throw e;
			}
			break;
		//ɾ��
		case 1:
			try
			{
				deleteRec(record);
			}
			catch(CException e)
			{
				throw e;
			}
			break;
		//����(������ȫ��)	
		case 8:
			try
			{
				insert(record);
			}	
			catch(CException e)
			{
				throw e;
			}
			break;	
		//ȱʡ
		default:
			throw CException(__FILE__, __LINE__, "�������������Ͻӿڼ�¼�в������Ͳ���ʶ��,��������Ϊ:=%d", opertype);
			break;
	}

}


void RecordProcess::clearIT(Record* record)
{	
	//(1)��ȡ��Ϣ��Դ�Ļ�׼λ��
	int baselen = INFO_ADDLEN;
	bool isPro  = isProvinceInfo(record);
	
	//(2)ɾ���������ݿ�����,���������ģʽ��ִ�� wangfc@lianchuang.com 2010.04.13
	if ( (m_objdbis->size()!=0) && (!m_batchIntoBilling) )
	{
#ifdef _DEBUG_		   
        printf("������ģʽclearIT\n");
#endif 		  
		int ipos = 0;
		string deletecond = "";
		vector<DbParam> dbParamVector;
		dbParamVector.clear();

		string dbtablename = m_loadtable->m_dbtablename;	
		for (int j=0; j<(m_loadtable->m_dbkeycolumnposVector).size(); ++j)
		{
			int i = (m_loadtable->m_dbkeycolumnposVector)[j];
			int dbkeytag = (m_loadtable->m_dbcolumns[i]).m_keytag;
			string dbcolumnname = (m_loadtable->m_dbcolumns[i]).m_columnname;
			PROCPARAM_TYPE dbcolumntype = (m_loadtable->m_dbcolumns[i]).m_columntype;
			char dbgetvalue = (m_loadtable->m_dbcolumns[i]).m_getvaluetype;
			int dbgetvaluepos = (m_loadtable->m_dbcolumns[i]).m_getvaluepos;			
			string s = (*record)[baselen + (m_loadtable->m_infokeyfieldposVector)[j]];
	   	
			//����׼��
			++ipos;
			char c[1000];
			sprintf(c, "%s=:%d", dbcolumnname.c_str(), ipos);
			string sc = c;
			if (ipos == 1)
			{
				deletecond = deletecond + sc;
			}
			else
			{
				deletecond = deletecond + " and " + sc;
			}
	   		
			//����׼��
			DbParam dbParam;
			dbParam.m_seq = ipos;
			dbParam.m_action = PARAM_ACT_IN;
			dbParam.m_type = dbcolumntype;
			getdbparamvalue(dbcolumntype, dbgetvalue, s, dbParam);
			dbParamVector.push_back(dbParam);
			
		}
		string strSql = "DELETE FROM " + dbtablename + " WHERE " + deletecond;
	   
	    //����м��������Ϣ
	  if (isPro)
	  {
			for (vector<DbInterface*>::iterator iter=m_objdbisPro->begin(); iter!=m_objdbisPro->end(); ++iter)
			{
				(*iter)->execDynamicSql(strSql, dbParamVector);
			}	    	
		}
		//������Ϣ
		else
		{
			for (vector<DbInterface*>::iterator iter=m_objdbis->begin(); iter!=m_objdbis->end(); ++iter)
			{
				(*iter)->execDynamicSql(strSql, dbParamVector);
			}
		}
	}
	

}


void RecordProcess::clearIT_N(Record* record)
{		
	//��ȡ�ؼ���keystring
	string keystring = "";
	int baselen = INFO_ADDLEN;
	for (int j=0; j<(m_loadtable->m_infokeyfieldposVector).size(); ++j)
	{
		keystring += (*record)[baselen + (m_loadtable->m_infokeyfieldposVector)[j]];
	}
			
	//û�в�ѯ����¼,��������
	if (m_keySet.find(keystring) == m_keySet.end())
	{
		clearIT(record);
		m_keySet.insert(keystring);
	}
}


void RecordProcess::clearKeySet()
{
	m_keySet.clear();
}


void RecordProcess::batchInsert(Info* info)
{
	//(1)��ȡ��Ϣ��Դ�Ļ�׼λ��
	int baselen = INFO_ADDLEN;
	bool isPro = false;
	
	//(2)���������������ݿ�
	if(m_objdbis->size() != 0)
	{
		vector<DbParam> dbParamVector;
		dbParamVector.clear();   
   	
		//(2.1)���INSERT��SQL
		string dbtablename = m_loadtable->m_dbtablename;
		int dbcolumnnum = m_loadtable->m_dbcolumnnum;
		string columnnames = "";
		string valuenames = "";
		for (int j=0; j<dbcolumnnum; ++j)
		{
			string dbcolumnname = (m_loadtable->m_dbcolumns[j]).m_columnname;
			PROCPARAM_TYPE dbcolumntype = (m_loadtable->m_dbcolumns[j]).m_columntype;
   		
			//���SQL������Ϣ
			if (j != 0)
			{
				columnnames = columnnames + "," + dbcolumnname;
			}
			else
			{
				columnnames = columnnames + dbcolumnname;
			}
			//���SQL��ֵ��Ϣ
			char c[1000];
			sprintf(c, ":%d", j+1);
			string sc = c;
			if (j != 0)
			{
				valuenames = valuenames + "," + sc;
			}
			else
			{
				valuenames = valuenames + sc;
			}
		}				
		string strSql = "INSERT INTO " + dbtablename + " (" + columnnames + ") VALUES (" + valuenames + ") ";
		
		//(2.2)������
		for (int i=0; i<info->size(); ++i)
		{
			Record *record = (*info)[i];
			isPro = isProvinceInfo(record);
			
			dbParamVector.clear();
			for (int j=0; j<dbcolumnnum; ++j)
			{
				int dbkeytag = (m_loadtable->m_dbcolumns[j]).m_keytag;
				string dbcolumnname = (m_loadtable->m_dbcolumns[j]).m_columnname;
				PROCPARAM_TYPE dbcolumntype = (m_loadtable->m_dbcolumns[j]).m_columntype;
				char dbgetvalue = (m_loadtable->m_dbcolumns[j]).m_getvaluetype;
				int dbgetvaluepos = (m_loadtable->m_dbcolumns[j]).m_getvaluepos;
				string s = (*record)[baselen + dbgetvaluepos];
				   			
				DbParam dbParam;
				dbParam.m_seq = j+1;
				dbParam.m_action = PARAM_ACT_IN;
				dbParam.m_type = dbcolumntype;	
				getdbparamvalue(dbcolumntype, dbgetvalue, s, dbParam);
				dbParamVector.push_back(dbParam);
			}
			
			//(2.3)����ִ��INSERT
			//�����������Ϣ
			if (isPro)
			{
				for (vector<DbInterface*>::iterator iter=m_objdbisPro->begin(); iter!=m_objdbisPro->end(); ++iter)
				{
					(*iter)->execDynamicSql(strSql, dbParamVector);
				}
			}
			//������������Ϣ
			else
			{
				for (vector<DbInterface*>::iterator iter=m_objdbis->begin(); iter!=m_objdbis->end(); ++iter)
				{
					(*iter)->execDynamicSql(strSql, dbParamVector);
				}
			}
		}
	}

	//(3)�����ڴ����
	
	
}


void RecordProcess::deleteRec(Record* record)
{	
	//(1)��ȡ��Ϣ��Դ�Ļ�׼λ��
	int baselen = INFO_ADDLEN;
	bool isPro = isProvinceInfo(record);

	//(2)ɾ���������ݿ����� ,���������ģʽ��ִ�� wangfc@lianchuang.com
	if ( (m_objdbis->size() != 0) && (!m_batchIntoBilling) )
	{
#ifdef _DEBUG_		   
        printf("������ģʽdelete\n");
#endif 		  
		int ipos = 0;
		string deletecond = "";
		vector<DbParam> dbParamVector;
		dbParamVector.clear();

		string dbtablename = m_loadtable->m_dbtablename;
		int dbcolumnnum = m_loadtable->m_dbcolumnnum;     
		for (int i=0; i<dbcolumnnum; ++i)
		{	   	
			int dbkeytag = (m_loadtable->m_dbcolumns[i]).m_keytag;
			string dbcolumnname = (m_loadtable->m_dbcolumns[i]).m_columnname;
			PROCPARAM_TYPE dbcolumntype = (m_loadtable->m_dbcolumns[i]).m_columntype;
			char dbgetvalue = (m_loadtable->m_dbcolumns[i]).m_getvaluetype;
			int dbgetvaluepos = (m_loadtable->m_dbcolumns[i]).m_getvaluepos;			
			string s = (*record)[baselen + dbgetvaluepos];
	   	
			if (dbkeytag == 1)
			{
				//����׼��
				++ipos;
				char c[1000];
				sprintf(c, "%s=:%d", dbcolumnname.c_str(), ipos);
				string sc = c;
				if (ipos == 1)
				{
					deletecond = deletecond + sc;
				}
				else
				{
					deletecond = deletecond + " and " + sc;
				}
	   		
				//����׼��
				DbParam dbParam;
				dbParam.m_seq = ipos;
				dbParam.m_action = PARAM_ACT_IN;
				dbParam.m_type = dbcolumntype;
				getdbparamvalue(dbcolumntype, dbgetvalue, s, dbParam);
				dbParamVector.push_back(dbParam);
			}		
		}
		string strSql = "DELETE FROM " + dbtablename + " WHERE " + deletecond;
	   
	    if (isPro)
	    {
	    	for (vector<DbInterface*>::iterator iter=m_objdbisPro->begin(); iter!=m_objdbisPro->end(); ++iter)
			{
				(*iter)->execDynamicSql(strSql, dbParamVector);
			}
		}
		else
		{
			for (vector<DbInterface*>::iterator iter=m_objdbis->begin(); iter!=m_objdbis->end(); ++iter)
			{
				(*iter)->execDynamicSql(strSql, dbParamVector);
			}
		}
	}

	
	
}


void RecordProcess::insert(Record* record)
{	
	//(1)��ȡ��Ϣ��Դ�Ļ�׼λ��
	int baselen = INFO_ADDLEN;
	bool isPro = isProvinceInfo(record);
	//(2)�����������ݿ� ,���������ģʽ��ִ�� wangfc@lianchuang.com
	if ( (m_objdbis->size() != 0) && (!m_batchIntoBilling) )
	{  
#ifdef _DEBUG_		   
        printf("������ģʽInsert\n");
#endif 		  
		string columnnames = "";
		string valuenames = "";
		vector<DbParam> dbParamVector;
		dbParamVector.clear();

		string dbtablename = m_loadtable->m_dbtablename;
		int dbcolumnnum = m_loadtable->m_dbcolumnnum;
		for (int j=0; j<dbcolumnnum; ++j)
		{
			int dbkeytag = (m_loadtable->m_dbcolumns[j]).m_keytag;
			string dbcolumnname = (m_loadtable->m_dbcolumns[j]).m_columnname;
			PROCPARAM_TYPE dbcolumntype = (m_loadtable->m_dbcolumns[j]).m_columntype;
			char dbgetvalue = (m_loadtable->m_dbcolumns[j]).m_getvaluetype;
			int dbgetvaluepos = (m_loadtable->m_dbcolumns[j]).m_getvaluepos;
			string s = (*record)[baselen + dbgetvaluepos];
	   	
			//(2.1)������
			DbParam dbParam;
			dbParam.m_seq = j+1;
			dbParam.m_action = PARAM_ACT_IN;
			dbParam.m_type = dbcolumntype;		
			getdbparamvalue(dbcolumntype, dbgetvalue, s, dbParam);
			dbParamVector.push_back(dbParam);
				   	
			//(2.2)���SQL
			//���SQL������Ϣ
			if (j != 0)
			{
				columnnames = columnnames + "," + dbcolumnname;
			}
			else
			{
				columnnames = columnnames + dbcolumnname;
			}
			//���SQL��ֵ��Ϣ
			char c[1000];
			sprintf(c, ":%d", j+1);
			
			string sc = c;
			if (j != 0)
			{
				valuenames = valuenames + "," + sc;
			}
			else
			{
				valuenames = valuenames + sc;
			}
		}
		string strSql = "INSERT INTO " + dbtablename + " (" + columnnames + ") VALUES ( " + valuenames + ") ";

		//(2.3)ִ��INSERT
		if (isPro)
		{
			for (vector<DbInterface*>::iterator iter=m_objdbisPro->begin(); iter!=m_objdbisPro->end(); ++iter)
			{
//				//���������,�����¼
//				if ( !isExist(0, record, *iter, NULL) )
//				{
					(*iter)->execDynamicSql(strSql, dbParamVector);
//				}
			}
		}
		else
		{
			for (vector<DbInterface*>::iterator iter=m_objdbis->begin(); iter!=m_objdbis->end(); ++iter)
			{
//				//���������,�����¼
//				if ( !isExist(0, record, *iter, NULL) )
//				{
					(*iter)->execDynamicSql(strSql, dbParamVector);
//				}
			}
		}
	}

	//(3)�����ڴ����
	
}


void RecordProcess::update(Record* record)
{
	//(1)��ȡ��Ϣ��Դ�Ļ�׼λ��
	int baselen = INFO_ADDLEN;
	bool isPro = isProvinceInfo(record);
	
	//(2)�����������ݿ�,���������ģʽ��ִ�� wangfc@lianchuang.com
	if( (m_objdbis->size() != 0) && (!m_batchIntoBilling))
	{
#ifdef _DEBUG_		   
        printf("������ģʽupdate\n");
#endif 	  
		//(2.1)׼��������SQL  
		int icontent = 0;
		int icond = 0;
		string updatecontent = "";
		string updatecond = "";
		vector<DbParam> dbParamVector;
		dbParamVector.clear();

		string dbtablename = m_loadtable->m_dbtablename;
		int dbcolumnnum = m_loadtable->m_dbcolumnnum;
		
		int icount=0;	   
		for (int i=0; i<dbcolumnnum; ++i)
		{
			int dbkeytag = (m_loadtable->m_dbcolumns[i]).m_keytag;
			string dbcolumnname = (m_loadtable->m_dbcolumns[i]).m_columnname;
			PROCPARAM_TYPE dbcolumntype = (m_loadtable->m_dbcolumns[i]).m_columntype;
			char dbgetvalue = (m_loadtable->m_dbcolumns[i]).m_getvaluetype;
			int dbgetvaluepos = (m_loadtable->m_dbcolumns[i]).m_getvaluepos;
			string s = (*record)[baselen + dbgetvaluepos];
	   	
	   		if (dbkeytag != 1)
	   		{
	   			++icount;
				//����׼��
				DbParam dbParam;
				dbParam.m_seq = icount;
				dbParam.m_action = PARAM_ACT_IN;
				dbParam.m_type = dbcolumntype;
				getdbparamvalue(dbcolumntype, dbgetvalue, s, dbParam);
				dbParamVector.push_back(dbParam);

				//����������׼��
				char c[1000];	
				sprintf(c, "%s=:%d", dbcolumnname.c_str(), icount);
	     	
				string sc = c;

	 			//����׼��
				if (icount == 1)
				{
					updatecontent = updatecontent + sc;
				}
				else
				{
					updatecontent = updatecontent + " , " + sc;
				}    
			}
		}

		for (int i=0; i<dbcolumnnum; ++i)
		{
			int dbkeytag = (m_loadtable->m_dbcolumns[i]).m_keytag;
			string dbcolumnname = (m_loadtable->m_dbcolumns[i]).m_columnname;
			PROCPARAM_TYPE dbcolumntype = (m_loadtable->m_dbcolumns[i]).m_columntype;
			char dbgetvalue = (m_loadtable->m_dbcolumns[i]).m_getvaluetype;
			int dbgetvaluepos = (m_loadtable->m_dbcolumns[i]).m_getvaluepos;
			string s = (*record)[baselen + dbgetvaluepos];		
		
			//�ؼ����ж�
			if (dbkeytag == 1)
			{
	   			++icount;
				//����׼��
				DbParam dbParam;
				dbParam.m_seq = icount;
				dbParam.m_action = PARAM_ACT_IN;
				dbParam.m_type = dbcolumntype;
				getdbparamvalue(dbcolumntype, dbgetvalue, s, dbParam);
				dbParamVector.push_back(dbParam);

				//����������׼��
				char c[1000];	
				sprintf(c, "%s=:%d", dbcolumnname.c_str(), icount);
	     	
				string sc = c;
				
				//����׼��
				++icond;
				if (icond == 1)
				{
					updatecond = updatecond + sc;			
				}
				else
				{
					updatecond = updatecond + " and " + sc;				
				}
			}
		}		
		string strSql = "UPDATE " + dbtablename + " SET " + updatecontent + " WHERE " + updatecond;	
	   
		//(2.2)ִ��UPDATE����
		if (isPro)
		{
			for (vector<DbInterface*>::iterator iter=m_objdbisPro->begin(); iter!=m_objdbisPro->end(); ++iter)
			{
				(*iter)->execDynamicSql(strSql, dbParamVector);
			}
		}
		else
		{
			for (vector<DbInterface*>::iterator iter=m_objdbis->begin(); iter!=m_objdbis->end(); ++iter)
			{
				(*iter)->execDynamicSql(strSql, dbParamVector);
			}
		}
	}

	//(3)�����ڴ����ݿ�
	
}


bool RecordProcess::isExist(const int &dbtype, Record* record, DbInterface* dbi, MmInterface* mmi)
{
	//(1)��ȡ��Ϣ��Դ�Ļ�׼λ��
	int baselen = INFO_ADDLEN;	
	//(2)�������ݿ����ݼ�¼��ѯ 
	if (dbtype == 0) 
	{  
		int ipos = 0;
		string selectcond = "";
		vector<DbParam> dbParamVector;
		dbParamVector.clear();
		
		string dbtablename = m_loadtable->m_dbtablename;
		int dbcolumnnum = m_loadtable->m_dbcolumnnum;
		for (int i=0; i<dbcolumnnum; ++i)
		{
			int dbkeytag = (m_loadtable->m_dbcolumns[i]).m_keytag;
 			string dbcolumnname = (m_loadtable->m_dbcolumns[i]).m_columnname;
			PROCPARAM_TYPE dbcolumntype = (m_loadtable->m_dbcolumns[i]).m_columntype;
			char dbgetvalue = (m_loadtable->m_dbcolumns[i]).m_getvaluetype;
      		int dbgetvaluepos = (m_loadtable->m_dbcolumns[i]).m_getvaluepos;
      		string s = (*record)[baselen + dbgetvaluepos];
      		
			if (dbkeytag == 1)
			{
				//����׼��
				++ipos;
				char c[1000];
				sprintf(c, "%s=:%d", dbcolumnname.c_str(), ipos);
				string sc = c;
				if (ipos == 1)
				{
					selectcond = selectcond + sc;
				}
				else
				{
					selectcond = selectcond + " and " + sc;
				}
				
				//����׼��
				DbParam dbParam;
				dbParam.m_seq = ipos;
				dbParam.m_action = PARAM_ACT_IN;
				dbParam.m_type = dbcolumntype;
				getdbparamvalue(dbcolumntype, dbgetvalue, s, dbParam);
				dbParamVector.push_back(dbParam);
			}		
		}
		string strSql = "SELECT 1 FROM " + dbtablename + " WHERE " + selectcond;	
		
		dbi->execDynamicSql(strSql, dbParamVector);
		if (dbi->getNext())
		{
			return true;
		}
	}
	//(3)�ڴ����ݿ����ݼ�¼��ѯ
	
	
	return false;
}


void RecordProcess::getdbparamvalue(const PROCPARAM_TYPE &dbcolumntype, const char &dbgetvalue, const string &dbcolumnvalue, DbParam &dbparam)
{
	switch(dbcolumntype)
	{
		case PARAM_TYPE_INT:
			if (dbgetvalue == '1')
				dbparam.m_intValue = atol(dbcolumnvalue.c_str())%10000;
			else
				dbparam.m_intValue = atoi(dbcolumnvalue.c_str());
			break;

		case PARAM_TYPE_LONG:
			if (dbgetvalue == '1')
				dbparam.m_longValue = atol(dbcolumnvalue.c_str())%10000;
			else
				dbparam.m_longValue = atol(dbcolumnvalue.c_str());
			break;

		case PARAM_TYPE_FLOAT:
			dbparam.m_floatValue = atof(dbcolumnvalue.c_str());
			break;

		case PARAM_TYPE_STR:
			strcpy(dbparam.m_strValue, dbcolumnvalue.c_str());
			break;

		case PARAM_TYPE_DATE:
			strcpy(dbparam.m_dateValue, dbcolumnvalue.c_str());
			break;

		default:
			throw CException(__FILE__, __LINE__, "����SQL�����õ������ֶ����Ͳ���ʶ��,�ֶ�����Ϊ:=%d", dbcolumntype);
			break;
	}
}



bool RecordProcess::isProvinceInfo(Record* record)
{
  if ( !m_ifProvinceMode ) return false;
	int infotype = atoi((*record)[1]);
	switch(infotype)
	{
		//�ͻ�
		case 100:
			//�ж�CUST_CLASS_TYPE,���ſͻ����������0��ʡ����1���м���2�����ؼ�
			if (strlen((*record)[INFO_ADDLEN+23])>0 && (*record)[INFO_ADDLEN+23][0]=='0')
			{
				return true;
			}
			break;
		//���ſͻ�
		case 102:
			//�ж�CUST_CLASS_TYPE,���ſͻ����������0��ʡ����1���м���2�����ؼ�
			if (strlen((*record)[INFO_ADDLEN+5])>0 && (*record)[INFO_ADDLEN+5][0]=='0')
			{
				return true;
			}
			break;
		//�˻�
		case 200:
		//�û�
		case 300:
		//�û�_��Ա
		case 301:
		//�û�_��Ҫ
		case 302:
		//�û�_����
		case 307:
			//��99��ͷ���˻����û���ʶ���漰����е��˻����û�
			if (strncmp((*record)[INFO_ADDLEN], "99" ,2) == 0)
			{
				return true;
			}
			break;
		//�û�_����
		case 304:
		//�û�_ ����״̬
		case 306:
		//�˻���������
		case 203:
			//��99��ͷ���˻����û���ʶ���漰����е��˻����û�
			if (strncmp((*record)[INFO_ADDLEN+1], "99" ,2) == 0)
			{
				return true;
			}
			break;
		//�ʷ�ʵ��
		case 401:
			//��99��ͷ���˻����û���ʶ���漰����е��˻����û�
			if (strncmp((*record)[INFO_ADDLEN+2], "99" ,2)==0 && ((*record)[INFO_ADDLEN+1][0]=='0'||(*record)[INFO_ADDLEN+1][0]=='1'))
			{
				return true;
			}
			break;
		//���ѹ�ϵ
		case 313:
			//��99��ͷ���˻����û���ʶ���漰����е��˻����û�
			if (strncmp((*record)[INFO_ADDLEN+1], "99", 2)==0 || strncmp((*record)[INFO_ADDLEN+9], "99", 2)==0)
			{
				return true;
			}
			break;
		//���д��۱�
		case 319:
			//��99��ͷ���˻����û���ʶ���漰����е��˻����û�
			if (strncmp((*record)[INFO_ADDLEN+3], "99", 2)==0 || strncmp((*record)[INFO_ADDLEN+4], "99", 2)==0)
			{
				return true;
			}
			break;
		default:
			break;
	}
	return false;
}

