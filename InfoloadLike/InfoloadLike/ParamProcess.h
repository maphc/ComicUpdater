#ifndef _ParamProcess_H_
#define _ParamProcess_H_


#include "InfoPublic.h"


class DbInterface;
class MmInterface;


/**
* ParamProcess:参数数据处理类,完成参数等全表数据的载入处理。
* 	ParamProcess:	参数数据处理类构造函数
*	~ParamProcess:	参数数据处理类析构函数
*	init:			参数数据处理类的初始函数,设置数据源的物理数据库和数据目标的内存数据库集合
*	process:		参数数据载入处理
*/
class ParamProcess
{
	public:
		
		/**
		* ParamProcess:参数数据处理类构造函数
		* @param	无
		* @return
		*/
		ParamProcess();
		
		/**
		* ~ParamProcess:参数数据处理类析构函数
		* @param	无
		* @return
		*/
		~ParamProcess();

		/**
		* init:参数数据处理类的初始函数,设置数据源的物理数据库和数据目标的内存数据库集合
		* @param	srcdbi	指向数据源的物理数据库操作类的指针
		* @param	objmmis	指向数据目标的内存数据库操作类集合的指针
		* @return	无
		*/
		void init(DbInterface *srcdbi, vector<MmInterface*> *objmmis);

		/**
		* process:参数数据载入处理(抛异常)
		* @param	dealtype	载入方式 0:先TRUNCATE,然后全部数据INSERT进入。1:只做UPDATE和INSERT操作，已有数据UPDATE，没有的数据INSERT。
		* @param	selectsql	源数据查询SQL:普通查询SQL，对于载入方式是1的时候，最后WHERE条件里面必定有UPDATE_TIME>=to_date(:1,'yyyymmddhh24miss') AND UPDATE_TIME<to_date(:2,'yyyymmddhh24miss')
		* @param	tablename	目标表名
		* @param	updatesql	目标更新SQL：里面定义格式为：indexname[column1,column2,…,columnN]| expr[column1>=:1 AND .. AND columnN>:N]| update[column1,column2,…,columnN]
		* @param	starttime	起始时间,格式"yyyymmddhh24miss"
		* @param	endtime		终止时间,格式"yyyymmddhh24miss"
		* @return	无
		*/
		void process(const char& dealtype, const string& selectsql, const string& tablename, 
					 const string& updatesql, const string& starttime, const string& endtime);
    //批量提交接口  (只使用于IPC单库）
    void  processbatch (const char& dealtype, const string& selectsql, const string& tablename, 
					 const string& updatesql, const string& starttime, const string& endtime);
   //信息提示
    void  showPromtMessage(const char* cMsg);
    void  showErrorMessage(const char* cMsg );
   
	private:
		
		//指向数据源的物理数据库操作类的指针
		DbInterface				*m_srcdbi;
		//指向数据目标的内存数据库操作类集合的指针
		vector<MmInterface*>	*m_objmmis;

};


#endif

