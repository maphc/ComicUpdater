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
	//(1)获取操作类型
	int opertype = atoi((*record)[2]);
	
	//(2)基于操作类型做相应的处理
	switch(opertype)
	{
		//增加
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
		//更新
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
		//删除
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
		//特殊(增量的全量)	
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
		//缺省
		default:
			throw CException(__FILE__, __LINE__, "增量的增量资料接口记录中操作类型不可识别,操作类型为:=%d", opertype);
			break;
	}

}


void RecordProcess::clearIT(Record* record)
{	
	//(1)获取信息来源的基准位置
	int baselen = INFO_ADDLEN;
	bool isPro  = isProvinceInfo(record);
	
	//(2)删除物理数据库数据,如果非批量模式才执行 wangfc@lianchuang.com 2010.04.13
	if ( (m_objdbis->size()!=0) && (!m_batchIntoBilling) )
	{
#ifdef _DEBUG_		   
        printf("非批量模式clearIT\n");
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
	   	
			//条件准备
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
	   		
			//参数准备
			DbParam dbParam;
			dbParam.m_seq = ipos;
			dbParam.m_action = PARAM_ACT_IN;
			dbParam.m_type = dbcolumntype;
			getdbparamvalue(dbcolumntype, dbgetvalue, s, dbParam);
			dbParamVector.push_back(dbParam);
			
		}
		string strSql = "DELETE FROM " + dbtablename + " WHERE " + deletecond;
	   
	    //跨地市集团相关信息
	  if (isPro)
	  {
			for (vector<DbInterface*>::iterator iter=m_objdbisPro->begin(); iter!=m_objdbisPro->end(); ++iter)
			{
				(*iter)->execDynamicSql(strSql, dbParamVector);
			}	    	
		}
		//本地信息
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
	//获取关键字keystring
	string keystring = "";
	int baselen = INFO_ADDLEN;
	for (int j=0; j<(m_loadtable->m_infokeyfieldposVector).size(); ++j)
	{
		keystring += (*record)[baselen + (m_loadtable->m_infokeyfieldposVector)[j]];
	}
			
	//没有查询到记录,进行清理
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
	//(1)获取信息来源的基准位置
	int baselen = INFO_ADDLEN;
	bool isPro = false;
	
	//(2)批量插入物理数据库
	if(m_objdbis->size() != 0)
	{
		vector<DbParam> dbParamVector;
		dbParamVector.clear();   
   	
		//(2.1)求解INSERT的SQL
		string dbtablename = m_loadtable->m_dbtablename;
		int dbcolumnnum = m_loadtable->m_dbcolumnnum;
		string columnnames = "";
		string valuenames = "";
		for (int j=0; j<dbcolumnnum; ++j)
		{
			string dbcolumnname = (m_loadtable->m_dbcolumns[j]).m_columnname;
			PROCPARAM_TYPE dbcolumntype = (m_loadtable->m_dbcolumns[j]).m_columntype;
   		
			//求解SQL中列信息
			if (j != 0)
			{
				columnnames = columnnames + "," + dbcolumnname;
			}
			else
			{
				columnnames = columnnames + dbcolumnname;
			}
			//求解SQL中值信息
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
		
		//(2.2)求解参数
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
			
			//(2.3)批量执行INSERT
			//跨地市数据信息
			if (isPro)
			{
				for (vector<DbInterface*>::iterator iter=m_objdbisPro->begin(); iter!=m_objdbisPro->end(); ++iter)
				{
					(*iter)->execDynamicSql(strSql, dbParamVector);
				}
			}
			//本地市数据信息
			else
			{
				for (vector<DbInterface*>::iterator iter=m_objdbis->begin(); iter!=m_objdbis->end(); ++iter)
				{
					(*iter)->execDynamicSql(strSql, dbParamVector);
				}
			}
		}
	}

	//(3)插入内存管理
	
	
}


