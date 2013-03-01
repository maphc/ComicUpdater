#ifndef _InfoPublic_H_
#define _InfoPublic_H_


#include "stdafx.h"

using namespace std;

namespace StringUtil{
	static string & ltrim(string & str) {
		string::iterator pos = str.begin();
		for (; pos != str.end() && (* pos == ' ' || * pos == '\t'); pos++);
		if (pos != str.begin()) str.erase(str.begin(), pos);
		return str;
	};

	///去掉字符串右边空白字符
	static string & rtrim(string & str) {
		string::reverse_iterator pos = str.rbegin();
		for (; pos != str.rend() && (* pos == ' ' || * pos == '\t'); pos++);
		if (pos != str.rbegin()) str.erase(pos.base(), str.end());
		return str;
	}; 

	///去掉字符串两边边空白字符
	static string & trim(string & str) {        
		return ltrim(rtrim(str));
	}; 

	static size_t split(const string& str, ///源字符串 
		const string& delim, ///分割字符 
		vector<string>& result, ///结果
		bool needTrim ///是否去掉结果字串两边空格
		) {
			if(str.length() == 0) return 0;

			if (delim.length() <=0) { 
				result.push_back(str);
				return 1;
			}

			size_t c = 0;
			for (size_t i, pos=0; ; pos = i+delim.length()) {
				i = str.find(delim, pos);

				if (i == string::npos) 
					result.push_back(str.substr(pos));
				else
					result.push_back(str.substr(pos,i - pos));

				if(needTrim) 
					trim(result[result.size()-1]);

				c++;                        
				if (i == string::npos) break;
			}
			return c;
	};
}

//(1)常量定义
//(1.2)文件相关常量属性定义
//系统文件路径的最大长度
const unsigned int MAX_FILEPATH_LEN = 100;
//系统文件名称的最大长度
const unsigned int MAX_FILENAME_LEN = 100;
//系统文件记录最大长度
const unsigned int MAX_FILERECORD_LEN = 2000;
//系统文件处理过程中最大记录提交数
const unsigned int MAX_RECCOMMIT_NUM = 1;
//系统文件中各列的分隔符号最大长度
const unsigned int MAX_SEPERATOR_LEN = 5;
//错误资料信息记录表的字段数
const unsigned int ERRMSG_FILED_NUM = 4;

//(1.3)资料相关常量属性定义
//文件资料开始位置
const unsigned int INFO_ADDLEN = 3;
//资料的最大列数
const unsigned int MAX_FIELD_NUM = 50;
//资料的列内容最大长度
//const unsigned int MAX_FIELD_LEN = 101;
//Modify by wangfc 2011-10-22 11:16:46 大字段同步
const unsigned int MAX_FIELD_LEN = 301;
//资料类型的最小值
const unsigned int MIN_INFOTYPE_VALUE = 0;
//资料类型的最大值
const unsigned int MAX_INFOTYPE_VALUE = 999;


//(1.4)系统错误信息定义
const unsigned int INFOLOAD_ERROR_BASE = 10000;
//物理库错误定义
const unsigned int DB_ERROR_CONNECT = INFOLOAD_ERROR_BASE;
const unsigned int DB_ERROR_EXECSQL = INFOLOAD_ERROR_BASE+1;
const unsigned int DB_ERROR_DISCONNECT = INFOLOAD_ERROR_BASE+2;
const unsigned int DB_ERROR_NOCFG = INFOLOAD_ERROR_BASE+3;
//执行存储过程错误   全量无缝导换(OEC) xueyf@lianchuang.com
const unsigned int DB_ERROR_BEGPRO  = INFOLOAD_ERROR_BASE+4;
const unsigned int DB_ERROR_ENDPRO  = INFOLOAD_ERROR_BASE+5;
const unsigned int DB_ERROR_SAMEPRO = INFOLOAD_ERROR_BASE+6;
//内存库错误定义
const unsigned int MM_ERROR_CONNECT = INFOLOAD_ERROR_BASE+10;
const unsigned int MM_ERROR_EXECSQL = INFOLOAD_ERROR_BASE+11;
const unsigned int MM_ERROR_DISCONNECT = INFOLOAD_ERROR_BASE+12;
const unsigned int MM_ERROR_NOCFG = INFOLOAD_ERROR_BASE+13;
//文件错误定义
const unsigned int ERROR_DBPARAM_CFG = INFOLOAD_ERROR_BASE+20;
const unsigned int ERROR_FILEINFO_DEAL = INFOLOAD_ERROR_BASE+21;
const unsigned int ERROR_UNLINK_FILE = INFOLOAD_ERROR_BASE+22;
const unsigned int ERROR_FILENAME_NAMERULE = INFOLOAD_ERROR_BASE+23;
const unsigned int ERROR_FILENAME_NOCONTINUE = INFOLOAD_ERROR_BASE+24;
const unsigned int ERROR_FILENAME_LENGTH = INFOLOAD_ERROR_BASE+25;

//在mdb/MdbConstDef.h中定义
const unsigned int  MAX_COLUMN_NUM=200;

#define MAX_BATCH_COMMIT_RECORD_SIZE 1000;

//(2)常用的类、结构定义
//(2.1)日志信息结构
struct Log
{
	string			m_filename;
	char			m_inouttag;
	unsigned long	m_totalrecnum;
	unsigned long	m_errrecnum;
};


