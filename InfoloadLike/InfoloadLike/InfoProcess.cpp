#include "stdafx.h"

InfoProcess::InfoProcess()
{
	m_loadtables = NULL;
	m_destRuleParse = NULL;
	m_recordProcessMap.clear();
}


InfoProcess::~InfoProcess()
{
	m_loadtables = NULL;
	m_destRuleParse = NULL;

	for (map<int, RecordProcess*>::iterator iter=m_recordProcessMap.begin(); iter!=m_recordProcessMap.end(); ++iter)
	{
		delete iter->second;
	}
	m_recordProcessMap.clear();
}		


void InfoProcess::init(LoadTable *loadtables, DestRuleParse *destRuleParse, vector<DbInterface*> *dbisPro, vector<MmInterface*> *mmisPro)
{
	m_loadtables = loadtables;
	m_destRuleParse = destRuleParse;
	
	m_dbisPro = dbisPro;
	m_mmisPro = mmisPro;
}


void InfoProcess::processTrade(string &chgid, int &infotype, const int &srcdbnum, Info *info)
{
   	vector<DbInterface*>	objdbis;
   	vector<MmInterface*>	objmmis;
   	int obj_db_deal = 0 ;

   	try
   	{
	   	m_destRuleParse->getobjdbi(infotype, srcdbnum, objdbis);
	   	
	   	// 获得infotype下的处理记录模式 wangfc@lianchuang.com 2010.03.30
	   	obj_db_deal = m_destRuleParse->getobj_db_deal( infotype , srcdbnum );
	}
	catch(CException e)
	{
		throw e;
	}

   	RecordProcess* recordProcess = getRecordProcess(infotype);
   	recordProcess->setLoadDatabase(&objdbis, &objmmis, m_dbisPro, m_mmisPro);
   	// 获得infotype下的处理记录模式 wangfc@lianchuang.com 2010.03.30
   	recordProcess->setbatchIntoBillingMode( (obj_db_deal == 1) ) ;
   	//Record* firstrecord = (*info)[0];
   	char dealmethod = ((*((*info)[0]))[2])[0];
   	string id = (*((*info)[0]))[0];
	//合并库信息
	set<DbInterface *> alldbis;

	alldbis.clear();
	
	if (objdbis.size() > 0)
	{
		for (vector<DbInterface*>::iterator iter=objdbis.begin(); iter!=objdbis.end(); ++iter)
		{
			alldbis.insert(*iter);
		}
		for (vector<DbInterface*>::iterator iter=(*m_dbisPro).begin(); iter!=(*m_dbisPro).end(); ++iter)
		{
			alldbis.insert(*iter);
		}
	}
			
    try
    {
    	//(1)开启事务
 	
    	
     	
    	for (set<DbInterface *>::iterator iter=alldbis.begin(); iter!=alldbis.end(); ++iter)
    	{
    		   //(*iter)->beginTrans();
    	}  	

    	//(2)资料处理
 	
    	recordProcess->clearKeySet();
    	//   是否是省中心模式 wangfc@lianchuang.com 2010.04.08
    	recordProcess->setProvinceMode( m_dbisPro->size() != 0 ) ;
    	for(int i=0; i<info->size(); ++i)
    	{	
    		Record *record = (*info)[i];
        //add by wangfc 2010.01.06  03：40 修正同一序列+Info_Type下Modify_Tag不同时程序处理上的BUG，
        //当时内蒙有这样的记录：info_type : 401 第一条数据为,Modify_Tag为2,其他八条为Modify_Tag为8 
        //插入oracle数据库时报错!
        dealmethod = ((*((*info)[i]))[2])[0];//取本记录下的modify_tag标志，dealmethod 原来的是取自 ((*((*info)[0]))[2])[0];
        //end
    		//如果处理方式是8:增量的全量
    		if (dealmethod == '8')
    		{
    			recordProcess->clearIT_N(record);
			}
			
    		recordProcess->processRecord(record);
    	}
    	
    	//(3)提交事务
    	
   	
    	for (set<DbInterface *>::iterator iter=alldbis.begin(); iter!=alldbis.end(); ++iter)
    	{
    		  (*iter)->commitTrans();
    	}
   	
    }
    catch (CException e)	
    {
    	//(4)回滚事务
    		
    	for (set<DbInterface *>::iterator iter=alldbis.begin(); iter!=alldbis.end(); ++iter)
    	{
    		  //(*iter)->rollbackTrans();
    	}
    	//throw e;
    	throw CException(__FILE__, __LINE__, "流水号:=[%s],信息:=[%s]", id.c_str(), e.GetString());
    }
}


RecordProcess* InfoProcess::getRecordProcess(const int &infotype)
{
	RecordProcess* recordProcess = NULL;
	
	map<int, RecordProcess*>::iterator iter = m_recordProcessMap.find(infotype);
	//查询到，取已有的
	if (iter != m_recordProcessMap.end())
	{
		recordProcess = iter->second;
	}
	//查询不到，新生成一个
	else
	{
		recordProcess = new RecordProcess(&m_loadtables[infotype]);
		m_recordProcessMap.insert(map<int, RecordProcess*>::value_type(infotype, recordProcess));
	}
	
	return recordProcess;
}