void RecordProcess::deleteRec(Record* record)
{	
	//(1)获取信息来源的基准位置
	int baselen = INFO_ADDLEN;
	bool isPro = isProvinceInfo(record);

	//(2)删除物理数据库数据 ,如果非批量模式才执行 wangfc@lianchuang.com
	if ( (m_objdbis->size() != 0) && (!m_batchIntoBilling) )
	{
#ifdef _DEBUG_		   
        printf("非批量模式delete\n");
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
				//条件准备
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
	   		
				//参数准备
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
	//(1)获取信息来源的基准位置
	int baselen = INFO_ADDLEN;
	bool isPro = isProvinceInfo(record);
	//(2)插入物理数据库 ,如果非批量模式才执行 wangfc@lianchuang.com
	if ( (m_objdbis->size() != 0) && (!m_batchIntoBilling) )
	{  
#ifdef _DEBUG_		   
        printf("非批量模式Insert\n");
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
	   	
			//(2.1)求解参数
			DbParam dbParam;
			dbParam.m_seq = j+1;
			dbParam.m_action = PARAM_ACT_IN;
			dbParam.m_type = dbcolumntype;		
			getdbparamvalue(dbcolumntype, dbgetvalue, s, dbParam);
			dbParamVector.push_back(dbParam);
				   	
			//(2.2)求解SQL
			//求解SQL中列信息
			if (j != 0)
			{
				columnnames = columnnames + "," + dbcolumnname;
			}
			else
			{
				columnnames = columnnames + dbcolumnname;
			}
			//求解SQL中值信息
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

		//(2.3)执行INSERT
		if (isPro)
		{
			for (vector<DbInterface*>::iterator iter=m_objdbisPro->begin(); iter!=m_objdbisPro->end(); ++iter)
			{
//				//如果不存在,插入记录
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
//				//如果不存在,插入记录
//				if ( !isExist(0, record, *iter, NULL) )
//				{
					(*iter)->execDynamicSql(strSql, dbParamVector);
//				}
			}
		}
	}

	//(3)插入内存管理
	
}


void RecordProcess::update(Record* record)
{
	//(1)获取信息来源的基准位置
	int baselen = INFO_ADDLEN;
	bool isPro = isProvinceInfo(record);
	
	//(2)更新物理数据库,如果非批量模式才执行 wangfc@lianchuang.com
	if( (m_objdbis->size() != 0) && (!m_batchIntoBilling))
	{
#ifdef _DEBUG_		   
        printf("非批量模式update\n");
#endif 	  
		//(2.1)准备参数与SQL  
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
				//参数准备
				DbParam dbParam;
				dbParam.m_seq = icount;
				dbParam.m_action = PARAM_ACT_IN;
				dbParam.m_type = dbcolumntype;
				getdbparamvalue(dbcolumntype, dbgetvalue, s, dbParam);
				dbParamVector.push_back(dbParam);

				//条件、更新准备
				char c[1000];	
				sprintf(c, "%s=:%d", dbcolumnname.c_str(), icount);
	     	
				string sc = c;

	 			//更新准备
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
		
			//关键字判断
			if (dbkeytag == 1)
			{
	   			++icount;
				//参数准备
				DbParam dbParam;
				dbParam.m_seq = icount;
				dbParam.m_action = PARAM_ACT_IN;
				dbParam.m_type = dbcolumntype;
				getdbparamvalue(dbcolumntype, dbgetvalue, s, dbParam);
				dbParamVector.push_back(dbParam);

				//条件、更新准备
				char c[1000];	
				sprintf(c, "%s=:%d", dbcolumnname.c_str(), icount);
	     	
				string sc = c;
				
				//条件准备
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
	   
		//(2.2)执行UPDATE更新
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

	//(3)更新内存数据库
	
}


bool RecordProcess::isExist(const int &dbtype, Record* record, DbInterface* dbi, MmInterface* mmi)
{
	//(1)获取信息来源的基准位置
	int baselen = INFO_ADDLEN;	
	//(2)物理数据库数据记录查询 
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
				//条件准备
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
				
				//参数准备
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
	//(3)内存数据库数据记录查询
	
	
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
			throw CException(__FILE__, __LINE__, "物理SQL参数用到的列字段类型不可识别,字段类型为:=%d", dbcolumntype);
			break;
	}
}



bool RecordProcess::isProvinceInfo(Record* record)
{
  if ( !m_ifProvinceMode ) return false;
	int infotype = atoi((*record)[1]);
	switch(infotype)
	{
		//客户
		case 100:
			//判断CUST_CLASS_TYPE,集团客户级别归属：0－省级、1－市级、2－区县级
			if (strlen((*record)[INFO_ADDLEN+23])>0 && (*record)[INFO_ADDLEN+23][0]=='0')
			{
				return true;
			}
			break;
		//集团客户
		case 102:
			//判断CUST_CLASS_TYPE,集团客户级别归属：0－省级、1－市级、2－区县级
			if (strlen((*record)[INFO_ADDLEN+5])>0 && (*record)[INFO_ADDLEN+5][0]=='0')
			{
				return true;
			}
			break;
		//账户
		case 200:
		//用户
		case 300:
		//用户_成员
		case 301:
		//用户_重要
		case 302:
		//用户_其他
		case 307:
			//以99打头的账户、用户标识是涉及跨地市的账户、用户
			if (strncmp((*record)[INFO_ADDLEN], "99" ,2) == 0)
			{
				return true;
			}
			break;
		//用户_服务
		case 304:
		//用户_ 服务状态
		case 306:
		//账户托收银行
		case 203:
			//以99打头的账户、用户标识是涉及跨地市的账户、用户
			if (strncmp((*record)[INFO_ADDLEN+1], "99" ,2) == 0)
			{
				return true;
			}
			break;
		//资费实例
		case 401:
			//以99打头的账户、用户标识是涉及跨地市的账户、用户
			if (strncmp((*record)[INFO_ADDLEN+2], "99" ,2)==0 && ((*record)[INFO_ADDLEN+1][0]=='0'||(*record)[INFO_ADDLEN+1][0]=='1'))
			{
				return true;
			}
			break;
		//付费关系
		case 313:
			//以99打头的账户、用户标识是涉及跨地市的账户、用户
			if (strncmp((*record)[INFO_ADDLEN+1], "99", 2)==0 || strncmp((*record)[INFO_ADDLEN+9], "99", 2)==0)
			{
				return true;
			}
			break;
		//银行代扣表
		case 319:
			//以99打头的账户、用户标识是涉及跨地市的账户、用户
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

