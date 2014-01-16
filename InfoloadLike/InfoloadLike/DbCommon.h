#ifndef _DbCommon_H_
#define _DbCommon_H_

#include "stdafx.h"

using namespace oracle::occi;

//(1.1)数据库相关常量属性定义
//数据库一次读取的记录数
const unsigned int	DEFAULT_RECORD_NUM = 10000;
//批量导数据使用（OEC）by xueyf@lianchuang.com 2010-6-29 10:32:58
const unsigned int      MAX_RECORD_NUM = 20000;
const unsigned int MAX_STR_NUM = 401;
const unsigned int MAX_CHAR_ARRAY_SIZE =100;
//----end
//数据库字符串值的最大长度
const unsigned int MAX_DBSTRVALUE_LEN = 3072;

//参数方向定义:进、出
enum PROCPARAM_ACTION {PARAM_ACT_IN, PARAM_ACT_OUT};

//参数类型定义:整型、长整型、浮点型、日期型、字符型
enum PROCPARAM_TYPE {PARAM_TYPE_INT, PARAM_TYPE_LONG, PARAM_TYPE_FLOAT, PARAM_TYPE_DATE, PARAM_TYPE_STR};

/**
* DbParam:物理数据库的参数结构。
*/
struct DbParam
{
	//参数顺序,从1开始递增
	unsigned int		m_seq;
	//参数方向
	PROCPARAM_ACTION	m_action;
	//参数类型
	PROCPARAM_TYPE		m_type;
	//参数值
	union
	{
		int		m_intValue;
		long	m_longValue;
		float	m_floatValue;
		char	m_dateValue[15];
		char	m_strValue[MAX_DBSTRVALUE_LEN];
	};
};
/*
  BatchDbParam:批量插入数据库的参数结构 wangfc@lianchuang.com at 2010.03.29
*/
struct BatchDateParam
{
  int iYear ;
  int iMonth ;
  int iDay ;
  int iHour;
  int iMin;
  int iSec;
};



struct BatchDbParam{
	//参数顺序,从1开始递增
	unsigned int      m_seq ;
	//参数类型
	PROCPARAM_TYPE		m_type;
	int               size ;
	//参数值
	union
	{
		int*    m_intValue ;
		long*   m_longValue ;
		float*  m_floatValue ;
		char*   m_strValue;

	};
	struct  
	{ 
		BatchDateParam*  _Param ;//配合日期类型实现，第二次绑定
		Date*	_date;
	} m_dateValue;
};

struct DataParam
{
  unsigned int      m_seq ;
  unsigned int      m_type;
  unsigned int      m_size ;
  void*             m_data ;
  short*              m_dataElemSize ;//每个元素大小
};
//----end


#endif

