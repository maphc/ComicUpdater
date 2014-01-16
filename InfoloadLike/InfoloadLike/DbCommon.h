#ifndef _DbCommon_H_
#define _DbCommon_H_

#include "stdafx.h"

using namespace oracle::occi;

//(1.1)���ݿ���س������Զ���
//���ݿ�һ�ζ�ȡ�ļ�¼��
const unsigned int	DEFAULT_RECORD_NUM = 10000;
//����������ʹ�ã�OEC��by xueyf@lianchuang.com 2010-6-29 10:32:58
const unsigned int      MAX_RECORD_NUM = 20000;
const unsigned int MAX_STR_NUM = 401;
const unsigned int MAX_CHAR_ARRAY_SIZE =100;
//----end
//���ݿ��ַ���ֵ����󳤶�
const unsigned int MAX_DBSTRVALUE_LEN = 3072;

//����������:������
enum PROCPARAM_ACTION {PARAM_ACT_IN, PARAM_ACT_OUT};

//�������Ͷ���:���͡������͡������͡������͡��ַ���
enum PROCPARAM_TYPE {PARAM_TYPE_INT, PARAM_TYPE_LONG, PARAM_TYPE_FLOAT, PARAM_TYPE_DATE, PARAM_TYPE_STR};

/**
* DbParam:�������ݿ�Ĳ����ṹ��
*/
struct DbParam
{
	//����˳��,��1��ʼ����
	unsigned int		m_seq;
	//��������
	PROCPARAM_ACTION	m_action;
	//��������
	PROCPARAM_TYPE		m_type;
	//����ֵ
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
  BatchDbParam:�����������ݿ�Ĳ����ṹ wangfc@lianchuang.com at 2010.03.29
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
	//����˳��,��1��ʼ����
	unsigned int      m_seq ;
	//��������
	PROCPARAM_TYPE		m_type;
	int               size ;
	//����ֵ
	union
	{
		int*    m_intValue ;
		long*   m_longValue ;
		float*  m_floatValue ;
		char*   m_strValue;

	};
	struct  
	{ 
		BatchDateParam*  _Param ;//�����������ʵ�֣��ڶ��ΰ�
		Date*	_date;
	} m_dateValue;
};

struct DataParam
{
  unsigned int      m_seq ;
  unsigned int      m_type;
  unsigned int      m_size ;
  void*             m_data ;
  short*              m_dataElemSize ;//ÿ��Ԫ�ش�С
};
//----end


#endif

