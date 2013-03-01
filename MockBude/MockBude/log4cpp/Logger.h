#ifndef _LOGGER_H_
#define _LOGGER_H_
#include "../base/config-all.h"
#include "../base/Exception.h"
#include <vector>
#include <map>
#include <stdarg.h>
#include <stdio.h>     

using namespace std;

namespace log4cpp {



    /**
     * 定义日志器
     * 使用方法: 
     * static Logger& logger = Logger::getLogger("com.linkage.eboss.crm.createuser");
     * //...
     * logger.info("新开用户%s", userid);
     * 
     */ 
    class Logger {
	private:
		Logger(string c=""){
			category=c;
		}
		virtual ~Logger(){
			delete _inst;
		}

		static Logger* _inst;
		const static char* fmt;
		string category;
		
    public:
		
        
		


        /**
         * @param dn 描述对象的唯一路径, 命名规则使用"."进行分割, 如划价通道一的键 eboss.cbs.billing.rate.ch1 营业开户的键 eboss.crm.createuser
         * @return 返回日志对象
         */
		static Logger & getLogger(string dn){
			if(_inst==NULL){
				_inst=new Logger(dn);
			}
			return *_inst;
		}

		bool isTraceEnabled(){
			return true;
		}
		bool isDebugEnabled(){
			return true;
		}
		bool isAlertEnabled(){
			return true;
		}
        
        /** 跟踪信息 YYYY-mm-dd HH-MM-SS.QQQ TRACE [dn] FILE:LINE message */
		virtual void trace(const char * file, int line, const char * msg, ...) {

			char buf[1000] = { 0 };
			sprintf(buf, fmt,category.c_str(), file, line, msg);
			va_list ap;
			va_start(ap, msg);
			vprintf(buf, ap);
			va_end(ap);
		}
		virtual void debug(const char * file, int line, const char * msg, ...) {

			char buf[1000] = { 0 };
			sprintf(buf, fmt, category.c_str(), file, line, msg);
			va_list ap;
			va_start(ap, msg);
			vprintf(buf, ap);
			va_end(ap);
		}
		virtual void alert(const char * file, int line, const char * msg, ...) {

			char buf[1000] = { 0 };
			sprintf(buf, fmt, category.c_str(), file, line, msg);
			va_list ap;
			va_start(ap, msg);
			vprintf(buf, ap);
			va_end(ap);
		}
		virtual void alert(Exception& e){
			cout<<"[ALERT][EXCEPTION]"<<e.toString()<<endl;
		}
        
    };
	


};



#define LOG_TRACE(log, msg) {if(log.isTraceEnabled()) log.trace(__FILE__, __LINE__, msg);}
#define LOG_TRACE_P1(log, msg, p1) {if(log.isTraceEnabled()) log.trace(__FILE__, __LINE__, msg, p1);}
#define LOG_TRACE_P2(log, msg, p1, p2) {if(log.isTraceEnabled()) log.trace(__FILE__, __LINE__, msg, p1, p2);}
#define LOG_TRACE_P3(log, msg, p1, p2, p3) {if(log.isTraceEnabled()) log.trace(__FILE__, __LINE__, msg, p1, p2, p3);}
#define LOG_TRACE_P4(log, msg, p1, p2, p3, p4) {if(log.isTraceEnabled()) log.trace(__FILE__, __LINE__, msg, p1, p2, p3, p4);}

#define LOG_DEBUG(log, msg) {if(log.isDebugEnabled()) log.debug(__FILE__, __LINE__, msg);}
#define LOG_DEBUG_P1(log, msg, p1) {if(log.isDebugEnabled()) log.debug(__FILE__, __LINE__, msg, p1);}
#define LOG_DEBUG_P2(log, msg, p1, p2) {if(log.isDebugEnabled()) log.debug(__FILE__, __LINE__, msg, p1, p2);}
#define LOG_DEBUG_P3(log, msg, p1, p2, p3) {if(log.isDebugEnabled()) log.debug(__FILE__, __LINE__, msg, p1, p2, p3);}
#define LOG_DEBUG_P4(log, msg, p1, p2, p3, p4) {if(log.isDebugEnabled()) log.debug(__FILE__, __LINE__, msg, p1, p2, p3, p4);}

#define LOG_ALERT(log, msg) {if(log.isAlertEnabled()) log.alert(__FILE__, __LINE__, msg);}
#define LOG_ALERT_P1(log, msg, p1) {if(log.isAlertEnabled()) log.alert(__FILE__, __LINE__, msg, p1);}
#define LOG_ALERT_P2(log, msg, p1, p2) {if(log.isAlertEnabled()) log.alert(__FILE__, __LINE__, msg, p1, p2);}
#define LOG_ALERT_P3(log, msg, p1, p2, p3) {if(log.isAlertEnabled()) log.alert(__FILE__, __LINE__, msg, p1, p2, p3);}
#define LOG_ALERT_P4(log, msg, p1, p2, p3, p4) {if(log.isAlertEnabled()) log.alert(__FILE__, __LINE__, msg, p1, p2, p3, p4);}



#endif //_LOGGER_H_
