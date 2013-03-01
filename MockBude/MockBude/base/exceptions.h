#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_

#include "base/Exception.h"

/******************************************************************************/
/*                                                                            */
/*  ϵͳԭʼ�쳣                                                               */
/*  �쳣��ʹ�õĴ������, ֱ��ʹ��ԭ���Ĵ������                                 */
/*                                                                            */
/******************************************************************************/

/**
 * ����ϵͳ��ԭʼ���쳣  
 */ 
DECLARE_EXCEPTION(PrimalException, Exception);

/**
 * ����Oracle���ݿ��쳣
 */ 
DECLARE_EXCEPTION(OracleException, PrimalException);

/**
 * ����Tuxedo�м���쳣
 */ 
DECLARE_EXCEPTION(TuxedoException, PrimalException);

/**
 * �������ϵͳ����
 */ 
DECLARE_EXCEPTION(UnixException, PrimalException);

/**
 * ����IO���쳣
 */ 
DECLARE_EXCEPTION(IOException, PrimalException);

/**
 * �����ļ�ϵͳ�쳣
 */ 
DECLARE_EXCEPTION(FileSystemException, IOException);

/**
 * ��������ͨѶ�쳣
 */ 
DECLARE_EXCEPTION(NetException, IOException);

/******************************************************************************/
/*                                                                            */
/*  �����쳣                                                                             */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

DECLARE_EXCEPTION(AssertException, Exception);
#define ASSERT(x) if(!(x)) THROW(AssertException, #x)

/******************************************************************************/
/*                                                                            */
/*  ��������쳣                                                               */
/*  �����������̳�, ��ֱ��ʹ�ô��쳣                                          */
/*                                                                            */
/******************************************************************************/

/**
 * ������������쳣
 */
DECLARE_EXCEPTION(BaseException, Exception);
/**
 * ���ݷ��ʲ��쳣
 */
DECLARE_EXCEPTION(DAOException, BaseException);

/******************************************************************************/
/*                                                                            */
/*  ҵ������쳣                                                               */
/*  ҵ���������̳�, ��ֱ��ʹ�ô��쳣                                          */
/*                                                                            */
/******************************************************************************/

/**
 * ҵ�����
 */
DECLARE_EXCEPTION(BusinessException, Exception);

DECLARE_EXCEPTION(CRMException, BusinessException);

#endif //_EXCEPTIONS_H_
