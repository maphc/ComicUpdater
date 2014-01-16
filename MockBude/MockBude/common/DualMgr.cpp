#include "stdafx.h"
#include "DualMgr.h"
#include "../commondao/DualDAO.h"

#include "../log4cpp/Logger.h"
using namespace log4cpp;
static Logger& logger = Logger::getLogger("gboss.crm.CustServ");


using namespace MiscDom;

static DualMgr g_DualMgr;

/**
 *  ������ˮ��
 */
extern "C" void GenerateID(CFmlBuf &inBuf,CFmlBuf &outBuf)
{
    LOG_TRACE(logger, "����GenerateID����");

    string strEparchyCode, strSequenceName, strSequenceId;

    strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");
    strSequenceName = inBuf.GetString("X_SEQUENCENAME");
    g_DualMgr.GenerateID(strEparchyCode, strSequenceName, strSequenceId);

    LOG_DEBUG_P2(logger, "���룺EparchyCode = %s, SequenceName = %s", strEparchyCode.c_str(), strSequenceName.c_str());
    LOG_DEBUG_P1(logger, "�����SequenceId = %s", strSequenceId.c_str());
    outBuf.SetString("X_SEQUENCEID", strSequenceId);

    LOG_TRACE(logger, "�˳�GenerateID����");
}

/**
 *  ��ȡϵͳʱ��
 */
extern "C" void GetSysDate(CFmlBuf &inBuf,CFmlBuf &outBuf)
{
    LOG_TRACE(logger, "����GetSysDate����");

    string strSysdate;

    g_DualMgr.GetSysDate(strSysdate);

    outBuf.SetString("X_SYSDATE", strSysdate);

    LOG_TRACE(logger, "�˳�GetSysDate����");
}

/**
 *  ��ȡ���µ�һ���ʱ��
 */
extern "C" void GetFirstDayOfNextMonth(CFmlBuf &inBuf,CFmlBuf &outBuf)
{
    LOG_TRACE(logger, "����GetFirstDayOfNextMonth����");

    string strDateTime;

    g_DualMgr.GetFirstDayOfNextMonth(strDateTime);

    outBuf.SetString("X_SYSDATE", strDateTime);

    LOG_TRACE(logger, "�˳�GetFirstDayOfNextMonth����");
}

/**
 *  ��ȡ���µ�һ���ʱ��
 */
extern "C" void GetLastDayOfCurMonth(CFmlBuf &inBuf,CFmlBuf &outBuf)
{
    LOG_TRACE(logger, "����GetLastDayOfCurMonth����");

    string strDateTime;

    g_DualMgr.GetLastDayOfCurMonth(strDateTime);

    outBuf.SetString("X_SYSDATE", strDateTime);

    LOG_TRACE(logger, "�˳�GetLastDayOfCurMonth����");
}

/**
 *  ��ȡ����
 *  X_GETMODE��0-��ȡ��ǰ���ڣ�1-��ȡ��Сδ�������ڣ�2-����ʱ���ȡ����
 */
extern "C" void GetAcycId(CFmlBuf &inBuf,CFmlBuf &outBuf)
{
	LOG_TRACE(logger, "����GetAcycId����");

    int iAcycId = -1;
    int getMode = 0;
    string parameter = "";
    getMode = inBuf.GetInt("X_GETMODE");

    if(getMode == 2)    //����ʱ���ȡ����ʱ����inBuf�л�ȡʱ��
    {
        parameter = inBuf.GetString("START_DATE");
    }

    g_DualMgr.GetAcycId(iAcycId, getMode, parameter);

    outBuf.SetInt("X_ACYC_ID", iAcycId);

    LOG_TRACE(logger, "�˳�GetAcycId����");
}

/**
 * ���ݴ����ID����ͨ�����ݿ���������ɸ���ID
 * @param eparchyCode ������б����ַ���������
 * @param SequenceName �������������ַ���������
 * @param SequenceId ������к��ַ���������
 * @return �ޣ�ʧ��ʱ�׳�����
 */
