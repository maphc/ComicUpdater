#ifndef _RecordProcess_H_
#define _RecordProcess_H_


#include "stdafx.h"

class DbInterface;
class MmInterface;


/**
* RecordProcess:资料记录处理类,提供资料单条记录载入的功能。
* 	RecordProcess:		资料记录处理类构造函数
*	~RecordProcess:		资料记录处理类析构函数
*	setLoadTable:		设置载入资料对应的目标库表信息
*	setLoadDatabase:	设置载入资料对应的目标库信息
*	processRecord:		完成(增量的增量)单条资料记录载入处理
*	clearIT:			完成(增量的全量)关键字涉及表记录删除
*/
class RecordProcess
{
	public:
		
		/**
		* RecordProcess:资料记录处理类构造函数
		* @param	无
		* @return
		*/
		RecordProcess();
		
		/**
		* RecordProcess:资料记录处理类构造函数
		* @param	loadtable	载入资料对应的目标库表信息
		* @return
		*/
		RecordProcess(LoadTable* loadtable);
		
		/**
		* ~RecordProcess:资料记录处理类析构函数
		* @param	无
		* @return
		*/
		~RecordProcess();
		
		/**
		* setLoadTable:设置载入资料对应的目标库表信息
		* @param	loadtable	载入资料对应的目标库表信息
		* @return	无
		*/
		void setLoadTable(LoadTable* loadtable);
		
		/**
		* setLoadDatabase:设置载入资料对应的目标库信息
		* @param	objdbis		指向目标物理数据库操作类集合的指针
		* @param	objmmis		指向目标内存数据库操作类集合的指针
		* @param	objdbisPro	省中心库需要载入的对应的目标物理库集信息
		* @param	objmmisPro	省中心库需要载入的对应的目标内存库集信息		
		* @return	无
		*/
		void setLoadDatabase(vector<DbInterface*>* objdbis, vector<MmInterface*>* objmmis, vector<DbInterface*>* objdbisPro, vector<MmInterface*>* objmmisPro);
		
		/**
		* processRecord:完成(增量的增量)单条资料记录载入处理(抛异常)
		* @param	record		指向(增量的增量)单条资料记录的指针
		* @return	无
		*/
		void processRecord(Record* record);
		
		/**
		* clearIT:完成(增量的全量)关键字涉及表记录删除(抛异常)
		* @param	allobjdbis	该载入资料涉及的所有的目标物理数据库操作类集合
		* @param	allobjmmis	该载入资料涉及的所有的目标内存数据库操作类集合
		* @param	keyid		该载入资料的关键字
		* @return	无
		*/
		void clearIT(Record* record);

		/**
		* clearIT_N:完成(增量的全量)关键字涉及表记录删除(抛异常)
		* @param	allobjdbis	该载入资料涉及的所有的目标物理数据库操作类集合
		* @param	allobjmmis	该载入资料涉及的所有的目标内存数据库操作类集合
		* @param	keyid		该载入资料的关键字
		* @return	无
		*/
		void clearIT_N(Record* record);
		
		
		void clearKeySet();
				
	private:
	
		/**
		* batchInsert:批量插入全量信息
		* @param	filetype	资料文件类型 0:增量的增量 1:增量的全量 2:全量的全量;这边只可能出现1,2
		* @param	info		指向(增/全量的全量)多条资料记录的指针
		* @return	无
		*/
		void batchInsert(Info* info);
		
		/**
		* deleteRec:根据单条资料记录进行删除
		* @param	record	指向(增量的增量)单条资料记录的指针
		* @return	无
		*/
		void deleteRec(Record* record);
		
		/**
		* insert:根据单条资料记录进行插入
		* @param	record	指向(增量的增量)单条资料记录的指针
		* @return	无
		*/
		void insert(Record* record);
		
		/**
		* update:根据单条资料记录进行更新
		* @param	record	指向(增量的增量)单条资料记录的指针
		* @return	无
		*/
		void update(Record* record);
		
		/**
		* isExist:判断该记录在数据库中是否存在
		* @param	dbtype	数据库类型 0:物理数据库 1:内存数据库
		* @param	record	指向记录的指针
		* @param	dbi		指向物理数据库操作类的指针
		* @param	mmi		指向内存数据库操作类的指针						
		* @return	返回该记录在数据库中是否存在的结果 true:存在 false:不存在
		*/		
		bool isExist(const int &dbtype, Record* record, DbInterface* dbi, MmInterface* mmi);
		
		/**
		* getdbparamvalue:根据列类型、列取值方式、和列源值给对应物理库参数的内容赋值
		* @param	dbcolumntype	列类型
		* @param	dbgetvalue		列取值方式
		* @param	dbcolumnvalue	列源值
		* @param	dbparam			物理库参数		
		* @return	无
		*/
		void getdbparamvalue(const PROCPARAM_TYPE &dbcolumntype, const char &dbgetvalue, const string &dbcolumnvalue, DbParam &dbparam);
		
		/**
		* getmmparamvalue:根据列类型、列取值方式、和列源值给对应物理库参数的内容赋值
		* @param	mmcolumntype	列类型
		* @param	mmgetvalue		列取值方式
		* @param	mmcolumnvalue	列源值
		* @param	mmparam			内存库参数		
		* @return	无
		*/
		//void getmmparamvalue(const COLUMN_VAR_TYPE &mmcolumntype, const char &mmgetvalue, const string &mmcolumnvalue, MmParam &mmparam);
		
		/**
		* isProvinceInfo:判断该记录是否是跨地市集团客户信息
		* @param	record	指向(增量的增量)单条资料记录的指针	
		* @return	true:跨地市集团信息 false:本地信息
		*/		
		bool isProvinceInfo(Record* record);

		
	protected:
		
		//需要载入的资料对应的目标库表信息
		LoadTable				*m_loadtable;
		//需要载入的资料对应的目标物理库集信息
		vector<DbInterface*>	*m_objdbis;
		//需要载入的资料对应的目标内存库集信息
		vector<MmInterface*>	*m_objmmis;
		
		//省中心库需要载入的对应的目标物理库集信息
		vector<DbInterface*>	*m_objdbisPro;
		//省中心库需要载入的对应的目标内存库集信息
		vector<MmInterface*>	*m_objmmisPro;
		
		set<string>				m_keySet;
	 public :// 批量增加部分 wangfc@lianchuang.com
	  bool              m_batchIntoBilling ;//是否是批量入库标志
	  bool              m_ifProvinceMode ;
	  void              setbatchIntoBillingMode(bool batchIntoBilling)
	  {
	    m_batchIntoBilling = batchIntoBilling;
	  }
	  bool              getbatchIntoBillingMode()
	  {
	    return m_batchIntoBilling ;
	  }
	  //省中心模式
  	void setProvinceMode(bool bisProv)
  	 {
  	   m_ifProvinceMode = bisProv;
  	 }
    
};


#endif

