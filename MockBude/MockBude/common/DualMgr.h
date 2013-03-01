#ifndef DualMGR_H_HEADER_INCLUDED_BE13A0DE
#define DualMGR_H_HEADER_INCLUDED_BE13A0DE

#include <iostream>
#include <string>
#include <stdio.h>
#include "../base/config-all.h"

USING_NAMESPACE(std)

/**
 *  生成流水号
 */
extern "C" void GenerateID(CFmlBuf &inBuf,CFmlBuf &outBuf);

/**
 *  获取当前系统时间
 */
extern "C" void GetSysDate(CFmlBuf &inBuf,CFmlBuf &outBuf);

/**
 *  获取下月第一天的时间
 */
extern "C" void GetFirstDayOfNextMonth(CFmlBuf &inBuf,CFmlBuf &outBuf);

/**
 *  获取本月最后一天的时间
 */
extern "C" void GetLastDayOfCurMonth(CFmlBuf &inBuf,CFmlBuf &outBuf);

/**
 *  获取帐期
 */
extern "C" void GetAcycId(CFmlBuf &inBuf,CFmlBuf &outBuf);

/**
 *  根据员工获取归属部门        jianghp@20050601 add
 */
extern "C" void GetDepartidByStaffid(CFmlBuf &inBuf,CFmlBuf &outBuf);

/**
 *  根据部门获取归属业务区        jianghp@20050601 add
 */
extern "C" void GetCitycodeByDepartid(CFmlBuf &inBuf,CFmlBuf &outBuf);

/** @class DualMgr
 *  @brief Dual管理器
 *  提供对数据库Dual的所有操作方法，如获取序列、当前日期等。
 */
//##ModelId=41EC613D001E
class DualMgr
{
    public:
        /**
         * 根据传入的ID类型通过数据库的序列生成各种ID
         * @param eparchyCode 输入地市编码字符串的引用
         * @param SequenceName 输入序列类型字符串的引用
         * @param SequenceId 输出序列号字符串的引用
         * @return 无，失败时抛出例外
         */
        //##ModelId=41EC62590239
        void GenerateID(string &eparchyCode, string &sequenceName, string &sequenceId);

        /**
         *  获取传入时间与系统时间差值
         */
        static void GetOddsDays(const string &strCurdate,string &strStartDateTim);

        /**
         * 获取系统时间
         * @param strSysdate 输出系统时间字符串的引用
         * @return 无，失败时抛出例外
         */
        void GetSysDate(string &strSysdate);

        /**
         * 获取帐期
         * @param iAcycId 输出帐期的引用
         * @param getMode 获取方式：0-获取当前帐期，1-获取最小未开帐帐期，2-根据时间获取帐期
         * @param parameter getMode=2时的输入时间
         * @return 无，失败时抛出例外
         */
        void GetAcycId(int &iAcycId, const int &getMode, const string &parameter);

        /**
         * 获取下月第一天
         * @param strDateTime 输出时间字符串的引用
         * @return 无，失败时抛出例外
         */
        static void GetFirstDayOfNextMonth(string &strDateTime);

        /**
         * 获取指定时间＋n天的时间
         */
        static void AddDays(const string &strStartDateTime,const int &iDays, string &strResultDateTime);

        /**
         * 获取指定时间＋n天的时间
         */
        static void AddDays(const string &strStartDateTime,const float &fDays, string &strResultDateTime);
        
        /**
         * 获取指定时间＋n个月份
         */
        void AddMonths(const string &strStartDateTime,const int &iMonths, string &strResultDateTime);

        /**
         * 获取指定时间＋n个月份(精确到分)
         */
        void AddMonthsExact(const string &strStartDateTime,const int &iMonths, string &strResultDateTime);


        /**
         * 获取本月最后一天
         * @param strDateTime 输出时间字符串的引用
         * @return 无，失败时抛出例外
         */
        void GetLastDayOfCurMonth(string &strDateTime);

        /**
         * 根据传入的员工编码获取归属部门   jianghp@20050601 add
         * @param strStaffId 输入员工编码
         * @param strDepartId 输出归属部门
         * @return 无，失败时抛出例外
         */
        void GetDepartidByStaffid(const string &strStaffId, string &strDepartId);

        /**
         * 根据传入的部门编码获取归属业务区   jianghp@20050601 add
         * @param strDepartId 输入部门编码
         * @param strCityCode 输出归属业务区
         * @return 无，失败时抛出例外
         */
        void GetCitycodeByDepartid(const string &strDepartId, string &strCityCode);
        
		/**
		 *  获取N月末时间	add by zhoush @ 2005-11-17
		 */
		void GetLastDayOfNMonths(const string &strInputDate,const int &iMonths,string &strResultDateTime);
		/**  
		 *帐期加N个月
 		*/  
	  	void GetAcycIdOfNMonths(int &iAcycId,const int &iMonths);
};



#endif /* DualMGR_H_HEADER_INCLUDED_BE13A0DE */