//(2.2)记录类---单条记录信息
class Record
{
	public:
		Record()
		{
			for (int i=0; i<MAX_FIELD_NUM; ++i)
			{
				m_data[i] = new char[MAX_FIELD_LEN];
			}
			//modify by chenxt 20081024
			m_maxFieldNum = 0;
			//m_maxFieldNum = -1;
		}
		
		
		~Record()
		{
			for (int i=0; i<MAX_FIELD_NUM; ++i)
			{
				delete [] m_data[i]; // 动态数组释放 wangfc@lianchuang.com
				m_data[i] = NULL;
			}
			m_maxFieldNum = 0;
		}
		
		
		bool push_back(const char* fieldvalue)
		{
			if(m_maxFieldNum < (MAX_FIELD_NUM-1))
			{ 
				//strcpy(m_data[m_maxFieldNum], fieldvalue);
				//add by wangfc 2009-12-18  10：20 修正输入的字段长度过程，导致infoload core ，现增加长度有效性判断
        if ( strlen(fieldvalue) > MAX_FIELD_LEN -1 )
          strncpy(m_data[m_maxFieldNum], fieldvalue,MAX_FIELD_LEN - 1 );
        else
          strcpy(m_data[m_maxFieldNum], fieldvalue);
        //end

				m_maxFieldNum++;
				return true;
			}
			else
			{
				return false;
			}
		}
		
		
		void clear()
		{
			m_maxFieldNum = 0;
		}
		
		
		int size()
		{
			return m_maxFieldNum;
		}

		
		char* operator [](const int& i)
		{
			if (i <= m_maxFieldNum)
			{
				return m_data[i];
			}
			return NULL;
		}

		
	private:
		char*	m_data[MAX_FIELD_NUM];	
		int		m_maxFieldNum;
};


//(2.3)信息类---记录集
class Info
{
	public:
		Info() {;}

		
		~Info()
		{
			clear();
		}

		
		void clear()
		{
			for (int i=0; i<m_data.size(); ++i)
			{
				delete m_data[i];
				m_data[i] = NULL;
			}
			m_data.clear();
		}

		
		int size()
		{
			return m_data.size();
		}


		void push_back(Record* record)
		{
			m_data.push_back(record);
		}
		
		
		Record* operator [](const int& i)
		{
			return m_data[i];
		}
		
		
	private:
		vector<Record*> m_data;
};

//(2.5)需要载入的物理表描述(结构)
struct DbColumn
{
	PROCPARAM_TYPE		m_columntype;		//列类型,主要有PARAM_TYPE_INT, PARAM_TYPE_LONG, PARAM_TYPE_FLOAT, PARAM_TYPE_STRING, PARAM_TYPE_DATE
	unsigned int		  m_keytag;			//列是否是关键字, 0:不是 1:是
	string			    	m_columnname;		//列名称
	char				      m_getvaluetype;		//获取值方式 '0':直接取值  '1':取%10000余
	int					      m_getvaluepos;		//获取值在记录中位置
};

