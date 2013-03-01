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
	
	//所有的目标物理库名称
	vector<string> objdbnameVector;
	objdbnameVector.clear();
	//一个目标规则涉及的所有的目标物理库
	vector<string> tmpobjdbnameVector;
	tmpobjdbnameVector.clear();
	
	DestRule tmpdestrule;
	tmpdestrule.m_infotype = infotype;
	vector<DestRule>::iterator iter=lower_bound(m_destruleVector->begin(), m_destruleVector->end(), tmpdestrule);

	while(iter!=m_destruleVector->end() && iter->m_infotype==infotype)
	{
		//获取这条目标规则涉及的所有的目标物理库
		string objdbnames = iter->m_objdbnames;
		StringUtil::split(objdbnames, ";", tmpobjdbnameVector, false);
	
		//排重插入
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

	//根据所有的目标物理库名称查到所有的目标物理库类
	for (vector<string>::iterator iterd=objdbnameVector.begin(); iterd!=objdbnameVector.end(); ++iterd)
	{
		objdbis.push_back((*m_objdbiMap)[*iterd]);
	}
}


void DestRuleParse::getallobjmmis(const int &infotype, vector<MmInterface*> &objmmis)
{
	objmmis.clear();
	
	//所有的目标内存库名称
	vector<string> objmmnameVector;
	objmmnameVector.clear();
	//一个目标规则涉及的所有的目标内存库
	vector<string> tmpobjmmnameVector;
	tmpobjmmnameVector.clear();
	
	DestRule tmpdestrule;
	tmpdestrule.m_infotype = infotype;
	vector<DestRule>::iterator iter=lower_bound(m_destruleVector->begin(), m_destruleVector->end(), tmpdestrule);
	while(iter!=m_destruleVector->end() && iter->m_infotype==infotype)
	{
		//获取这条目标规则涉及的所有的目标内存库
		string objmmnames = iter->m_objmmnames;
		StringUtil::split(objmmnames, ";", tmpobjmmnameVector, false);
		
		//排重插入
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
	
	//根据所有的目标内存库名称查到所有的目标内存库类
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
/*取某个infotype下入库目标库的处理方式  0 ：正常入库  1 ：批量入库 */
int  DestRuleParse::getobj_db_deal(int& infotype,const int& srcdbnum )
{
  int ret = 0 ;
 // DestRule tmpdestrule;
	//tmpdestrule.m_infotype = infotype;
	//vector< DestRule >::iterator iter = lower_bound( m_destruleVector->begin(), m_destruleVector->end(), tmpdestrule,PriorityLesser());
	//while(iter!=m_destruleVector->end() && iter->m_infotype==infotype)
	//{
	//  //并且是同一个CRM资料库的
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
	  //并且是同一个CRM资料库的
		if (iter->m_srcdbnum == srcdbnum)
		{ 
		    destrule = *iter; break;//找到一个就退出循环
		}
		++iter;		   
	}
}
void DestRuleParse::getAllDestRule (const int &srcdbnum,vector<DestRule> &destrule)
{
   destrule.clear();
   for ( int i = 0 ; i < m_destruleVector->size() ; ++ i ) 
   {
     if ( (*m_destruleVector)[i].m_srcdbnum == srcdbnum) //同一个CRM库
     {
       destrule.push_back( (*m_destruleVector)[i] );
     }//
   }
}

void DestRuleParse::getobjdbi(const int &infotype, const int &srcdbnum, vector<DbInterface*> &objdbis)
{
	objdbis.clear();
	
	//所有的目标物理库名称
	vector<string> objdbnameVector;
	objdbnameVector.clear();
	//一个目标规则涉及的所有的目标物理库
	vector<string> tmpobjdbnameVector;
	tmpobjdbnameVector.clear();
	
	DestRule tmpdestrule;
	tmpdestrule.m_infotype = infotype;
	//vector<DestRule>::iterator iter=lower_bound(m_destruleVector->begin(), m_destruleVector->end(), tmpdestrule,PriorityLesser());
	for(vector<DestRule>::iterator iter=m_destruleVector->begin();iter!=m_destruleVector->end();iter++){
		if(*iter==tmpdestrule){
			if (iter->m_srcdbnum == srcdbnum)
			{
				//获取这条目标规则涉及的所有的目标物理库
				string objdbnames = iter->m_objdbnames;
				StringUtil::split(objdbnames, ";", tmpobjdbnameVector, false);

				//排重插入
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
		//并且是同一个CRM资料库的
		
	//	++iter;
	//}
	
	//根据所有的目标物理库名称查到所有的目标物理库类
	for (vector<string>::iterator iterd=objdbnameVector.begin(); iterd!=objdbnameVector.end(); ++iterd)
	{	
		if ((*m_objdbiMap).find(*iterd) == (*m_objdbiMap).end())
		{
			throw CException(__FILE__,__LINE__,"名称为:=%s的物理数据库没有配置，请核查!", (*iterd).c_str());
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
	
	//所有的目标内存库名称
	vector<string> objmmnameVector;
	objmmnameVector.clear();
	//一个目标规则涉及的所有的目标内存库
	vector<string> tmpobjmmnameVector;
	tmpobjmmnameVector.clear();
	
	DestRule tmpdestrule;
	tmpdestrule.m_infotype = infotype;
	vector<DestRule>::iterator iter=lower_bound(m_destruleVector->begin(), m_destruleVector->end(), tmpdestrule,PriorityLesser());
	while(iter!=m_destruleVector->end() && iter->m_infotype==infotype)
	{
		//并且是CRM资料数据库的
		if (iter->m_srcdbnum == srcdbnum)
		{
			//获取这条目标规则涉及的所有的目标内存库
			string objmmnames = iter->m_objmmnames;
			StringUtil::split(objmmnames, ";", tmpobjmmnameVector, false);
			
			//排重插入
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
	
	//根据所有的目标内存库名称查到所有的目标内存库类
	for (vector<string>::iterator iter = objmmnameVector.begin(); iter!=objmmnameVector.end(); ++iter)
	{
		if ((*m_objmmiMap).find(*iter) == (*m_objmmiMap).end())
		{
			throw CException(__FILE__,__LINE__,"名称为:=%s的内存数据库没有配置，请核查!", (*iter).c_str());
		}
		else
		{
			objmmis.push_back((*m_objmmiMap)[*iter]);
		}
	}
}

