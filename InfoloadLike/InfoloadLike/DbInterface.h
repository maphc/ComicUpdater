#ifndef _DbInterface_H_
#define _DbInterface_H_

#include "stdafx.h"

using namespace oracle::occi;

/**
* DbInterface:物理数据库操作类,提供对物理数据库的各种操作。
* 	DbInterface:			物理数据库操作类构造函数
*	~DbInterface:			物理数据库操作类析构函数
*	connect:				建立与物理数据库的连接
*	disconnect:				断开与物理数据库的连接
*	beginTrans:				开启事务
*	commitTrans:			提交事务
*	rollbackTrans:			回滚事务
*	execStaticSql:			执行静态SQL
*	execDynamicSql:			执行动态SQL
*	execProcedure:			执行存储过程
*	getNext:				获取一行记录
*	getColumnNum:			获取本次查询的记录的列数
*	getColumnIntValue:		获取记录列的整型值
*	getColumnLongValue:		获取记录列的长整型值
*	getColumnFloatValue:	获取记录列的浮点值
*	getColumnDateValue:		获取记录列的日期值
*	getColumnStringValue:	获取记录列的字符值 
*	getColumnToStringValue: 获取记录列的值并转换为字符值
*/

class DbInterface
{
	public:
		/**
		* DbInterface:物理数据库操作类构造函数
		* @param	无
		* @return
		*/
		DbInterface(const Environment::Mode &mode=Environment::OBJECT);

		/**
		* ~DbInterface:物理数据库操作类析构函数
		* @param	无
		* @return
		*/		
		~DbInterface();
		
		/**
		* connect:建立与物理数据库的连接(抛异常)
		* @param	无
		* @return
		*/		
		void connect(const string& username, const string& password, const string& servname);

		/**
		* disconnect:断开与物理数据库的连接(抛异常)
		* @param	无
		* @return
		*/		
		void disconnect();

		/**
		* beginTrans:开启事务
		* @param	无
		* @return
		*/
		void beginTrans();

		/**
		* commitTrans:提交事务(抛异常)
		* @param	无
		* @return
		*/				
		void commitTrans();

		/**
		* rollbackTrans:回滚事务(抛异常)
		* @param	无
		* @return
		*/		
		void rollbackTrans();
		
		/**
		* execStaticSql:执行静态SQL(抛异常)
		* @param	无
		* @return
		*/
		int execStaticSql(const string& staticsql);
		
		/**
		* execDynamicSql:执行动态SQL(抛异常);params里面的seq顺序必须和dynamicsql参数出现的顺序一致,和MM区别很大
		* @param	无
		* @return
		*/
		int execDynamicSql(const string& dynamicsql, const vector<DbParam>& params);
				
		/**
		* execProcedure:执行存储过程(抛异常)
		* @param	无
		* @return
		*/
		void execProcedure(const string& procedurename, vector<DbParam>& params);
		
		/**
		* getNext:获取一行记录(抛异常)
		* @param	无
		* @return
		*/
		bool getNext();
		
		/**
		* getColumnNum:获取本次查询的记录的列数
		* @param	无
		* @return
		*/
		int	 getColumnNum();

		/**
		* getColumnIntValue:获取记录列的整型值,pos最小为1(抛异常)
		* @param	无
		* @return
		*/		
		int getColumnIntValue(const unsigned int& pos);

		/**
		* getColumnLongValue:获取记录列的长整型值,pos最小为1(抛异常)
		* @param	无
		* @return
		*/
		long getColumnLongValue(const unsigned int& pos);
				
		/**
		* getColumnFloatValue:获取记录列的浮点值,pos最小为1(抛异常)
		* @param	无
		* @return
		*/
		float getColumnFloatValue(const unsigned int& pos);

		/**
		* getColumnDateValue:获取记录列的日期值,pos最小为1(抛异常)
		* @param	无
		* @return
		*/		
		string getColumnDateValue(const unsigned int& pos);
		
		/**
		* getColumnStringValue:获取记录列的字符值,pos最小为1(抛异常)
		* @param	无
		* @return
		*/		
		string getColumnStringValue(const unsigned int& pos);
		