//(2.4)数据目标库判定规则类
class DestRule
{
	public:
		DestRule() {}

		
		DestRule(const DestRule &right)
		{
			m_infotype = right.m_infotype;
			m_srcdbnum = right.m_srcdbnum;
			m_objdbnames = right.m_objdbnames;
			m_objmmnames = right.m_objmmnames;
			//批量增加部分   wangfc@lianchuang.com 2010.04.06
			m_objdeal                   =  right.m_objdeal;
		  m_obj_db_tablenames         =  right.m_obj_db_tablenames;
		  m_obj_db_procs              =  right.m_obj_db_procs;
		  m_obj_provdb_tablenames     =  right.m_obj_provdb_tablenames ; 
		  m_obj_provdb_procs          =  right.m_obj_provdb_procs ; 
		  m_data                      =  right.m_data; 
		  m_CurBatchRecordcount       =  right.m_CurBatchRecordcount;
		  m_provdata                  =  right.m_provdata ;
		  m_CurBatchProvRecordcount   =  right.m_CurBatchProvRecordcount;
		  m_MaxRecordCount            =  right.m_MaxRecordCount ;
		  m_isExistsRecord            =  right.m_isExistsRecord ;
		  m_isExistsRecordProv        =  right.m_isExistsRecordProv;
		  //--end
		}

		
		~DestRule() {}

		
		bool operator <  (const DestRule &right)
		{
			return (m_infotype < right.m_infotype);
		}

				
		bool operator == (const DestRule &right)
		{
			return (m_infotype == right.m_infotype);
		}

		
		DestRule& operator = (const DestRule &right)
		{
			if (this != &right)
			{
				m_infotype = right.m_infotype;
				m_srcdbnum = right.m_srcdbnum;
				m_objdbnames = right.m_objdbnames;
				m_objmmnames = right.m_objmmnames;
				//批量增加部分   wangfc@lianchuang.com 2010.04.06
  			m_objdeal                   =  right.m_objdeal;
  		  m_obj_db_tablenames         =  right.m_obj_db_tablenames;
  		  m_obj_db_procs              =  right.m_obj_db_procs;
  		  m_obj_provdb_tablenames     =  right.m_obj_provdb_tablenames ; 
  		  m_obj_provdb_procs          =  right.m_obj_provdb_procs ; 
  		  m_data                      =  right.m_data; 
  		  m_CurBatchRecordcount       =  right.m_CurBatchRecordcount;
  		  m_provdata                  =  right.m_provdata ;
  		  m_CurBatchProvRecordcount   =  right.m_CurBatchProvRecordcount;		
  		  m_MaxRecordCount            =  right.m_MaxRecordCount;
  		  m_isExistsRecord            =  right.m_isExistsRecord ;
  		  m_isExistsRecordProv        =  right.m_isExistsRecordProv;  		  	
  		  //end	
			}
			return *this;
		}

		
	public:
		int		  m_infotype;
		int    	m_srcdbnum;
		string	m_objdbnames;	//比如:"库1;库2;库3",用分号进行分割,可以为NULL
		string	m_objmmnames;	//比如:"内存1;内存2",用分号进行分割,可以为NULL
	public:	
//---------------------------------以下是批量入库参数 wangfc@lianchuang.com	2010.03.29
    int                   m_objdeal ;                // 入目标物理库模式，0 : 正常入库模式(默认) 1 ：批量入库模式
		string                m_obj_db_tablenames;       // 比如:"库名1|表;库2|表；库3|表",用分号进行分割，可以为NULL,可以设置相同的名称，则直接设置一个就可以
		string                m_obj_db_procs;            // 比如:"库名1|存储过程;库名2|存储过程；库名3|存储过程",用分号进行分割，可以为NULL,
		string                m_obj_provdb_tablenames ;  // 省中心库配置
		string                m_obj_provdb_procs ;       // 省中心库对应的存储过程
		vector<BatchDbParam>  m_data;                    // 库记录
		vector<BatchDbParam>  m_provdata ;               // 中心库记录
		long*                 m_CurBatchRecordcount;     // 记录计数
		long*                 m_CurBatchProvRecordcount; // 中心库记录计数
		long                  m_MaxRecordCount ;         // 最大批量数目
		bool*                 m_isExistsRecord ;         // 正常模式下是否有入库的数据
		bool*                 m_isExistsRecordProv ;     // 正常模式下是否有入库的数据(省中心模式)
public:
	  //设置最大批量提交的行数
	  /*
	     一个字符字段占用的大小：最大记录数 * (   101 ) = 1000*101 = 100K;
	      如果一张表30个字段，30×100K = 3M	     
	  */
	  void setBatchParam(DbColumn* cols,int& r_size,int& m_maxBatchRecordcount)
	  {	    
	    long*  longValue  = NULL;
	    int*   intValue   = NULL;
	    float* floatValue = NULL;
	    char*  strValue   = NULL;
	    Date*  dateValue  = NULL ;
	    m_isExistsRecord          = new bool();
	    *m_isExistsRecord         = false ;
	    m_CurBatchRecordcount     = new long(0);//分配计数地址空间,并初始化为0
	    BatchDateParam* dateParam = NULL ;
	    m_MaxRecordCount    = m_maxBatchRecordcount;
	    //1.通道号
	    BatchDbParam dbparam ;
	    dbparam.m_seq       = 1 ;
	    dbparam.m_type      = PARAM_TYPE_INT;
	    dbparam.size        = sizeof(int);
	    intValue            = new int[m_maxBatchRecordcount];
	    dbparam.m_intValue  = intValue ;
	    m_data.push_back(dbparam);	    
	    //2.同步序列号
	    dbparam.m_seq       = 2 ;
	    dbparam.m_type      = PARAM_TYPE_LONG;
	    dbparam.size        = sizeof(long);
	    longValue           = new long[m_maxBatchRecordcount];
	    dbparam.m_longValue = longValue ;
	    m_data.push_back(dbparam);
	    //3.infotype类型	
	    dbparam.m_seq  = 3 ;
	    dbparam.m_type = PARAM_TYPE_INT;
	    dbparam.size   = sizeof(int);
	    intValue       = new int[m_maxBatchRecordcount];
	    dbparam.m_intValue = intValue ;	 
	    m_data.push_back(dbparam);   
	    //4.modify_tag 更新方式
	    dbparam.m_seq  = 4 ;
	    dbparam.m_type = PARAM_TYPE_INT;
	    dbparam.size   = sizeof(int);
	    intValue       = new int[m_maxBatchRecordcount];
	    dbparam.m_intValue = intValue ;	 
	    m_data.push_back(dbparam); 	    
	    //5.正常列的配置，内存分配
	    for( int i= 0 ; i < r_size ;++i )
	    {	    
	      dbparam.m_seq      = i + 5;  
	      dbparam.m_type     = cols[i].m_columntype;
	      switch( cols[i].m_columntype )
	      {
	        case PARAM_TYPE_INT:
	             dbparam.size       = sizeof(int); 
	             intValue           = new int[m_maxBatchRecordcount];
	             dbparam.m_intValue = intValue ;		             
	             break;
	        case PARAM_TYPE_LONG:
	             dbparam.size   = sizeof(long); 
	             longValue      = new long[m_maxBatchRecordcount];
	             dbparam.m_longValue = longValue ;	          
	             break;
	        case PARAM_TYPE_FLOAT:
	             dbparam.size   = sizeof(float); 
	             floatValue     = new float[m_maxBatchRecordcount];
	             dbparam.m_floatValue = floatValue ;		          
	             break;
	        case PARAM_TYPE_STR:
	             dbparam.size   = sizeof(char) * MAX_FIELD_LEN; 
	             strValue       = new char[m_maxBatchRecordcount * MAX_FIELD_LEN];	             
	             dbparam.m_strValue = strValue ;	          
	             break;
	        case PARAM_TYPE_DATE:
	             dbparam.size                = sizeof(Date); ;
	             dateValue                   = new Date[m_maxBatchRecordcount ];	             
	             dbparam.m_dateValue._date   = dateValue ;		 
	             dateParam                   = new BatchDateParam[m_maxBatchRecordcount ];
	             dbparam.m_dateValue._Param  = dateParam;
	             break;
	      }
	      m_data.push_back(dbparam);
	    }
	  }
	  //设置最大批量提交的行数（省中心模式）
		void setBatchProvParam(DbColumn* cols,int& r_size,int& m_maxBatchRecordcount )
	  {
	    long*            longValue  = NULL;
	    int*             intValue   = NULL;
	    float*           floatValue = NULL;
	    char*            strValue   = NULL;
	    Date*            dateValue  = NULL ;
	    BatchDateParam*  dateParam   = NULL ;
	    m_isExistsRecordProv      = new bool();
	    *m_isExistsRecordProv     = false ;	    
	    m_CurBatchProvRecordcount   = new long(0);//分配计数地址空间,并初始化为0
	    //1.通道号
	    BatchDbParam dbparam ;
	    dbparam.m_seq  = 1 ;
	    dbparam.m_type = PARAM_TYPE_INT;
	    dbparam.size   = sizeof(int);
	    intValue       = new int[m_maxBatchRecordcount];
	    dbparam.m_intValue = intValue ;
	    m_provdata.push_back(dbparam);	    
	    //2.同步序列号
	    dbparam.m_seq  = 2 ;
	    dbparam.m_type = PARAM_TYPE_LONG;
	    dbparam.size   = sizeof(long);
	    longValue      = new long[m_maxBatchRecordcount];
	    dbparam.m_longValue = longValue ;
	    m_provdata.push_back(dbparam);
	    //3.infotype类型	
	    dbparam.m_seq  = 3 ;
	    dbparam.m_type = PARAM_TYPE_INT;
	    dbparam.size   = sizeof(int);
	    intValue       = new int[m_maxBatchRecordcount];
	    dbparam.m_intValue = intValue ;	 
	    m_provdata.push_back(dbparam);   
	    //4.modify_tag 更新方式
	    dbparam.m_seq  = 4 ;
	    dbparam.m_type = PARAM_TYPE_INT;
	    dbparam.size   = sizeof(int);
	    intValue       = new int[m_maxBatchRecordcount];
	    dbparam.m_intValue = intValue ;	 
	    m_provdata.push_back(dbparam); 	    
	    //5.正常列的配置
	    for( int i= 0 ; i < r_size ;++i )
	    {	    
	      dbparam.m_seq  = i + 5;  
	      dbparam.m_type = cols[i].m_columntype;
	      switch( cols[i].m_columntype )
	      {
	        case PARAM_TYPE_INT:
	             dbparam.size   = sizeof(int); 
	             intValue       = new int[m_maxBatchRecordcount];
	             dbparam.m_intValue = intValue ;		             
	             break;
	        case PARAM_TYPE_LONG:
	             dbparam.size   = sizeof(long); 
	             longValue      = new long[m_maxBatchRecordcount];
	             dbparam.m_longValue = longValue ;	          
	             break;
	        case PARAM_TYPE_FLOAT:
	             dbparam.size   = sizeof(float); 
	             floatValue     = new float[m_maxBatchRecordcount];
	             dbparam.m_floatValue = floatValue ;		          
	             break;
	        case PARAM_TYPE_STR:
	             dbparam.size   = sizeof (char ) * MAX_FIELD_LEN; 
	             strValue       = new char[m_maxBatchRecordcount * MAX_FIELD_LEN];	             
	             dbparam.m_strValue = strValue ;	          
	             break;
	        case PARAM_TYPE_DATE:
	             dbparam.size   = sizeof(Date); 
	             dateValue           = new Date[m_maxBatchRecordcount ];	             
	             dbparam.m_dateValue._date = dateValue ;		 
	             dateParam           = new BatchDateParam[m_maxBatchRecordcount ];
	             dbparam.m_dateValue._Param  = dateParam;	                       
	             break;
	      }
	      m_provdata.push_back(dbparam);
	    }
	  }  
	  /*
	    内存释放
	  */
	  void BatchParamclear()
	  {
	    for( int  i = 0 ;  i< m_data.size() ;++i )
	    {
	      switch(m_data[i].m_type)
	      {
	        case PARAM_TYPE_INT:
                 delete [] m_data[i].m_intValue ;break;
	        case PARAM_TYPE_LONG:
                 delete [] m_data[i].m_longValue ; break;
	        case PARAM_TYPE_FLOAT:
                 delete [] m_data[i].m_floatValue ; break ;
	        case PARAM_TYPE_STR:
                 delete [] m_data[i].m_strValue ;break;
	        case PARAM_TYPE_DATE:
                 delete [] m_data[i].m_dateValue._Param ;
                 delete [] m_data[i].m_dateValue._date ;
                 break;
	        default:
	           break;
	      }
	    }
	    m_data.clear();
	    //省中心库
	    for( int  i = 0 ;  i< m_provdata.size() ;++i )
	    {
	      switch(m_provdata[i].m_type)
	      {
	        case PARAM_TYPE_INT:
                 delete [] m_provdata[i].m_intValue   ; break;
	        case PARAM_TYPE_LONG:
                 delete [] m_provdata[i].m_longValue  ; break;
	        case PARAM_TYPE_FLOAT:
                 delete [] m_provdata[i].m_floatValue ; break ;
	        case PARAM_TYPE_STR:
                 delete [] m_provdata[i].m_strValue   ; break;
	        case PARAM_TYPE_DATE:
                 delete [] m_provdata[i].m_dateValue._Param ;
                 delete [] m_provdata[i].m_dateValue._date ;
                 break;
	        default:
	           break;
	      }
	    }	   
	    m_provdata.clear(); 
	    //删除计数的内存指针
	    delete m_CurBatchProvRecordcount;
	    delete m_CurBatchRecordcount;
	    delete m_isExistsRecordProv ;
	    delete m_isExistsRecord ;
	    m_isExistsRecordProv = NULL ;
	    m_isExistsRecord     = NULL ;
	    m_CurBatchProvRecordcount = NULL;
	    m_CurBatchRecordcount  = NULL;
	  }
	  /*
	    复位
	  */
	  void ResetBatchParam()
	  {
	    *m_CurBatchRecordcount     = 0;
	    *m_CurBatchProvRecordcount = 0 ;
	    *m_isExistsRecord     = false ;
	    *m_isExistsRecordProv = false ;
	  }
	  void ResetBatchParambytype(bool isPro)
	  {
	    if (isPro )
	       *m_CurBatchProvRecordcount  = 0;
	    else 
	       *m_CurBatchRecordcount      = 0 ;
	  }
	  /*
	    添加记录
	  */
	  void AddRecordData(int& channel_no , Record* r_data,DbColumn* cols,int ncolsize)
	  {
  	   if ( (m_data.size()) == 0 ) return ; 
  	   *m_isExistsRecord   = true;
//#ifdef _DEBUG_		   
//        printf("AddRecordData\n");
//#endif    	    
  	   // 1.通道号
  	   m_data[0].m_intValue[ *m_CurBatchRecordcount] = channel_no;
  	   // 2.同步序列号
  	   m_data[1].m_longValue[*m_CurBatchRecordcount] = atol( (*r_data)[0] );
  	   // 3.同步类型
  	   m_data[2].m_intValue[*m_CurBatchRecordcount]  = atoi( (*r_data)[1] );	   
  	   // 4.修改标志位
  	   m_data[3].m_intValue[*m_CurBatchRecordcount]  = atoi( (*r_data)[2] );	
  	   // 5.以下是设置列的相关字段
  	   for( int i = 0 ; i < ncolsize ;++i ) 
  	   {
  	     PROCPARAM_TYPE dbcolumntype = cols[i].m_columntype;
  			 char dbgetvalue      = cols[i].m_getvaluetype;
  			 int dbgetvaluepos    = cols[i].m_getvaluepos;
  			 string dbcolumnvalue = (*r_data)[INFO_ADDLEN + dbgetvaluepos];
  			 char* p = NULL ;
  	     switch( dbcolumntype )
  	     {
  	        case PARAM_TYPE_INT:
  	      	     if (dbgetvalue == '1')
  				         m_data[i+4].m_intValue[*m_CurBatchRecordcount] = atol(dbcolumnvalue.c_str())%10000;
  		           else
  				         m_data[i+4].m_intValue[*m_CurBatchRecordcount] = atoi(dbcolumnvalue.c_str());
  				       break;
  				  case PARAM_TYPE_LONG:
  	      	     if (dbgetvalue == '1')
  				         m_data[i+4].m_longValue[*m_CurBatchRecordcount] = atol(dbcolumnvalue.c_str())%10000;
  		           else
  				         m_data[i+4].m_longValue[*m_CurBatchRecordcount] = atol(dbcolumnvalue.c_str());
  				       break;		
  				  case PARAM_TYPE_FLOAT:
  				       m_data[i+4].m_floatValue[ *m_CurBatchRecordcount ] = atof(dbcolumnvalue.c_str());    
  				       break;
  				  case PARAM_TYPE_STR:
  				       p = &(m_data[i+4].m_strValue[ (*m_CurBatchRecordcount)*MAX_FIELD_LEN ] );
  				       memset( p ,0x00 ,sizeof(char)*MAX_FIELD_LEN);
  				       /*如果字段超长的话，自动截断*/
  				       if ( strlen(dbcolumnvalue.c_str())> MAX_FIELD_LEN -1 )
  				          strncpy(p , dbcolumnvalue.c_str(),MAX_FIELD_LEN - 1 );
  				       else
  				          strcpy( p , dbcolumnvalue.c_str());
  				       break;
  				  case PARAM_TYPE_DATE:		
  				       int iyear  = 0 ;
  				       int imonth =	0 ;
  				       int iday   = 0 ;
  				       int ihour  = 0 ;
  				       int iminute =0 ;
  				       int isecond =0 ;      
  				       if (dbcolumnvalue.length() > 0)
  				       {	
          					iyear = atoi(dbcolumnvalue.substr(0,4).c_str());
          					imonth = atoi(dbcolumnvalue.substr(4,2).c_str());
          					iday = atoi(dbcolumnvalue.substr(6,2).c_str());
          					ihour = atoi(dbcolumnvalue.substr(8,2).c_str());
          					iminute = atoi(dbcolumnvalue.substr(10,2).c_str());
          					isecond = atoi(dbcolumnvalue.substr(12,2).c_str());
          					//日期类型需要绑定系统的环境变量，这边目前只记录下日期
          					//真正操作数据库时，需要再次绑定一下该日期类型的字段,!!!!
          				}		
          				m_data[i+4].m_dateValue._Param[*m_CurBatchRecordcount].iYear  = iyear;
          				m_data[i+4].m_dateValue._Param[*m_CurBatchRecordcount].iMonth = imonth;
          				m_data[i+4].m_dateValue._Param[*m_CurBatchRecordcount].iDay   = iday;
          				m_data[i+4].m_dateValue._Param[*m_CurBatchRecordcount].iHour  = ihour;
          				m_data[i+4].m_dateValue._Param[*m_CurBatchRecordcount].iMin   = iminute;
          				m_data[i+4].m_dateValue._Param[*m_CurBatchRecordcount].iSec   = isecond;	
          				break;	            
  	     }
  	   }   	   
  	   ++(*m_CurBatchRecordcount) ;
	  }
	  /*
	   添加中心库的记录
	  */
	  void AddRecordProvData(int& channel_no,Record* r_data,DbColumn* cols,int ncolsize)
	  {
  	   if ( (m_provdata.size()) == 0 ) return ; 
  	   *m_isExistsRecordProv = true ;
//#ifdef _DEBUG_		   
//        printf("AddRecordProvData\n");
//#endif   	    
  	   m_provdata[0].m_intValue [*m_CurBatchProvRecordcount] = channel_no;
  	   //同步序列号
  	   m_provdata[1].m_longValue[*m_CurBatchProvRecordcount] = atol( (*r_data)[0] );
  	   //同步类型
  	   m_provdata[2].m_intValue [*m_CurBatchProvRecordcount] = atoi( (*r_data)[1] );	   
  	   //修改标志位
  	   m_provdata[3].m_intValue [*m_CurBatchProvRecordcount] = atoi( (*r_data)[2] );	
  	   //以下是设置列的相关字段
  	   for( int i = 0 ; i < ncolsize ;++i ) 
  	   {
  	     PROCPARAM_TYPE dbcolumntype = cols[i].m_columntype;
  			 char dbgetvalue      = cols[i].m_getvaluetype;
  			 int dbgetvaluepos    = cols[i].m_getvaluepos;
  			 string dbcolumnvalue = (*r_data)[INFO_ADDLEN + dbgetvaluepos];
  			 char* p = NULL ;
  	     switch( dbcolumntype )
  	     {
  	        case PARAM_TYPE_INT:
  	      	     if (dbgetvalue == '1')
  				        m_provdata[i+4].m_intValue[*m_CurBatchProvRecordcount] = atol(dbcolumnvalue.c_str())%10000;
  		           else
  				        m_provdata[i+4].m_intValue[*m_CurBatchProvRecordcount] = atoi(dbcolumnvalue.c_str());
  				       break;
  				  case PARAM_TYPE_LONG:
  	      	     if (dbgetvalue == '1')
  				        m_provdata[i+4].m_longValue[*m_CurBatchProvRecordcount] = atol(dbcolumnvalue.c_str())%10000;
  		           else
  				        m_provdata[i+4].m_longValue[*m_CurBatchProvRecordcount] = atol(dbcolumnvalue.c_str());
  				       break;		
  				  case PARAM_TYPE_FLOAT:
  				       m_provdata[i+4].m_floatValue[ *m_CurBatchProvRecordcount ] = atof(dbcolumnvalue.c_str());   
  				       break;
  				  case PARAM_TYPE_STR:
  				       p = &(m_provdata[i+4].m_strValue[ (*m_CurBatchProvRecordcount)*MAX_FIELD_LEN ] );
  				       memset( p ,0x0 ,sizeof(char)*MAX_FIELD_LEN);
  				       /*有效性校验*/
  				       if ( strlen(dbcolumnvalue.c_str())> MAX_FIELD_LEN -1 )
  				          strncpy(p , dbcolumnvalue.c_str(),MAX_FIELD_LEN - 1 );
  				       else
  				          strcpy( p , dbcolumnvalue.c_str());
  				       break;
  				  case PARAM_TYPE_DATE:				       
  				       int iyear  = 0 ;
  				       int imonth =	0 ;
  				       int iday   = 0 ;
  				       int ihour  = 0 ;
  				       int iminute =0 ;
  				       int isecond =0 ;      
  				       if (dbcolumnvalue.length() > 0)
  				       {	
          					iyear = atoi(dbcolumnvalue.substr(0,4).c_str());
          					imonth = atoi(dbcolumnvalue.substr(4,2).c_str());
          					iday = atoi(dbcolumnvalue.substr(6,2).c_str());
          					ihour = atoi(dbcolumnvalue.substr(8,2).c_str());
          					iminute = atoi(dbcolumnvalue.substr(10,2).c_str());
          					isecond = atoi(dbcolumnvalue.substr(12,2).c_str());
          					//日期类型需要绑定系统的环境变量，这边目前只记录下日期
          					//真正操作数据库时，需要再次绑定一下该日期类型的字段,!!!!
          				}		
          				m_provdata[i+4].m_dateValue._Param[*m_CurBatchProvRecordcount].iYear  = iyear;
          				m_provdata[i+4].m_dateValue._Param[*m_CurBatchProvRecordcount].iMonth = imonth;
          				m_provdata[i+4].m_dateValue._Param[*m_CurBatchProvRecordcount].iDay   = iday;
          				m_provdata[i+4].m_dateValue._Param[*m_CurBatchProvRecordcount].iHour  = ihour;
          				m_provdata[i+4].m_dateValue._Param[*m_CurBatchProvRecordcount].iMin   = iminute;
          				m_provdata[i+4].m_dateValue._Param[*m_CurBatchProvRecordcount].iSec   = isecond;	
          				break;	            
  	     }
  	   }   	   
  	   ++(*m_CurBatchProvRecordcount) ;
	  }
	  long getCurRecordsize()     {
	    //#ifdef _DEBUG_
	   //    printf("getCurRecordsize = %d \n",*m_CurBatchRecordcount);
	   // #endif
	    return *m_CurBatchRecordcount; 
	    
	    };
	  long getCurRecordProvsize() {
	   // #ifdef _DEBUG_
	  //     printf("getCurRecordProvsize = %d \n",*m_CurBatchProvRecordcount);
	   // #endif	    
	    return *m_CurBatchProvRecordcount;
	    
	   };
	  /*
	   根据数据库名称取得入物理库的临时表名称,系统默认所有库都是用一临时表
	   也可以分开进行配置，使之【个性化】
	  */
	  void getTempTableName(string& r_dbname ,string& r_tablename )
	  {
	    r_tablename = "";
	    vector<string> tmpobjdbnameVector;
	    vector<string> tmpobjtablenameVector;
	    tmpobjdbnameVector.clear();
	    tmpobjtablenameVector.clear();
	    StringUtil::split(m_obj_db_tablenames, ";", tmpobjdbnameVector, false);	
	    if ( m_obj_db_tablenames.size() == 0 ) return ;
	    if ( tmpobjdbnameVector.size() > 1 ) 
	    {
	      for (vector<string>::iterator itert=tmpobjdbnameVector.begin(); itert!=tmpobjdbnameVector.end(); ++itert)
	      {
		        tmpobjtablenameVector.clear();
		        StringUtil::split(*itert, "|", tmpobjtablenameVector, false);
		        if ( tmpobjtablenameVector.size()>1 )
		        {
		            //如果是指定的库(不区分大小写)
		            if ( tmpobjtablenameVector[0] == r_dbname ) 
		            {
		              r_tablename = tmpobjtablenameVector[1];
		              break;
		            }
		        }	        
	      }
	    }else
	      r_tablename = tmpobjdbnameVector[0];
	    //如果没找到建议，抛异常
	    if ( r_tablename.size() == 0 ) {}
	  }
	  /*
	   根据数据库名称取得入物理库的存储过程名称
	  */
	  void getActProcName(string& r_dbname ,string& r_procname )
	  {
	    r_procname = "";
	    vector<string> tmpobjdbnameVector;
	    vector<string> tmpobjprocnameVector;
	    StringUtil::split(m_obj_db_procs, ";", tmpobjdbnameVector, false);	
	    if ( m_obj_db_procs.size() == 0 ) return ;
	    if ( tmpobjdbnameVector.size() > 1 ) 
	    {
	      for (vector<string>::iterator itert=tmpobjdbnameVector.begin(); itert!=tmpobjdbnameVector.end(); ++itert)
	      {
		        tmpobjprocnameVector.clear();
		        StringUtil::split(*itert, "|", tmpobjprocnameVector, false);
		        if ( tmpobjprocnameVector.size()>1 )
		        {
		            //如果是指定的库(不区分大小写)
		            if ( tmpobjprocnameVector[0] == r_dbname ) 
		            {
		              r_procname = tmpobjprocnameVector[1];
		              break;
		            }
		        }	        
	      }
	    }else
	      r_procname = tmpobjdbnameVector[0];
	    //如果没找到建议，抛异常
	    if ( r_procname.size() == 0 ) {}	      
	  }
	  /*
	   根据数据库名称取得入物理库的临时表名称（省中心库模式）
	  */
	  void getProvTempTableName(string& r_dbname ,string& r_tablename )
	  {
	    r_tablename = "";
	    vector<string> tmpobjdbnameVector;
	    vector<string> tmpobjtablenameVector;
	    StringUtil::split(m_obj_provdb_tablenames, ";", tmpobjdbnameVector, false);	
	    if ( m_obj_provdb_tablenames.size() == 0 ) return ;
	    if ( tmpobjdbnameVector.size() > 1 ) 
	    {
	      for (vector<string>::iterator itert=tmpobjdbnameVector.begin(); itert!=tmpobjdbnameVector.end(); ++itert)
	      {
		        tmpobjtablenameVector.clear();
		        StringUtil::split(*itert, "|", tmpobjtablenameVector, false);
		        if ( tmpobjtablenameVector.size()>1 )
		        {
		           //如果是指定的库(不区分大小写)
		            if ( tmpobjtablenameVector[0] == r_dbname ) 
		            {
		              r_tablename = tmpobjtablenameVector[1];
		              break;
		            }
		        }	        
	      }
	    }else
	      r_tablename = tmpobjdbnameVector[0];
	    //如果没找到建议，抛异常

	  }
	  /*
	   根据数据库名称取得入物理库的存储过程名称（省中心库模式）
	  */	  
	  void getProvActProcName(string& r_dbname ,string& r_procname )
	  {
	    r_procname = "";
	    vector<string> tmpobjdbnameVector;
	    vector<string> tmpobjprocnameVector;
	    StringUtil::split(m_obj_provdb_procs, ";", tmpobjdbnameVector, false);	
	    if ( m_obj_provdb_procs.size() == 0 ) return ;
	    if ( tmpobjdbnameVector.size() > 1 ) 
	    {
	      for (vector<string>::iterator itert=tmpobjdbnameVector.begin(); itert!=tmpobjdbnameVector.end(); ++itert)
	      {
		        tmpobjprocnameVector.clear();
		        StringUtil::split(*itert, "|", tmpobjprocnameVector, false);
		        if ( tmpobjprocnameVector.size()>1 )
		        {
		           //如果是指定的库(不区分大小写)
		            if ( tmpobjprocnameVector[0] == r_dbname ) 
		            {
		              r_procname = tmpobjprocnameVector[1];
		              break;
		            }
		        }	        
	      }
	    }else
	      r_procname = tmpobjdbnameVector[0];
	    //如果没找到建议，抛异常
	    if ( r_procname.size() == 0 ) {}	      
	  }
//--------------------------------end----------------		  			
};




