#ifndef _InfoProcess_H_
#define _InfoProcess_H_


#include "stdafx.h"


class RecordProcess;
class DestRuleParse;
class DbInterface;
class MmInterface;

/**
* InfoProcess:资料信息处理类,提供资料多条记录载入的功能。
* 	InfoProcess:		资料信息处理类构造函数
*	~InfoProcess:		资料信息处理类析构函数
*	init:				资料信息处理类的初始函数,设置装载表信息集和目标库规则分析类指针
*	processTrade:		完成(增量的增量)的资料信息载入处理
* 	getRecordProcess:	根据变更的信息类型获取指向资料信息记录处理类的指针
*/
class InfoProcess
{
	public:
		
		/**
		* InfoProcess:资料信息处理类构造函数
		* @param	无
		* @return
		*/
		InfoProcess();
		
		/**
		* ~InforProcess:资料信息处理类析构函数
		* @param	无
		* @return
		*/		
		~InfoProcess();
		
		/**
		* init:资料信息处理类的初始函数,设置装载表信息集和目标库规则分析类指针
		* @param	loadtables		指向所有需要装载的资料信息数组集合的指针
		* @param	destRuleParse	指向数据目标库规则分析类的指针
		* @param	dbisPro			省中心库资料对应的物理数据库
		* @param	mmisPro			省中心库资料对应的内存数据库
		* @return	无
		*/
		void init(LoadTable *loadtables, DestRuleParse *destRuleParse, vector<DbInterface*> *dbisPro, vector<MmInterface*> *mmisPro);
		
		/**
		* processTrade:完成(增量的增量)的资料信息载入处理(抛异常)
		* @param	chgid	变更流水号
		* @param	info	本次变更修改的增量的增量数据
		* @return	无
		*/	
		void processTrade(string &chgid, int &infotype, const int &srcdbnum, Info *info);
		
		
	private:
		
		/**
		* getRecordProcess:根据变更的信息类型获取指向资料信息记录处理类的指针
		* @param	infotype	变更的信息类型
		* @return	指向资料信息记录处理类的指针
		*/		
		RecordProcess *getRecordProcess(const int &infotype);
		
		
	private:
		
		//指向所有需要装载的资料信息数组集合的指针
		LoadTable					*m_loadtables;
		//指向数据目标库规则分析类的指针
		DestRuleParse				*m_destRuleParse;
		//存放信息类型和资料记录处理指针的容器
		map<int, RecordProcess*>	m_recordProcessMap;
		
		//省中心库资料对应的物理数据库
		vector<DbInterface*>		*m_dbisPro;
		//省中心库资料对应的内存数据库
		vector<MmInterface*>		*m_mmisPro;
};


#endif

