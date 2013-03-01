// MWconfig-all.h  --  the configuration file 
// Language:    C++
// 
// Authors:     Weiguo Wang <wangwg@lianchuang.com>
// 
// History:
// Copyright (C) 2001 by Weiguo Wang, Linkage. All Rights Reserved.
// 
// Comments: configuration for different platforms that application runs on,
//       different c++ compliers.
//       It also includes debug, trace and test mechanism.
//       Based on preprocessor definitions, it can includes other more 
//       specific configuration files.
// 

#ifndef  CONFIG_ALL_H_
#define  CONFIG_ALL_H_

/////////////////////////////////////////////////////////////////////////////
// os platforms support specials

// define different os platforms
// HP UNIX                HPUX_OS_CONFIG    (hpux, __hpux__, __hpux)
// IBM AIX                IBMAIX_OS_CONFIG  (_AIX)
// Linux                  LINUX_OS_CONFIG   (__linux__)
// DEC UNIX               DECUNIX_OS_CONFIG (DGUX)
// Sun Solaris            SUN_OS_CONFIG   (SUNOS)
// Microsoft Windows 32   WIN32_OS_CONFIG   (WIN32)

// HP UNIX
//#define HPUX_OS_CONFIG
#if defined(hpux) || defined(__hpux__) || defined(__hpux)
#define HPUX_OS_CONFIG
#endif

// IBM AIX
//#define IBMAIX_OS_CONFIG
#if defined(_AIX)
#define IBMAIX_OS_CONFIG
#endif

// Linux OS 
//#define LINUX_OS_CONFIG
#if defined(__linux__)
#define LINUX_OS_CONFIG
#endif

// Digital UNIX
//#define DECUNIX_OS_CONFIG
#if defined(__digital__)
#define DECUNIX_OS_CONFIG
#endif

// Sun Solaris 
//#define SUN_OS_CONFIG
#if defined(__sun)
#define SUN_OS_CONFIG
#endif

// Microsoft Win32
//#define WIN32_OS_CONFIG
#if defined(WIN32)
#define WIN32_OS_CONFIG
#endif



// HP UNIX                HPUX_OS_CONFIG    (hpux, __hpux__, __hpux)
// IBM AIX                IBMAIX_OS_CONFIG  (_AIX)
// Linux                  LINUX_OS_CONFIG   (__linux__)
// DEC UNIX               DECUNIX_OS_CONFIG (DGUX)
// Sun Solaris            SUN_OS_CONFIG   (SUNOS)
// Microsoft Windows 32   WIN32_OS_CONFIG   (WIN32)

/////////////////////////////////////////////////////////////////////////////
// os special configuration
//if define VFS_INCLUDE then include "sys/vfs.h"
#define VFS_INCLUDE

#if defined(HPUX_OS_CONFIG)
#define MMV_HP
#endif

#if defined(IBMAIX_OS_CONFIG)
#define MMV_IBM
#endif

#if defined(LINUX_OS_CONFIG)
#define MMV_LNX
#endif

#if defined(DECUNIX_OS_CONFIG)
#define MMV_DEC
#undef VFS_INCLUDE
#endif

#if defined(SUN_OS_CONFIG)
#define MMV_SUN
#endif

#if defined(WIN32_OS_CONFIG)
#endif



/////////////////////////////////////////////////////////////////////////////
// c++ compiler support specials 

// HP ACC                 HPACC_CONFIG    (__HP_aCC)
// IBM Visual Age C++     IBMCPP_CONFIG   (__IBMCPP__)
// Gnu g++                GNUCPP_CONFIG   (__GNUC__)
// DEC CXX                DECCXX_CONFIG   (__DECCXX_VER)
// Sun Workshop C++       SUNCPP_CONFIG   (__SUNPRO_CC)
// Microsoft Visual C++   MSVCPP_CONFIG   (_MSC_VER) 

