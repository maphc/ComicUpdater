#ifndef _DestRuleParse_H_
#define _DestRuleParse_H_


#include "stdafx.h"

class DbInterface;
class MmInterface;


/**
* DestRuleParse:数据目表判断类,根据资料决定资料的目标库信息。
* 	DestRuleParse:	数据目表判断类构造函数
*	~DestRuleParse:	数据目表判断类析构函数
*	init:			数据目表判断类的初始函数,设置目标规则集合和目标库操作类集合指针
*	getallobjdbis:	根据资料类型获取所有的目标物理库操作类
*	getallobjmmis:	根据资料类型获取所有的目标内存库操作类
*	getobjdbi:		根据资料类型、地市编码获取所有的目标物理库操作类
*	getobjmmi:		根据资料类型、地市编码获取所有的目标内存库操作类
*/
class DestRuleParse
{
	public:

		/**
		* InfoProcess:资料信息处理类构造函数
		* @param	无
		* @return
		*/		
		DestRuleParse();

		/**
		* InfoProcess:资料信息处理类构造函数
		* @param	无
		* @return
		*/		
		~DestRuleParse();

		/**
		* InfoProcess:资料信息处理类构造函数
		* @param	无
		* @return
		*/		
		void init(map<string, DbInterface*> *objdbiMap, map<string, MmInterface*> *objmmiMap, vector<DestRule> *destruleVector);		

		/**
		* InfoProcess:资料信息处理类构造函数
		* @param	无
		* @return
		*/
		//void getallobjdbis(const int &infotype, vector<DbInterface*> &objdbis);

		/**
		* InfoProcess:资料信息处理类构造函数
		* @param	无
		* @return
		*/		
		//void getallobjmmis(const int &infotype, vector<MmInterface*> &objmmis);

		/**
		* InfoProcess:资料信息处理类构造函数
		* @param	无
		* @return
		*/		
		void getobjdbi(const int &infotype, const int &srcdbnum, vector<DbInterface*> &objdbis);

		/**
		* InfoProcess:资料信息处理类构造函数
		* @param	无
		* @return
		*/		
		void getobjmmi(const int &infotype, const int &srcdbnum, vector<MmInterface*> &objmmis);
		
//modify by chenxt 20081024
//private:
	public:
		//物理库名称和物理库操作类对应关系集合
		map<string, DbInterface*>	*m_objdbiMap;
		//内存库名称和内存库操作类对应关系集合
		map<string, MmInterface*>	*m_objmmiMap;
		//资料数据目标规则集合(已经排序)
		vector<DestRule>			*m_destruleVector;
  public:		
    //批量增加部分  wangfc@lianchuang.com 2010.03.29
    /*
     *@getobj_db_deal 取某个infotype下入库目标库的处理方式  0 ：正常入库  1 ：批量入库 
    */
    int  getobj_db_deal(int& infotype,const int& srcdbnum );
    /*
      @getAllDestRule 取某个库下所有的类型目标规则
    */
    void getAllDestRule (const int &srcdbnum,vector<DestRule> &destrule);
    /*
      @getDestRule 根据infotype和srcdbnum取得指定destrule
    */
    void getDestRule(int& infotype ,int& srcdbnum,DestRule& destrule );
    
};


#endif

