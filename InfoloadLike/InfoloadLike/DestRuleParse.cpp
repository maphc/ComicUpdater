#include "stdafx.h"


DestRuleParse::DestRuleParse()
{
	m_objdbiMap = NULL;
	m_objmmiMap = NULL;
	m_destruleVector = NULL;
}


DestRuleParse::~DestRuleParse()
{
	;
}


void DestRuleParse::init(map<string, DbInterface*> *objdbiMap, map<string, MmInterface*> *objmmiMap, vector< DestRule> *destruleVector)
{
	m_objdbiMap = objdbiMap;			
	m_objmmiMap = objmmiMap;
	m_destruleVector = destruleVector;
}


/*
void DestRuleParse::getallobjdbis(const int &infotype, vector<DbInterface*> &objdbis)
{
	objdbis.clear();
	
	//���е�Ŀ�����������
	vector<string> objdbnameVector;
	objdbnameVector.clear();
	//һ��Ŀ������漰�����е�Ŀ�������
	vector<string> tmpobjdbnameVector;
	tmpobjdbnameVector.clear();
	
	DestRule tmpdestrule;
	tmpdestrule.m_infotype = infotype;
	vector<DestRule>::iterator iter=lower_bound(m_destruleVector->begin(), m_destruleVector->end(), tmpdestrule);

	while(iter!=m_destruleVector->end() && iter->m_infotype==infotype)
	{
		//��ȡ����Ŀ������漰�����е�Ŀ�������
		string objdbnames = iter->m_objdbnames;
		StringUtil::split(objdbnames, ";", tmpobjdbnameVector, false);
	
		//���ز���
		for (vector<string>::iterator itert=tmpobjdbnameVector.begin(); itert!=tmpobjdbnameVector.end(); ++itert)
		{
			vector<string>::iterator iters=objdbnameVector.begin();
			for (; iters!=objdbnameVector.end(); ++iters)
			{
				if (*itert == *iters)
				{
					break;
				}	
			}
			if (iters == objdbnameVector.end())
			{
				objdbnameVector.push_back(*itert);
			}
		}
		
		++iter;
	}

	//�������е�Ŀ����������Ʋ鵽���е�Ŀ���������
	for (vector<string>::iterator iterd=objdbnameVector.begin(); iterd!=objdbnameVector.end(); ++iterd)
	{
		objdbis.push_back((*m_objdbiMap)[*iterd]);
	}
}


void DestRuleParse::getallobjmmis(const int &infotype, vector<MmInterface*> &objmmis)
{
	objmmis.clear();
	
	//���е�Ŀ���ڴ������
	vector<string> objmmnameVector;
	objmmnameVector.clear();
	//һ��Ŀ������漰�����е�Ŀ���ڴ��
	vector<string> tmpobjmmnameVector;
	tmpobjmmnameVector.clear();
	
	DestRule tmpdestrule;
	tmpdestrule.m_infotype = infotype;
	vector<DestRule>::iterator iter=lower_bound(m_destruleVector->begin(), m_destruleVector->end(), tmpdestrule);
	while(iter!=m_destruleVector->end() && iter->m_infotype==infotype)
	{
		//��ȡ����Ŀ������漰�����е�Ŀ���ڴ��
		string objmmnames = iter->m_objmmnames;
		StringUtil::split(objmmnames, ";", tmpobjmmnameVector, false);
		
		//���ز���
		for (vector<string>::iterator itert=tmpobjmmnameVector.begin(); itert!=tmpobjmmnameVector.end(); ++itert)
		{
			vector<string>::iterator iters=objmmnameVector.begin();
			for (; iters!=objmmnameVector.end(); ++iters)
			{
				if (*itert == *iters)
				{
					break;
				}	
			}
			if (iters == objmmnameVector.end())
			{
				objmmnameVector.push_back(*itert);
			}
		}
		
		++iter;
	}
	
	//�������е�Ŀ���ڴ�����Ʋ鵽���е�Ŀ���ڴ����
	for (vector<string>::iterator iterd=objmmnameVector.begin(); iterd!=objmmnameVector.end(); ++iterd)
	{
		objmmis.push_back((*m_objmmiMap)[*iterd]);
	}
}
*/
struct PriorityLesser
{
	bool operator()(DestRule* pkTask, int nPriority) const
	{
		return pkTask->m_infotype >= nPriority;
	}
#ifdef _DEBUG
	bool operator()(int nPriority, DestRule* pkTask) const
	{
		return pkTask->m_infotype >= nPriority;
	}
	bool operator()(const DestRule pkTask1, const DestRule pkTask2) const
	{
		return pkTask1.m_infotype > pkTask2.m_infotype;
	}
#endif // _DEBUG
};
/*ȡĳ��infotype�����Ŀ���Ĵ���ʽ  0 ���������  1 ��������� */
int  DestRuleParse::getobj_db_deal(int& infotype,const int& srcdbnum )
{
  int ret = 0 ;
 // DestRule tmpdestrule;
	//tmpdestrule.m_infotype = infotype;
	//vector< DestRule >::iterator iter = lower_bound( m_destruleVector->begin(), m_destruleVector->end(), tmpdestrule,PriorityLesser());
	//while(iter!=m_destruleVector->end() && iter->m_infotype==infotype)
	//{
	//  //������ͬһ��CRM���Ͽ��
	//	if (iter->m_srcdbnum == srcdbnum)
	//	  { 
	//	     ret = iter->m_objdeal;break;
	//	  }
	//	++iter;		   
	//}
	return ret ;	
}