// HP aCC
//#define HPACC_CONFIG
#if defined(__HP_aCC)
#define HPACC_CONFIG
#endif

// IBM Visual Age C++
//#define IBMCPP_CONFIG
#if defined(__IBMCPP__)
#define IBMCPP_CONFIG
#endif

// Gnu g++
//#define GNUCPP_CONFIG
#if defined(__GNUC__)
#define GNUCPP_CONFIG
#endif

// DEC CXX
//#define DECCXX_CONFIG
#if defined(__DECCXX_VER)
#define DECCXX_CONFIG
#endif

// Sun Workshop C++
//#define SUNCPP_CONFIG
#if defined(__SUNPRO_CC)
#define SUNCPP_CONFIG
#endif

// Mocrosoft Visual C++
//#define MSVCPP_CONFIG
#if defined(_MSC_VER)
#define MSVCPP_CONFIG
#endif


/////////////////////////////////////////////////////////////////////////////
// os and compiler special configuration
#if defined(MSVCPP_CONFIG)
//#include "StdAfx.h"
#pragma warning(disable:4786)
#endif


/////////////////////////////////////////////////////////////////////////////
// support options

// compiler support namespace or not
#define NAMESPACE_DISABLE

// support standard template i/o stream
#define STDIOSTREAM_DISABLE

// support member template functions
#define MEMBER_TEMPLATE_FUNCTION_DISABLE

//
#define REL_OPS_NAMESPACE_DISABLE

#if defined(MSVCPP_CONFIG)
#undef NAMESPACE_DISABLE
#undef STDIOSTREAM_DISABLE
#endif

#if defined(GNUCPP_CONFIG)
#undef NAMESPACE_DISABLE
#undef STDIOSTREAM_DISABLE
#undef MEMBER_TEMPLATE_FUNCTION_DISABLE
#endif

#if defined(GNUCPP_CONFIG)
#define NONSTANDARD_STRING_COMPARE
#endif

#if defined(HPACC_CONFIG)
#undef NAMESPACE_DISABLE
#undef STDIOSTREAM_DISABLE
#undef MEMBER_TEMPLATE_FUNCTION_DISABLE
#endif


#if defined(IBMCPP_CONFIG)
#undef MEMBER_TEMPLATE_FUNCTION_DISABLE
#undef NAMESPACE_DISABLE
#undef STDIOSTREAM_DISABLE
#undef REL_OPS_NAMESPACE_DISABLE
#endif

#if defined(SUNCPP_CONFIG)
#undef MEMBER_TEMPLATE_FUNCTION_DISABLE
#undef NAMESPACE_DISABLE
#undef STDIOSTREAM_DISABLE
#undef REL_OPS_NAMESPACE_DISABLE
#endif

#if defined(DECCXX_CONFIG)
#undef MEMBER_TEMPLATE_FUNCTION_DISABLE
#undef NAMESPACE_DISABLE
#undef REL_OPS_NAMESPACE_DISABLE
//#ifndef __USE_STD_IOSTREAM
//#define __USE_STD_IOSTREAM
//#endif
#ifndef __STD_STRICT_ANSI
#define __STD_STRICT_ANSI
#endif
#endif

/////////////////////////////////////////////////////////////////////////////
// different config support

// USING_NAMESPACE

#define USING_NAMESPACE(x) using namespace x;


#if defined(REL_OPS_NAMESPACE_DISABLE)
#define USING_REL_OPS_NAMESPACE
#else
#define USING_REL_OPS_NAMESPACE using namespace std::rel_ops;using std::rel_ops::operator!=;\
using std::rel_ops::operator>;using std::rel_ops::operator<=;using std::rel_ops::operator>=;
#endif // REL_OPS_NAMESPACE_DISABLE


#define dumpstream cout 


typedef counted_ptr < CFmlBuf >  PCFmlBuf;
typedef counted_ptr < Exception >  PException;

#endif // CONFIG_ALL_H_


