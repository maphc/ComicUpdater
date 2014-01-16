#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_

#include "base/Exception.h"

/******************************************************************************/
/*                                                                            */
/*  系统原始异常                                                               */
/*  异常中使用的错误编码, 直接使用原生的错误编码                                 */
/*                                                                            */
/******************************************************************************/

/**
 * 定义系统级原始的异常  
 */ 
DECLARE_EXCEPTION(PrimalException, Exception);

/**
 * 定义Oracle数据库异常
 */ 
DECLARE_EXCEPTION(OracleException, PrimalException);

/**
 * 定义Tuxedo中间件异常
 */ 
DECLARE_EXCEPTION(TuxedoException, PrimalException);

/**
 * 定义操作系统错误
 */ 
DECLARE_EXCEPTION(UnixException, PrimalException);

/**
 * 定义IO流异常
 */ 
DECLARE_EXCEPTION(IOException, PrimalException);

/**
 * 定义文件系统异常
 */ 
DECLARE_EXCEPTION(FileSystemException, IOException);

/**
 * 定义网络通讯异常
 */ 
DECLARE_EXCEPTION(NetException, IOException);

/******************************************************************************/
/*                                                                            */
/*  断言异常                                                                             */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

DECLARE_EXCEPTION(AssertException, Exception);
#define ASSERT(x) if(!(x)) THROW(AssertException, #x)

/******************************************************************************/
/*                                                                            */
/*  技术组件异常                                                               */
/*  技术组件必须继承, 或直接使用此异常                                          */
/*                                                                            */
/******************************************************************************/

/**
 * 公共技术组件异常
 */
DECLARE_EXCEPTION(BaseException, Exception);
/**
 * 数据访问层异常
 */
DECLARE_EXCEPTION(DAOException, BaseException);

/******************************************************************************/
/*                                                                            */
/*  业务组件异常                                                               */
/*  业务组件必须继承, 或直接使用此异常                                          */
/*                                                                            */
/******************************************************************************/

/**
 * 业务错误
 */
DECLARE_EXCEPTION(BusinessException, Exception);

DECLARE_EXCEPTION(CRMException, BusinessException);

#endif //_EXCEPTIONS_H_
