#include "stdafx.h"
#include "DualMgr.h"
#include "../commondao/DualDAO.h"

#include "../log4cpp/Logger.h"
using namespace log4cpp;
static Logger& logger = Logger::getLogger("gboss.crm.CustServ");


using namespace MiscDom;

static DualMgr g_DualMgr;

/**
 *  生成流水号
 */
extern "C" void GenerateID(CFmlBuf &inBuf,CFmlBuf &outBuf)
{
    LOG_TRACE(logger, "进入GenerateID函数");

    string strEparchyCode, strSequenceName, strSequenceId;

    strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");
    strSequenceName = inBuf.GetString("X_SEQUENCENAME");
    g_DualMgr.GenerateID(strEparchyCode, strSequenceName, strSequenceId);

    LOG_DEBUG_P2(logger, "输入：EparchyCode = %s, SequenceName = %s", strEparchyCode.c_str(), strSequenceName.c_str());
    LOG_DEBUG_P1(logger, "输出：SequenceId = %s", strSequenceId.c_str());
    outBuf.SetString("X_SEQUENCEID", strSequenceId);

    LOG_TRACE(logger, "退出GenerateID函数");
}

/**
 *  获取系统时间
 */
extern "C" void GetSysDate(CFmlBuf &inBuf,CFmlBuf &outBuf)
{
    LOG_TRACE(logger, "进入GetSysDate函数");

    string strSysdate;

    g_DualMgr.GetSysDate(strSysdate);

    outBuf.SetString("X_SYSDATE", strSysdate);

    LOG_TRACE(logger, "退出GetSysDate函数");
}

/**
 *  获取下月第一天的时间
 */
extern "C" void GetFirstDayOfNextMonth(CFmlBuf &inBuf,CFmlBuf &outBuf)
{
    LOG_TRACE(logger, "进入GetFirstDayOfNextMonth函数");

    string strDateTime;

    g_DualMgr.GetFirstDayOfNextMonth(strDateTime);

    outBuf.SetString("X_SYSDATE", strDateTime);

    LOG_TRACE(logger, "退出GetFirstDayOfNextMonth函数");
}

/**
 *  获取下月第一天的时间
 */
extern "C" void GetLastDayOfCurMonth(CFmlBuf &inBuf,CFmlBuf &outBuf)
{
    LOG_TRACE(logger, "进入GetLastDayOfCurMonth函数");

    string strDateTime;

    g_DualMgr.GetLastDayOfCurMonth(strDateTime);

    outBuf.SetString("X_SYSDATE", strDateTime);

    LOG_TRACE(logger, "退出GetLastDayOfCurMonth函数");
}

/**
 *  获取帐期
 *  X_GETMODE：0-获取当前帐期，1-获取最小未开帐帐期，2-根据时间获取帐期
 */
extern "C" void GetAcycId(CFmlBuf &inBuf,CFmlBuf &outBuf)
{
	LOG_TRACE(logger, "进入GetAcycId函数");

    int iAcycId = -1;
    int getMode = 0;
    string parameter = "";
    getMode = inBuf.GetInt("X_GETMODE");

    if(getMode == 2)    //根据时间获取帐期时，从inBuf中获取时间
    {
        parameter = inBuf.GetString("START_DATE");
    }

    g_DualMgr.GetAcycId(iAcycId, getMode, parameter);

    outBuf.SetInt("X_ACYC_ID", iAcycId);

    LOG_TRACE(logger, "退出GetAcycId函数");
}

/**
 * 根据传入的ID类型通过数据库的序列生成各种ID
 * @param eparchyCode 输入地市编码字符串的引用
 * @param SequenceName 输入序列类型字符串的引用
 * @param SequenceId 输出序列号字符串的引用
 * @return 无，失败时抛出例外
 */
void DualMgr::GenerateID(string &eparchyCode, string &sequenceName, string &sequenceId)
{
    if(sequenceName.length() == 0)
    {
        THROW_C(CRMException, GENERATEID_ERR, "生成流水号:没有输入流水号类型！");;
    }

    DualDAO daoDual;
    try
    {
        daoDual.SetParam(":VEPARCHY_CODE", eparchyCode);
        daoDual.SetParam(":VSEQUENCENAME", sequenceName);
        daoDual.jselect(sequenceId, "SEQUENCE");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENERATEID_ERR, "生成流水号出错");
    }
}
/**
 * 获取传入时间与系统时间差值
 * @param strSysdate 输出系统时间字符串的引用
 * @return 无，失败时抛出例外
 */