//(2.6)需要载入的内存表描述(结构)



//(2.7)需要载入表载入信息描述(结构)
struct LoadTable
{
	int			m_infotype;						//信息类型
	int			m_infofieldnum;					//信息列数
	vector<int>	m_infokeyfieldposVector;		//信息关键字位置集,从0开始

	string		m_dbtablename;					//数据库表名
	int			m_dbcolumnnum;					//数据库表列数
	vector<int>	m_dbkeycolumnposVector;			//数据库关键字位置集,从0开始 
	DbColumn	m_dbcolumns[MAX_COLUMN_NUM];	//数据库列描述

	string		m_mmtablename;					//内存库表名
	int			m_mmcolumnnum;					//内存库表列数
	string		m_mmkeyindexname;				//内存库关键字更新基于的索引名称
	vector<int>	m_mmkeycolumnposVector;			//内存库关键字位置集,从0开始
	string		m_mmindexnamepk;				//内存库增量更新基于的主键名称
	//MmColumn	m_mmcolumns[MAX_COLUMN_NUM];	//内存库列描述

	char		m_triggerflag;					//触发参数刷新标志
	
	public:
		void parseinfokeyfieldpos(const string& s)
		{
			m_infokeyfieldposVector.clear();
			
			string d = "|";
			vector<string> sVector;
			StringUtil::split(s, d, sVector, false);
			for (int i=0; i<sVector.size(); ++i)
			{
				m_infokeyfieldposVector.push_back(atoi(sVector[i].c_str()));
			}
		}