void DualMgr::GenerateID(string &eparchyCode, string &sequenceName, string &sequenceId)
{
    if(sequenceName.length() == 0)
    {
        THROW_C(CRMException, GENERATEID_ERR, "������ˮ��:û��������ˮ�����ͣ�");;
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
        THROW_AGC(CRMException, ex, Grade::WARNING, GENERATEID_ERR, "������ˮ�ų���");
    }
}
/**
 * ��ȡ����ʱ����ϵͳʱ���ֵ
 * @param strSysdate ���ϵͳʱ���ַ���������
 * @return �ޣ�ʧ��ʱ�׳�����
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
        THROW_AGC(CRMException, ex, Grade::WARNING, GETODDSDAYS_ERR, "��ȡ��ǰʱ����ϵͳʱ���ʱ�����");
    }
}
/**
 * ��ȡϵͳʱ��
 * @param strSysdate ���ϵͳʱ���ַ���������
 * @return �ޣ�ʧ��ʱ�׳�����
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
        THROW_AGC(CRMException, ex, Grade::WARNING, GETSYSDATE_ERR, "��ȡϵͳʱ�����");
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
        THROW_AGC(CRMException, ex, Grade::WARNING, GETSYSDATE_ERR, "��ȡʱ�����");
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
        THROW_AGC(CRMException, ex, Grade::WARNING, GETSYSDATE_ERR, "��ȡʱ�����");
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
        THROW_AGC(CRMException, ex, Grade::WARNING, GETSYSDATE_ERR, "��ȡʱ�����");
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
        THROW_AGC(CRMException, ex, Grade::WARNING, GETSYSDATE_ERR, "��ȡʱ�����");
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
        THROW_AGC(CRMException, ex, Grade::WARNING, GETSYSDATE_ERR, "��ȡ���µ�һ�����");
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
        THROW_AGC(CRMException, ex, Grade::WARNING, GETSYSDATE_ERR, "��ȡ�������һ�����");
    }
}

/**
 * ��ȡ����
 * @param iAcycId ������ڵ�����
 * @param getMode ��ȡ��ʽ��0-��ȡ��ǰ���ڣ�1-��ȡ��Сδ�������ڣ�2-����ʱ���ȡ����
 * @param parameter getMode=2ʱ������ʱ��
 * @return �ޣ�ʧ��ʱ�׳�����
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
        THROW_AGC(CRMException, ex, Grade::WARNING, GETACYCID_ERR, "��ȡ���ڳ���!");
    }
}

/**
 *  ����Ա����ȡ��������        jianghp@20050601 add
 */
extern "C" void GetDepartidByStaffid(CFmlBuf &inBuf,CFmlBuf &outBuf)
{
    LOG_TRACE(logger, "����GetDepartidByStaffid����");

    string strStaffId,strDepartId;

    strStaffId = inBuf.GetString("STAFF_ID");
    g_DualMgr.GetDepartidByStaffid(strStaffId, strDepartId);

    outBuf.SetString("DEPART_ID", strDepartId);

    LOG_TRACE(logger, "�˳�GetDepartidByStaffid����");
}

/**
 * ���ݴ����Ա�������ȡ��������   jianghp@20050601 add
 * @param strStaffId ����Ա������
 * @param strDepartId �����������
 * @return �ޣ�ʧ��ʱ�׳�����
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
        THROW_AGC(CRMException, ex, Grade::WARNING, GETDEPARTIDBYSTAFFID_ERR, "��ȡ�������ų���");
    }
}

/**
 *  ���ݲ��Ż�ȡ����ҵ����        jianghp@20050601 add
 */
extern "C" void GetCitycodeByDepartid(CFmlBuf &inBuf,CFmlBuf &outBuf)
{
    LOG_TRACE(logger, "����GetCitycodeByDepartid����");

    string strDepartId,strCityCode;

    strDepartId = inBuf.GetString("DEPART_ID");
    g_DualMgr.GetCitycodeByDepartid(strDepartId, strCityCode);

    outBuf.SetString("CITY_CODE", strCityCode);

    LOG_TRACE(logger, "�˳�GetCitycodeByDepartid����");
}

/**
 * ���ݴ���Ĳ��ű����ȡ����ҵ����   jianghp@20050601 add
 * @param strDepartId ���벿�ű���
 * @param strCityCode �������ҵ����
 * @return �ޣ�ʧ��ʱ�׳�����
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
        THROW_AGC(CRMException, ex, Grade::WARNING, GETCITYCODEBYDEPARTID_ERR, "��ȡ����ҵ��������");
    }
}


/**
 *  ��ȡN��ĩʱ��	add by zhoush @ 2005-11-17
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
        THROW_AGC(CRMException, ex, Grade::WARNING, GETSYSDATE_ERR, "��ȡN��ĩʱ�����");
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
  			THROW_AGC(CRMException, ex, Grade::WARNING, GETACYCID_ERR, "��ȡ���ڳ���!");
  	}
}