void DualMgr::GetOddsDays(const string &strCurdate,string &strResultDateTime)
{
    DualDAO daoDual;
    try
    {
        daoDual.SetParam(":VDATE",strCurdate);
        daoDual.jselect(strResultDateTime, "GET_ODDSDATES");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GETODDSDAYS_ERR, "获取当前时间与系统时间的时差出错");
    }
}
/**
 * 获取系统时间
 * @param strSysdate 输出系统时间字符串的引用
 * @return 无，失败时抛出例外
 */
void DualMgr::GetSysDate(string &strSysdate)
{
    DualDAO daoDual;
    try
    {
        daoDual.jselect(strSysdate, "GET_SYSDATE");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GETSYSDATE_ERR, "获取系统时间出错");
    }
}

void DualMgr::AddDays(const string &strStartDateTime,const int &iDays, string &strResultDateTime)
{
    DualDAO daoDual;
    try
    {
        daoDual.Clear();
        daoDual.SetParam(":VDATE", strStartDateTime);
        daoDual.SetParam(":VNUM", iDays);
        daoDual.jselect(strResultDateTime, "GET_DAY_ADD");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GETSYSDATE_ERR, "获取时间出错");
    }
}

void DualMgr::AddDays(const string &strStartDateTime,const float &fDays, string &strResultDateTime)
{
    DualDAO daoDual;
    try
    {
        daoDual.Clear();
        daoDual.SetParam(":VDATE", strStartDateTime);
        daoDual.SetParam(":VNUM", fDays);
        daoDual.jselect(strResultDateTime, "GET_DAY_ADD");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GETSYSDATE_ERR, "获取时间出错");
    }
}

void DualMgr::AddMonths(const string &strStartDateTime,const int &iMonths, string &strResultDateTime)
{
    DualDAO daoDual;
    try
    {
        daoDual.Clear();
        daoDual.SetParam(":VDATE", strStartDateTime);
        daoDual.SetParam(":VNUM", iMonths);
        daoDual.jselect(strResultDateTime, "GET_MONTH_ADD");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GETSYSDATE_ERR, "获取时间出错");
    }
}

void DualMgr::AddMonthsExact(const string &strStartDateTime,const int &iMonths, string &strResultDateTime)
{
    DualDAO daoDual;
    try
    {
        daoDual.Clear();
        daoDual.SetParam(":VDATE", strStartDateTime);
        daoDual.SetParam(":VNUM", iMonths);
        daoDual.jselect(strResultDateTime, "GET_MONTH_ADD_EXACT");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GETSYSDATE_ERR, "获取时间出错");
    }
}
void DualMgr::GetFirstDayOfNextMonth(string &strDateTime)
{
    DualDAO daoDual;
    try
    {
        daoDual.jselect(strDateTime, "GET_FIRSTDAYOFNEXTMONTH");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GETSYSDATE_ERR, "获取下月第一天出错");
    }
}

void DualMgr::GetLastDayOfCurMonth(string &strDateTime)
{
    DualDAO daoDual;
    try
    {
        daoDual.jselect(strDateTime, "GET_LASTDAYOFCURMONTH");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GETSYSDATE_ERR, "获取本月最后一天出错");
    }
}

/**
 * 获取帐期
 * @param iAcycId 输出帐期的引用
 * @param getMode 获取方式：0-获取当前帐期，1-获取最小未开帐帐期，2-根据时间获取帐期
 * @param parameter getMode=2时的输入时间
 * @return 无，失败时抛出例外
 */