		void parsedbkeyfieldpos(const string& s)
		{
			m_dbkeycolumnposVector.clear();
			
			string d = "|";
			vector<string> sVector;
			StringUtil::split(s, d, sVector, false);
			for (int i=0; i<sVector.size(); ++i)
			{
				m_dbkeycolumnposVector.push_back(atoi(sVector[i].c_str()));
			}
		}


		void parsemmkeyfieldpos(const string& s)
		{
			m_mmkeycolumnposVector.clear();
			
			string d = "|";
			vector<string> sVector;
			StringUtil::split(s, d, sVector, false);
			for (int i=0; i<sVector.size(); ++i)
			{
				m_mmkeycolumnposVector.push_back(atoi(sVector[i].c_str()));
			}
		}


		void parseDbColumns(const string& s)
	  	{
			string d1 = ";";
			vector<string> s1Vector;
			StringUtil::split(s, d1, s1Vector, false);
			for (int i=0; i<s1Vector.size(); ++i)
			{
				string d2 = "|";
				vector<string> s2Vector;
				StringUtil::split(s1Vector[i], d2, s2Vector, false);
				
				if(s2Vector[0] == "0")
				{  
					m_dbcolumns[i].m_columntype = PARAM_TYPE_INT;           
          		}
          		else if(s2Vector[0] == "1")  
          		{
          			m_dbcolumns[i].m_columntype = PARAM_TYPE_LONG;     
          		}
          		else if(s2Vector[0] == "2")
          		{
          			m_dbcolumns[i].m_columntype = PARAM_TYPE_FLOAT;   
          		}
          		else if(s2Vector[0] == "3")
          		{
          			m_dbcolumns[i].m_columntype = PARAM_TYPE_STR;     
          		}
          		else if(s2Vector[0] == "4")
          		{
          			m_dbcolumns[i].m_columntype = PARAM_TYPE_DATE;
				}
				
				m_dbcolumns[i].m_keytag = atoi(s2Vector[1].c_str());
	        	m_dbcolumns[i].m_columnname = s2Vector[2];
	        	m_dbcolumns[i].m_getvaluetype = (s2Vector[3].c_str())[0];
	        	m_dbcolumns[i].m_getvaluepos = atoi(s2Vector[4].c_str());
			}
		}