void DestRuleParse::getDestRule(int& infotype ,int& srcdbnum,DestRule& destrule )
{
  DestRule tmpdestrule;
	tmpdestrule.m_infotype = infotype;
	vector<DestRule>::iterator iter=lower_bound(m_destruleVector->begin(), m_destruleVector->end(), tmpdestrule,PriorityLesser());
	while(iter!=m_destruleVector->end() && iter->m_infotype==infotype)
	{
	  //������ͬһ��CRM���Ͽ��
		if (iter->m_srcdbnum == srcdbnum)
		{ 
		    destrule = *iter; break;//�ҵ�һ�����˳�ѭ��
		}
		++iter;		   
	}
}
void DestRuleParse::getAllDestRule (const int &srcdbnum,vector<DestRule> &destrule)
{
   destrule.clear();
   for ( int i = 0 ; i < m_destruleVector->size() ; ++ i ) 
   {
     if ( (*m_destruleVector)[i].m_srcdbnum == srcdbnum) //ͬһ��CRM��
     {
       destrule.push_back( (*m_destruleVector)[i] );
     }//
   }
}

void DestRuleParse::getobjdbi(const int &infotype, const int &srcdbnum, vector<DbInterface*> &objdbis)
{
	objdbis.clear();
	
	//���е�Ŀ�����������
	vector<string> objdbnameVector;
	objdbnameVector.clear();
	//һ��Ŀ������漰�����е�Ŀ�������
	vector<string> tmpobjdbnameVector;
	tmpobjdbnameVector.clear();
	
	DestRule tmpdestrule;
	tmpdestrule.m_infotype = infotype;
	//vector<DestRule>::iterator iter=lower_bound(m_destruleVector->begin(), m_destruleVector->end(), tmpdestrule,PriorityLesser());
	for(vector<DestRule>::iterator iter=m_destruleVector->begin();iter!=m_destruleVector->end();iter++){
		if(*iter==tmpdestrule){
			if (iter->m_srcdbnum == srcdbnum)
			{
				//��ȡ����Ŀ������漰�����е�Ŀ�������
				string objdbnames = iter->m_objdbnames;
				StringUtil::split(objdbnames, ";", tmpobjdbnameVector, false);

				//���ز���
				for (vector<string>::iterator itert=tmpobjdbnameVector.begin(); itert!=tmpobjdbnameVector.end(); ++itert)
				{
					vector<string>::iterator iters=objdbnameVector.begin();
					for (; iters!=objdbnameVector.end(); ++iters)
					{
						if (*itert == *iters)
						{
							break;
						}	
					}
					if (iters == objdbnameVector.end())
					{
						objdbnameVector.push_back(*itert);
					}
				}
			}
		}
	}
	//while(iter!=m_destruleVector->end() && iter->m_infotype==infotype)
	//{
		//������ͬһ��CRM���Ͽ��
		
	//	++iter;
	//}
	
	//�������е�Ŀ����������Ʋ鵽���е�Ŀ���������
	for (vector<string>::iterator iterd=objdbnameVector.begin(); iterd!=objdbnameVector.end(); ++iterd)
	{	
		if ((*m_objdbiMap).find(*iterd) == (*m_objdbiMap).end())
		{
			throw CException(__FILE__,__LINE__,"����Ϊ:=%s���������ݿ�û�����ã���˲�!", (*iterd).c_str());
		}
		else
		{
			objdbis.push_back((*m_objdbiMap)[*iterd]);
		}
	}
}


void DestRuleParse::getobjmmi(const int &infotype, const int &srcdbnum, vector<MmInterface*> &objmmis)
{
	objmmis.clear();
	
	//���е�Ŀ���ڴ������
	vector<string> objmmnameVector;
	objmmnameVector.clear();
	//һ��Ŀ������漰�����е�Ŀ���ڴ��
	vector<string> tmpobjmmnameVector;
	tmpobjmmnameVector.clear();
	
	DestRule tmpdestrule;
	tmpdestrule.m_infotype = infotype;
	vector<DestRule>::iterator iter=lower_bound(m_destruleVector->begin(), m_destruleVector->end(), tmpdestrule,PriorityLesser());
	while(iter!=m_destruleVector->end() && iter->m_infotype==infotype)
	{
		//������CRM�������ݿ��
		if (iter->m_srcdbnum == srcdbnum)
		{
			//��ȡ����Ŀ������漰�����е�Ŀ���ڴ��
			string objmmnames = iter->m_objmmnames;
			StringUtil::split(objmmnames, ";", tmpobjmmnameVector, false);
			
			//���ز���
			for (vector<string>::iterator itert=tmpobjmmnameVector.begin(); itert!=tmpobjmmnameVector.end(); ++itert)
			{
				vector<string>::iterator iters = objmmnameVector.begin();
				for (; iters!=objmmnameVector.end(); ++iters)
				{
					if (*itert == *iters)
					{
						break;
					}	
				}
				if (iters == objmmnameVector.end())
				{
					objmmnameVector.push_back(*itert);
				}
			}
		}
		++iter;
	}
	
	//�������е�Ŀ���ڴ�����Ʋ鵽���е�Ŀ���ڴ����
	for (vector<string>::iterator iter = objmmnameVector.begin(); iter!=objmmnameVector.end(); ++iter)
	{
		if ((*m_objmmiMap).find(*iter) == (*m_objmmiMap).end())
		{
			throw CException(__FILE__,__LINE__,"����Ϊ:=%s���ڴ����ݿ�û�����ã���˲�!", (*iter).c_str());
		}
		else
		{
			objmmis.push_back((*m_objmmiMap)[*iter]);
		}
	}
}

