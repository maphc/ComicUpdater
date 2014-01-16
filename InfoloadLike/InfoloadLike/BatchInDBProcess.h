#ifndef _BATCHINDBPROCESS_H_
#define _BATCHINDBPROCESS_H_
#include "stdafx.h"
/**
 ** 完成批量入临时表及调用存储过程完成批量入帐务库等过程
**/
class DestRuleParse;

class DbInterface;

class BatchInDBProcess
{
  
  public:
    BatchInDBProcess();
    ~BatchInDBProcess();
  private:
		//指向数据目标库规则分析类的指针
		DestRuleParse				    *m_destRuleParse;
		
		//省中心库资料对应的物理数据库
		vector<DbInterface*>		*m_dbisPro;
		
		LoadTable               *m_loadtables;//配置表
		
		string                   m_filename ;	//当前处理的文件名	
	public:	  
		/**
		* init:资料信息处理类的初始函数,设置目标库规则分析类指针
		* @param	destRuleParse	指向数据目标库规则分析类的指针
		* @param	dbisPro			省中心库资料对应的物理数据库
		* @return	无
		*/	  
	  void init(LoadTable *loadtables,DestRuleParse *destRuleParse, 
	            vector<DbInterface*> *dbisPro);
	
	  /**
		* processRecord:完成(增量的增量)的资料信息载入处理(抛异常)
		* @param	chgid	变更流水号
		* @param	info	本次变更修改的增量的增量数据
		* @return	无
		*/	
		
		void processRecord(string &chgid, int &infotype, int &srcdbnum, Info *info);
		/**
		* indb :完成增量数据的正式入库(抛异常)
		* @param 无
		* @return 无
		**/	    
		
		void indb(int& srcdbNum);
		/**
		* inserttable :完成增量数据入临时表（抛异常）
		* @param infotype 变更类型
		* @param maxrecordsize 最大提交记录
		**/
		
		void insertTable(int& infotype,int& srcdbnum ,bool isprov ) ;
		/**
		* isProvinceInfo :判断是否入省中心库
		* @param record 增量记录
		**/		
		
		bool isProvinceInfo(Record* record);
		/*
		 * getinsertsql 组合入临时表的insert语句
		 * @infotype
		 * @srcdbnum
		 * @tablenames
		*/
		
		string getinsertsql(int& infotype,int& srcdbnum,string& tablenames );
		/*
		 * executedbproc 执行存储过程
		 * @dbi :物理库实例
		 * @infotype 
		 * @srcdbnum    库号
		 * @procname    存储过程名字
		*/
		
		bool executedbproc(DbInterface* dbi ,int& infotype,int& srcdbnum,string& procname );
	
		/*
		* setfilename           设置文件名
		* @filename             文件名
		*/
		void  setfilename(const char* filename );
		/*
		 *setchannelno          设置通道号
		 *@ rchannelno          通道号
		*/
		void  setchannelno ( int & rchannelno );
		/*
		 * setMaxBatchRecordNum  设置最大的批量提交数目
		 * @maxrum  数目
		*/
		void  setMaxBatchRecordNum(int & maxrum);
		
		/*
		* 设置是否是省中心模式
		* @bisProv 是否是省中心模式
		*/
		void  setProvinceMode(bool bisProv);
    
  private:
    
    bool  m_ifProvinceMode;       // 是否是省中心模式    
    int   m_maxBatchRecordNum;    // 最大提交到临时表的记录数目   
    int   m_channelNo ;           // 通道号    
};

#endif