		void parseMmColumns(const string& s)
		{
			string d1 = ";";
			vector<string> s1Vector;
			StringUtil::split(s, d1, s1Vector, false);
			for (int i=0; i<s1Vector.size(); ++i)
			{
				string d2 = "|";
				vector<string> s2Vector;
				StringUtil::split(s1Vector[i], d2, s2Vector, false);
				
				if(s2Vector[0] == "0")
				{  
					//m_mmcolumns[i].m_columntype = VAR_TYPE_INT2;
          		}
          		else if(s2Vector[0] == "1")  
          		{
          			//m_mmcolumns[i].m_columntype = VAR_TYPE_INT;
          		}
          		else if(s2Vector[0] == "2")
          		{
          			//m_mmcolumns[i].m_columntype = VAR_TYPE_DATE;
          		}
          		else if(s2Vector[0] == "3")
          		{
          			//m_mmcolumns[i].m_columntype = VAR_TYPE_LONG;
          		}
          		else if(s2Vector[0] == "4")
          		{
          			//m_mmcolumns[i].m_columntype = VAR_TYPE_REAL;
				}
          		else if(s2Vector[0] == "5")
          		{
          			//m_mmcolumns[i].m_columntype = VAR_TYPE_NUMSTR;
				}
          		else if(s2Vector[0] == "6")
          		{
          			//m_mmcolumns[i].m_columntype = VAR_TYPE_VSTR;
				}
          		else if(s2Vector[0] == "7")
          		{
          			//m_mmcolumns[i].m_columntype = VAR_TYPE_UNKNOW;
				}
																
				//m_mmcolumns[i].m_keytag = atoi(s2Vector[1].c_str());
				//m_mmcolumns[i].m_columnname = s2Vector[2];
				//m_mmcolumns[i].m_getvaluetype = (s2Vector[3].c_str())[0];
				//m_mmcolumns[i].m_getvaluepos = atoi(s2Vector[4].c_str());
			}
		};
};


//(2.8)物理数据库描述(结构)
struct DbDesc
{
	string				m_username;
	string				m_password;
	string				m_servname;	
};


//(2.9)内存数据库描述(结构)



//(2.10)表载入信息描述
struct TableLoadRule
{
	char		  m_loadtype;
	string		m_srcdbname;
	string		m_srcselsql;
	string		m_objmmnames;
	string		m_tablename;
	string		m_objupdatesql;
	char		  m_triggermethod;
	char		  m_triggertag;
	int			  m_intervaltime;
	string		m_updatetime;
	string		m_addupdatetime;
	int			  m_loadruleid;
};
//(2.11) CRM全量无缝同步载入参数描述 by xueyf@lianchuang.com
struct TabCrmLoadRule
{
      int    m_infotype;
      string      m_srcdbcrmname;
      string      m_srcselsql;
      string      m_objactnames;
      string    m_use;
      string      m_tablename;
      string      m_objupdatesql;
      string    m_pgbegname;
      string    m_pgendname;
      char        m_triggertag;
      int     m_loadruleid;
};
struct Synrule
{
    int         m_infotype;
    string      m_objactnames;
};
#endif