		/**
		* getColumnToStringValue:获取记录列的值并转换为字符值,pos最小为1(抛异常)
		* @param	无
		* @return
		*/	
    string getColumnToStringValue(const unsigned int& pos);
public:    
//--------------- 批量操作(插入 更新 删除)相关接口 wangfc@lianchuang.com at 2010.03.29 
    /*
     * createStatement 创建操作SQL对象
    */
    void  createStatement() ;
    /*
     * setSql  设置SQL语句  
    */
    void  setSql(char* sSQL);
    /*
     * setMaxIteration 设置批量更新最大数目
    */
    void  setMaxIteration(const int &iMaxIteration);
    /*
     * addIteration     增加批量向量
    */
    void  addIteration();
    /*
     * setMaxParamSize  设置每列的参数最大的大小
    */
    void  setMaxParamSize(const int &riPosition, const int &riSize);
    /*
     * setParameter 设置每一行的数值
    */
    void  setParameter(const int &iPosition, const int & iValue);
    void  setParameter(const int &iPosition, const char *sValue);
    void  setParameter(const int &iPosition, const string &sValue);
    void  setParameter(const int &iPosition, const long   &lValue);
    void  setParameter(const int &iPosition, const Date   &dataValue);
    //void  setParameter(const int &iPosition, const Number &nValue);
    void  setParameter(const int &iPosition, const double &dValue);    
    void  setParameter(const int &iPosition, const float  &fValue);
    /*
     *    setBatchParameter 批量数据处理（包含 setMaxIteration setParameter等)
    */
    void  setBatchParameter(vector<BatchDbParam>& dbparams,int& rmaxrow);
   /*
    *     executeUpdate  执行SQL更新
   */
    int   executeUpdate();
    /*
     * executeUpdate(string) 执行SQL更新 重载一个 add xueyf 20101229;
    */
    void   executeUpdate(const string &str);
    /*
     *    executeQuery  执行查询(批量)
    */
    void  executeQuery(const int rmaxrecordsize = DEFAULT_RECORD_NUM) ;
    /*
     *    SetResultDataBuffer 批量查询，内存空间绑定
    */
    void  SetResultDataBuffer(const int& iPosition,void* buf ,const Type & coltype ,const unsigned int & size );
    /*
     *    SetResultDataBuffer 批量查询，内存空间绑定 重载一个 add xueyf 20101229
    */
    void  SetResultDataBuffer(const int& iPosition,void* buf ,const Type & coltype ,const unsigned int & size ,
                              ub2 *length = NULL,sb2 *ind = NULL, ub2 *rc = NULL);
    void  ResultDataBuffer(const int& iPosition,void* buf ,const Type & coltype ,const unsigned int & size ,
                              ub2 *length = NULL,sb2 *ind = NULL, ub2 *rc = NULL);
    //void  SetResultDataBuffer(const int& iPosition,void* buf ,const Type & coltype ,const unsigned int & size,ub2 *elementLength,sb2 *ind);
    /*
     *    BatchNext  批量数据集下移
    */
    bool  BatchNext(int& rcount ,const int& rmaxrecordsize = DEFAULT_RECORD_NUM) ;
    /*
     *getColCount 取得列数目
    */
    int   getColCount();
    /*
     *  getColumnType  取得指定列的类型
    */
    int   getColumnType(const unsigned int& pos);
    //add xueyf 求取列大小
    int   getColumnSize(const unsigned int& pos);
    void  execArrayUpdate(const unsigned int& arry_num);
    void  getUpdateNum();
//--------------------------end---------------------------------------
	private:		
		/**
		* strcasestr:获取第二个字符串在第一个字符串首次出现的地方,比如"ACCDDEE","CD";那么返回的指针指向"CDDEE"
		* @param	无
		* @return
		*/
		char* strcasestr(const char *str1,const char *str2);
		
	public:
		//指向OCCI定义的数据库环境类的指针
		Environment			*m_env;
		//指向OCCI定义的数据库连接类的指针
		Connection			*m_conn;
		//指向OCCI定义的数据库SQL执行类的指针
		Statement			*m_stmt;
		//指向OCCI定义的数据库查询结果类的指针
		ResultSet			*m_rs;
		//本次查询的记录中列数
		int					m_columnnum;
		//是否开启事务标志 0:无事务 1:有事务
		int       			m_transflag;
	public://批量增加部分 wangfc@lianchuang.com 2010.04.10
	  string          m_objalisename;
	  //设置别名
	  void            setobjalisename(string& objname ) 
	  {
	    this->m_objalisename = objname ;
	  }
	  string          getobjalisename() 
	  {
	    return this->m_objalisename;
	  }
		
};


#endif

