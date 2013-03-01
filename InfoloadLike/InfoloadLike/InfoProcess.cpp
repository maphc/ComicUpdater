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
	   	
	   	// ���infotype�µĴ����¼ģʽ wangfc@lianchuang.com 2010.03.30
	   	obj_db_deal = m_destRuleParse->getobj_db_deal( infotype , srcdbnum );
	}
	catch(CException e)
	{
		throw e;
	}

   	RecordProcess* recordProcess = getRecordProcess(infotype);
   	recordProcess->setLoadDatabase(&objdbis, &objmmis, m_dbisPro, m_mmisPro);
   	// ���infotype�µĴ����¼ģʽ wangfc@lianchuang.com 2010.03.30
   	recordProcess->setbatchIntoBillingMode( (obj_db_deal == 1) ) ;
   	//Record* firstrecord = (*info)[0];
   	char dealmethod = ((*((*info)[0]))[2])[0];
   	string id = (*((*info)[0]))[0];
	//�ϲ�����Ϣ
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
    	//(1)��������
 	
    	
     	
    	for (set<DbInterface *>::iterator iter=alldbis.begin(); iter!=alldbis.end(); ++iter)
    	{
    		   //(*iter)->beginTrans();
    	}  	

    	//(2)���ϴ���
 	
    	recordProcess->clearKeySet();
    	//   �Ƿ���ʡ����ģʽ wangfc@lianchuang.com 2010.04.08
    	recordProcess->setProvinceMode( m_dbisPro->size() != 0 ) ;
    	for(int i=0; i<info->size(); ++i)
    	{	
    		Record *record = (*info)[i];
        //add by wangfc 2010.01.06  03��40 ����ͬһ����+Info_Type��Modify_Tag��ͬʱ�������ϵ�BUG��
        //��ʱ�����������ļ�¼��info_type : 401 ��һ������Ϊ,Modify_TagΪ2,��������ΪModify_TagΪ8 
        //����oracle���ݿ�ʱ����!
        dealmethod = ((*((*info)[i]))[2])[0];//ȡ����¼�µ�modify_tag��־��dealmethod ԭ������ȡ�� ((*((*info)[0]))[2])[0];
        //end
    		//�������ʽ��8:������ȫ��
    		if (dealmethod == '8')
    		{
    			recordProcess->clearIT_N(record);
			}
			
    		recordProcess->processRecord(record);
    	}
    	
    	//(3)�ύ����
    	
   	
    	for (set<DbInterface *>::iterator iter=alldbis.begin(); iter!=alldbis.end(); ++iter)
    	{
    		  (*iter)->commitTrans();
    	}
   	
    }
    catch (CException e)	
    {
    	//(4)�ع�����
    		
    	for (set<DbInterface *>::iterator iter=alldbis.begin(); iter!=alldbis.end(); ++iter)
    	{
    		  //(*iter)->rollbackTrans();
    	}
    	//throw e;
    	throw CException(__FILE__, __LINE__, "��ˮ��:=[%s],��Ϣ:=[%s]", id.c_str(), e.GetString());
    }
}


RecordProcess* InfoProcess::getRecordProcess(const int &infotype)
{
	RecordProcess* recordProcess = NULL;
	
	map<int, RecordProcess*>::iterator iter = m_recordProcessMap.find(infotype);
	//��ѯ����ȡ���е�
	if (iter != m_recordProcessMap.end())
	{
		recordProcess = iter->second;
	}
	//��ѯ������������һ��
	else
	{
		recordProcess = new RecordProcess(&m_loadtables[infotype]);
		m_recordProcessMap.insert(map<int, RecordProcess*>::value_type(infotype, recordProcess));
	}
	
	return recordProcess;
}