void DualMgr::GetAcycId(int &iAcycId, const int &getMode, const string &parameter)
{
	DualDAO daoDual;
	try
	{
		if(getMode == 0)
		{
		    daoDual.jselect(iAcycId, "GET_ACYCID");
		}
		else if(getMode == 1)
		{
		    daoDual.jselect(iAcycId, "GET_ACYCID_MAXNOT");
		}
		else if(getMode == 2)
		{
		    daoDual.SetParam(":VDATE", parameter);
		    daoDual.jselect(iAcycId, "GET_ACYCID_BY_DATE");
		}
		else if(getMode == 3)
		{
		    daoDual.jselect(iAcycId, "GET_NEXT_CYCLE");
		}
		else if(getMode == 4)
		{
		    daoDual.jselect(iAcycId, "GET_LAST_CYCLE");
		}
	}
	catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GETACYCID_ERR, "获取帐期出错!");
    }
}

/**
 *  根据员工获取归属部门        jianghp@20050601 add
 */
extern "C" void GetDepartidByStaffid(CFmlBuf &inBuf,CFmlBuf &outBuf)
{
    LOG_TRACE(logger, "进入GetDepartidByStaffid函数");

    string strStaffId,strDepartId;

    strStaffId = inBuf.GetString("STAFF_ID");
    g_DualMgr.GetDepartidByStaffid(strStaffId, strDepartId);

    outBuf.SetString("DEPART_ID", strDepartId);

    LOG_TRACE(logger, "退出GetDepartidByStaffid函数");
}

/**
 * 根据传入的员工编码获取归属部门   jianghp@20050601 add
 * @param strStaffId 输入员工编码
 * @param strDepartId 输出归属部门
 * @return 无，失败时抛出例外
 */
void DualMgr::GetDepartidByStaffid(const string &strStaffId, string &strDepartId)
{
    DualDAO daoDual;
    try
    {
        daoDual.SetParam(":VSTAFF_ID", strStaffId);
        daoDual.jselect(strDepartId, "GET_DEPARTID_BY_STAFFID");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GETDEPARTIDBYSTAFFID_ERR, "获取归属部门出错");
    }
}

/**
 *  根据部门获取归属业务区        jianghp@20050601 add
 */
extern "C" void GetCitycodeByDepartid(CFmlBuf &inBuf,CFmlBuf &outBuf)
{
    LOG_TRACE(logger, "进入GetCitycodeByDepartid函数");

    string strDepartId,strCityCode;

    strDepartId = inBuf.GetString("DEPART_ID");
    g_DualMgr.GetCitycodeByDepartid(strDepartId, strCityCode);

    outBuf.SetString("CITY_CODE", strCityCode);

    LOG_TRACE(logger, "退出GetCitycodeByDepartid函数");
}

/**
 * 根据传入的部门编码获取归属业务区   jianghp@20050601 add
 * @param strDepartId 输入部门编码
 * @param strCityCode 输出归属业务区
 * @return 无，失败时抛出例外
 */
void DualMgr::GetCitycodeByDepartid(const string &strDepartId, string &strCityCode)
{
    DualDAO daoDual;
    try
    {
        daoDual.SetParam(":VDEPART_ID", strDepartId);
        daoDual.jselect(strCityCode, "GET_CITYCODE_BY_DEPARTID");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GETCITYCODEBYDEPARTID_ERR, "获取归属业务区出错");
    }
}


/**
 *  获取N月末时间	add by zhoush @ 2005-11-17
 */
void DualMgr::GetLastDayOfNMonths(const string &strInputDate,const int &iMonths,string &strResultDateTime)
{
    DualDAO daoDual;
    try
    {
        daoDual.Clear();
        daoDual.SetParam(":VDATE", strInputDate);
        daoDual.SetParam(":VNUM", iMonths);
        daoDual.jselect(strResultDateTime, "GET_LASTDAYOFNMONTHS");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GETSYSDATE_ERR, "获取N月末时间出错");
    }
}

void DualMgr::GetAcycIdOfNMonths(int &iAcycId,const int &iMonths)
{	 
		try
		{
		    string strTempDate="";
		    string strResultDateTime="";
		    stringstream stm;
        stm << iAcycId;
        stm >> strTempDate;
        strTempDate=strTempDate+"01";	  
        g_DualMgr.AddMonths(strTempDate,iMonths,strResultDateTime);                    
		    g_DualMgr.GetAcycId(iAcycId, 2, strResultDateTime);		    		 			 
		}
		catch(Exception &ex)
  	{
  			THROW_AGC(CRMException, ex, Grade::WARNING, GETACYCID_ERR, "获取帐期出错!");
  	}
}
