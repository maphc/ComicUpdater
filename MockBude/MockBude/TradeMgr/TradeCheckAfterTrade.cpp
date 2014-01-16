#include "stdafx.h"
#include "TradeCheckAfterTrade.h"
#include "../commondao/StaticDAO.h"
#include "../log4cpp/Logger.h"
#include "../commondao/DualDAO.h"
#include "../common/DualMgr.h"
#include "../UniCommonMgr/UniTradeDao.h"
#include "../UniCommonMgr/UniDataMgr.h"
#include "../common/StrTable.h"
#include "../UniCommonMgr/Utility.h"
#include "../commondao/CParamDAO.h"

using namespace log4cpp;
static Logger& logTradeCheckAfterTrade = Logger::getLogger("gboss.crm.CustServ");

/*判断时间是否连续  Add by tz@2005-12-31 09:45*/
inline bool IsContinuousTime(const string &strFirstDate, const string &strSecondDate)
{
    if ( strFirstDate == "" || strSecondDate == "" ) return false;
    StaticDAO& dao = StaticDAO::getDAO();
    CFmlBuf bufRecordCount;
    try
    {
        dao.Clear();
        dao.SetParam(":VSTART_DATE", strFirstDate);
        dao.SetParam(":VEND_DATE", strSecondDate);
        dao.jselect(bufRecordCount,"IsContinuousTime","TD_S_CPARAM");
        return (bufRecordCount.GetInt("RECORDCOUNT") > 0)?true:false;

    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:判断时间是否连续！");
    }
}

/*判断是否有预约的产品变更(注意：预约的产品变更只有一条记录) Add by tz@2006-01-18 19:33*/
inline bool ExistsBookProductChange(const string &strUserId, string &strTradeId, string &strStartDate)
{
    strTradeId = strStartDate = "";
    StaticDAO& dao = StaticDAO::getDAO();
    CFmlBuf bufTrade;
    try
    {
        dao.Clear();
        dao.SetParam(":VTRADE_TYPE_CODE", "110");
        dao.SetParam(":VUSER_ID", strUserId);
        if (dao.jselect(bufTrade, "SEL_BOOK_BY_USERID_CODE","TF_B_TRADE") == 0) return false;
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:判断时间是否连续！");
    }

    strTradeId = bufTrade.GetString("TRADE_ID");
    strStartDate = string(bufTrade.GetString("EXEC_TIME")).substr(0,10) + " 00:00:00";

    return true;
}

/**
 *  组合产品与个人产品限制校验
 *  author:Yangf
 *  date:2008-12-27
 **/
void TradeCheckAfterTrade::ChkRelaUserProductLimit(const string &tradeId,
    const string &userId, const string &eparchyCode)
{
    LOG_TRACE(logTradeCheckAfterTrade, "进入ChkRelaUserProductLimit函数！");
    StaticDAO& dao = StaticDAO::getDAO();
    CFmlBuf tradRelaBuf;
    //获取台账关系子表新增记录
    try
    {
        dao.Clear();
        dao.SetParam(":VTRADE_ID", tradeId);
        dao.SetParam(":VMODIFY_TAG", "0");
        dao.jselect(tradRelaBuf, "SEL_BY_TRADEID_TAG","TF_B_TRADE_RELATION");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取台账关系子表出错！");
    }

    if(tradRelaBuf.GetCount("ID_A") == 0)
    {
        return;
    }
    //获取台账产品子表记录
    CFmlBuf tradProdBuf;
    try
    {
        dao.Clear();
        dao.SetParam(":VTRADE_ID", tradeId);
        dao.jselect(tradProdBuf, "SEL_BY_PK","TF_B_TRADE_PRODUCT");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取台账产品子表出错！");
    }
    //获取主卡用户有效账户
    string acctId;
    try
    {
        CFmlBuf tempBuf;
        dao.Clear();
        dao.SetParam(":VUSER_ID", userId);
        if(dao.jselect(tempBuf, "SEL_BY_USER","TF_A_PAYRELATION") == 0)
        {
            try
            {
                dao.Clear();
                tempBuf.ClearFmlValue();
                dao.SetParam(":VTRADE_ID", tradeId);
                if(dao.jselect(tempBuf, "SEL_BY_TRADE","TF_B_TRADE_PAYRELATION") == 0)
                {
                    acctId = "-1";
                    //THROW_C(CRMException, UPDATECUSTINFO_ERR, "业务登记后条件判断:用户付费关系资料不存在！");
                }
                else
                {
                    acctId = tempBuf.GetString("ACCT_ID");
                }
            }
            catch(Exception &ex)
            {
                THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取台账付费关系子表出错！");
            }
        }

    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取付费关系资料出错！");
    }

    for(int i=0; i<tradRelaBuf.GetCount("ID_A"); ++i)
    {
        for(int j=0; j<tradProdBuf.GetCount("USER_ID_A"); ++j)
        {
            //added by Yangf@2009-1-14 begin
            //合账校验
            CFmlBuf tempBuf;
            try
            {
                dao.Clear();
                dao.SetParam(":VPRODUCT_ID", (int)tradProdBuf.GetInt("PRODUCT_ID"));
                dao.SetParam(":VFORCE_TAG", "3");   //账户优惠包
                dao.SetParam(":VEPARCHY_CODE", eparchyCode);
                dao.jselect(tempBuf, "SEL_BY_PID_FORCETAG","TD_B_PRODUCT_PACKAGE");
            }
            catch(Exception &ex)
            {
                THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取付费关系资料出错！");
            }
            if(tempBuf.GetCount("PRODUCT_ID")>0)
            {
                try
                {
                    tempBuf.ClearFmlValue();
                    dao.Clear();
                    dao.SetParam(":VUSER_ID", tradRelaBuf.GetString("ID_B", i));
                    dao.SetParam(":VACCT_ID", acctId);
                    dao.jselect(tempBuf, "ExistPayrelaByUserAcct","TF_A_PAYRELATION");
                    if(tempBuf.GetInt("RECORDCOUNT") == 0)
                    {
                        THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR,"业务登记后条件判断:号码%s不是主卡的合账用户！",tradRelaBuf.GetString("SERIAL_NUMBER_B", i).c_str());
                    }
                }
                catch(Exception &ex)
                {
                    THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取付费关系资料出错！");
                }
            }
            //added by Yangf@2009-1-14 end

            if(tradRelaBuf.GetString("ID_A", i) == tradProdBuf.GetString("USER_ID_A", j) &&
                    tradProdBuf.GetString("MODIFY_TAG", j) != "1")
            {
                CFmlBuf userProdBuf;
                try
                {
                    dao.Clear();
                    dao.SetParam(":VUSER_ID", tradRelaBuf.GetString("ID_B", i));
                    dao.SetParam(":VPRODUCT_MODE", "00");
                    dao.jselect(userProdBuf, "SEL_BY_UID+MODE","TF_F_USER_PRODUCT");
                }
                catch(Exception &ex)
                {
                    THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取用户产品资料表出错！");
                }
                for(int k=0; k<userProdBuf.GetCount("PRODUCT_ID"); ++k)
                {
                    CFmlBuf tempBuf;
                    try
                    {
                        dao.Clear();
                        dao.SetParam(":VPRODUCT_ID_A", (int)userProdBuf.GetInt("PRODUCT_ID", k));
                        dao.SetParam(":VPRODUCT_ID_B", (int)tradProdBuf.GetInt("PRODUCT_ID", j));
                        dao.jselect(tempBuf, "ExistsProdLimit","TD_S_CPARAM")>0;
                        if(tempBuf.GetInt("RECORDCOUNT")>0)
                        {
                            char productId[10];
                            sprintf(productId,"%d",(int)userProdBuf.GetInt("PRODUCT_ID", k));
                            string userProduct = CParamDAO::getParam("ProductName", productId);

                            sprintf(productId,"%d",(int)tradProdBuf.GetInt("PRODUCT_ID", j));
                            string compProduct = CParamDAO::getParam("ProductName", productId);
                            THROW_C_P3(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:号码：%s个人产品：\"%s\"与组合产品：\"%s\"互斥！",
                                        tradRelaBuf.GetString("SERIAL_NUMBER_B", i).c_str(), userProduct.c_str(), compProduct.c_str());
                        }
                    }
                    catch(Exception &ex)
                    {
                        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取产品互斥参数出错！");
                    }
                }
            }
        }
    }
    LOG_TRACE(logTradeCheckAfterTrade, "退出ChkRelaUserProductLimit函数！");
}

//判断元素和其属性之间的限制关系
void TradeCheckAfterTrade::CheckElmentsAndItsDiscnt(CFmlBuf &inBuf)
{
    LOG_TRACE(logTradeCheckAfterTrade, "进入CheckElmentsAndItsDiscnt函数！");
    //优惠限制-------------------
    CFmlBuf bufCommpara;
    CFmlBuf bufUserCommpara;
    StaticDAO& dao = StaticDAO::getDAO();
    int iCount(0);
    int iUserCount(0);
    string tmpStr = "";
    string exStr = "";
    CFmlBuf bufTradeItem;
    CFmlBuf bufUserItem;
    int iTcount(0);
    int iUserTcount(0);

    try
    {
        dao.Clear();
        bufCommpara.ClearFmlValue();
        dao.SetParam(":VTRADE_ID", inBuf.GetString("TRADE_ID"));
        dao.SetParam(":VEPARCHY_CODE", inBuf.GetString("TRADE_EPARCHY_CODE"));
        iCount = dao.jselect(bufCommpara, "SEL_ELEMENTS_ATTR","TD_S_COMMPARA");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GETCOMMPARA_ERR, "属性限制元素:获取台帐优惠元素关系异常!");
    }
    try
    {
        dao.Clear();
        bufUserCommpara.ClearFmlValue();
        dao.SetParam(":VTRADE_ID", inBuf.GetString("TRADE_ID"));
        dao.SetParam(":VUSER_ID", inBuf.GetString("USER_ID"));
        dao.SetParam(":VEPARCHY_CODE", inBuf.GetString("TRADE_EPARCHY_CODE"));
        iUserCount = dao.jselect(bufUserCommpara, "SEL_ELEMENTS_ATTR_USER","TD_S_COMMPARA");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GETCOMMPARA_ERR, "属性限制元素:获取用户优惠元素关系异常!");
    }
    if(iUserCount > 0)
        bufCommpara.Append(bufUserCommpara);
    iCount = bufCommpara.GetCount("PARAM_CODE");

    if(iCount > 0)
    {
        try
        {
            dao.Clear();
            bufTradeItem.ClearFmlValue();
            dao.SetParam(":VTRADE_ID", inBuf.GetString("TRADE_ID"));
            iTcount = dao.jselect(bufTradeItem, "SEL_BY_PK","TF_B_TRADE_SUB_ITEM");
        }
        catch(Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, GETCOMMPARA_ERR, "属性限制元素:获取台帐属性元素关系异常!");
        }
        try
        {
            dao.Clear();
            bufUserItem.ClearFmlValue();
            dao.SetParam(":VTRADE_ID", inBuf.GetString("TRADE_ID"));
            dao.SetParam(":VUSER_ID", inBuf.GetString("USER_ID"));
            iUserTcount = dao.jselect(bufUserItem, "SEL_ATTRVALUE_BY_TRADE","TF_F_USER_ITEM");
        }
        catch(Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, GETCOMMPARA_ERR, "属性限制元素:获取用户属性元素关系异常!");
        }
        if(iUserTcount > 0)
            bufTradeItem.Append(bufUserItem);
        iTcount = bufTradeItem.GetCount("TRADE_ID");
        if(iTcount > 0)
        {
            for(int j = 0; j < iCount; j ++)
            {
                exStr = bufCommpara.GetString("PARA_CODE5",j);  //提示
                LOG_TRACE_P1(logTradeCheckAfterTrade, "bufCommpara@cccc2222DDcccc@@@@@@@@@@@]:%s",exStr.c_str());

                for(int i = 0; i < iTcount; i ++)
                {
                    if(bufCommpara.GetString("PARA_CODE6",j)=="1")  //取反
                    {
                        if(bufCommpara.GetString("PARA_CODE2", j) == bufTradeItem.GetString("ATTR_CODE", i))
                        {
                            if(bufCommpara.GetString("PARA_CODE3", j).find("|" + bufTradeItem.GetString("ATTR_VALUE", i) + "|") != string::npos)
                            {
                                THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "优惠和属性限制判断:%s!", exStr.c_str());
                            }
                        }
                    }
                    else
                    {
                        if(bufCommpara.GetString("PARA_CODE2", j) == bufTradeItem.GetString("ATTR_CODE", i))
                        {
                            if(bufCommpara.GetString("PARA_CODE3", j).find("|" + bufTradeItem.GetString("ATTR_VALUE", i) + "|") == string::npos)
                            {
                                THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "优惠和属性限制判断:%s!", exStr.c_str());
                            }
                        }
                    }
                }
            }
        }
    }
    LOG_TRACE(logTradeCheckAfterTrade, "退出CheckElmentsAndItsDiscnt函数！");
}

//判断元素和其属性之间的限制关系 add by zhouf@2009-10-18
void TradeCheckAfterTrade::CheckElmentsAndItsAttr(CFmlBuf &inBuf,CFmlBuf &bufUserAllElements)
{
    LOG_TRACE(logTradeCheckAfterTrade, "进入CheckElmentsAndItsAttr函数！");
    CFmlBuf bufCommpara;
    StaticDAO& dao = StaticDAO::getDAO();
    int iCount(0);
    //int index(0);
    int iEleCount = bufUserAllElements.GetCount("ELEMENT_ID");
    string tmpStr = "";
    string exStr = "";

    try
    {
        dao.Clear();
        bufCommpara.ClearFmlValue();
        dao.SetParam(":VTRADE_ID", inBuf.GetString("TRADE_ID"));
        dao.SetParam(":VEPARCHY_CODE", inBuf.GetString("TRADE_EPARCHY_CODE"));
        iCount = dao.jselect(bufCommpara, "SEL_ATTR_FORCE_INFO","TF_B_TRADE_SUB_ITEM");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GETCOMMPARA_ERR, "属性限制元素:获取属性限制元素关系异常!");
    }
    LOG_TRACE_P1(logTradeCheckAfterTrade, "bufCommpara@cciEleCountcccc@@@@@@@@@@@:%d",iEleCount);
    LOG_TRACE_P1(logTradeCheckAfterTrade, "bufUserAllElements@cccc1111cccc@@@@@@@@@@@:%s",bufUserAllElements.ToString().c_str());

    for(int j = 0; j < iCount; j ++)
    {

        tmpStr = bufCommpara.GetString("PARA_CODE4",j)+bufCommpara.GetString("PARA_CODE11",j)+bufCommpara.GetString("PARA_CODE12",j)+bufCommpara.GetString("PARA_CODE13",j)
               +bufCommpara.GetString("PARA_CODE14",j)+bufCommpara.GetString("PARA_CODE15",j)+bufCommpara.GetString("PARA_CODE16",j)+bufCommpara.GetString("PARA_CODE17",j)
               +bufCommpara.GetString("PARA_CODE18",j)+bufCommpara.GetString("PARA_CODE19",j)+bufCommpara.GetString("PARA_CODE20",j)+bufCommpara.GetString("PARA_CODE21",j)
               +bufCommpara.GetString("PARA_CODE22",j)+bufCommpara.GetString("PARA_CODE23",j)+bufCommpara.GetString("PARA_CODE24",j)+bufCommpara.GetString("PARA_CODE25",j);

        exStr = bufCommpara.GetString("PARA_CODE5",j);

        LOG_TRACE_P1(logTradeCheckAfterTrade, "bufCommpara@cccc2222AAcccc@@@@@@@@@@@:%s",exStr.c_str());


        for(int i = 0; i < iEleCount; i ++)
        {
            LOG_TRACE_P2(logTradeCheckAfterTrade, "bufUserAllElements优惠:%s->%s",bufUserAllElements.GetString("ELEMENT_NAME", i).c_str(),bufCommpara.GetString("PARA_CODE5",j).c_str());

            if(bufCommpara.GetString("PARA_CODE6",j)=="1")  //取反，当PARA_CODE6为1时表示属性和优惠列表里的优惠不能同时存在
            {
                if(tmpStr.find("|" + bufUserAllElements.GetString("ELEMENT_ID", i) + "|") != string::npos)
                {
                    if(bufCommpara.GetString("PARA_CODE1",j).find("|" + bufCommpara.GetString("ATTR_VALUE",j) + "|") != string::npos)
                    {

                        THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "优惠和属性限制判断:%s!", exStr.c_str());
                    }
                }
            }
            else
            {
                if(tmpStr.find("|" + bufUserAllElements.GetString("ELEMENT_ID", i) + "|") != string::npos)
                {
                    if(bufCommpara.GetString("PARA_CODE1",j).find("|" + bufCommpara.GetString("ATTR_VALUE",j) + "|") == string::npos)
                    {

                        THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "优惠和属性限制判断:%s!", exStr.c_str());
                    }
                }
            }
        }

    }

    LOG_TRACE(logTradeCheckAfterTrade, "退出CheckElmentsAndItsAttr函数！");

    /*
    //获取本次台帐增加的属性和限制表中属性和元素之间限制关系数据
    CFmlBuf bufCommpara;
    StaticDAO& dao = StaticDAO::getDAO();
    int iCount(0);
    int iEleCount = bufUserAllElements.GetCount("ELEMENT_ID");
    try
    {
        dao.Clear();
        bufCommpara.ClearFmlValue();
        dao.SetParam(":VTRADE_ID", inBuf.GetString("TRADE_ID"));
        dao.SetParam(":VEPARCHY_CODE", inBuf.GetString("TRADE_EPARCHY_CODE"));
        iCount = dao.jselect(bufCommpara, "SEL_ATTR_FORCE_INFO","TF_B_TRADE_SUB_ITEM");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GETCOMMPARA_ERR, "属性限制元素:获取属性限制元素关系异常!");
    }
    //   id         s,d       true,false  tipInfo
    map<string,pair<string,pair<bool,string> > > mapAttrInfo;
    map<string,pair<string,pair<bool,string> > >::iterator iter;
    string temp("");
    for(int j=0; j<iCount; ++j)
    {
        temp = "";
        temp = bufCommpara.GetString("PARA_CODE4",j)+bufCommpara.GetString("PARA_CODE11",j)+bufCommpara.GetString("PARA_CODE12",j)+bufCommpara.GetString("PARA_CODE13",j)
               +bufCommpara.GetString("PARA_CODE14",j)+bufCommpara.GetString("PARA_CODE15",j)+bufCommpara.GetString("PARA_CODE16",j)+bufCommpara.GetString("PARA_CODE17",j)
               +bufCommpara.GetString("PARA_CODE18",j)+bufCommpara.GetString("PARA_CODE19",j)+bufCommpara.GetString("PARA_CODE20",j)+bufCommpara.GetString("PARA_CODE21",j)
               +bufCommpara.GetString("PARA_CODE22",j)+bufCommpara.GetString("PARA_CODE23",j)+bufCommpara.GetString("PARA_CODE24",j)+bufCommpara.GetString("PARA_CODE25",j);
        if(bufCommpara.GetString("PARA_CODE6",j)=="1") //取反，当PARA_CODE6为1时表示属性和优惠列表里的优惠不能同时存在
        {
            mapAttrInfo.insert(make_pair(temp,make_pair(bufCommpara.GetString("PARA_CODE3",j), make_pair(false,bufCommpara.GetString("PARA_CODE5",j)) ) ));
        }
        else
        {
            mapAttrInfo.insert(make_pair(temp,make_pair(bufCommpara.GetString("PARA_CODE3",j), make_pair(true,bufCommpara.GetString("PARA_CODE5",j)) ) ));
        }
    }

    for(int j=0; j<iEleCount; ++j)
    {
        for(iter = mapAttrInfo.begin(); iter != mapAttrInfo.end(); ++iter)
        {
            if(iter->first.find("|"+bufUserAllElements.GetString("ELEMENT_ID",j)+"|") != string::npos)
            {
                if(iter->second.first == bufUserAllElements.GetString("ELEMENT_TYPE_CODE",j))
                {
                    if(iter->second.second.first)
                        iter->second.second.first = false;
                    else
                        THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "属性和优惠的限制判断:%s!",iter->second.second.second.c_str());
                }
            }
        }
    }
    for(iter = mapAttrInfo.begin(); iter != mapAttrInfo.end(); ++iter)
    {
        if(iter->second.second.first)
        {
            THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "属性和优惠的限制判断:%s!",iter->second.second.second.c_str());
        }
    }*/
}
/**
 * 业务登记后条件判断
 * @param inBuf 输入CFmlBuf对象的引用
 * @param outBuf 输出CFmlBuf对象的引用
 * @return 无，失败时抛出例外
 */
//##ModelId=41CA6C2A01D5
void TradeCheckAfterTrade::CheckAfterTrade(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
    //0.查找相关信息
    string strSysdate;
    DualMgr objDualMgr;
	objDualMgr.GetSysDate(strSysdate);
    string strTradeId = inBuf.GetString("TRADE_ID");
    string strExecTime = inBuf.GetString("EXEC_TIME");
    string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");
    string strTradeTypeCode = inBuf.GetString("TRADE_TYPE_CODE");
    string strTradeStaffId = inBuf.GetString("TRADE_STAFF_ID");
    string strUserId;
    string strCustId;
    string strAcctId;
    string strProductId;
    string strBrandCodeB;//Add by Anjun@20050827
    string strProductIdB; //Added by Yangf@2005-7-15
    string strOpenMode;
    string strProvince;
    string strFamilyCount;
	string strBrandCode;//Modify by Zhoush @ 2005-07-29
	string strSerialNumber;//add by zhangzh@2005-12-10 集团业务贵宾号
	string strProcessTagSet; //jianhgp@2005-12-24 13:46 add
	string strUserIdB; //WangRc@2006-01-11 Add
	string strCustIdB; //WangRc@2006-01-11 Add
	string strAcctIdB; //WangRc@2006-01-11 Add
	string strNewSerialNumber="";
	//add by zhangzh 2006-01-13 begin
	string strUserIdAV;
	string strBrandCodeV;

	//add by zhangzh 2006-01-13 end
	if (inBuf.IsFldExist("USER_ID"))
        strUserId = inBuf.GetString("USER_ID");
    if (inBuf.IsFldExist("CUST_ID"))
        strCustId = inBuf.GetString("CUST_ID");
    if (inBuf.IsFldExist("ACCT_ID"))
        strAcctId = inBuf.GetString("ACCT_ID");
    if (inBuf.IsFldExist("PRODUCT_ID"))
        strProductId = inBuf.GetString("PRODUCT_ID");

    if (inBuf.IsFldExist("OPEN_MODE"))
        strOpenMode = inBuf.GetString("OPEN_MODE");
    //Add By WangRc 一卡双号副号码三户标识 Begin

    else strFamilyCount = "1";

    if (inBuf.IsFldExist("BRAND_CODE"))
	    strBrandCode = inBuf.GetString("BRAND_CODE");//Modify by Zhoush @ 2005-07-29
	if (inBuf.IsFldExist("SERIAL_NUMBER"))
	    strSerialNumber = inBuf.GetString("SERIAL_NUMBER");//add by zhangzh@2005-12-10 集团业务贵宾号
    if (inBuf.IsFldExist("PROCESS_TAG_SET"))
        strProcessTagSet = inBuf.GetString("PROCESS_TAG_SET");

	string strInModeCode = inBuf.IsFldExist("IN_MODE_CODE")?inBuf.GetString("IN_MODE_CODE"):"Z"; //Add by tz@2005-12-06 17:27
	string strNetTypeCode = inBuf.IsFldExist("NET_TYPE_CODE")?inBuf.GetString("NET_TYPE_CODE"):"00"; //Add by jianghp@2006-11-15 10:33
  StaticDAO& dao = StaticDAO::getDAO();
  	
  //特殊判断不需要走 CheckAfterTrade 校验
  CFmlBuf bufItemSpecial;
  int specialC = 0;
  string attrValue = "";
  try
  {
      dao.Clear();
      dao.SetParam(":VTRADE_ID", strTradeId);
      specialC = dao.jselect(bufItemSpecial,"SEL_BY_TRADE","TF_B_TRADE_SUB_ITEM");
  }
  catch (Exception &ex)
  {
      THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取属性值错误！");
  }
  for(int i=0; i < specialC; i++)
  {
      if(bufItemSpecial.GetString("ATTR_CODE", i) == "rightCode")
      {
          attrValue = bufItemSpecial.GetString("ATTR_VALUE", i);
      }
  }
  
  if(attrValue == "csCreateWileBossYUserTrade"){//总部预付费无线上网卡开户 不需要走CheckAfterTrade
  	LOG_TRACE_P1(logTradeCheckAfterTrade, "attrValue=%s", attrValue.c_str());
  	return;	
  }
	//begin jianghp@2006-12-20 10:07 add
	//GtoC手机保号不做判断
    if(strTradeTypeCode == "189")
	{
		return;
	}
	//end jianghp@2006-12-20 10:07 add


    //begin jianghp@2006-11-15 10:35 add
    //不是双模卡，选了双模服务，则提示报错  2018(双模20),1023(双模10)

    CFmlBuf bufRecordCount;
    if(strTradeTypeCode == "10")
    {
        int iRecCount1, iRecCount2;
        try
        {
            dao.Clear();
            dao.SetParam(":VTRADE_ID", strTradeId);
            dao.SetParam(":VSERVICE_ID", 2018);
            dao.SetParam(":VMODIFY_TAG", "0");
            dao.jselect(bufRecordCount,"SELCNT_TRADESVC_BY_SVCTAG","TD_S_CPARAM");
            iRecCount1 = bufRecordCount.GetInt("RECORDCOUNT");
        }
        catch (Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取是否选择了双模服务1时出错！");
        }

        try
        {
            dao.Clear();
            bufRecordCount.ClearFmlValue();
            dao.SetParam(":VTRADE_ID", strTradeId);
            dao.SetParam(":VSERVICE_ID", 1023);
            dao.SetParam(":VMODIFY_TAG", "0");
            dao.jselect(bufRecordCount,"SELCNT_TRADESVC_BY_SVCTAG","TD_S_CPARAM");
            iRecCount2 = bufRecordCount.GetInt("RECORDCOUNT");
        }
        catch (Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取是否选择了双模服务2时出错！");
        }

		//2018(双模20) iRecCount1,1023(双模10) iRecCount2
   		if(strProcessTagSet.length() > 12)
   		{
	   		if(strProcessTagSet.substr(12,1)=="1")
	   		{
				if(strNetTypeCode == "10" && iRecCount1 > 0)
				{
		           THROW_C(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:该用户是GSM用户，不能选择CDMA双模服务!");
				}

				if(strNetTypeCode == "20" && iRecCount2 > 0)
				{
		           THROW_C(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:该用户是CDMA用户，不能选择GSM双模服务!");
				}

				int iRecCount3 = 0;
		        try
		        {
		            dao.Clear();
		            bufRecordCount.ClearFmlValue();
		            dao.SetParam(":VTRADE_ID", strTradeId);
		            dao.SetParam(":VUSER_ID", strUserId);
		            dao.SetParam(":VDISCNT_TYPE_CODE", "1041");
		            dao.jselect(bufRecordCount,"ExistsTradeDiscntType","TD_S_CPARAM");
		            iRecCount3 = bufRecordCount.GetInt("RECORDCOUNT");
		        }
		        catch (Exception &ex)
		        {
		            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取用户是否选择了双模优惠时出错！");
		        }
		        if(iRecCount3 == 0)
		        {
		           THROW_C(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:该双模卡用户没有选择双模优惠，请检查!");
		        }
	   		}
	   		else
	   		{
		        if(iRecCount1 + iRecCount2 > 0)
		        {
		           THROW_C(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:该用户不是双模卡，不能选择双模服务!");
		        }
	   		}
   		}
   		else
   		{
	        if(iRecCount1 + iRecCount2 > 0)
	        {
	           THROW_C(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:该用户不是双模卡，不能选择双模服务!");
	        }
   		}
    }
    //end jianghp@2006-11-15 10:35 add

    //begin jianghp@2006-12-27 11:20 add
    //双模卡产品变更必须选择双模优惠
    if(strTradeTypeCode == "110")
    {
   		if(strProcessTagSet.length() > 4)
   		{
	   		if(strProcessTagSet.substr(4,1)=="1")
	   		{

				int iRecCount3 = 0;
		        try
		        {
		            dao.Clear();
		            bufRecordCount.ClearFmlValue();
		            dao.SetParam(":VTRADE_ID", strTradeId);
		            dao.SetParam(":VUSER_ID", strUserId);
		            dao.SetParam(":VACCEPT_MONTH", strTradeId.substr(4,2));
		            dao.SetParam(":VDISCNT_TYPE_CODE", "1041");
		            dao.jselect(bufRecordCount,"ExistsAllUserDiscntType","TD_S_CPARAM");
		            iRecCount3 = bufRecordCount.GetInt("RECORDCOUNT");
		        }
		        catch (Exception &ex)
		        {
		            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取用户是否选择了双模优惠时出错！");
		        }
		        if(iRecCount3 == 0)
		        {
		           THROW_C(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:该双模卡用户没有选择双模优惠，请检查!");
		        }
	   		}
   		}
    }
    //end jianghp@2006-12-27 11:19 add

    CFmlBuf bufUser;

    //押金业务
    if(strTradeTypeCode == "290")
    {
        try
        {
            dao.Clear();
            dao.SetParam(":VUSER_ID", strUserId);
            dao.jselect(bufUser, "SEL_BY_PK","TF_F_USER");
        }
        catch(Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取用户资料出错！");
        }

        if(bufUser.GetString("REMOVE_TAG") != "0")
            return;

        //add by liut2@20110512 begin
        CFmlBuf tempBuf,userForeGift,tradeFeeSubBuf;
        int count= 0;
        int foreGiftCount = 0;

        try
        {
            dao.Clear();
            dao.SetParam(":VTRADE_ID", strTradeId);
            count = dao.jselect(tradeFeeSubBuf, "SEL_BY_TRADEID","TF_B_TRADEFEE_SUB");
        }
        catch(Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取台帐费用子表出错！");
        }

        if(count>0)
        {
            try
            {
                dao.Clear();
                dao.SetParam(":VUSER_ID", strUserId);
                foreGiftCount = dao.jselect(userForeGift, "SEL_BY_PK", "TF_F_USER_FOREGIFT");
                ConvertCodeToName(userForeGift, "ForegiftName", "FOREGIFT_CODE", "FOREGIFT_NAME");
            }
            catch(Exception &ex)
            {
                THROW_AGC(CRMException, ex, Grade::WARNING, GETUSERFOREGIFT_ERR, "业务登记后条件判断:查询用户押金出错！");
            }

            for (int i=0;i<count;i++)
            {
                if(atol(tradeFeeSubBuf.GetString("FEE",i).c_str()) <0 && tradeFeeSubBuf.GetString("FEE_MODE",i) == "1") //清退，判断是否能进行押金清退
                {
                    string foregiftCode = tradeFeeSubBuf.GetString("FEE_TYPE_CODE",i);
                    string foregiftName = "";
                    int  foregiftFee = atol(tradeFeeSubBuf.GetString("FEE",i).c_str());
                    LOG_TRACE_P1(logTradeCheckAfterTrade, "---foregiftFee---%d！",foregiftFee);

                    int money = 0;
                    for (int j=0;j<foreGiftCount;j++)
                    {
                        if(userForeGift.GetString("FOREGIFT_CODE",j) == foregiftCode)
                        {
                           money = userForeGift.GetInt("MONEY",j)*100;
                           LOG_TRACE_P1(logTradeCheckAfterTrade, "---money---%d！",money);
                           foregiftName = userForeGift.GetString("FOREGIFT_NAME",j);
                        }
                    }
                    //检查购机押金是否可清退处理
                    try
                    {
                        tempBuf.ClearFmlValue();
                        dao.Clear();
                        dao.SetParam(":VPARA_CODE1", strUserId); //user_id
                        dao.SetParam(":VPARA_CODE2", foregiftCode); //foregift_code
                        dao.SetParam(":VPARA_CODE3", money); //forgeift
                        dao.SetParam(":VPARA_CODE4", "");
                        dao.SetParam(":VPARA_CODE5", "");
                        dao.SetParam(":VPARA_CODE6", "");
                        dao.SetParam(":VPARA_CODE7", "");
                        dao.SetParam(":VPARA_CODE8", "");
                        dao.SetParam(":VPARA_CODE9", "");
                        dao.SetParam(":VPARA_CODE10", "");

                        dao.jselect(tempBuf, "SEL_REFNDFOGFT","TD_S_COMMPARA");
                    }
                    catch(Exception &e)
                    {
                        THROW_AGC(CRMException, e, Grade::WARNING, 8888, "押金清退异常：获取购机信息异常！");
                    }
                    int canRefundFee = int(atof(tempBuf.GetString("PARA_CODE1").c_str())*100);
                    LOG_TRACE_P1(logTradeCheckAfterTrade, "---canRefundFee---%d！",canRefundFee);
                    string refundDate = tempBuf.GetString("END_DATE");
                    if(canRefundFee < 0)
                    {
                        THROW_C_P2(CRMException, 8888, "押金清退:%s现在不可以清退，请到清退时间:%s后来办理清退！",foregiftName.c_str(),refundDate.c_str());
                    }
                    if( (canRefundFee + foregiftFee) < 0)
                    {
                        THROW_C_P2(CRMException, 8888, "押金清退:%s现在可以清退金额为: %d 元！",foregiftName.c_str(),canRefundFee/100);
                    }

                    //国际押金清退判断
                    try
                    {
                        tempBuf.ClearFmlValue();
                        dao.Clear();
                        dao.SetParam(":VPARA_CODE1", strUserId); //user_id
                        dao.SetParam(":VPARA_CODE2", strEparchyCode); //tradeEparchy
                        dao.SetParam(":VPARA_CODE3", "");
                        dao.SetParam(":VPARA_CODE4", "");
                        dao.SetParam(":VPARA_CODE5", "");
                        dao.SetParam(":VPARA_CODE6", "");
                        dao.SetParam(":VPARA_CODE7", "");
                        dao.SetParam(":VPARA_CODE8", "");
                        dao.SetParam(":VPARA_CODE9", "");
                        dao.SetParam(":VPARA_CODE10", "");

                        dao.jselect(tempBuf, "SEL_INTERFOGFT","TD_S_COMMPARA");
                    }
                    catch(Exception &e)
                    {
                        THROW_AGC(CRMException, e, Grade::WARNING, 8888, "押金清退异常：获取国际押金信息异常！");
                    }

                    int fee = 0;
                    for (int j=0;j<tempBuf.GetCount("PARA_CODE1");j++)
                    {
                        if(tempBuf.GetString("PARA_CODE2",j) == foregiftCode)
                        {
                            fee = int(atof(tempBuf.GetString("PARA_CODE1",j).c_str())*100);
                            canRefundFee = money - fee; //可以清退的国际押金金额
                            if (canRefundFee + foregiftFee<0)
                            {
                                if (canRefundFee > 0)
                                {
                                    CFmlBuf tagBuf;
                                    int count = -1;
                                    try
                                    {
                                        dao.Clear();
                                        dao.SetParam(":VEPARCHY_CODE",inBuf.GetString("TRADE_EPARCHY_CODE"));
                                        dao.SetParam(":VTAG_CODE","CS_NUM_BACKFOREGIFTDAYS");
                                        dao.SetParam(":VSUBSYS_CODE","CSM");
                                        dao.SetParam(":VUSE_TAG","0");
                                        count = dao.jselect(tagBuf,"SEL_BY_TAGCODE_1","TD_S_TAG");
                                    }
                                    catch(Exception &e)
                                    {
                                        THROW_AGC(CRMException, e, Grade::WARNING, 8888, "押金清退异常：获取押金清退期限异常！");
                                    }
                                    string day = "";
                                    if(count>0)
                                    {
                                        day = tagBuf.GetString("TAG_NUMBER");
                                    }
                                    else
                                    {
                                        day = "5";
                                    }
                                    THROW_C_P4(CRMException, 8888, "押金清退:目前只能清退%s金额为: %d 元,如需清退所有%s,请取消国际漫游或国际长途服务%s天后办理！",foregiftName.c_str(),canRefundFee/100,foregiftName.c_str(),day.c_str());
                                }
                                if (canRefundFee < 0)
                                {
                                     THROW_C_P1(CRMException, 8888, "押金清退:%s目前不可清退！",foregiftName.c_str());
                                }
                            }
                        }
                    }
                }
            }
        }
        //add by liut2@20110512 end

    }

    int iMainCount = 0;
    CFmlBuf bufTag;
    try
    {
        dao.Clear();
        dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
        dao.SetParam(":VTAG_CODE", "PUB_CUR_PROVINCE");
        dao.SetParam(":VSUBSYS_CODE", "PUB");
        dao.SetParam(":VUSE_TAG", "0");
        iMainCount = dao.jselect(bufTag, "SEL_ALL_BY_TAGCODE","TD_S_TAG");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取当前省代码出错！");
    }
    if(iMainCount == 0)
    {
        THROW_C(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取当前省代码无有效数据！");
    }
    else if(iMainCount == 1)
    {
        strProvince = bufTag.GetString("TAG_INFO");
    }
    if (strTradeTypeCode != "381")//sp订购绑定优惠不校验权限
    {
    	int iDiscntAdd = -1;
    	CFmlBuf bufVTradeDiscntAdd;
    	try
        {
            dao.Clear();
            dao.SetParam(":VTRADE_ID", strTradeId);
            dao.SetParam(":VMODIFY_TAG", "0");
            iDiscntAdd = dao.jselect(bufVTradeDiscntAdd, "SEL_BY_TRADE_MTAG","TF_B_TRADE_DISCNT");
        }
        catch (Exception &ex)
        {
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查询业务台账优惠子表出错！[%s]",ex.getCause()->what());
        }

    	for(int i = 0;i<iDiscntAdd; i++)
    	{
    	    int iDiscntCode = (int)bufVTradeDiscntAdd.GetInt("DISCNT_CODE",i);
    	    LOG_TRACE_P1(logTradeCheckAfterTrade, "iDiscntCode===【%d】",iDiscntCode);
    	    LOG_TRACE_P1(logTradeCheckAfterTrade, "strTradeStaffId===【%s】",strTradeStaffId.c_str());
    	    CFmlBuf bufVStaffdataright;
    	    int iRightCount = -1;
    	    try
    	    {
                char chIdA[10];
                sprintf(chIdA,"%d",iDiscntCode);
                dao.Clear();
    	        dao.SetParam(":VDATA_CODE", chIdA); //jianghp@2006-04-28 16:56 modify 数据类型报错
    	        dao.SetParam(":VDATA_TYPE", "D");
    	        dao.SetParam(":VSTAFF_ID", strTradeStaffId);
    	        iRightCount = dao.jselect(bufVStaffdataright, "SEL_STAFF_EXISTS_DATACODE_BY_TYPE","TF_M_STAFFDATARIGHT");
    	    }
    	    catch (Exception &ex)
            {
                THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查询员工数据权限表出错！[%s]",ex.getCause()->what());
            }
            if(iRightCount<1)
            {
                char chIdA[10];
                sprintf(chIdA,"%d",iDiscntCode);
                string strDiscntName = CParamDAO::getParam("DiscntName", chIdA);
                THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "对不起，您无权办理【%s】优惠！", strDiscntName.c_str());
            }
    	}
    }

    //Added by Qiumb@ 2006-11-27
    if(strTradeTypeCode == "330" || strTradeTypeCode == "350" )//积分兑换，积分调整，防止外围接口未判剩余积分强行做业务
    {
        try
        {
            dao.Clear();
            bufUser.ClearFmlValue();
            dao.SetParam(":VUSER_ID", strUserId);
            if ( dao.jselect(bufUser, "SEL_BY_PK","TF_F_USER") == 0)
            {
                THROW_C(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取用户资料无记录！");
            }
        }
        catch(Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取用户资料出错！");
        }

        CFmlBuf bufTradeScore;
        try
        {
            dao.Clear();
            dao.SetParam(":VTRADE_ID", strTradeId);
            if (0 == dao.jselect(bufTradeScore, "SEL_BY_TRADE","TF_B_TRADE_SCORE"))
            {
                THROW_C(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:积分业务未插积分台帐主表！");
            }
        }
        catch (CRMException &ex)
        {
        	throw;
        }
        catch (Exception &ex)
        {
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查询积分台帐主表！[%s]",ex.getCause()->what());
        }

        if ( atoi(string(bufUser.GetString("SCORE_VALUE")).c_str()) + atoi(string(bufTradeScore.GetString("SCORE_CHANGED")).c_str()) < 0)
        {
            //THROW_C_P2(CRMException, CHECKAFTERTRADE_ERR,"业务登记后条件判断:用户积分不够扣减！当前值[%s]，异动值[%s]",string(bufUser.GetString("SCORE_VALUE")).c_str(),string(bufTradeScore.GetString("SCORE_CHANGED")).c_str());
        }

        //ADD by Liujun 2011-05-07 ESS积分手工调整提交前判断
        if(strTradeTypeCode == "350")
        {
            LOG_TRACE(logTradeCheckAfterTrade, "进入积分手工调整检验函数");

         	  CFmlBuf userInfoBuf;

            StaticDAO& dao = StaticDAO::getDAO();

            int iRowCount;

            try
            {
                dao.Clear();
                dao.SetParam(":VTRADE_ID",strTradeId);
                iRowCount = dao.jselect(userInfoBuf, "SEL_BY_TRADEID", "TF_B_TRADE_ITEM");
            }
            catch (Exception &ex)
            {
            	  THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADESCOREMAIN_ERR, "获取台账属性表出错！");
            }

            string scoreRight;
            string scoreChange;
            LOG_TRACE_P1(logTradeCheckAfterTrade, "进入预提交函数，台账属性表RowCount: %d:", iRowCount);
            if(iRowCount>0)
                {
                for(int i=0; i<iRowCount; i++)
                   {
                       if(userInfoBuf.GetString("ATTR_CODE", i)=="SCORE_RIGHT")
                           {
                               scoreRight = userInfoBuf.GetString("ATTR_VALUE", i);
                           }
                       else if(userInfoBuf.GetString("ATTR_CODE", i)=="SCORE_CHANGE")
                           {
                               scoreChange = userInfoBuf.GetString("ATTR_VALUE", i);
                           }
                    }

                }
            LOG_TRACE_P1(logTradeCheckAfterTrade, "进入预提交函数，是否有修改限制的判断: %s", scoreRight.c_str());
            LOG_TRACE_P1(logTradeCheckAfterTrade, "进入预提交函数，tradeId: %s", strTradeId.c_str());
            LOG_TRACE_P1(logTradeCheckAfterTrade, "进入预提交函数，SerialNumber: %s", strSerialNumber.c_str());
            LOG_TRACE_P1(logTradeCheckAfterTrade, "进入预提交函数，业务员界面修改积分值: %s", scoreChange.c_str());

            if(scoreRight=="0")
            {
                LOG_TRACE(logTradeCheckAfterTrade, "进入预提交函数，权限是0，需要进行判断！");
            try
            {
                dao.Clear();
                dao.SetParam(":VSERIAL_NUMBER",strSerialNumber);
                iRowCount = dao.jselect(userInfoBuf, "SEL_MONTH_TOTAL_SCORE_BY_NUMBER", "TF_B_TRADE_SCORE");
            }
            catch (Exception &ex)
            {
                THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADESCOREMAIN_ERR, "获取台帐积分主表出错！");
            }

            //计算当月总共调整的积分金额
            int scoreValue=0;
            if(iRowCount>0)
            {
                for(int i=iRowCount-1;i>=0;--i)
                {
                    string valueChanged= userInfoBuf.GetString("VALUE_CHANGED",i);

                if (valueChanged=="350")
                    scoreValue= scoreValue + atoi(userInfoBuf.GetString("SCORE_CHANGED",i).c_str());
                }
            }

            LOG_TRACE_P1(logTradeCheckAfterTrade, "进入预提交函数，当月调整的积分总额是: %d:", scoreValue);

            try
            {
                dao.Clear();
                dao.SetParam(":VSUBSYS_CODE","CSM");
                dao.SetParam(":VPARAM_ATTR", 1650);
                dao.SetParam(":VPARAM_CODE","ZZZZ");
                dao.SetParam(":VEPARCHY_CODE",strEparchyCode);
                iRowCount = dao.jselect(userInfoBuf, "SEL1_PK_TD_S_COMMPARA","TD_S_COMMPARA");

            }
            catch (Exception &ex)
            {
        	      THROW_AGC(CRMException, ex, Grade::WARNING, 8888, "校验承载号码：获取用户欠费信息异常！");
            }

            //单次调整限额
            string oneChgLimit;

            //月度调整总
            string chgMonthLimit;

            if(iRowCount>=0)
            {
            //获取单次最大调整额和月度允许调整总额
            oneChgLimit=userInfoBuf.GetString("PARA_CODE1",0);
            LOG_TRACE_P1(logTradeCheckAfterTrade, "进入预提交函数oneChgLimit: %s:", oneChgLimit.c_str());

            chgMonthLimit = userInfoBuf.GetString("PARA_CODE2",0);
            LOG_TRACE_P1(logTradeCheckAfterTrade, "进入预提交函数chgMonthLimit: %s:", chgMonthLimit.c_str());

            //提示调整金额不能大于单次调整额!
            if(atoi(oneChgLimit.c_str())- atoi(scoreChange.c_str())<0)
                {
        	          LOG_TRACE(logTradeCheckAfterTrade, "进入预提交函数，一次调整额不能大于最大调整额！");

                    THROW_C(CRMException, 8888, "调整金额不能大于单次调整额！");
                }

            if(atoi(chgMonthLimit.c_str())-scoreValue-atoi(scoreChange.c_str())<0)
                {
        	          LOG_TRACE(logTradeCheckAfterTrade, "进入预提交函数，调整额不能大于当月最大调整额！");

                    //计算允许调整金额
                    int leftScore = atoi(chgMonthLimit.c_str())-scoreValue;

                    LOG_TRACE_P1(logTradeCheckAfterTrade, "进入预提交函数，当月还可以调整积分额: %d:", leftScore);

                    //调整的积分不能大于月调整额，您可以调整的金额为leftScore;
                    THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR,"调整的积分不能大于当月调整额，还可以调整[%d]积分！", leftScore);
                }
            }

            LOG_TRACE(logTradeCheckAfterTrade, "退出CheckUserRight函数");

            }
        }
    }

    //add by zhangzh 2006-01-13 begin
	if (strTradeTypeCode == "1025" && strProcessTagSet.length() >= 5)
	{
		if ( strProcessTagSet.substr(4,1) == "3")
		{
			int count = -1;
			CFmlBuf bufVUserDiscnt;
			try{
				dao.Clear();
				dao.SetParam(":VUSER_ID_A", strUserIdAV);
				dao.SetParam(":VBRAND_CODE", strBrandCodeV);
				count = dao.jselect(bufVUserDiscnt, "SEL_GRP_DISCNT_BY_BRAND","TF_F_USER_DISCNT");
			}
			catch(Exception &ex)
			{
				THROW_AGC(CRMException, ex, Grade::WARNING, ADDUSERDISCNT_ERR, "根据成员品牌查询集团优惠异常！");
			}
			if (count > 1)
			{
				string strDateTime;//是否有下月生效优惠
				DualMgr::GetFirstDayOfNextMonth(strDateTime);
				try{
					dao.Clear();
					dao.SetParam(":VUSER_ID_A", strUserIdAV);
					dao.SetParam(":VBRAND_CODE", strBrandCodeV);
					dao.SetParam(":VEND_DATE", strDateTime);
					count = dao.jselect(bufVUserDiscnt, "SEL_GRP_DISCNT_BY_BRAND_NEXT","TF_F_USER_DISCNT");
				}
				catch(Exception &ex)
				{
					THROW_AGC(CRMException, ex, Grade::WARNING, ADDUSERDISCNT_ERR, "根据成员品牌查询集团下月生效优惠异常！");
				}
				if (count == 0)
				{
					THROW_C(CRMException, ADDUSERDISCNT_ERR, "根据成员品牌获取集团优惠信息异常，请检查！");
				}
				else if (count > 1)
				{
					THROW_C(CRMException, ADDUSERDISCNT_ERR, "根据成员品牌选择集团优惠取到多条记录，请检查！");
				}
			}
			else if (count == 0)
			{
				try{
					dao.Clear();
					dao.SetParam(":VUSER_ID_A", strUserIdAV);
					count = dao.jselect(bufVUserDiscnt, "SEL_GRP_DISCNT_BY_BRAND1","TF_F_USER_DISCNT");
				}
				catch(Exception &ex)
				{
					THROW_AGC(CRMException, ex, Grade::WARNING, ADDUSERDISCNT_ERR, "查询成员可选集团优惠异常！");
				}
				if (count >1)
				{
					string strDateTime;//是否有下月生效优惠
					DualMgr::GetFirstDayOfNextMonth(strDateTime);

					try{
						dao.Clear();
						dao.SetParam(":VUSER_ID_A", strUserIdAV);
						dao.SetParam(":VEND_DATE", strDateTime);
						count = dao.jselect(bufVUserDiscnt, "SEL_GRP_DISCNT_BY_BRAND_NEXT1","TF_F_USER_DISCNT");
					}
					catch(Exception &ex)
					{
						THROW_AGC(CRMException, ex, Grade::WARNING, ADDUSERDISCNT_ERR, "根据成员品牌查询集团下月生效优惠异常！");
					}
					if (count == 0)
					{
						THROW_C(CRMException, ADDUSERDISCNT_ERR, "根据成员品牌没有查询到集团可选优惠1，请添加！");
					}
					else if (count >1)
					{
						THROW_C(CRMException, ADDUSERDISCNT_ERR, "根据成员品牌没有查询到集团可选优惠多条记录，请确认！");
					}
				}
				else if (count == 0)
				{
					THROW_C(CRMException, ADDUSERDISCNT_ERR, "根据成员品牌没有查询到集团可选优惠，请添加！");
				}
			}
		}
    }
	//add by zhangzh 2006-01-13 end
	/*
    //add by liudx2@2011-04-29 begin 参加在网有礼的号码，做产品变更不能低于66元套餐且赠送额的2倍要小于套餐费用
    if ( strTradeTypeCode == "110" || strTradeTypeCode == "120" )
    {
	    CFmlBuf bufGiftFor3G;
	    int countGift = 0;
        try
        {
            dao.Clear();
            dao.SetParam(":VTRADE_ID", strTradeId);
            countGift = dao.jselect(bufGiftFor3G, "SEL_FEE_BYTRADEIDFORGIFT","TD_S_COMMPARA");
        }
        catch(Exception &e)
        {
            THROW_AGC(CRMException, e, Grade::WARNING, GENEOLCOM_ERR, "获取在网有礼产品变更信息失败");
        }
        if ( countGift > 0)
        {
            int productFee = bufGiftFor3G.GetInt("PARA_CODE1");
            int productFeeDeal = bufGiftFor3G.GetInt("PARA_CODE3");
            int GiftFee = bufGiftFor3G.GetInt("BFEE");
            if ( productFee < 66 )
                THROW_C(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:参与赠送的3G号码套餐值不得低于66元！");
            if ( GiftFee/100 > productFeeDeal)
                THROW_C_P2(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:套餐设定赠送额%d元不能小于已参加赠送金额%d元！",productFeeDeal,GiftFee);
        }
    }*/
    //add by liudx2@2011-04-29 end
	//add by zhangzh@2006-03-14 begin 如果集团编码在267参数配置，则判断部分依赖关系
	 CFmlBuf bufCommpara;
	 if (strTradeTypeCode != "381")//sp订购不校验
   {
        try
        {
            int count = 0;
            dao.Clear();
            dao.SetParam(":VSUBSYS_CODE", "CSM");
            dao.SetParam(":VPARAM_ATTR", 267);
            dao.SetParam(":VPARAM_CODE", strBrandCodeV);//集团产品编码
            dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
            count = dao.jselect(bufCommpara, "SEL1_PK_TD_S_COMMPARA","TD_S_COMMPARA");
        }
        catch(Exception &e)
        {
            THROW_AGC(CRMException, e, Grade::WARNING, GENEOLCOM_ERR, "取产品依赖标志出错：通用参数267获取产品部分依赖标志失败");
        }

        string strLimitType;
        try
        {
            iMainCount = 0;
            dao.Clear();
            bufTag.ClearFmlValue();
            dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
            dao.SetParam(":VTAG_CODE", "CS_CHR_SVCTRADELIMIT");
            dao.SetParam(":VSUBSYS_CODE", "CSM");
            dao.SetParam(":VUSE_TAG", "0");
            iMainCount = dao.jselect(bufTag, "SEL_ALL_BY_TAGCODE","TD_S_TAG");
        }
        catch(Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取服务变更限制标记出错！");
        }
        if(iMainCount == 1)
        {
            strLimitType = bufTag.GetString("TAG_CHAR");
        }
   }

    //集团用户新增  判断是否允许加入这个集团(是否存在产品限制)
    if (strTradeTypeCode == "1025")
    {
        LOG_TRACE_P1(logTradeCheckAfterTrade, "----strTradeTypeCode:[%s]-----", strTradeTypeCode.c_str());
        string strFirstDayOfNextMonth("");
        DualMgr::GetFirstDayOfNextMonth(strFirstDayOfNextMonth);

        CFmlBuf bufTradeRelaTemp;
        int iTradeRela = 0;
        string strTradeRelaTypeCode = "";
        try
        {
            dao.Clear();
            bufTradeRelaTemp.ClearFmlValue();
            dao.SetParam(":VTRADE_ID", inBuf.GetString("TRADE_ID"));
            iTradeRela = dao.jselect(bufTradeRelaTemp, "SEL_BY_PK","TF_B_TRADE_RELATION") ;
        }
        catch(Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取当前省代码出错！");
        }
        if(iTradeRela == 0)
        {
            THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取台帐关系子表异常[TRADE_ID:%s]！", inBuf.GetString("TRADE_ID").c_str());
        }

        string strUserIdA = "";
        if(iTradeRela > 0)
        {
            strUserIdA = bufTradeRelaTemp.GetString("ID_A");
            strTradeRelaTypeCode = bufTradeRelaTemp.GetString("RELATION_TYPE_CODE");
LOG_TRACE_P1(logTradeCheckAfterTrade, "----strTradeRelaTypeCode:[%s]-----", strTradeRelaTypeCode.c_str());
        }

        CFmlBuf bufUser;//取集团的产品
        try
        {
            dao.Clear();
            dao.SetParam(":VUSER_ID", strUserIdA);
            if (dao.jselect(bufUser, "SEL_BY_PK","TF_F_USER") == 0)
            {
                THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:归属的集团用户不存在[ID:%s]！", strUserIdA.c_str());
            }
        }
        catch(Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取当前省代码出错！");
        }

        CFmlBuf bufUserRoleB;//取用户的产品
        try
        {
            dao.Clear();
            dao.SetParam(":VUSER_ID",  inBuf.GetString("USER_ID"));
            if (dao.jselect(bufUserRoleB, "SEL_BY_PK","TF_F_USER") == 0)
            {
                THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:归属的集团用户不存在[ID:%s]！", strUserIdA.c_str());
            }
        }
        catch(Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取当前省代码出错！");
        }

        CFmlBuf bufVProductLimit;
        int iProductRoleCodeA = -1;
        iProductRoleCodeA = (int)bufUser.GetInt("PRODUCT_ID");

        int iProductLimit = -1;
        try
        {
            dao.Clear();
            bufVProductLimit.ClearFmlValue();
            dao.SetParam(":VPRODUCT_ID_A", iProductRoleCodeA);//集团产品
            dao.SetParam(":VPRODUCT_ID_B", (int)bufUserRoleB.GetInt("PRODUCT_ID"));//用户产品
            iProductLimit = dao.jselect(bufVProductLimit, "SEL_CHECK_LIMIT_0_NEW","TD_S_PRODUCTLIMIT");
        }
        catch(Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查询产品间限制关系出错！");
        }

        if ( iProductLimit > 0)//表示不允许
        {
            char chIdA[10];
            sprintf(chIdA,"%d",(int)bufVProductLimit.GetInt("PRODUCT_ID_B"));
            string strNameA = CParamDAO::getParam("ProductName", chIdA);
            THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:用户产品受限[%s]，不能加入该集团！", strNameA.c_str());
        }

        if (iProductLimit == 0)//表示允许,进入下次逻辑判断
        {
            //如果用户有下月生效产品，并且下月生效产品和集团产品不存在互斥，则允许办理
            CFmlBuf bufUserPreProd;
            try
            {
                dao.Clear();
                dao.SetParam(":VUSER_ID", inBuf.GetString("USER_ID"));
                iMainCount = dao.jselect(bufUserPreProd, "SEL_PRE_PROD_NEW","TF_F_USER_PRODUCT");
            }
            catch(Exception &ex)
            {
                THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:检查用户是否有新产品出错！");
            }

            if (iMainCount == 0)//无下月生效主产品
            {
                //char chIdA[10];
                //sprintf(chIdA,"%d",(int)bufVProductLimit.GetInt("PRODUCT_ID_B"));
                //string strNameA = CParamDAO::getParam("ProductName", chIdA);
                //THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:用户受限[%s]，不能加入该集团！", strNameA.c_str());
            }
            else if (iMainCount > 0)
            {   //判断新产品是否允许加入集团
                int iProductId = bufUserPreProd.GetInt("PRODUCT_ID");
                CFmlBuf bufVProductLimitPre;
                int iProductLimitPre = -1;
                try
                {
                    dao.Clear();
                    bufVProductLimitPre.ClearFmlValue();
                    dao.SetParam(":VPRODUCT_ID_A", iProductRoleCodeA);  //集团产品
                    dao.SetParam(":VPRODUCT_ID_B", iProductId);         //用户下月产品
                    iProductLimitPre=dao.jselect(bufVProductLimitPre, "SEL_CHECK_LIMIT_0_NEW","TD_S_PRODUCTLIMIT");
                }
                catch(Exception &ex)
                {
                    THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查询产品间限制关系出错！");
                }
                if (iProductLimitPre > 0)
                {
                    char chIdA[10];
                    sprintf(chIdA,"%d",(int)bufVProductLimitPre.GetInt("PRODUCT_ID_B"));
                    string strNameA = CParamDAO::getParam("ProductName", chIdA);
                    THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:用户预约的产品受限[%s]，不能加入该集团！", strNameA.c_str());
                }
                else
                {
                    ;
                }
            }
        }
    	//MODIFY BY HUANGM@2006-04-06 END

     	/*
        //VPMN成员，如果是立即新增,判断是否有本月vpmn注销的记录，如果有只能下月生效，不能本月生效
        if (inBuf.GetString("EXEC_TIME") < strFirstDayOfNextMonth && inBuf.GetString("RSRV_STR2") == "20")
        {
            //判断是否有本月取消的，如果是本月取消的则不能新增
            CFmlBuf bufUserRelation;
            try
            {
                dao.Clear();
                dao.SetParam(":VUSER_ID_B", strUserId);
                dao.SetParam(":VRELATION_TYPE_CODE", "20");
                if (dao.jselect(bufUserRelation, "SEL_USERRELA_BY_IDB","TF_F_RELATION_UU") > 0)
                {
                    //THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:该号码目前在集团[%s]中！", string(bufUserRelation.GetString("SERIAL_NUMBER_A")).c_str());
                }
            }
            catch(Exception &ex)
            {
                THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:检查用户关系表出错！");
            }
        }
        */
    }

	//Modify by Zhoush @ 2005-07-09
	if(strTradeTypeCode=="241")
	{
		/*//是否可以购机取消
        int iCount = 0;
        CFmlBuf bufVPurchasetrade;
        string strRsrvStr2 = inBuf.GetString("RSRV_STR2");
        try
        {
            dao.Clear();
            dao.SetParam(":VPURCHASE_ATTR", strRsrvStr2);
            dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
            iCount = dao.jselect(bufVPurchasetrade, "SEL_PURCHASE_TRADE_PK","TD_B_PURCHASETRADE");
        }
        catch(Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取购机业务参数出错");
        }

		if(iCount > 0)
		{
			if(string(bufVPurchasetrade.GetString("PARA_CODE1")).substr(0,1) != "1")
			{
				THROW_C(CRMException, CHECKAFTERTRADE_ERR, "此购机业务不可以取消！");
			}
		}
		else
		{
            THROW_C(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取购机业务参数无记录！");
		}
		*/
	}

	if(strTradeTypeCode == "192")
	{
		//20110628 add by wuxg for 立即销户
		/*int iCount=0;
        CFmlBuf bufCommpara;
        try
        {
            dao.Clear();
            bufCommpara.ClearFmlValue();
            dao.SetParam(":VUSER_ID", strUserId);
            //dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
            iCount = dao.jselect(bufCommpara, "SEL_ROAM_ORDER","TF_F_USER_SVCSTATE");
        }
        catch (Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "检查用户国际漫游业务定制信息失败！");
        }

        if(iCount>0)
        {
			THROW_C(CRMException, CHECKAFTERTRADE_ERR, "用户定制了国际漫游或取消国际漫游不足两个话费出账期，不能立即销户！");
        }*/
	}

	if (strTradeTypeCode != "381" && strTradeStaffId!="Z0000VAC")//sp订购不校验
  {
	//Modify by Zhoush @ 2005-07-09
    if (strTradeTypeCode == "121")//服务赠送
    {
    	int iCount=0;
        CFmlBuf bufCommpara;
        try
        {
            dao.Clear();
            bufCommpara.ClearFmlValue();
            dao.SetParam(":VSUBSYS_CODE", "CSM");
            dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
            dao.SetParam(":VTRADE_ID", strTradeId);
            iCount = dao.jselect(bufCommpara, "SEL_BY_PRESENT","TD_S_COMMPARA");
        }
        catch (Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "检查用户是否存在受限优惠失败！");
        }

        if(iCount>0)
        {
        	  THROW_C(CRMException, CHECKAFTERTRADE_ERR, "被赠送用户当前有限制优惠，不能继续办理！");
        }
    }
    else
    {
        int iCount=0;
        CFmlBuf bufCommpara;
        try
        {
            dao.Clear();
            dao.SetParam(":VSUBSYS_CODE", "CSM");
            dao.SetParam(":VPARAM_ATTR", "14");
            dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
            iCount = dao.jselect(bufCommpara, "SEL_ONLY_BY_ATTR_ORDERED","TD_S_COMMPARA");
        }
        catch (Exception &ex)
        {
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取通用参数（副卡限制数）出错！[%s]",ex.getCause()->what());
        }

        if(iCount>0)
        {
    	    iCount=0;
            try
            {
                dao.Clear();
                bufCommpara.ClearFmlValue();
                dao.SetParam(":VSUBSYS_CODE", "CSM");
                dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
                dao.SetParam(":VTRADE_ID", strTradeId);
                dao.SetParam(":VUSER_ID", strUserId);
                iCount = dao.jselect(bufCommpara, "SEL_BY_PRESENT_LIMIT","TD_S_COMMPARA");
            }
            catch (Exception &ex)
            {
                THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取通用参数（副卡限制数）出错！[%s]",ex.getCause()->what());
            }

            if(iCount>0)
            {
            	  THROW_C(CRMException, CHECKAFTERTRADE_ERR, "用户正享受服务赠送，不能办理此优惠！");
            }
        }
    }


        //1.获取台账产品子表信息
        int iPdtCnt;
        CFmlBuf bufTradeProduct;
        try
        {
            dao.Clear();
            dao.SetParam(":VTRADE_ID", strTradeId);
            iPdtCnt = dao.jselect(bufTradeProduct, "SEL_BY_PK","TF_B_TRADE_PRODUCT");
        }
        catch (Exception &ex)
        {
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查询业务台账产品子表出错！[%s]",ex.getCause()->what());
        }

        //2.获取台账服务子表信息
        int iSvcCnt;
        CFmlBuf bufVTradeSvc;
        try
        {
            dao.Clear();
            dao.SetParam(":VTRADE_ID", strTradeId);
            iSvcCnt = dao.jselect(bufVTradeSvc, "SEL_BY_TRADEID","TF_B_TRADE_SVC");
        }
        catch (Exception &ex)
        {
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查询业务台账服务子表出错！[%s]",ex.getCause()->what());
        }
         /*
        if (strInModeCode.find_first_of("03") == string::npos)         //Add by tz@2005-12-06 17:30 排除 "普通营业厅"和"网上客服"
        {
            //2.1检查是否有产品不允许的服务 Add by Anj@2005-12-02
            int count = -1;
            int ProductId = -1;
            CFmlBuf bufVProductSvc;
            try
            {
                dao.Clear();
                dao.SetParam(":VTRADE_ID", strTradeId);
                dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
                count = dao.jselect(bufVProductSvc, "SEL_USERSVC_BYTRADESVC","TF_B_TRADE_SVC");
            }
            catch (Exception &ex)
            {
                THROW_AGC(CRMException, ex, Grade::WARNING, GETUSERALLSERVICE_ERR, "查询产品服务表错误！");
            }
            int iProductSvc = count;
            for(int i=0; i<iSvcCnt; i++)//tradesvc
            {
                int exist = 0;
                for(int j=0; j<iProductSvc; j++)
                {
                    if(bufVTradeSvc.GetString("MODIFY_TAG",i) == "0"
                        && ((int)bufVTradeSvc.GetInt("PRODUCT_ID",i)==-1 || (int)bufVProductSvc.GetInt("PRODUCT_ID",j) == (int)bufVTradeSvc.GetInt("PRODUCT_ID",i))
                        && ((int)bufVTradeSvc.GetInt("PACKAGE_ID",i)==-1 || (int)bufVProductSvc.GetInt("PACKAGE_ID",j) == (int)bufVTradeSvc.GetInt("PACKAGE_ID",i))
                        && (int)bufVProductSvc.GetInt("SERVICE_ID",j) == (int)bufVTradeSvc.GetInt("SERVICE_ID",i))
                        exist++;
                }
                if(exist == 0 && bufVTradeSvc.GetString("MODIFY_TAG",i) == "0")
                {
                    char chIdA[10];
                    char chIdB[10];
                    sprintf(chIdA,"%d", (int)bufVTradeSvc.GetInt("SERVICE_ID",i));
                    sprintf(chIdB,"%d", (int)bufVTradeSvc.GetInt("PRODUCT_ID",i));
                    string strNameA = CParamDAO::getParam("ServiceName", chIdA);
                    string strNameB = CParamDAO::getParam("ProductName", chIdB);
                    THROW_C_P2(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:产品【%s】不能办理服务【%s】，请修改后再提交！", strNameB.c_str(), strNameA.c_str());
                }
            }
        }

        //检查服务的必输参数是否输入 td_b_serv_itema.item_can_null
        CFmlBuf bufVServItema;
        int iServItemaCount;
        for(int i=0; i<iSvcCnt; i++)
        {
            if (bufVTradeSvc.GetString("MODIFY_TAG",i) == "1" ) continue;

            //检查该服务是否有必输项设置
            bufVServItema.ClearFmlValue();
            dao.Clear();
            dao.SetParam(":VID", (int)bufVTradeSvc.GetInt("SERVICE_ID",i));
            dao.SetParam(":VITEM_CAN_NULL", "0");  //不能为空
            dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
            if (iServItemaCount = dao.jselect(bufVServItema, "SEL_BY_SERVICEID_NOTNULL","TD_B_SERV_ITEMA") > 0)
            {
                for(int j=0; j<iServItemaCount; j++)
                {
                    if (    ((int)bufVServItema.GetInt("ITEM_INDEX",j) == 1 && bufVTradeSvc.GetString("SERV_PARA1",i) == "")
                         || ((int)bufVServItema.GetInt("ITEM_INDEX",j) == 2 && bufVTradeSvc.GetString("SERV_PARA2",i) == "")
                         || ((int)bufVServItema.GetInt("ITEM_INDEX",j) == 3 && bufVTradeSvc.GetString("SERV_PARA3",i) == "")
                         || ((int)bufVServItema.GetInt("ITEM_INDEX",j) == 4 && bufVTradeSvc.GetString("SERV_PARA4",i) == "")
                         || ((int)bufVServItema.GetInt("ITEM_INDEX",j) == 5 && bufVTradeSvc.GetString("SERV_PARA5",i) == "")
                         || ((int)bufVServItema.GetInt("ITEM_INDEX",j) == 6 && bufVTradeSvc.GetString("SERV_PARA6",i) == "")
                         || ((int)bufVServItema.GetInt("ITEM_INDEX",j) == 7 && bufVTradeSvc.GetString("SERV_PARA7",i) == "")
                         || ((int)bufVServItema.GetInt("ITEM_INDEX",j) == 8 && bufVTradeSvc.GetString("SERV_PARA8",i) == ""))
                    {
                        char chIdA[10];
                        sprintf(chIdA,"%d",(int)bufVTradeSvc.GetInt("SERVICE_ID",i));
                        string strNameA = CParamDAO::getParam("ServiceName", chIdA);
                        THROW_C_P2(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:服务【%s】的参数【%s】为必输项，请输入后再提交！", strNameA.c_str(), string(bufVServItema.GetString("ITEM_LABLE",i)).c_str());
                    }
                }
            }
        }

        //将2010-12-31 23:59:59设置为无穷远时间
        for ( int i=0; i<iSvcCnt; i++)
        {
            if (bufVTradeSvc.GetString("END_DATE",i) > "2010-12-31 23:59:59")
                bufVTradeSvc.SetString("END_DATE","2010-12-31 23:59:59",i);
        }

        */
        //3.获取台账优惠子表信息
        int iDstCnt;
        int count = -1;
        CFmlBuf bufVTradeDiscnt;
        try
        {
            dao.Clear();
            dao.SetParam(":VTRADE_ID", strTradeId);
            dao.SetParam(":VACCEPT_MONTH", strTradeId.substr(4,2));
            iDstCnt = dao.jselect(bufVTradeDiscnt, "SEL_BY_TRADEID","TF_B_TRADE_DISCNT");
        }
        catch (Exception &ex)
        {
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查询业务台账优惠子表出错！[%s]",ex.getCause()->what());
        }
        for(int i=0; i<iDstCnt ; i++) //it_D
        {
            try
            {
                dao.Clear();
                bufCommpara.ClearFmlValue();
                dao.SetParam(":VPARAM_ATTR", 1228);
                dao.SetParam(":VSUBSYS_CODE", "CSM");
                dao.SetParam(":VPARAM_CODE", bufVTradeDiscnt.GetString("MODIFY_TAG",i));
                dao.SetParam(":VPARA_CODE1", (int)bufVTradeDiscnt.GetInt("DISCNT_CODE",i));
                dao.SetParam(":VPARA_CODE2", strTradeTypeCode);
                dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
                count = dao.jselect(bufCommpara, "SEL_PURCHASE_COMMPARA","TD_S_COMMPARA");
            }
            catch(Exception &e)
            {
                THROW_C(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取通用参数信息错误!");
            }
            if (count >0 && bufVTradeDiscnt.GetString("MODIFY_TAG",i)=="0")
            {
                char ch[10];
                string strDiscntCode;
                sprintf(ch,"%d",(int)bufVTradeDiscnt.GetInt("DISCNT_CODE",i));
                strDiscntCode=ch;
                string strDiscntName = CParamDAO::getParam("DiscntName", strDiscntCode);
                THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:[%s]优惠不可以在此界面申请!", strDiscntName.c_str());
            }
            else
            if (count >0 && bufVTradeDiscnt.GetString("MODIFY_TAG",i)=="1")
            {
            	  char ch[10];
              	string strDiscntCode;
              	sprintf(ch,"%d",(int)bufVTradeDiscnt.GetInt("DISCNT_CODE",i));
              	strDiscntCode=ch;
              	string strDiscntName = CParamDAO::getParam("DiscntName", strDiscntCode);
                THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:[%s]优惠不可以在此界面取消!", strDiscntName.c_str());
            }
        }

        //将2010-12-31 23:59:59设置为无穷远时间
        for ( int i=0; i<iDstCnt; i++)
        {
            if (bufVTradeDiscnt.GetString("END_DATE",i) > "2050-12-31 23:59:59")
                bufVTradeDiscnt.SetString("END_DATE","2050-12-31 23:59:59",i);
        }

        LOG_TRACE_P1(logTradeCheckAfterTrade, "----0.0[%d]-----", iPdtCnt);
        LOG_TRACE_P1(logTradeCheckAfterTrade, "----0.1[%d]-----", iSvcCnt);
        LOG_TRACE_P1(logTradeCheckAfterTrade, "----0.2[%d]-----", iDstCnt);

        //4.获取未展开台账子表信息
        int iNexpCnt;
        CFmlBuf bufTradeOther;
        try
        {
            dao.Clear();
            dao.SetParam(":VTRADE_ID", strTradeId);
            dao.SetParam(":VRSRV_VALUE_CODE", "NEXP");
            iNexpCnt = dao.jselect(bufTradeOther, "SEL_BY_TRADEID_RSRVVLLUE", "TF_B_TRADE_OTHER");
        }
        catch (Exception &ex)
        {
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查询业务台账其他信息子表出错！[%s]",ex.getCause()->what());
        }


        //5.获取SP台帐子表信息
        int iSpCnt;
        CFmlBuf bufTradeSp;
        try
        {
            dao.Clear();
            dao.SetParam(":VTRADE_ID", strTradeId);
            iSpCnt = dao.jselect(bufTradeSp, "SEL_BY_TRADEID", "TF_B_TRADE_SP");
        }
        catch (Exception &ex)
        {
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查询业务台账SP信息子表出错！[%s]",ex.getCause()->what());
        }

        //6.获取物品等其他元素台帐子表信息
        int iElementCnt;
        CFmlBuf bufTradeElement;
        try
        {
            dao.Clear();
            dao.SetParam(":VTRADE_ID", strTradeId);
            iElementCnt = dao.jselect(bufTradeElement, "SEL_BY_TRADEID", "TF_B_TRADE_ELEMENT");
        }
        catch (Exception &ex)
        {
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查询业务台账其他元素信息子表出错！[%s]",ex.getCause()->what());
        }

        //7.当有产品、服务或优惠发生变更时，进行相关判断
        if ((iPdtCnt != 0 || iSvcCnt != 0 || iDstCnt != 0 || iNexpCnt != 0 || iSpCnt != 0 || iElementCnt != 0
            || strTradeTypeCode == "93" || strTradeTypeCode == "240"|| strTradeTypeCode == "72" || strTradeTypeCode == "73" || strTradeTypeCode == "79") 
            && strTradeTypeCode != "190" && strTradeTypeCode != "310" && strTradeTypeCode != "790" && strTradeTypeCode != "791")
        {
			try
		    {
				CFmlBuf bufCheckTagR;
				dao.Clear();
		        dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
		        dao.SetParam(":VTAG_CODE", "CSM_CHR_NOPRODUCTCHECKED");
		        dao.SetParam(":VSUBSYS_CODE", "CSM");
		        dao.SetParam(":VUSE_TAG", "0");
		        if(dao.jselect(bufCheckTagR, "SEL_BY_TAGCODE_1","TD_S_TAG")>0){
					LOG_TRACE(logTradeCheckAfterTrade, "退出下面的校验。。。");
					return;
				}else{
					//产品限制检查
		            CheckProductModelLimit(inBuf, outBuf);
		            CheckElmentsAndItsDiscnt(inBuf);
				}
		    }
		    catch(Exception &ex)
		    {
		        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:退出产品校验TAG校验异常！");
		    }

        }


    	if (strNewSerialNumber!="")
    	{
            CFmlBuf tempBuf;
            int iRowCount = 0;
            try
            {
                dao.Clear();
                dao.SetParam(":PHONE_CODE",strNewSerialNumber);
                dao.SetParam(":PRODUCT_ID",strProductId);
                dao.SetParam(":EPARCHY_CODE",strEparchyCode);
                iRowCount = dao.jselect(tempBuf,"SEL_EXIST_OR_3","TD_B_DEFPRODUCT_PHONE");
            }
            catch (Exception &ex)
            {
                THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查找号码绑定产品信息失败[%s]",ex.getCause()->what());
            }
            if (tempBuf.GetInt("X_SELCOUNT")>0  && strTradeTypeCode!="1060")
            {
                try
                {
                    tempBuf.ClearFmlValue();
                    dao.Clear();
                    dao.SetParam(":PHONE_CODE",strNewSerialNumber);
                    dao.SetParam(":PRODUCT_ID",strProductId);
                    dao.SetParam(":EPARCHY_CODE",strEparchyCode);
                    iRowCount = dao.jselect(tempBuf,"SEL_EXIST_AND_3","TD_B_DEFPRODUCT_PHONE");
                }
                catch (Exception &ex)
                {
                    THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查找号码绑定产品信息失败",ex.getCause()->what());
                }
                if (tempBuf.GetInt("X_SELCOUNT")==0)
                {
                    THROW_C(CRMException, CHECKAFTERTRADE_ERR, "号码与产品绑定校验:号码与产品不匹配");
                }
            }
      }

    	CFmlBuf bufVTradeProduct;
    	try
        {
            dao.Clear();
            dao.SetParam(":VTRADE_ID", strTradeId);
             dao.SetParam(":VPRODUCT_MODE", "00");
            iPdtCnt = dao.jselect(bufVTradeProduct, "SEL_BY_PK_PMODE","TF_B_TRADE_PRODUCT");
        }
        catch (Exception &ex)
        {
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查询业务台账产品子表出错！[%s]",ex.getCause()->what());
        }

        for (int i=0;i<iPdtCnt;i++)
        {
            if ((bufVTradeProduct.GetString("MODIFY_TAG",i)=="0"||bufVTradeProduct.GetString("MODIFY_TAG",i)=="2")&&(strTradeTypeCode!="250"))
            {
                CFmlBuf tempBuf;
                int iRowCount ;
                bool bFound = false ;
                 //号码产品相互限制
                iRowCount = 0;
                try
                {
                	tempBuf.ClearFmlValue();
                    dao.Clear();
                    dao.SetParam(":PHONE_CODE",strSerialNumber);
                    dao.SetParam(":PRODUCT_ID",(int)bufVTradeProduct.GetInt("PRODUCT_ID",i));
                    dao.SetParam(":EPARCHY_CODE",strEparchyCode);
                    iRowCount = dao.jselect(tempBuf,"SEL_EXIST_OR_3","TD_B_DEFPRODUCT_PHONE");
                }
                catch (Exception &ex)
                {
                    THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查找号码绑定产品信息失败[%s]",ex.getCause()->what());
                }
                if (tempBuf.GetInt("X_SELCOUNT")>0)
                {
                    try
                    {
                        tempBuf.ClearFmlValue();
                        dao.Clear();
                        dao.SetParam(":PHONE_CODE",strSerialNumber);
                        dao.SetParam(":PRODUCT_ID",(int)bufVTradeProduct.GetInt("PRODUCT_ID",i));
                        dao.SetParam(":EPARCHY_CODE",strEparchyCode);
                        iRowCount = dao.jselect(tempBuf,"SEL_EXIST_AND_3","TD_B_DEFPRODUCT_PHONE");
                    }
                    catch (Exception &ex)
                    {
                        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查找号码绑定产品信息失败",ex.getCause()->what());
                    }
                    if (tempBuf.GetInt("X_SELCOUNT")==0)
                    {
                        try
                    	{
                    	    tempBuf.ClearFmlValue();
                    	    dao.Clear();
                    	    dao.SetParam(":PHONE_CODE",strSerialNumber);
                    	    dao.SetParam(":PRODUCT_ID",(int)bufVTradeProduct.GetInt("PRODUCT_ID",i));
                    	    dao.SetParam(":EPARCHY_CODE",strEparchyCode);
                    	    iRowCount = dao.jselect(tempBuf,"SEL_EXIST_AND_1","TD_B_DEFPRODUCT_PHONE");
                    	}
                    	catch (Exception &ex)
                    	{
                    	    THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查找号码绑定产品信息失败",ex.getCause()->what());
                    	}
                    	if (tempBuf.GetInt("X_SELCOUNT")==0)
                    	{
                    	    THROW_C(CRMException, CHECKAFTERTRADE_ERR, "号码与产品绑定校验[产品与号段互相强制]:号码与产品不匹配");
                    	}
                    	else  bFound = true ;
                    }
                    else  bFound = true ;
                }
                if( bFound ) continue ;

                //产品限制号码段 add by digy@20070908 start
                bFound = false ;
                iRowCount = 0;
                try
                {
                	tempBuf.ClearFmlValue();
                    dao.Clear();
                    dao.SetParam(":PHONE_CODE",strSerialNumber);
                    dao.SetParam(":EPARCHY_CODE",strEparchyCode);
                    iRowCount = dao.jselect(tempBuf,"SEL_EXIST_PHONE_1","TD_B_DEFPRODUCT_PHONE");
                }
                catch (Exception &ex)
                {
                    THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查找号码绑定产品信息失败[%s]",ex.getCause()->what());
                }
				// 排除1060限制验证 update by dangsw@20120206 xizhaosi
				if (tempBuf.GetInt("X_SELCOUNT")>0 && strTradeTypeCode!="1060")
                {
                    try
                    {
                        tempBuf.ClearFmlValue();
                        dao.Clear();
                        dao.SetParam(":PHONE_CODE",strSerialNumber);
                        dao.SetParam(":PRODUCT_ID",(int)bufVTradeProduct.GetInt("PRODUCT_ID",i));
                        dao.SetParam(":EPARCHY_CODE",strEparchyCode);
                        iRowCount = dao.jselect(tempBuf,"SEL_EXIST_AND_1","TD_B_DEFPRODUCT_PHONE");
                    }
                    catch (Exception &ex)
                    {
                        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查找号码绑定产品信息失败",ex.getCause()->what());
                    }
                    if (tempBuf.GetInt("X_SELCOUNT")==0)
                    {
                        THROW_C(CRMException, CHECKAFTERTRADE_ERR, "号码与产品限制：该号码不能受理此产品");
                    }
                    else bFound = true ;
                }				
                if( bFound ) continue ;
                //产品限制号码段
                bFound = false ;
                iRowCount = 0;
                try
                {
                	tempBuf.ClearFmlValue();
                    dao.Clear();
                    dao.SetParam(":PRODUCT_ID",(int)bufVTradeProduct.GetInt("PRODUCT_ID",i));
                    dao.SetParam(":EPARCHY_CODE",strEparchyCode);
                    iRowCount = dao.jselect(tempBuf,"SEL_EXIST_PRODUCT_2","TD_B_DEFPRODUCT_PHONE");
                }
                catch (Exception &ex)
                {
                    THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查找号码绑定产品信息失败1[%s]",ex.getCause()->what());
                }
                if (tempBuf.GetInt("X_SELCOUNT")>0)
                {
                    try
                    {
                        tempBuf.ClearFmlValue();
                        dao.Clear();
                        dao.SetParam(":PHONE_CODE",strSerialNumber);
                        dao.SetParam(":PRODUCT_ID",(int)bufVTradeProduct.GetInt("PRODUCT_ID",i));
                        dao.SetParam(":EPARCHY_CODE",strEparchyCode);
                        iRowCount = dao.jselect(tempBuf,"SEL_EXIST_AND_2","TD_B_DEFPRODUCT_PHONE");
                    }
                    catch (Exception &ex)
                    {
                        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查找号码绑定产品信息失败",ex.getCause()->what());
                    }
                    if (tempBuf.GetInt("X_SELCOUNT")==0)
                    {
                        THROW_C(CRMException, CHECKAFTERTRADE_ERR, "号码与产品绑定校验[产品强制号段]:产品与号码不匹配");
                    }
                }
                //add by digy@20070908 end

            }
            
            
        }     
        for (int i=0;i<iPdtCnt;i++)
        {
        	
        	if (bufVTradeProduct.GetString("MODIFY_TAG",i)=="0" && (strTradeTypeCode=="120"||strTradeTypeCode=="110"))
        	{
            	
            	
            	LOG_TRACE_P1(logTradeCheckAfterTrade, "+++++++++++++[%s]strNetTypeCode++++++++++++=", strNetTypeCode.c_str());
            	LOG_TRACE_P1(logTradeCheckAfterTrade, "+++++++++++++[%s]strSerialNumber++++++++++++=", strSerialNumber.c_str());
            	 
            	CFmlBuf bufTag1;
            	CFmlBuf bufPhoneProductProduct;
            	int tagCnt = 0;
            	int phoneProductCnt =0;
            	string checkErrorInfo  ="";
            	string netTypeTag  ="1";
            	bool bPhoneProductFinded = false;
            	StaticDAO& daoTag1 = StaticDAO::getDAO();
	    	    daoTag1.Clear();
	    	    daoTag1.SetParam(":VEPARCHY_CODE", "ZZZZ");
	    		daoTag1.SetParam(":VTAG_CODE", "CS_3GNETCODE_TAG");
	    		daoTag1.SetParam(":VSUBSYS_CODE", "CSM");
	    		daoTag1.SetParam(":VUSE_TAG", "0");
	    		tagCnt = daoTag1.jselect(bufTag1, "SEL_ALL_BY_TAGCODE","TD_S_TAG");
	    		 
	    	string netStrTagInfo = tagCnt > 0 ? bufTag1.GetString("TAG_INFO") : "||";
				LOG_TRACE_P1(logTradeCheckAfterTrade, "+++++++++++++tagCnt++++++++++++=[%d]",tagCnt);
			
				
				CFmlBuf bufQryPhoneProduct;
				int qryProductCnt =0;
				try
                {
					dao.Clear();
	                dao.SetParam(":VPRODUCT_ID", bufVTradeProduct.GetString("PRODUCT_ID",i));
	                qryProductCnt = dao.jselect(bufQryPhoneProduct, "SEL_BY_PK","TD_B_PRODUCT");
                
            	}catch(Exception &ex)
                {
                    THROW_AGC(CRMException, ex, Grade::WARNING, GETSVCSTATE_ERR, "获取产品信息异常！！");
                }
                if(qryProductCnt > 0)
                {
                	if(netStrTagInfo.find("|"+bufQryPhoneProduct.GetString("NET_TYPE_CODE")+"|", 0) != string::npos )
                   		netTypeTag  ="1";
                  else
                    netTypeTag  ="0";
                }
				
	        LOG_TRACE_P1(logTradeCheckAfterTrade, "+++++++++++++netTypeTag++++++++++++=[%s]",netTypeTag.c_str());
			    		
	        //QC:3410 Begin
		//北京需求，3G靓号不能变更资费低于现有套餐的套餐
	        int iTagCount = -1;
					try
        	{
            	   dao.Clear();
            	   bufTag.ClearFmlValue();
            	   dao.SetParam(":VEPARCHY_CODE", "ZZZZ");
            	   dao.SetParam(":VTAG_CODE", "N4_3410_LIANHAOXUANZE");
            	   dao.SetParam(":VSUBSYS_CODE", "CSM");
            	   dao.SetParam(":VUSE_TAG", "0");
            	   iTagCount = dao.jselect(bufTag, "SEL_ALL_BY_TAGCODE","TD_S_TAG");
        	}
        	catch(Exception &ex)
        	{
            	   THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "获取实例化版本标记出错！");
        	}
					if(iTagCount > 0) 
					{
						  //qc:8651 Begin
							int lastMonthCnt12 = -1;
							CFmlBuf bufIsLuckyNumber;
              dao.Clear();
              dao.SetParam(":VUSER_ID", strUserId);
              lastMonthCnt12 = dao.jselect(bufIsLuckyNumber, "CHECK_LUCKEY_NUMBER_LASTMONTH_BJ","TD_S_CPARAM");
              //当还存在88888888，88888881资费优惠时，判断为靓号，若不存在则根据配置走，
              if(lastMonthCnt12 > 0)
              {
              	LOG_TRACE(logTradeCheckAfterTrade, "该用户存在88888888资费，且没有过期");
              	for (int i=0;i<iPdtCnt;i++)
				   			{
									if(bufVTradeProduct.GetString("MODIFY_TAG",i)=="1")
									{
					    			CFmlBuf newPhoneProductFeeBuf;
					    			int iFeeCount = -1;
					    			dao.Clear();
		                dao.SetParam(":VUSER_ID", bufVTradeProduct.GetString("USER_ID",i));
		                iFeeCount = dao.jselect(newPhoneProductFeeBuf, "SEL_NEW_PRODUCT_FEE","TF_F_USER_PRODUCT");
		                if(iFeeCount > 0) 
		               	{
		                		dao.Clear();
		                		dao.SetParam(":VRSRV_VALUE2", newPhoneProductFeeBuf.GetString("RSRV_VALUE2"));
		                		dao.SetParam(":VSERIAL_NUMBER", strSerialNumber);
		                		dao.SetParam(":VNET_TYPE", netTypeTag);
		                		phoneProductCnt = dao.jselect(bufPhoneProductProduct, "SEL_SERIAL_NUMBER_PRODUCT_BY_FEE","TF_R_MPHONECODE_USE");
		                		//LOG_TRACE_P1(logTradeCheckAfterTrade, "查询靓号可办理套餐的dao:%s",dao.parToString().c_str());
		                		LOG_TRACE_P1(logTradeCheckAfterTrade, "+++++++++++++查询出的靓号可以办理的套餐个数++++++++++++=[%d]",phoneProductCnt);
		                }
					 				}
					 			}
              }
              else
              {
              	LOG_TRACE(logTradeCheckAfterTrade, "该用户不存在88888888资费，或者已经过期，需设置标志跳过靓号判断");
              	//增加设置，跳过靓号判断
              	bPhoneProductFinded = true;
              	dao.Clear();
            		dao.SetParam(":VSERIAL_NUMBER", strSerialNumber);
            		dao.SetParam(":VNET_TYPE", netTypeTag);
            		// dao.SetParam(":VUSER_ID",strUserId);   
            		phoneProductCnt = dao.jselect(bufPhoneProductProduct, "SEL_SERIAL_NUMBER_PRODUCT","TF_R_MPHONECODE_USE");
              }
              //qc:8651 End 
					}
	        else
	        {
            dao.Clear();
            dao.SetParam(":VSERIAL_NUMBER", strSerialNumber);
            dao.SetParam(":VNET_TYPE", netTypeTag);
            // dao.SetParam(":VUSER_ID",strUserId);   
            phoneProductCnt = dao.jselect(bufPhoneProductProduct, "SEL_SERIAL_NUMBER_PRODUCT","TF_R_MPHONECODE_USE");
	        }
          //QC:3410 End
          LOG_TRACE_P1(logTradeCheckAfterTrade, "+++++++++++++phoneProductCnt++++++++++++=[%d]",phoneProductCnt);
          for (int z = 0 ; z<phoneProductCnt; z++){
            if (bufVTradeProduct.GetInt("PRODUCT_ID",i) == bufPhoneProductProduct.GetInt("PRODUCT_ID",z)){
              LOG_TRACE_P1(logTradeCheckAfterTrade, "+++++++++++++PRODUCT_ID_Z_0++++++++++++=[%d]",bufPhoneProductProduct.GetInt("PRODUCT_ID",z));
              bPhoneProductFinded = true;
              break;
            }else{
              LOG_TRACE_P1(logTradeCheckAfterTrade, "+++++++++++++PRODUCT_ID_Z_1++++++++++++=[%d]",bufPhoneProductProduct.GetInt("PRODUCT_ID",z));
              if (checkErrorInfo =="")
                checkErrorInfo = bufPhoneProductProduct.GetString("PRODUCT_NAME",z);
              else
                checkErrorInfo = checkErrorInfo+","+ bufPhoneProductProduct.GetString("PRODUCT_NAME",z);
            }   		
          }
                if(!bPhoneProductFinded){
                	//靓号最后一个月可办理变更
                	StaticDAO& daoTag2 = StaticDAO::getDAO();
                	CFmlBuf bufTag2;
                	CFmlBuf bufCheckLastMonth;
                	int tagCnt1 = 0;
                	int lastMonthCnt1 = 0;
                	daoTag2.Clear();
	                daoTag2.SetParam(":VEPARCHY_CODE", "ZZZZ");
                	daoTag2.SetParam(":VTAG_CODE", "CS_CHECKLUCKEYNUMBERLASTMONTH_TAG");
                	daoTag2.SetParam(":VSUBSYS_CODE", "CSM");
                	daoTag2.SetParam(":VUSE_TAG", "0");
                	tagCnt1 = daoTag2.jselect(bufTag2, "SEL_ALL_BY_TAGCODE","TD_S_TAG");
                	string strTagChar = tagCnt1 > 0 ? bufTag2.GetString("TAG_CHAR") : "0";

                	if(strTagChar=="1"){
                	     dao.Clear();
                	     dao.SetParam(":VUSER_ID", strUserId);
                	     lastMonthCnt1 = dao.jselect(bufCheckLastMonth, "CHECK_LUCKEY_NUMBER_LASTMONTH","TD_S_CPARAM");
                	     if(lastMonthCnt1 > 0){
                	         bPhoneProductFinded = true;
                	     }
                	}
                }

                if (checkErrorInfo !="" &&  !bPhoneProductFinded) 
            		THROW_C_P1(CRMException, 8888, "此号码为靓号，不允许变更为此产品，可变更产品为:%s！",checkErrorInfo.c_str());
        	}
        	
        }

        //5.和服务状态有关的判断
        //5.0获取业务台账服务状态子表
        int iSvcStateCnt;
        CFmlBuf bufVTradeSvcState;
        try
        {
            dao.Clear();
            dao.SetParam(":VACCEPT_MONTH", strTradeId.substr(4,2));
            dao.SetParam(":VTRADE_ID", strTradeId);
            iSvcStateCnt = dao.jselect(bufVTradeSvcState, "SEL_BY_IDMONTH","TF_B_TRADE_SVCSTATE");
        }
        catch (Exception &ex)
        {
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查询业务台账服务状态子表出错！[%s]",ex.getCause()->what());
        }

        //5.1服务状态间限制
        if (iSvcStateCnt != 0)
        {
            //获取业务办理后用户所有的服务状态
            CFmlBuf bufVTradeSvcState_UserAllSvcState;
            GetUserAllSvcstateAfterTrade(strTradeId, strUserId, bufVTradeSvcState_UserAllSvcState);
            int iUserAllSvcState = bufVTradeSvcState_UserAllSvcState.GetCount("SERVICE_ID");
            //限制判断
            CFmlBuf bufVSvcstateLimit;
            for (int i = 0 ; i<iSvcStateCnt; i++) //it
            {
                if (bufVTradeSvcState.GetString("MODIFY_TAG",i) == "0" || bufVTradeSvcState.GetString("MODIFY_TAG",i) == "A")  //增
                {
                    //互斥判断
                    if (ExistsSvcstateLimitA((int)bufVTradeSvcState.GetInt("SERVICE_ID",i), bufVTradeSvcState.GetString("STATE_CODE",i), strEparchyCode, "0", bufVSvcstateLimit))
                    {
                        //
                        int iSvcstateLimit = bufVSvcstateLimit.GetCount("SERVICE_ID");
                        for (int j=0; j<iUserAllSvcState; j++) //itAllSvcstate
                        {
                            //排除自己的那条记录，但不排除用户已有的同一条服务
                            if ((int)bufVTradeSvcState.GetInt("SERVICE_ID",i) == (int)bufVTradeSvcState_UserAllSvcState.GetInt("SERVICE_ID",j) &&
                                bufVTradeSvcState.GetString("STATE_CODE",i) == bufVTradeSvcState_UserAllSvcState.GetString("STATE_CODE",j) &&
                                bufVTradeSvcState_UserAllSvcState.GetString("MODIFY_TAG",j) == "0" &&
                                bufVTradeSvcState.GetString("START_DATE",i) == bufVTradeSvcState_UserAllSvcState.GetString("START_DATE",j))
                                continue;

                            int iLimitSvcstate = bufVSvcstateLimit.GetCount("SERVICE_ID");
                            for (int k=0; k<iLimitSvcstate; k++)
                            {
                                if ((int)bufVTradeSvcState_UserAllSvcState.GetInt("SERVICE_ID",j) == (int)bufVSvcstateLimit.GetInt("SERVICE_ID",k) &&
                                    bufVTradeSvcState_UserAllSvcState.GetString("STATE_CODE",j) == bufVSvcstateLimit.GetString("STATE_CODE_B",k) &&
                                    ((bufVTradeSvcState_UserAllSvcState.GetString("END_DATE",j) >= bufVTradeSvcState.GetString("START_DATE",i) &&
                                    bufVTradeSvcState_UserAllSvcState.GetString("START_DATE",j) <= bufVTradeSvcState.GetString("START_DATE",i) ) ||
                                    (bufVTradeSvcState_UserAllSvcState.GetString("START_DATE",j) >= bufVTradeSvcState.GetString("START_DATE",i) &&
                                    bufVTradeSvcState_UserAllSvcState.GetString("START_DATE",j) <= bufVTradeSvcState.GetString("END_DATE",i))))
                                {
                                    char chId[10];
                                    sprintf(chId, "%d", (int)bufVTradeSvcState.GetInt("SERVICE_ID",i));
                                    string strName = CParamDAO::getParam("ServiceName", chId);
                                    string strNameA = CParamDAO::getParam("SvcstateName", chId, bufVTradeSvcState.GetString("STATE_CODE",i));
                                    string strNameB = CParamDAO::getParam("SvcstateName", chId, bufVTradeSvcState_UserAllSvcState.GetString("STATE_CODE",j));
                                    THROW_C_P4(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:新增服务状态[%s.%s]和服务状态[%s.%s]互斥，不能同时生效，业务不能继续办理！", strName.c_str(), strNameA.c_str(), strName.c_str(), strNameB.c_str());
                                }
                            }
                        }
                    }

                    //部分依赖判断
                    if (ExistsSvcstateLimitA((int)bufVTradeSvcState.GetInt("SERVICE_ID",i), bufVTradeSvcState.GetString("STATE_CODE",i), strEparchyCode, "1", bufVSvcstateLimit))
                    {
                        int iSvcstateLimit = bufVSvcstateLimit.GetCount("SERVICE_ID");
                        bool bfinded = false;
                        for (int j=0; j<iUserAllSvcState; j++)
                        {
                            for (int k=0; k<iSvcstateLimit; k++)
                            {
                                if ( (int)bufVTradeSvcState_UserAllSvcState.GetInt("SERVICE_ID",j) == (int)bufVSvcstateLimit.GetInt("SERVICE_ID",k)&&
                                     bufVTradeSvcState_UserAllSvcState.GetString("STATE_CODE",j) == bufVSvcstateLimit.GetString("STATE_CODE_B",k) &&
                                     bufVTradeSvcState_UserAllSvcState.GetString("START_DATE",j) <= bufVTradeSvcState.GetString("START_DATE",i)&&
                                     bufVTradeSvcState_UserAllSvcState.GetString("END_DATE",j) >= bufVTradeSvcState.GetString("END_DATE",i))
                                {
                                    bfinded = true;
                                    break;
                                }
                            }
                            if (bfinded) break;
                        }
                        if (!bfinded)
                        {
                            char chId[10];
                            sprintf(chId, "%d", (int)bufVTradeSvcState.GetInt("SERVICE_ID",i));
                            string strName = CParamDAO::getParam("ServiceName", chId);
                            string strNameA = CParamDAO::getParam("SvcstateName", chId, bufVTradeSvcState.GetString("STATE_CODE",i));
                            string strNameB;
                            for(int k=0; k<iSvcstateLimit; k++)
                            {
                                if (k != 0) strNameB += " 或 ";
                                strNameB += CParamDAO::getParam("SvcstateName", chId, bufVSvcstateLimit.GetString("STATE_CODE_B",k));
                            }
                            THROW_C_P3(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:新增服务状态[%s.%s]不能生效，因为它所依赖的服务状态[%s]不存在。业务不能继续办理！", strName.c_str(), strNameA.c_str(), strNameB.c_str() );
                        }
                    }

                    //完全依赖
                    if (ExistsSvcstateLimitA((int)bufVTradeSvcState.GetInt("SERVICE_ID",i), bufVTradeSvcState.GetString("STATE_CODE",i), strEparchyCode, "2", bufVSvcstateLimit))
                    {
                        bool bfinded;
                        int iLimitSvcstate = bufVSvcstateLimit.GetCount("SERVICE_ID");
                        for (int k=0; k<iLimitSvcstate; k++)
                        {
                            bfinded = false;
                            for (int j=0; j<iUserAllSvcState; j++)
                            {
                                if ( (int)bufVTradeSvcState_UserAllSvcState.GetInt("SERVICE_ID",j) == (int)bufVSvcstateLimit.GetInt("SERVICE_ID",k) &&
                                    bufVTradeSvcState_UserAllSvcState.GetString("STATE_CODE",j) == bufVSvcstateLimit.GetString("STATE_CODE_B",k) &&
                                    bufVTradeSvcState_UserAllSvcState.GetString("START_DATE",j) <= bufVTradeSvcState.GetString("START_DATE",i) &&
                                    bufVTradeSvcState_UserAllSvcState.GetString("END_DATE",j) >= bufVTradeSvcState.GetString("END_DATE",i))
                                {
                                    bfinded = true;
                                    break;
                                }
                            }
                            if (!bfinded)
                            {
                                char chId[10];
                                sprintf(chId, "%d", (int)bufVTradeSvcState.GetInt("SERVICE_ID",i));
                                string strName = CParamDAO::getParam("ServiceName", chId);
                                string strNameA = CParamDAO::getParam("SvcstateName", chId, bufVTradeSvcState.GetString("STATE_CODE",i));
                                string strNameB = CParamDAO::getParam("SvcstateName", chId, bufVSvcstateLimit.GetString("STATE_CODE_B",k));
                                THROW_C_P4(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:新增服务状态[%s.%s]不能生效，因为它所依赖的服务状态[%s.%s]不存在。业务不能继续办理！", strName.c_str(), strNameA.c_str(), strName.c_str(), strNameB.c_str() );
                            }
                        }
                    }
                }
                else if (bufVTradeSvcState.GetString("MODIFY_TAG",i) == "1" || bufVTradeSvcState.GetString("MODIFY_TAG",i) == "B")  //删
                {
                    //被部分依赖
                    if (ExistsSvcstateLimitB((int)bufVTradeSvcState.GetInt("SERVICE_ID",i), bufVTradeSvcState.GetString("STATE_CODE",i), strEparchyCode, "1", bufVSvcstateLimit))
                    {
                        int iSvcstateLimit = bufVSvcstateLimit.GetCount("STATE_CODE_A");
                        for (int k=0; k<iSvcstateLimit; k++) //itLimitSvcstate
                        {
                            for (int j=0; j<iUserAllSvcState; j++) //itAllSvcstate
                            {
                                if ((int)bufVTradeSvcState_UserAllSvcState.GetInt("SERVICE_ID",j) == (int)bufVSvcstateLimit.GetInt("SERVICE_ID",k) &&
                                    bufVTradeSvcState_UserAllSvcState.GetString("STATE_CODE",j) == bufVSvcstateLimit.GetString("STATE_CODE_A",k) &&
                                    bufVTradeSvcState_UserAllSvcState.GetString("END_DATE",j) >= bufVTradeSvcState.GetString("END_DATE",i))
                                {
                                    CFmlBuf bufVSvcstateLimit2;
                                    ExistsSvcstateLimitA((int)bufVTradeSvcState_UserAllSvcState.GetInt("SERVICE_ID",j), bufVTradeSvcState_UserAllSvcState.GetString("STATE_CODE",j), strEparchyCode, "1", bufVSvcstateLimit2);
                                    bool bfinded = false;
                                    for (int m=0; m<iUserAllSvcState; m++) //itAllSvcstate2
                                    {
                                        int iSvcstateLimit2 = bufVSvcstateLimit2.GetCount("SERVICE_ID");
                                        for (int n=0; n<iSvcstateLimit2; i++)
                                        {
                                            if ((int)bufVTradeSvcState_UserAllSvcState.GetInt("SERVICE_ID",m)== (int)bufVSvcstateLimit2.GetInt("SERVICE_ID",n) &&
                                                bufVTradeSvcState_UserAllSvcState.GetString("STATE_CODE",m) == bufVSvcstateLimit2.GetString("STATE_CODE_B",k) &&
                                                bufVTradeSvcState_UserAllSvcState.GetString("START_DATE",m) <= bufVTradeSvcState_UserAllSvcState.GetString("START_DATE",j) &&
                                                bufVTradeSvcState_UserAllSvcState.GetString("END_DATE",m) >= bufVTradeSvcState_UserAllSvcState.GetString("END_DATE",j))
                                            {
                                                bfinded = true;
                                                break;
                                            }
                                        }
                                        if (bfinded) break;
                                    }
                                    if (!bfinded)
                                    {
                                        char chId[10];
                                        sprintf(chId, "%d", (int)bufVTradeSvcState.GetInt("SERVICE_ID",i));
                                        string strName = CParamDAO::getParam("ServiceName", chId);
                                        string strNameA = CParamDAO::getParam("SvcstateName", chId, bufVTradeSvcState_UserAllSvcState.GetString("STATE_CODE",j));
                                        string strNameB = CParamDAO::getParam("SvcstateName", chId, bufVTradeSvcState.GetString("STATE_CODE",i));
                                        THROW_C_P4(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:服务状态[%s.%s]不能生效，因为它所依赖的服务状态[%s.%s]被删除。业务不能继续办理！", strName.c_str(), strNameA.c_str(), strName.c_str(), strNameB.c_str() );
                                    }
                                    ////////////
                                }
                            }
                        }
                    }
                    //被完全依赖
                    if (ExistsSvcstateLimitB((int)bufVTradeSvcState.GetInt("SERVICE_ID",i), bufVTradeSvcState.GetString("STATE_CODE",i), strEparchyCode, "2", bufVSvcstateLimit))
                    {
                        int iSvcstateLimit = bufVSvcstateLimit.GetCount("STATE_CODE_A");
                        for ( int k=0; k<iSvcstateLimit; k++) //itLimitSvcstate
                        {
                            for (int j=0; j<iUserAllSvcState; j++) //itAllSvcstate
                            {
                                if ((int)bufVTradeSvcState_UserAllSvcState.GetInt("SERVICE_ID",j) == (int)bufVSvcstateLimit.GetInt("SERVICE_ID",k) &&
                                    bufVTradeSvcState_UserAllSvcState.GetString("STATE_CODE",j) == bufVSvcstateLimit.GetString("STATE_CODE_A",k) &&
                                    bufVTradeSvcState_UserAllSvcState.GetString("END_DATE",j) >= bufVTradeSvcState.GetString("END_DATE",i))
                                {
                                    char chId[10];
                                    sprintf(chId, "%d", (int)bufVTradeSvcState.GetInt("SERVICE_ID",i));
                                    string strName = CParamDAO::getParam("ServiceName", chId);
                                    string strNameA = CParamDAO::getParam("SvcstateName", chId, bufVTradeSvcState.GetString("STATE_CODE",i));
                                    string strNameB = CParamDAO::getParam("SvcstateName", chId, bufVTradeSvcState_UserAllSvcState.GetString("STATE_CODE",j));
                                    THROW_C_P4(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:服务状态[%s.%s]不能被删除，因为它被用户的另一个服务状态[%s.%s]所依赖。业务不能继续办理！", strName.c_str(), strNameA.c_str(), strName.c_str(), strNameB.c_str() );
                                }
                            }
                        }
                    }
                }
                else if (bufVTradeSvcState.GetString("MODIFY_TAG",i) == "2")  //改
                {
                    ;
                }
                else if (bufVTradeSvcState.GetString("MODIFY_TAG",i) == "3")  //保留
                {
                    ;
                }
                else
                {
                    THROW_C(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:服务状态的修改标记不正确！");
                }
            }
        }
        //账户变更限制
        int iTradeAcct;
        CFmlBuf bufVTradeAcct;
        try
        {
            dao.Clear();
            dao.SetParam(":VTRADE_ID", strTradeId);
            iTradeAcct = dao.jselect(bufVTradeAcct, "SEL_BY_TRADE","TF_B_TRADE_ACCT");
        }
        catch (Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查询业务台账账户子表出错！");
        }
        if (iTradeAcct>0)
        {
        	if (bufVTradeAcct.GetString("ACCT_ID")!=inBuf.GetString("ACCT_ID"))
        	{
        		int iRelationUu;
        		CFmlBuf bufVRelationUu;
        		try
    			{
    			    dao.Clear();
    			    dao.SetParam(":VUSER_ID", strUserId);
    			    iRelationUu = dao.jselect(bufVRelationUu, "SEL_BY_USERID","TF_F_RELATION_UU");
    			}
    			catch (Exception &ex)
    			{
    			    THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查询业务台账账户子表出错！");
    			}
    			if (iRelationUu>0)
    				THROW_C(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:成员用户不能更改账户");
    		}
        }
  }
  else
  {
    CheckSpProductModelLimit(inBuf, outBuf);
  }
    //Added by Yangf@2008-12-27：组合产品与个人产品互斥校验(含合账校验)
    ChkRelaUserProductLimit(strTradeId, strUserId, strEparchyCode);
}


/**
 *检查服务iServiceId的状态strStateCode与其他状态是否存在互斥、部分依赖、完全依赖的关系
 *
 */
bool TradeCheckAfterTrade::ExistsSvcstateLimitA(const int & iServiceId,const string &strStateCode, const string &strEparchyCode, const string &strLimitTag, CFmlBuf &bufVSvcstateLimit)
{
    StaticDAO& dao = StaticDAO::getDAO();
    bufVSvcstateLimit.ClearFmlValue();
    try
    {
        dao.Clear();
        dao.SetParam(":VSERVICE_ID", iServiceId);
        dao.SetParam(":VSTATE_CODE_A", strStateCode);
        dao.SetParam(":VLIMIT_TAG", strLimitTag);
        dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
        if (dao.jselect(bufVSvcstateLimit, "JUDGE_A_LIMIT_EXISTS","TD_S_SVCSTATE_LIMIT") > 0)
        {
            return true;
        }
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查询服务状态限制表出错！[%s]",ex.getCause()->what());
    }
    return false;
}

/**
 *检查服务iServiceId的状态strStateCode与其他状态是否存在被部分依赖、被完全依赖的关系
 *
 */
bool TradeCheckAfterTrade::ExistsSvcstateLimitB(const int & iServiceId,const string &strStateCode, const string &strEparchyCode, const string &strLimitTag, CFmlBuf &bufVSvcstateLimit)
{
    StaticDAO& dao = StaticDAO::getDAO();
    bufVSvcstateLimit.ClearFmlValue();
    try
    {
        dao.Clear();
        dao.SetParam(":VSERVICE_ID", iServiceId);
        dao.SetParam(":VSTATE_CODE_B", strStateCode);
        dao.SetParam(":VLIMIT_TAG", strLimitTag);
        dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
        if (dao.jselect(bufVSvcstateLimit, "JUDGE_B_LIMIT_EXISTS","TD_S_SVCSTATE_LIMIT") > 0)
        {
            return true;
        }
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查询服务状态限制表出错！[%s]",ex.getCause()->what());
    }
    return false;
}

/**
 *获取办理业务后用户的所有服务状态
 *
 */
void TradeCheckAfterTrade::GetUserAllSvcstateAfterTrade(const string &strTradeId, const string &strUserId, CFmlBuf &bufVTradeSvcState_UserAllSvcState)
{
    bufVTradeSvcState_UserAllSvcState.ClearFmlValue();

    StaticDAO& dao = StaticDAO::getDAO();
    CFmlBuf bufTag;
    string strTag="";
    try  //仅在中间层cpp中获取该参数
    {
        dao.Clear();
        bufTag.ClearFmlValue();
        dao.SetParam(":VEPARCHY_CODE", "ZZZZ");
        dao.SetParam(":VTAG_CODE", "CS_CHR_GENESVCSTATENEW");
        dao.SetParam(":VSUBSYS_CODE", "CSM");
        dao.SetParam(":VUSE_TAG", "0");
        if (dao.jselect(bufTag, "SEL_ALL_BY_TAGCODE","TD_S_TAG") == 0)
        	strTag= "0";
        else
        	strTag=bufTag.GetString("TAG_CHAR");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "服务状态变更:获取当前参数出错！");
    }

	if (strTag != "1")
    {
    	//获取办理业务后用户所有服务状态
		try
		{
		    dao.Clear();
		    dao.SetParam(":VTRADE_ID", strTradeId);
		    dao.SetParam(":VACCEPT_MONTH", strTradeId.substr(4,2));
		    dao.SetParam(":VUSER_ID", strUserId);
		    dao.jselect(bufVTradeSvcState_UserAllSvcState, "SEL_ALLSVCSTATE_AFTER_TRADE","TF_B_TRADE_SVCSTATE");
		}
		catch (Exception &ex)
		{
		    THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取办理业务后用户的服务状态出错！");
		}
	}
	else //本笔流水台帐子表，即为最终表
	{
        try
        {
            dao.Clear();
            dao.SetParam(":VTRADE_ID", strTradeId);
            dao.SetParam(":VACCEPT_MONTH", strTradeId.substr(4,2));
            dao.jselect(bufVTradeSvcState_UserAllSvcState, "SEL_BY_IDMONTH","TF_B_TRADE_SVCSTATE");
        }
        catch (Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取办理业务后用户的服务状态出错！");
        }
	}
}


//判断是否是电话号码
inline bool IsPhone(const string &strPhone, string &strMsg)
{
    StaticDAO& dao = StaticDAO::getDAO();
    if ((strPhone.substr(0,2) == "13" || strPhone.substr(0,3) == "159"))
    {
        if (strPhone.length() != 11)
        {
            strMsg = "手机号码的长度必须为11位！";
            return false;
        }
        else
            return true; //手机号码
    }
    else if (strPhone.substr(strPhone.length()-5) == "12580") //特殊号码
    {
        //检查固话开头是否为完整的区号 以及长度是否合法
        CFmlBuf bufVTag;
        int iTag;
        try
        {
            dao.Clear();
            dao.SetParam(":VTAG_CODE", "CS_INF_AREACODE");
            dao.SetParam(":VSUBSYS_CODE", "CSM");
            dao.SetParam(":VUSE_TAG", "0");
            iTag = dao.jselect(bufVTag, "SEL_BY_TAGCODE_NO_EPARCHY","TD_S_TAG");
        }
        catch(Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取当前省代码出错！");
        }
        bool bFind(false);
        for(int i=0; i<iTag; i++)
    	{
            string strPhoneHead = bufVTag.GetString("TAG_INFO",i);
            int iPhoneHeadLength = atoi(string(bufVTag.GetString("TAG_NUMBER",i)).c_str());
            int iPhoneLength = atoi(bufVTag.GetString("TAG_CHAR",i) == ""?"8":(string(bufVTag.GetString("TAG_CHAR",i)).c_str()));

            if (strPhone.substr(0,iPhoneHeadLength) == strPhoneHead && strPhone.length() == iPhoneHeadLength + 5)
            {
                bFind = true;
                break;
            }
    	}
    	if (!bFind)
    	{
    	    strMsg = "请在12580前面加上电话区号";
    	}

        return bFind;
    }
    else
    {
        //检查固话开头是否为完整的区号 以及长度是否合法
        if (strPhone.substr(0,1) != "0" || strPhone.substr(0,2)=="00")
        {
            strMsg = "固定电话前面请输入区号！";
            return false;
        }
        else if (strPhone.length() < 11)
        {
            strMsg = "固定电话长度不能小于11位！";
            return false;
        }
        else if (strPhone.length() > 12)
        {
            strMsg = "固定电话长度不能大于12位！";
            return false;
        }
        else if (strPhone.substr(strPhone.length()-8) == "00000000")
        {
            strMsg = "固定电话不能为00000000！";
            return false;
        }

        return true;
    }
}


/**
 * 检查绑定的号码是否规则
 */
void TradeCheckAfterTrade::CheckBindPhone(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
    LOG_TRACE(logTradeCheckAfterTrade, "进入CheckBindPhone函数");

    string strTradeId = inBuf.GetString("TRADE_ID");

    int iCnt(0);
    StaticDAO& dao = StaticDAO::getDAO();
    //无条件呼转，对呼转号码做检查
    CFmlBuf bufTradeSvc;
    try
    {
        dao.Clear();
        dao.SetParam(":VTRADE_ID", strTradeId);
        dao.SetParam(":VSERVICE_ID", 12);
        iCnt = dao.jselect(bufTradeSvc, "SEL_BY_TRADE_SVC","TF_B_TRADE_SVC");
    }
    catch (Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取办理业务后用户的服务状态出错！[%s]");
    }

    if (iCnt > 0 && bufTradeSvc.GetString("MODIFY_TAG") != "1")
    {
        string strSN = bufTradeSvc.GetString("SERV_PARA1");
        string strMsg;
        if (!IsPhone(strSN, strMsg))
        {
            string strName = CParamDAO::getParam("ServiceName", "12");
            THROW_C_P2(CRMException, CHECKAFTERTRADE_ERR, "服务[%s]的呼转号码格式不正确:%s！", strName.c_str(), strMsg.c_str());
        }
    }

    LOG_TRACE(logTradeCheckAfterTrade, "退出CheckBindPhone函数");
}

//调用完毕帐务流程 QAM_YEARFEEQRY_OS 获取了用户的年协消费额后，处理帐务返回结果
void TradeCheckAfterTrade::CheckPurchaseAcctReturn(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckAfterTrade, "进入 CheckPurchaseAcctReturn 函数");
	//判断调用结果
    int resultCode = inBuf.GetInt("X_RESULTCODE");
    string resultInfo = inBuf.GetString("X_RESULTINFO");
    if (resultCode != 0)
    {
        THROW_C(CRMException, CHECKAFTERTRADE_ERR, "调用帐务年协消费查询接口失败:" + resultInfo);
    }

    int iAcctCount = inBuf.GetInt("X_RECORDNUM");
    long lUsedFee = 0;
    int iFoundCount = 0;
	float iTotalFee = 0;
	float iCurrFee  = 0;

    string strAddupValue;
    StaticDAO& dao = StaticDAO::getDAO();
	CFmlBuf ibuf;
	//获取可清退时间，以及消费额度
	string strUserId = inBuf.GetString("USER_ID");
    try
    {
        dao.Clear();
        ibuf.ClearFmlValue();
        dao.SetParam(":VUSER_ID", strUserId);
        dao.SetParam(":VBINDSALE_ATTR", "0"); //1倍押金购机
        //查询的是用户所有1倍押金的购机纪录，并按照倒序排列,modify by chenzm@2007-3-9
        dao.jselect(ibuf, "SEL_PURCHASE_BY_USERID_BINDSALE_ATTR","TF_F_USER_PURCHASE");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:判断是否清退押金出错！");
    }

	if (ibuf.GetInt("X_RECORDNUM") == 0)
	{
		outBuf.SetInt("X_RESULTCODE",0);
		outBuf.SetString("X_RESULTINFO","Acct Purchase check ok!");
		return;
	}
	string strConsumeValue = ibuf.GetString("CONSUME_VALUE");
	iTotalFee = atof(strConsumeValue.c_str())/(float)100;
	string strEndDate = ibuf.GetString("END_DATE");

    for (int i=0; i<iAcctCount; i++ )
    {
    	if (inBuf.GetInt("DISCNT_CODE",i) != 10021)
    		continue;

		strAddupValue = inBuf.GetString("ADDUP_VALUE",i);
		lUsedFee += atol(strAddupValue.c_str());
		iFoundCount ++;
	}
	if (iFoundCount == 0)//未查询到年协累计消费值,按照实效时间来判断
	{
		THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "未满消费额度,不能清退押金或者办理捆绑销售业务:消费额度[%.2f],已经消费 0 元",iTotalFee);
	}

	if (lUsedFee >= atol(strConsumeValue.c_str())) //消费额度已经满足
	{
		outBuf.SetInt("X_RESULTCODE",0);
		outBuf.SetString("X_RESULTINFO","Acct Purchase check ok!");
		return;
	}
	//未到期,消费额度也未满,报错
	iCurrFee  = (float)lUsedFee/(float)100;

    THROW_C_P2(CRMException, CHECKAFTERTRADE_ERR, "未满消费额度,不能清退押金或者办理捆绑销售业务:消费额度:%.2f元,已经消费:%.2f元",iTotalFee,iCurrFee);

    LOG_TRACE(logTradeCheckAfterTrade, "退出 CheckPurchaseAcctReturn 函数");
}


//产品模型相关限制
void TradeCheckAfterTrade::CheckProductModelLimit(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
    int iCount = 0;

    string strTradeTypeCode = inBuf.GetString("TRADE_TYPE_CODE");
    if(strTradeTypeCode == "190" || strTradeTypeCode == "191"  || strTradeTypeCode == "192" || strTradeTypeCode == "250"||strTradeTypeCode == "143"|| strTradeTypeCode == "311" ||strTradeTypeCode == "124"
       || strTradeTypeCode == "255" || strTradeTypeCode == "256" || strTradeTypeCode == "295" || strTradeTypeCode == "296" || strTradeTypeCode == "291" || strTradeTypeCode == "292"|| strTradeTypeCode == "310"||strTradeTypeCode == "288" ||strTradeTypeCode == "369")
        return;

    string strTradeId = inBuf.GetString("TRADE_ID");
    string strUserId = inBuf.GetString("USER_ID");
    string strCustId = inBuf.GetString("CUST_ID");
    string strAcctId = inBuf.GetString("ACCT_ID");
    string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");
    string strTradeStaffId = inBuf.GetString("TRADE_STAFF_ID");
    CFmlBuf bfUserProd;                 //用户产品
    CFmlBuf bufUserPackageByProd;       //用户包（按产品）
    CFmlBuf bufUserPackage;             //用户包（所有）
    CFmlBuf bufTradeDiscnt_AllDiscnt;   //用户优惠
    CFmlBuf bufTradeSvc_UserAllSvc;     //用户服务
    CFmlBuf bufTradeSp_UserAllSp;       //用户SP
    CFmlBuf bufTradeElement_UserAllElement; //用户物品等其他元素
    CFmlBuf bufUserAllElements;         //用户元素
    CFmlBuf bufDelPackage;              //删除的包
    CFmlBuf bufNoChangePackage;         //当前业务未发生变化的包
    CFmlBuf bufElementTimeSeries;       //包内元素时间连续，用于计算包内元素最大最小数时，时间连续的两个元素计算为一个

    int iProductId = 0;
    string strProductName = "";
    string strProductMode = "";
    int iMinPackage = -1;
    int iMaxPackage = -1;
    int iPackageCount = 0;
    string strProductEndDate;

    int iMainCount = 0;
    string hbtag = "0";
    CFmlBuf hbTagBuf;
    StaticDAO& hbdao = StaticDAO::getDAO();
    try
    {
        hbdao.Clear();
        hbdao.SetParam(":VEPARCHY_CODE", strEparchyCode);
        hbdao.SetParam(":VTAG_CODE", "CSM_CHR_PRODUCTMODELLIMIT");
        hbdao.SetParam(":VSUBSYS_CODE", "CSM");
        hbdao.SetParam(":VUSE_TAG", "0");
        iMainCount = hbdao.jselect(hbTagBuf, "SEL_ALL_BY_TAGCODE","TD_S_TAG");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取当前省代码出错！");
    }
    if( iMainCount > 0 &&  hbTagBuf.GetString("TAG_CHAR")=="1" )
    {
    	hbtag = "1"  ;
    }

//-------------start--------------------
    iMainCount = 0;
    string checkSameEleTag = "0";
    CFmlBuf checkSameEleBuf;
    StaticDAO& checkdao = StaticDAO::getDAO();
    try
    {
        checkdao.Clear();
        checkdao.SetParam(":VEPARCHY_CODE", strEparchyCode);
        checkdao.SetParam(":VTAG_CODE", "CSM_CHR_CHECKSAMEELEMENT");
        checkdao.SetParam(":VSUBSYS_CODE", "CSM");
        checkdao.SetParam(":VUSE_TAG", "0");
        iMainCount = hbdao.jselect(checkSameEleBuf, "SEL_ALL_BY_TAGCODE","TD_S_TAG");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取当前省代码出错！");
    }
    if( iMainCount > 0 &&  checkSameEleBuf.GetString("TAG_CHAR")=="1" )
    {
    	checkSameEleTag = "1"  ;
    }
//-----------~end----------------------

    if( hbtag == "1" &&  (strTradeTypeCode == "310" || strTradeTypeCode == "250" || strTradeTypeCode=="291" || strTradeTypeCode=="292"
       || strTradeTypeCode=="255" || strTradeTypeCode=="256" || strTradeTypeCode=="295" || strTradeTypeCode=="296"
       || strTradeTypeCode=="240" || strTradeTypeCode=="93" || strTradeTypeCode=="233" || strTradeTypeCode=="241"||strTradeTypeCode=="330")
      )
       return ;

    if(strTradeTypeCode == "190" || strTradeTypeCode == "191" || strTradeTypeCode == "192"||strTradeTypeCode == "241"||strTradeTypeCode == "310")//add 取消预约拆机不做校验 310
        return;

    bool bCheck = true;
    if (strTradeTypeCode == "7230" || strTradeTypeCode == "7240" || strTradeTypeCode == "7302" ||
        strTradeTypeCode == "1025" || strTradeTypeCode == "1028" || strTradeTypeCode == "1029" ||
        strTradeTypeCode == "112" || strTradeTypeCode == "70")
    {
        bCheck = false;
    }

    //校验用户产品是否达到最大数
    //CheckUserProductMaxSale(strTradeId,strUserId);

 
	LOG_TRACE_P1(logTradeCheckAfterTrade, "--bufTradeSvc_UserAllSvc:%s",bufTradeSvc_UserAllSvc.ToString().c_str());
    //获取当前业务未变化的包信息
    GetNoChangePackage(strTradeId, strUserId, bufNoChangePackage);

    //获取删除的包(用户原有包按包直接删除)
    GetDelPackage(strTradeId, bufDelPackage);

    //获取用户优惠信息
    if(hbtag == "1" )
    	GetAllDiscntAfterTradehb(strTradeId, strUserId, strCustId, strAcctId, bufTradeDiscnt_AllDiscnt);
    else
    	GetAllDiscntAfterTrade(strTradeId, strUserId, strCustId, strAcctId, bufTradeDiscnt_AllDiscnt);
	LOG_TRACE_P1(logTradeCheckAfterTrade, "--bufTradeDiscnt_AllDiscnt:%s",bufTradeDiscnt_AllDiscnt.ToString().c_str());
    //获取用户服务信息
    if(hbtag == "1" )
    	GetUserAllServiceAfterTradehb(strTradeId, strUserId, bufTradeSvc_UserAllSvc);
    else
    	GetUserAllServiceAfterTrade(strTradeId, strUserId, bufTradeSvc_UserAllSvc);
	LOG_TRACE_P1(logTradeCheckAfterTrade, "--bufTradeSvc_UserAllSvc:%s",bufTradeSvc_UserAllSvc.ToString().c_str());
    //获取用户物品等其他元素信息
    GetUserAllElementAfterTrade(strTradeId, strUserId, bufTradeElement_UserAllElement);
LOG_TRACE_P1(logTradeCheckAfterTrade, "--bufTradeElement_UserAllElement:%s",bufTradeElement_UserAllElement.ToString().c_str());
    //获取用户SP信息
    GetUserAllSpAfterTrade(strTradeId, strUserId, bufTradeSp_UserAllSp);
	LOG_TRACE_P1(logTradeCheckAfterTrade, "--bufTradeSp_UserAllSp:%s",bufTradeSp_UserAllSp.ToString().c_str());
    //整合用户元素
    GeneUserAllElements(bufTradeDiscnt_AllDiscnt, bufTradeSvc_UserAllSvc, bufTradeSp_UserAllSp, bufTradeElement_UserAllElement, bufUserAllElements);
	LOG_TRACE_P1(logTradeCheckAfterTrade, "--bufUserAllElements:%s",bufUserAllElements.ToString().c_str());
    //获取用户包（所有）
    GetUserPackageByProduct(-1, bufTradeDiscnt_AllDiscnt, bufTradeSp_UserAllSp, bufTradeElement_UserAllElement, bufTradeSvc_UserAllSvc, bufUserPackage);
	LOG_TRACE_P1(logTradeCheckAfterTrade, "--bufUserPackage:%s",bufUserPackage.ToString().c_str());
    //获取包内元素时间连续的元素个数，用于计算包内元素最大最小数时，时间连续的两个元素计算为一个
    GetElementTimeSeries(bufUserAllElements, bufElementTimeSeries);
	LOG_TRACE_P1(logTradeCheckAfterTrade, "--bufElementTimeSeries:%s",bufElementTimeSeries.ToString().c_str());
    //获取用户产品信息
    iCount = GetUserProductInfo(strTradeId, strUserId, bfUserProd);
	LOG_TRACE_P1(logTradeCheckAfterTrade, "--bfUserProd:%s",bfUserProd.ToString().c_str());
	  //校验用户元素是否达到最大体验数
    CheckUserElement(strTradeId,strUserId,strEparchyCode,bfUserProd);
    //判断元素和其属性之间的限制关系
    //CheckElmentsAndItsAttr(inBuf,bufUserAllElements);
    //按用户产品分别校验限制
    for(int i = 0; i < iCount; i++)
    {
        iProductId = bfUserProd.GetInt("PRODUCT_ID", i);
        strProductName = bfUserProd.GetString("PRODUCT_NAME", i);
        strProductMode = bfUserProd.GetString("PRODUCT_MODE", i);
        iMinPackage = bfUserProd.GetInt("MIN_NUMBER", i);
        iMaxPackage = bfUserProd.GetInt("MAX_NUMBER", i);
        strProductEndDate = bfUserProd.GetString("END_DATE", i);
        if(strTradeTypeCode == "1060" && strProductMode=="20")  continue;
        //获取用户某一产品的业务包
        iPackageCount = GetUserPackageByProduct(iProductId, bufTradeDiscnt_AllDiscnt, bufTradeSp_UserAllSp, bufTradeElement_UserAllElement, bufTradeSvc_UserAllSvc, bufUserPackageByProd, strProductEndDate);
	LOG_TRACE_P1(logTradeCheckAfterTrade, "--bufUserPackageByProd:%s",bufUserPackageByProd.ToString().c_str());
        //产品内包最小选择数判断
        if(iMinPackage >= 0 && iPackageCount < iMinPackage)
            THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR, "产品：\"" +
                        strProductName + "\"最少选择" + to_string(iMinPackage) + "个业务包，业务无法继续！");

        //产品内包最大选择数判断
        if(iMaxPackage >= 0 && iPackageCount > iMaxPackage)
            THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR, "产品：\"" +
                        strProductName + "\"最多选择" + to_string(iMaxPackage) + "个业务包，业务无法继续！");

        //产品必选包判断
        if(bCheck)
            CheckForcePackageByProduct(iProductId, strEparchyCode, strTradeStaffId, bufUserPackageByProd);

        //产品内包间关系判断(TD_B_PROD_PACKAGE_LIMIT)
        CheckPackageLimitByProduct(iProductId, bufUserPackageByProd, strEparchyCode);

        //包内元素必选判断
        if(bCheck)
            CheckPackageForceElements(iProductId, strProductName, bufUserPackageByProd,
                bufTradeDiscnt_AllDiscnt, bufTradeSvc_UserAllSvc,
                bufTradeSp_UserAllSp, bufTradeElement_UserAllElement,
                bufNoChangePackage, bufDelPackage);

        //产品包内元素的最大最小选择数判断(TD_B_PACKAGE)
        CheckPackageElementChoiceByProduct(strEparchyCode, strTradeStaffId, bufUserPackageByProd, bufUserAllElements, bufNoChangePackage, bufDelPackage, bufElementTimeSeries);
        LOG_TRACE_P1(logTradeCheckAfterTrade, "--strEparchyCode:%s",strEparchyCode.c_str());
        LOG_TRACE_P1(logTradeCheckAfterTrade, "--strTradeStaffId:%s",strTradeStaffId.c_str());
        LOG_TRACE_P1(logTradeCheckAfterTrade, "--bufTradeSvc_UserAllSvc:%s",bufTradeSvc_UserAllSvc.ToString().c_str());
        LOG_TRACE_P1(logTradeCheckAfterTrade, "--bufTradeDiscnt_AllDiscnt:%s",bufTradeDiscnt_AllDiscnt.ToString().c_str());
        LOG_TRACE_P1(logTradeCheckAfterTrade, "--bufUserPackageByProd:%s",bufUserPackageByProd.ToString().c_str());
        LOG_TRACE_P1(logTradeCheckAfterTrade, "--bufUserAllElements:%s",bufUserAllElements.ToString().c_str());
        LOG_TRACE_P1(logTradeCheckAfterTrade, "--bufNoChangePackage:%s",bufNoChangePackage.ToString().c_str());
        LOG_TRACE_P1(logTradeCheckAfterTrade, "--bufDelPackage:%s",bufDelPackage.ToString().c_str());
        LOG_TRACE_P1(logTradeCheckAfterTrade, "--bufElementTimeSeries:%s",bufElementTimeSeries.ToString().c_str());
        //产品包内元素的最大最小选择数判断(TD_B_PRODUCT_PACKAGE)
        CheckPackageElementChoiceByProduct(strEparchyCode, strTradeStaffId, bufUserPackageByProd, bufUserAllElements, bufNoChangePackage, bufDelPackage, bufElementTimeSeries, 1);

        //包与元素关系判断
        CheckPackageAndElementsLimit(bufUserPackageByProd, bufUserAllElements);
    }

    //协议与资费、组合产品之间限制关系判断
    if(inBuf.GetString("NET_TYPE_CODE")!="CP")
    	CheckAttrElementLimit(bufTradeDiscnt_AllDiscnt, strEparchyCode, strTradeId, strUserId, strCustId, strAcctId);

    //全局包间关系判断(TD_B_PACKAGE_LIMIT)
     if(hbtag != "1" )
    	CheckPackageLimit(bufUserPackage, strEparchyCode);

    //包内元素关系判断
    CheckPackageElementLimit(bufUserAllElements, strEparchyCode, bufNoChangePackage, bufDelPackage);

    //全局元素关系判断
    CheckElementLimit(bufUserAllElements, strEparchyCode,strTradeId);

    //同一元素在一笔业务中只能选择一次
    if(checkSameEleTag != "1")
    	CheckSameElement(bufUserAllElements);

    //SP服务判断：同一个SP_ID只能选择一个
    CheckSameSPID(bufTradeSp_UserAllSp);

    //产品间限制
    if (strTradeTypeCode != "70" && strTradeTypeCode != "1060")
    {
        CheckProductLimit(bfUserProd);
    }

    //产品资源约束
    //CheckProductRes(bfUserProd, strTradeId, strUserId, strEparchyCode);

    //产品客户限制
    CheckCustProductLimit(inBuf);
}

void TradeCheckAfterTrade::CheckCustProductLimit(CFmlBuf &bufTrade)
{
    LOG_TRACE(logTradeCheckAfterTrade,"进入CheckCustProductLimit函数....");
    //取tag检查是否需要校验
    CFmlBuf bufTag;
    StaticDAO& dao = StaticDAO::getDAO();
    int iCount(0);
    try
    {
        dao.Clear();
        dao.SetParam(":VEPARCHY_CODE", bufTrade.GetString("TRADE_EPARCHY_CODE"));
        dao.SetParam(":VTAG_CODE", "CSM_CHR_CHECK_CUST_PRODUCT");
        dao.SetParam(":VSUBSYS_CODE", "CSM");
        dao.SetParam(":VUSE_TAG", "0");
        iCount = dao.jselect(bufTag, "SEL_ALL_BY_TAGCODE","TD_S_TAG");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取是否校验产品与客户限制失败！");
    }

    CFmlBuf tempBuf;
    //校验是否该可以允许受理
    if(iCount>0)
    {
        if(bufTag.GetString("TAG_CHAR")=="1")//需要校验
        {
            //获取业务新增主产品
            CFmlBuf bufTradeProduct;
            int iPCount(0);
            try
            {
                dao.Clear();
                dao.SetParam(":VTRADE_ID",bufTrade.GetString("TRADE_ID"));
                iPCount = dao.jselect(bufTradeProduct,"SEL_BY_PK", "TF_B_TRADE_PRODUCT");
            }
            catch(Exception &ex)
            {
                THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "根据流水获取受理产品信息异常！");
            }

            //查客户信息
            CFmlBuf bufCust;
            if(iPCount>0)
            {
                try
                {
                    dao.Clear();
                    dao.SetParam(":VCUST_ID",bufTrade.GetString("CUST_ID"));
                    iCount = dao.jselect(bufCust,"SEL_BY_PK", "TF_F_CUSTOMER");
                }
                catch(Exception &ex)
                {
                    THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "根据根据客户标示获取客户资料异常！");
                }

                if(iCount<1)
                {
                    return;
                    //THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:根据用户标示[%s]未获取到客户资料!", bufTrade.GetStr("CUST_ID"));
                }
            }
            LOG_TRACE_P1(logTradeCheckAfterTrade,"iPCount==%d",iPCount);
            LOG_TRACE_P1(logTradeCheckAfterTrade,"bufTradeProduct==%s",bufTradeProduct.ToString().c_str());
            iCount = 0;

            CFmlBuf bufParam;
            for(int i=0;i<iPCount;i++)
            {
                if(bufTradeProduct.GetString("PRODUCT_MODE",i)=="00"
                    &&(bufTradeProduct.GetString("MODIFY_TAG",i)=="0"||bufTradeProduct.GetString("MODIFY_TAG",i)=="A"))
                    ;
                else continue;//不是新增产品进入下次循环
                //产品客户限制规则
                try
                {
                    dao.Clear();
                    bufParam.ClearFmlValue();
                    dao.SetParam(":VSUBSYS_CODE", "CSM");
                    dao.SetParam(":VPARAM_ATTR", 2134);
                    dao.SetParam(":VPARAM_CODE", "TP-PROD");
                    dao.SetParam(":VPARA_CODE1", bufTrade.GetString("NET_TYPE_CODE"));
                    dao.SetParam(":VPARA_CODE2", bufTrade.GetString("BRAND_CODE"));
                    dao.SetParam(":VPARA_CODE3", (int)bufTradeProduct.GetInt("PRODUCT_ID",i));
                    dao.SetParam(":VEPARCHY_CODE",bufTrade.GetString("TRADE_EPARCHY_CODE"));
                    iCount = dao.jselect(bufParam, "SEL_BY_PK_123", "TD_S_COMMPARA");
                }
                catch(Exception &ex)
                {
                    THROW_AGC(CRMException, ex, Grade::WARNING, GETCOMMPARA_ERR, "获取产品与客户分类参数异常！");
                }
                LOG_TRACE_P1(logTradeCheckAfterTrade,"iCount====%d",iCount);
                if(iCount > 0)//没有配置默认不校验
                {
                    //0:个客, 1:家客, 2:集客
                    string strUserTypeCode = bufParam.GetString("PARA_CODE4");
                    string strAllTag = bufParam.GetString("PARA_CODE5");
                    if(strAllTag == "1")//PARA_CODE5 为1表示按照PARA_CODE4判断,为0表示家客集客都可办理
                    {
                        if(bufCust.GetString("CUST_TYPE")=="1"&&strUserTypeCode!="2")
                        {
                            THROW_C(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:客户为集团客户,不能办理个人客户产品!");
                        }
                        if(bufCust.GetString("CUST_TYPE")=="0"&&strUserTypeCode!="0"&&strUserTypeCode!="1")
                        {
                            THROW_C(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:客户为个人客户,不能办理集团客户产品!");
                        }
                    }
                }
            }
        }
    }
    LOG_TRACE(logTradeCheckAfterTrade,"退出CheckCustProductLimit函数....");
}

//获取用户产品信息（业务受理后）
int TradeCheckAfterTrade::GetUserProductInfo(const string &strTradeId, const string &strUserId,
                                             CFmlBuf &bfUserProd)
{
    StaticDAO& dao = StaticDAO::getDAO();
	int iCount = 0;

    try
    {
        bfUserProd.ClearFmlValue();
        dao.Clear();
        dao.SetParam(":VTRADE_ID", strTradeId);
        dao.SetParam(":VUSER_ID", strUserId);
        iCount = dao.jselect(bfUserProd, "SEL_ALLPRODUCTS_AFTER_TRADE", "TF_B_TRADE_PRODUCT");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取用户产品信息出错！");
    }

    ConvertCodeToName(bfUserProd, "ProductName", "PRODUCT_ID", "PRODUCT_NAME");
    bfUserProd.SetInt("X_RECORDNUM", iCount);

    return iCount;
}

/**
 *校验用户产品是否达到最大数
 *
 */
int TradeCheckAfterTrade::CheckUserProductMaxSale(const string &strTradeId, const string &strUserId)
{
    StaticDAO& dao = StaticDAO::getDAO();
    //1.获取台账产品子表信息
    int iPdtCnt;
    CFmlBuf bufTradeProduct;
    try
    {
        dao.Clear();
        dao.SetParam(":VTRADE_ID", strTradeId);
        iPdtCnt = dao.jselect(bufTradeProduct, "SEL_BY_PK","TF_B_TRADE_PRODUCT");
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查询业务台账产品子表出错！[%s]",ex.getCause()->what());
    }

    CFmlBuf tmpBuff;
    int iCount = 0;
    for(int i = 0;i<iPdtCnt;i++)
    {
        iCount = 0;
        if(bufTradeProduct.GetString("MODIFY_TAG")!="0")
        	continue;

        //校验是否达到订购最大数
        try
    	{
            dao.Clear();
            tmpBuff.ClearFmlValue();
            dao.SetParam(":VTRADE_ID", strTradeId);
            dao.SetParam(":VACCEPT_MONTH", strTradeId.substr(4,2));
            dao.SetParam(":VPRODUCT_ID", (int)bufTradeProduct.GetInt("PRODUCT_ID"));
            iCount = dao.jselect(tmpBuff, "IsGetMaxProducts","TD_S_CPARAM");
    	}
        catch (Exception &ex)
    	{
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取办理业务后三户的所有优惠出错！[%s]",ex.getCause()->what());
    	}

        if((int)tmpBuff.GetInt("RECORDCOUNT") > 0 )
    	{
            char chIdA[10];
            sprintf(chIdA,"%d",(int)bufTradeProduct.GetInt("PRODUCT_ID"));
            string strNameA = CParamDAO::getParam("ProductName", chIdA);
            THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:[%s]该产品已经达到订购最大数,不可以订购!", strNameA.c_str());
    	}

        //校验是否是体验产品限制体验次数
        iCount = 0;
        try
    	{
            dao.Clear();
            tmpBuff.ClearFmlValue();
            dao.SetParam(":VTRADE_ID", strTradeId);
            dao.SetParam(":VUSER_ID", strUserId);
            dao.SetParam(":VACCEPT_MONTH", strTradeId.substr(4,2));
            dao.SetParam(":VPRODUCT_ID", (int)bufTradeProduct.GetInt("PRODUCT_ID"));
            iCount = dao.jselect(tmpBuff, "isGetExpProdLimit","TD_S_CPARAM");
    	}
        catch (Exception &ex)
    	{
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取办理业务后三户的所有优惠出错！[%s]",ex.getCause()->what());
    	}

        if((int)tmpBuff.GetInt("RECORDCOUNT") > 0 )
    	{
            char chIdA[10];
            sprintf(chIdA,"%d",(int)bufTradeProduct.GetInt("PRODUCT_ID"));
            string strNameA = CParamDAO::getParam("ProductName", chIdA);
            THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:[%s]该体验产品已经达到订购体验次数或者定期内不可在体验,不可以订购!", strNameA.c_str());
    	}
    }

    return iCount;
}

/**
 *校验用户元素是否达到最大数
 *0：服务
 *1：优惠
 *2：sp
 */
void TradeCheckAfterTrade::CheckUserElement(const string &strTradeId, const string &strUserId, const string &strEparchyCode,CFmlBuf &bfUserProd)
{
    StaticDAO& dao = StaticDAO::getDAO();
    //配置用户体验允许册数tag，如果大于，则提示，否则继续体验
    string strGDTag;
    int intGDTag(0);
    CFmlBuf bufGDTag;
    int ProdCnt = bfUserProd.GetCount("PRODUCT_ID");
    try
    {
        int iTagCnt(0);
        dao.Clear();
        dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
        dao.SetParam(":VTAG_CODE", "CSM_CHR_USER_EXPERIENCE_LIMIT");
        dao.SetParam(":VSUBSYS_CODE", "CSM");
        dao.SetParam(":VUSE_TAG", "0");
        iTagCnt = dao.jselect(bufGDTag, "SEL_ALL_BY_TAGCODE","TD_S_TAG");//SEL_BY_TAGCODE_1
        if(iTagCnt == 0)
        {
            strGDTag = "1";   //体验一次 modify hhx
        }
        else
            strGDTag = bufGDTag.GetString("TAG_CHAR");
    	intGDTag = atoi(strGDTag.c_str());
     }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GETTRADEINFO_ERR, "更改台帐资料:获取tag出错！");
    }
    //0.获取台账服务子表信息
    int iSvcCnt;
    CFmlBuf bufTradeSvc;
    try
    {
        dao.Clear();
        dao.SetParam(":VTRADE_ID", strTradeId);
        dao.SetParam(":VMODIFY_TAG", "0");
        iSvcCnt = dao.jselect(bufTradeSvc, "SEL_BY_TRADE_TAG","TF_B_TRADE_SVC");
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查询业务台账服务子表出错！[%s]",ex.getCause()->what());
    }

    CFmlBuf tmpBuff;
    int iCount = 0;
    for(int i = 0;i<iSvcCnt;i++)
    {

        //校验是否是体验服务限制体验次数
        iCount = 0;
        try
    	{
            dao.Clear();
            tmpBuff.ClearFmlValue();
            dao.SetParam(":VUSER_ID", strUserId);
            dao.SetParam(":VTRADE_ID", strTradeId);
            dao.SetParam(":VSERVICE_ID", (int)bufTradeSvc.GetInt("SERVICE_ID",i));
            dao.SetParam(":VATTR_CODE", "experience");
            iCount = dao.jselect(tmpBuff, "isGetExpSvcLimit","TD_S_CPARAM");
    	}
        catch (Exception &ex)
    	{
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取办理业务后三户的所有服务出错！[%s]",ex.getCause()->what());
    	}

        if((int)tmpBuff.GetInt("RECORDCOUNT") >=  intGDTag  )
    	{
            char chIdA[10];
            sprintf(chIdA,"%d",(int)bufTradeSvc.GetInt("SERVICE_ID",i));
            string strNameA = CParamDAO::getParam("ServiceName", chIdA);
            THROW_C_P2(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:[%s]该体验服务只能体验[%d]次!", strNameA.c_str(),intGDTag);
    	}
    }
    
   
    //1.获取台账优惠子表信息
    int iDisCnt;
    CFmlBuf bufTradeDis;
    try
    {
        dao.Clear();
        dao.SetParam(":VTRADE_ID", strTradeId);
        dao.SetParam(":VACCEPT_MONTH", strTradeId.substr(4,2));
        dao.SetParam(":VMODIFY_TAG", "0");
        iDisCnt = dao.jselect(bufTradeDis, "SEL_BY_TRADEID_TAG","TF_B_TRADE_DISCNT");
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查询业务台账优惠子表出错！[%s]",ex.getCause()->what());
    }
    LOG_TRACE_P1(logTradeCheckAfterTrade, "进入ChkRelaUserProductLimit函数--[%d]！",iDisCnt);

    iCount = 0;
    for(int i = 0;i<iDisCnt;i++)
    {

        //校验是否是体验优惠限制体验次数
        iCount = 0;
        try
    	{
            LOG_TRACE_P2(logTradeCheckAfterTrade, "进入ChkRelaUserProductLimit函数--[%s,%d]！",strUserId.c_str(),(int)bufTradeDis.GetInt("DISCNT_CODE",i));
            dao.Clear();
            tmpBuff.ClearFmlValue();
            dao.SetParam(":VUSER_ID", strUserId);
            dao.SetParam(":VTRADE_ID", strTradeId);
            dao.SetParam(":VDISCNT_CODE", (int)bufTradeDis.GetInt("DISCNT_CODE",i));
            dao.SetParam(":VATTR_CODE", "experience");
            iCount = dao.jselect(tmpBuff, "isGetExpDisLimit","TD_S_CPARAM");
    	}
        catch (Exception &ex)
    	{
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取办理业务后三户的所有优惠出错！[%s]",ex.getCause()->what());
    	}
    LOG_TRACE_P1(logTradeCheckAfterTrade, "进入ChkRelaUserProductLimit函数--[%d]！",(int)tmpBuff.GetInt("RECORDCOUNT"));
        if((int)tmpBuff.GetInt("RECORDCOUNT")>=  intGDTag )
    	{
            char chIdA[10];
            sprintf(chIdA,"%d",(int)bufTradeDis.GetInt("DISCNT_CODE",i));
            string strNameA = CParamDAO::getParam("DiscntName", chIdA);
            THROW_C_P2(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:[%s]该体验优惠只能体验[%d]次!", strNameA.c_str(),intGDTag);
    	}
    }

    //2.获取台账sp子表信息
    int iSpCnt;
    CFmlBuf bufTradeSp;
    try
    {
        dao.Clear();
        dao.SetParam(":VTRADE_ID", strTradeId);
        dao.SetParam(":VUSER_ID", strUserId);
        iSpCnt = dao.jselect(bufTradeSp, "SEL_TRADE_SP","TF_B_TRADE_SP");
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查询业务台账sp子表出错！[%s]",ex.getCause()->what());
    }


    iCount = 0;
    for(int i = 0;i<iSpCnt;i++)
    {

        if(bufTradeSp.GetString("MODIFY_TAG")!="0")
        	continue;
        //校验是否是体验sp限制体验次数
        iCount = 0;
        try
    	{
            dao.Clear();
            tmpBuff.ClearFmlValue();
            dao.SetParam(":VUSER_ID", strUserId);
            dao.SetParam(":VTRADE_ID", strTradeId);
            dao.SetParam(":VSP_PRODUCT_ID", bufTradeSp.GetString("SP_PRODUCT_ID",i));
            dao.SetParam(":VATTR_CODE", "experience");
            iCount = dao.jselect(tmpBuff, "isGetExpSpLimit","TD_S_CPARAM");
    	}
        catch (Exception &ex)
    	{
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取办理业务后三户的所有sp出错！[%s]",ex.getCause()->what());
    	}

        if((int)tmpBuff.GetInt("RECORDCOUNT") >=  intGDTag )
    	{
            string strNameA = CParamDAO::getParam("SpProductName", bufTradeSp.GetString("SP_PRODUCT_ID", i));
            THROW_C_P2(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:[%s]该体验sp只能体验[%d]次!", strNameA.c_str(), intGDTag);
    	}
    }
    
    
    //校验元素是否超出产品范围
    if(ProdCnt >0)
    {
        int iSvcCntTemp = 0;
        CFmlBuf bufTradeSvcTemp;
        try
        {
            dao.Clear();
            dao.SetParam(":VTRADE_ID", strTradeId);
            dao.SetParam(":VMODIFY_TAG", "A");
            iSvcCntTemp = dao.jselect(bufTradeSvcTemp, "SEL_BY_TRADE_TAG","TF_B_TRADE_SVC");
        }
        catch (Exception &ex)
        {
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查询业务台账服务子表出错！[%s]",ex.getCause()->what());
        }
        if(iSvcCntTemp> 0)
        {
            for(int i = 0;i<iSvcCntTemp;i++)
            {
                bufTradeSvc.CoverLine(iSvcCnt+i,bufTradeSvcTemp,i);
            }
        }
        //服务
        for(int i = 0;i<iSvcCnt+iSvcCntTemp;i++)
        { 
             for(int j = 0;j<ProdCnt;j++)
             {
             
                 if (bufTradeSvc.GetString("PRODUCT_ID",i) == bfUserProd.GetString("PRODUCT_ID",j) 
                    &&(bufTradeSvc.GetString("START_DATE",i).substr(0,10)< bfUserProd.GetString("START_DATE",j).substr(0,10)) )
                 {
                     string serviceName = CParamDAO::getParam("ServiceName", bufTradeSvc.GetString("SERVICE_ID",i));
                     THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:[%s]该服务超出产品范围，请检查服务订购时间!", serviceName.c_str());
                 }
             }
                  
        }
        int iDisCntTemp = 0;
        CFmlBuf bufTradeDisTemp;
        try
        {
            dao.Clear();
            dao.SetParam(":VTRADE_ID", strTradeId);
            dao.SetParam(":VACCEPT_MONTH", strTradeId.substr(4,2));
            dao.SetParam(":VMODIFY_TAG", "A");
            iDisCntTemp = dao.jselect(bufTradeDisTemp, "SEL_BY_TRADEID_TAG","TF_B_TRADE_DISCNT");
        }
        catch (Exception &ex)
        {
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查询业务台账优惠子表出错！[%s]",ex.getCause()->what());
        }
        
        if(iDisCntTemp> 0)
        {
            for(int i = 0;i<iDisCntTemp;i++)
            {
                bufTradeDis.CoverLine(iDisCnt+i,bufTradeDisTemp,i);
            }
        }
        
        
        //资费
        for(int i = 0;i<iDisCnt+iDisCntTemp;i++)
        {
             for(int j = 0;j<ProdCnt;j++)
             {
                 if (bufTradeDis.GetString("PRODUCT_ID",i) == bfUserProd.GetString("PRODUCT_ID",j) 
                    &&(bufTradeDis.GetString("START_DATE",i).substr(0,10)< bfUserProd.GetString("START_DATE",j).substr(0,10)) )
                 {
                     string discntName = CParamDAO::getParam("DiscntName", bufTradeDis.GetString("DISCNT_CODE",i));
                     THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:[%s]该优惠超出产品范围，请检查优惠订购时间!", discntName.c_str());
                 }
             } 
        }
        //sp
        for(int i = 0;i<iSpCnt;i++)
        {
            if(bufTradeSp.GetString("MODIFY_TAG")!="0" && bufTradeSp.GetString("MODIFY_TAG")!="A")
        	      continue;
            for(int j = 0;j<ProdCnt;j++)
            {
                 if (bufTradeSp.GetString("PRODUCT_ID",i) == bfUserProd.GetString("PRODUCT_ID",j) 
                    &&(bufTradeSp.GetString("START_DATE",i).substr(0,10)< bfUserProd.GetString("START_DATE",j).substr(0,10)) )
                 {
                     string spName = CParamDAO::getParam("SpProductName", bufTradeSp.GetString("SP_PRODUCT_ID",i));
                     THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "业务登记后条件判断:[%s]该sp超出产品范围，请检查sp订购时间!", spName.c_str());
                 }
            } 
        }
    }
}

/**
 *获取办理业务后的用户所有优惠和优惠的开始时间、结束时间、和修改状态（三户原有优惠的修改状态默认为'A')
 *
 */
int TradeCheckAfterTrade::GetAllDiscntAfterTrade(const string &strTradeId, const string &strUserId,
                                                 const string &strCustId, const string &strAcctId,
                                                 CFmlBuf &bufTradeDiscnt_AllDiscnt)
{
    bufTradeDiscnt_AllDiscnt.ClearFmlValue();
    StaticDAO& dao = StaticDAO::getDAO();
    int iCount = 0;

    try
    {
        dao.Clear();
        dao.SetParam(":VTRADE_ID", strTradeId);
        dao.SetParam(":VACCEPT_MONTH", strTradeId.substr(4,2));
        dao.SetParam(":VUSER_ID", strUserId);
        dao.SetParam(":VCUST_ID", strCustId);
        dao.SetParam(":VACCT_ID", strAcctId);

        iCount = dao.jselect(bufTradeDiscnt_AllDiscnt, "SEL_ALLDISCNTS_AFTER_TRADE","TF_B_TRADE_DISCNT");
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取办理业务后三户的所有优惠出错！[%s]",ex.getCause()->what());
    }

    /*if(iCount > 0 )
    {
        ConvertCodeToName(bufTradeDiscnt_AllDiscnt, "ProductName", "PRODUCT_ID", "PRODUCT_NAME");
        ConvertCodeToName(bufTradeDiscnt_AllDiscnt, "PackageName", "PACKAGE_ID", "PACKAGE_NAME");
        ConvertCodeToName(bufTradeDiscnt_AllDiscnt, "DiscntName", "DISCNT_CODE", "DISCNT_NAME");
    }*/

    bufTradeDiscnt_AllDiscnt.SetInt("X_RECORDNUM", iCount);

    return iCount;
}


/**
 *获取办理业务后用户的所有服务
 *
 */
int TradeCheckAfterTrade::GetUserAllServiceAfterTrade(const string &strTradeId, const string &strUserId,
                                                       CFmlBuf &bufTradeSvc_UserAllSvc)
{
    bufTradeSvc_UserAllSvc.ClearFmlValue();
    StaticDAO& dao = StaticDAO::getDAO();
    int iCount = 0;

    try
    {
        dao.Clear();
        dao.SetParam(":VTRADE_ID", strTradeId);
        dao.SetParam(":VACCEPT_MONTH", strTradeId.substr(4,2));
        dao.SetParam(":VUSER_ID", strUserId);
        iCount = dao.jselect(bufTradeSvc_UserAllSvc, "SEL_ALLSERVICES_AFTER_TRADE","TF_B_TRADE_SVC");
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取办理业务后用户的所有服务出错！[%s]",ex.getCause()->what());
    }
    LOG_TRACE_P1(logTradeCheckAfterTrade, "--bufTradeSvc_UserAllSvc1  strTradeId:%s",strTradeId.c_str());
    LOG_TRACE_P1(logTradeCheckAfterTrade, "--bufTradeSvc_UserAllSvc1  ACCEPT_MONTH:%s",strTradeId.substr(4,2).c_str());
    LOG_TRACE_P1(logTradeCheckAfterTrade, "--bufTradeSvc_UserAllSvc1  strUserId:%s",strUserId.c_str());
    LOG_TRACE_P1(logTradeCheckAfterTrade, "--bufTradeSvc_UserAllSvc1  iCount:%s",to_string(iCount).c_str());
    /*if(iCount > 0)
    {
        ConvertCodeToName(bufTradeSvc_UserAllSvc, "ProductName", "PRODUCT_ID", "PRODUCT_NAME");
        ConvertCodeToName(bufTradeSvc_UserAllSvc, "PackageName", "PACKAGE_ID", "PACKAGE_NAME");
        ConvertCodeToName(bufTradeSvc_UserAllSvc, "ServiceName", "SERVICE_ID", "SERVICE_NAME");
    }*/

    bufTradeSvc_UserAllSvc.SetInt("X_RECORDNUM", iCount);

    return iCount;
}
/**
 *获取办理业务后的用户所有优惠和优惠的开始时间、结束时间、和修改状态（三户原有优惠的修改状态默认为'A')
 *
 */
int TradeCheckAfterTrade::GetAllDiscntAfterTradehb(const string &strTradeId, const string &strUserId,
                                                 const string &strCustId, const string &strAcctId,
                                                 CFmlBuf &bufTradeDiscnt_AllDiscnt)
{
    bufTradeDiscnt_AllDiscnt.ClearFmlValue();
    StaticDAO& dao = StaticDAO::getDAO();
    int iCount = 0;

    try
    {
        dao.Clear();
        dao.SetParam(":VTRADE_ID", strTradeId);
        dao.SetParam(":VACCEPT_MONTH", strTradeId.substr(4,2));
        dao.SetParam(":VUSER_ID", strUserId);
        dao.SetParam(":VCUST_ID", strCustId);
        dao.SetParam(":VACCT_ID", strAcctId);

        iCount = dao.jselect(bufTradeDiscnt_AllDiscnt, "SEL_ALLDISCNTS_AFTER_TRADE_HB","TF_B_TRADE_DISCNT");
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取办理业务后三户的所有优惠出错！[%s]",ex.getCause()->what());
    }

    if(iCount > 0 )
    {
        ConvertCodeToName(bufTradeDiscnt_AllDiscnt, "ProductName", "PRODUCT_ID", "PRODUCT_NAME");
        ConvertCodeToName(bufTradeDiscnt_AllDiscnt, "PackageName", "PACKAGE_ID", "PACKAGE_NAME");
        ConvertCodeToName(bufTradeDiscnt_AllDiscnt, "DiscntName", "DISCNT_CODE", "DISCNT_NAME");
    }

    bufTradeDiscnt_AllDiscnt.SetInt("X_RECORDNUM", iCount);

    return iCount;
}
/**
 *获取办理业务后用户的所有服务
 *
 */
int TradeCheckAfterTrade::GetUserAllServiceAfterTradehb(const string &strTradeId, const string &strUserId,
                                                       CFmlBuf &bufTradeSvc_UserAllSvc)
{
    bufTradeSvc_UserAllSvc.ClearFmlValue();
    StaticDAO& dao = StaticDAO::getDAO();
    int iCount = 0;

    try
    {
        dao.Clear();
        dao.SetParam(":VTRADE_ID", strTradeId);
        dao.SetParam(":VACCEPT_MONTH", strTradeId.substr(4,2));
        dao.SetParam(":VUSER_ID", strUserId);
        iCount = dao.jselect(bufTradeSvc_UserAllSvc, "SEL_ALLSERVICES_AFTER_TRADE_HB","TF_B_TRADE_SVC");
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取办理业务后用户的所有服务出错！[%s]",ex.getCause()->what());
    }

    if(iCount > 0)
    {
        ConvertCodeToName(bufTradeSvc_UserAllSvc, "ProductName", "PRODUCT_ID", "PRODUCT_NAME");
        ConvertCodeToName(bufTradeSvc_UserAllSvc, "PackageName", "PACKAGE_ID", "PACKAGE_NAME");
        ConvertCodeToName(bufTradeSvc_UserAllSvc, "ServiceName", "SERVICE_ID", "SERVICE_NAME");
    }

    bufTradeSvc_UserAllSvc.SetInt("X_RECORDNUM", iCount);

    return iCount;
}

/**
 *获取办理业务后用户的所有SP
 *
 */
int TradeCheckAfterTrade::GetUserAllSpAfterTrade(const string &strTradeId, const string &strUserId,
                                                 CFmlBuf &bufTradeSp_UserAllSp)
{
    bufTradeSp_UserAllSp.ClearFmlValue();
    StaticDAO& dao = StaticDAO::getDAO();
    int iCount = 0;

    try
    {
        dao.Clear();
        dao.SetParam(":VTRADE_ID", strTradeId);
        dao.SetParam(":VACCEPT_MONTH", strTradeId.substr(4,2));
        dao.SetParam(":VUSER_ID", strUserId);
        iCount = dao.jselect(bufTradeSp_UserAllSp, "SEL_ALLSP_AFTER_TRADE","TF_B_TRADE_SP");
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取办理业务后用户的所有SP出错！[%s]",ex.getCause()->what());
    }

    /*if(iCount > 0)
    {
        ConvertCodeToName(bufTradeSp_UserAllSp, "ProductName", "PRODUCT_ID", "PRODUCT_NAME");
        ConvertCodeToName(bufTradeSp_UserAllSp, "PackageName", "PACKAGE_ID", "PACKAGE_NAME");
        ConvertCodeToName(bufTradeSp_UserAllSp, "SpProductName", "SP_SERVICE_ID", "SP_PRODUCT_NAME");
    }*/

    bufTradeSp_UserAllSp.SetInt("X_RECORDNUM", iCount);

    return iCount;
}

/**
 *获取办理业务后用户的所有物品等其他元素
 *
 */
int TradeCheckAfterTrade::GetUserAllElementAfterTrade(const string &strTradeId, const string &strUserId,
                                                      CFmlBuf &bufTradeElement_UserAllElement)
{
    bufTradeElement_UserAllElement.ClearFmlValue();
    StaticDAO& dao = StaticDAO::getDAO();
    int iCount = 0;

    try
    {
        dao.Clear();
        dao.SetParam(":VTRADE_ID", strTradeId);
        dao.SetParam(":VACCEPT_MONTH", strTradeId.substr(4,2));
        dao.SetParam(":VUSER_ID", strUserId);
        iCount = dao.jselect(bufTradeElement_UserAllElement, "SEL_ALLELEMENT_AFTER_TRADE","TF_B_TRADE_ELEMENT");
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取办理业务后用户的所有其他元素出错！[%s]",ex.getCause()->what());
    }

    if(iCount > 0)
    {
        /*ConvertCodeToName(bufTradeElement_UserAllElement, "ProductName", "PRODUCT_ID", "PRODUCT_NAME");
        ConvertCodeToName(bufTradeElement_UserAllElement, "PackageName", "PACKAGE_ID", "PACKAGE_NAME");
        ConvertCodeToName(bufTradeElement_UserAllElement, "ActionName", "ID", "ACTION_NAME");
        ConvertCodeToName(bufTradeElement_UserAllElement, "ParentActionName", "ID", "PARENT_ACTION_NAME");        */
        char cTemp[10];
        for(int i = 0; i < iCount; i++)
        {
            /*cTemp[0] = '\0';
            sprintf(cTemp,"%d",(int)bufTradeElement_UserAllElement.GetInt("PRODUCT_ID", i));
            bufTradeElement_UserAllElement.SetString("PRODUCT_NAME", CParamDAO::getParam("ProductName", cTemp),i);
            cTemp[0] = '\0';
            sprintf(cTemp,"%d",(int)bufTradeElement_UserAllElement.GetInt("PACKAGE_ID", i));
            bufTradeElement_UserAllElement.SetString("PACKAGE_NAME", CParamDAO::getParam("PackageName", cTemp),i);*/
            if(bufTradeElement_UserAllElement.GetString("ID_TYPE", i) == "A" ||
                bufTradeElement_UserAllElement.GetString("ID_TYPE", i) == "C")
            {
                bufTradeElement_UserAllElement.SetString("ACTION_NAME", CParamDAO::getParam("ActionName", bufTradeElement_UserAllElement.GetString("ID", i)),i);
            }
            else
            {
                bufTradeElement_UserAllElement.SetString("PARENT_ACTION_NAME", CParamDAO::getParam("ParentActionName", bufTradeElement_UserAllElement.GetString("ID", i)),i);
            }
        }
    }

    bufTradeElement_UserAllElement.SetInt("X_RECORDNUM", iCount);

    return iCount;
}


//按获取用户在业务受理后的业务包信息
//iProductId:-1时取所有产品，即用户所有包信息
int TradeCheckAfterTrade::GetUserPackageByProduct(const int &iProductId, CFmlBuf &bufTradeDiscnt_AllDiscnt,
                                                  CFmlBuf &bufTradeSp_UserAllSp, CFmlBuf &bufTradeElement_UserAllElement,
                                                  CFmlBuf &bufTradeSvc_UserAllSvc, CFmlBuf &bufUserPackageByProd, const string &strProdEndDate)
{
	LOG_TRACE(logTradeCheckAfterTrade, "进入 GetUserPackageByProduct 函数");
    bufUserPackageByProd.ClearFmlValue();
    bool bFound = false;
    int iPackageId = 0;
    int iPackageCount = 0;
    string strProductEndDate;
    
    string strLastDate="";
    DualMgr objDualMgr;
    objDualMgr.GetLastDayOfCurMonth(strLastDate);
    string strCurrentDate="";
    objDualMgr.GetSysDate(strCurrentDate);  

    strProductEndDate= strProdEndDate;
    if (strProductEndDate>="2015-12-31")
        strProductEndDate="2015-12-31"; //设置为无穷大. 有的省时2019(山西)所以设置2015为无穷大
	//modfiy by lirui 20120220 月底结束的包不作为用户已有包
    for(int i = 0; i < bufTradeDiscnt_AllDiscnt.GetInt("X_RECORDNUM"); i++)
    {
        if(iProductId != -1 && bufTradeDiscnt_AllDiscnt.GetInt("PRODUCT_ID", i) != iProductId)
            continue;

        iPackageId = bufTradeDiscnt_AllDiscnt.GetInt("PACKAGE_ID", i);
        bFound = false;

        iPackageCount = 0;
        if(bufUserPackageByProd.IsFldExist("PACKAGE_ID"))
            iPackageCount = bufUserPackageByProd.GetCount("PACKAGE_ID");

        for(int j = 0; j < iPackageCount; j++)
        {
            if(bufUserPackageByProd.GetInt("PACKAGE_ID", j) == iPackageId)
            {
                bFound = true;

                if(bufUserPackageByProd.GetString("START_DATE", j) > bufTradeDiscnt_AllDiscnt.GetString("START_DATE", i))
                    bufUserPackageByProd.SetString("START_DATE", bufTradeDiscnt_AllDiscnt.GetString("START_DATE", i), j);

                if(bufUserPackageByProd.GetString("END_DATE", j) < bufTradeDiscnt_AllDiscnt.GetString("END_DATE", i)&&strProductEndDate<=bufTradeDiscnt_AllDiscnt.GetString("END_DATE", i))//不判断结束到月底的资费  modify by tz@2009-5-27 09:47上午
                    bufUserPackageByProd.SetString("END_DATE", bufTradeDiscnt_AllDiscnt.GetString("END_DATE", i), j);

                break;
            }
        }
        //&&strProductEndDate<=bufTradeDiscnt_AllDiscnt.GetString("END_DATE", i)
        //if(!bFound&&bufTradeDiscnt_AllDiscnt.GetString("END_DATE", i)>strLastDate)  //不判断结束到月底的资费  modify by tz@2009-5-27 09:47上午
        //liuhj 2012-03-22 QC_30587 月底失效的产品和优惠，其对应的包也应该加进去，加了一个||，同时修改SQL:SEL_ALLPRODUCTS_AFTER_TRADE
        //随手改成strLastDate，领导要求走QC，不得已改回去吧，谁觉得有问题谁再把strCurrentDate改成 strLastDate
        if(!bFound && (bufTradeDiscnt_AllDiscnt.GetString("END_DATE", i)>strCurrentDate || 
                       (bufTradeDiscnt_AllDiscnt.GetString("END_DATE", i) == strLastDate && strProductEndDate == strLastDate)))
        {
            bufUserPackageByProd.SetInt("PRODUCT_ID", iProductId, iPackageCount);
            bufUserPackageByProd.SetString("PRODUCT_NAME", bufTradeDiscnt_AllDiscnt.GetString("PRODUCT_NAME", i), iPackageCount);
            bufUserPackageByProd.SetInt("PACKAGE_ID", iPackageId, iPackageCount);
            bufUserPackageByProd.SetString("PACKAGE_NAME", bufTradeDiscnt_AllDiscnt.GetString("PACKAGE_NAME", i), iPackageCount);
            bufUserPackageByProd.SetString("START_DATE", bufTradeDiscnt_AllDiscnt.GetString("START_DATE", i), iPackageCount);
            bufUserPackageByProd.SetString("END_DATE", bufTradeDiscnt_AllDiscnt.GetString("END_DATE", i), iPackageCount);
        }
    }

    for(int i = 0; i < bufTradeSvc_UserAllSvc.GetInt("X_RECORDNUM"); i++)
    {
        if(iProductId != -1 && bufTradeSvc_UserAllSvc.GetInt("PRODUCT_ID", i) != iProductId)
            continue;

        iPackageId = bufTradeSvc_UserAllSvc.GetInt("PACKAGE_ID", i);
        bFound = false;

        iPackageCount = 0;
        if(bufUserPackageByProd.IsFldExist("PACKAGE_ID"))
            iPackageCount = bufUserPackageByProd.GetCount("PACKAGE_ID");

        for(int j = 0; j < iPackageCount; j++)
        {
            if(bufUserPackageByProd.GetInt("PACKAGE_ID", j) == iPackageId)
            {
                bFound = true;

                if(bufUserPackageByProd.GetString("START_DATE", j) > bufTradeSvc_UserAllSvc.GetString("START_DATE", i))
                    bufUserPackageByProd.SetString("START_DATE", bufTradeSvc_UserAllSvc.GetString("START_DATE", i), j);

                if(bufUserPackageByProd.GetString("END_DATE", j) < bufTradeSvc_UserAllSvc.GetString("END_DATE", i))
                    bufUserPackageByProd.SetString("END_DATE", bufTradeSvc_UserAllSvc.GetString("END_DATE", i), j);

                break;
            }
        }

        if(!bFound&&(bufTradeSvc_UserAllSvc.GetString("END_DATE", i)>strLastDate || 
        			(bufTradeSvc_UserAllSvc.GetString("END_DATE", i) == strLastDate && strProductEndDate == strLastDate)))
        {
            bufUserPackageByProd.SetInt("PRODUCT_ID", iProductId, iPackageCount);
            bufUserPackageByProd.SetString("PRODUCT_NAME", bufTradeSvc_UserAllSvc.GetString("PRODUCT_NAME", i), iPackageCount);
            bufUserPackageByProd.SetInt("PACKAGE_ID", iPackageId, iPackageCount);
            bufUserPackageByProd.SetString("PACKAGE_NAME", bufTradeSvc_UserAllSvc.GetString("PACKAGE_NAME", i), iPackageCount);
            bufUserPackageByProd.SetString("START_DATE", bufTradeSvc_UserAllSvc.GetString("START_DATE", i), iPackageCount);
            bufUserPackageByProd.SetString("END_DATE", bufTradeSvc_UserAllSvc.GetString("END_DATE", i), iPackageCount);
        }
    }

    for(int i = 0; i < bufTradeSp_UserAllSp.GetInt("X_RECORDNUM"); i++)
    {
        if(iProductId != -1 && bufTradeSp_UserAllSp.GetInt("PRODUCT_ID", i) != iProductId)
            continue;

        iPackageId = bufTradeSp_UserAllSp.GetInt("PACKAGE_ID", i);
        bFound = false;

        iPackageCount = 0;
        if(bufUserPackageByProd.IsFldExist("PACKAGE_ID"))
            iPackageCount = bufUserPackageByProd.GetCount("PACKAGE_ID");

        for(int j = 0; j < iPackageCount; j++)
        {
            if(bufUserPackageByProd.GetInt("PACKAGE_ID", j) == iPackageId)
            {
                bFound = true;

                if(bufUserPackageByProd.GetString("START_DATE", j) > bufTradeSp_UserAllSp.GetString("START_DATE", i))
                    bufUserPackageByProd.SetString("START_DATE", bufTradeSp_UserAllSp.GetString("START_DATE", i), j);

                if(bufUserPackageByProd.GetString("END_DATE", j) < bufTradeSp_UserAllSp.GetString("END_DATE", i))
                    bufUserPackageByProd.SetString("END_DATE", bufTradeSp_UserAllSp.GetString("END_DATE", i), j);

                break;
            }
        }

        if(!bFound&&bufTradeSp_UserAllSp.GetString("END_DATE", i)>strLastDate ||
            (bufTradeSp_UserAllSp.GetString("END_DATE", i) == strLastDate && strProductEndDate == strLastDate))
        {
            bufUserPackageByProd.SetInt("PRODUCT_ID", iProductId, iPackageCount);
            bufUserPackageByProd.SetString("PRODUCT_NAME", bufTradeSp_UserAllSp.GetString("PRODUCT_NAME", i), iPackageCount);
            bufUserPackageByProd.SetInt("PACKAGE_ID", iPackageId, iPackageCount);
            bufUserPackageByProd.SetString("PACKAGE_NAME", bufTradeSp_UserAllSp.GetString("PACKAGE_NAME", i), iPackageCount);
            bufUserPackageByProd.SetString("START_DATE", bufTradeSp_UserAllSp.GetString("START_DATE", i), iPackageCount);
            bufUserPackageByProd.SetString("END_DATE", bufTradeSp_UserAllSp.GetString("END_DATE", i), iPackageCount);
        }
    }

    for(int i = 0; i < bufTradeElement_UserAllElement.GetInt("X_RECORDNUM"); i++)
    {
        if(iProductId != -1 && bufTradeElement_UserAllElement.GetInt("PRODUCT_ID", i) != iProductId)
            continue;

        iPackageId = bufTradeElement_UserAllElement.GetInt("PACKAGE_ID", i);
        bFound = false;

        iPackageCount = 0;
        if(bufUserPackageByProd.IsFldExist("PACKAGE_ID"))
            iPackageCount = bufUserPackageByProd.GetCount("PACKAGE_ID");

        for(int j = 0; j < iPackageCount; j++)
        {
            if(bufUserPackageByProd.GetInt("PACKAGE_ID", j) == iPackageId)
            {
                bFound = true;

                if(bufUserPackageByProd.GetString("START_DATE", j) > bufTradeElement_UserAllElement.GetString("START_DATE", i))
                    bufUserPackageByProd.SetString("START_DATE", bufTradeElement_UserAllElement.GetString("START_DATE", i), j);

                if(bufUserPackageByProd.GetString("END_DATE", j) < bufTradeElement_UserAllElement.GetString("END_DATE", i))
                    bufUserPackageByProd.SetString("END_DATE", bufTradeElement_UserAllElement.GetString("END_DATE", i), j);

                break;
            }
        }

        if(!bFound&&bufTradeElement_UserAllElement.GetString("END_DATE", i)>strLastDate ||
            (bufTradeElement_UserAllElement.GetString("END_DATE", i) == strLastDate && strProductEndDate == strLastDate))
        {
            bufUserPackageByProd.SetInt("PRODUCT_ID", iProductId, iPackageCount);
            bufUserPackageByProd.SetString("PRODUCT_NAME", bufTradeElement_UserAllElement.GetString("PRODUCT_NAME", i), iPackageCount);
            bufUserPackageByProd.SetInt("PACKAGE_ID", iPackageId, iPackageCount);
            bufUserPackageByProd.SetString("PACKAGE_NAME", bufTradeElement_UserAllElement.GetString("PACKAGE_NAME", i), iPackageCount);
            bufUserPackageByProd.SetString("START_DATE", bufTradeElement_UserAllElement.GetString("START_DATE", i), iPackageCount);
            bufUserPackageByProd.SetString("END_DATE", bufTradeElement_UserAllElement.GetString("END_DATE", i), iPackageCount);
        }
    }

    iPackageCount = 0;
    if(bufUserPackageByProd.IsFldExist("PACKAGE_ID"))
        iPackageCount = bufUserPackageByProd.GetCount("PACKAGE_ID");

    bufUserPackageByProd.SetInt("X_RECORDNUM", iPackageCount);
    return iPackageCount;
}


//获取产品构成包信息
//xTag:0-包内元素最大最小值取TD_B_PACKAGE  1-包内元素最大最小值取TD_B_PRODUCT_PACKAGE
int TradeCheckAfterTrade::GetPackageByProduct(const int &iProductId, const string &strEparchyCode, const string &strTradeStaffId, CFmlBuf &bufPackageByProd, const int xTag)
{
    bufPackageByProd.ClearFmlValue();
    StaticDAO& dao = StaticDAO::getDAO();
    int iCount = 0;

    try
    {
        dao.Clear();
        dao.SetParam(":VPRODUCT_ID", iProductId);
        dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
        //dao.SetParam(":VTRADE_STAFF_ID", strTradeStaffId);
        dao.SetParam(":VX_TAG", xTag);
        iCount = dao.jselect(bufPackageByProd, "SEL_PACKAGES_BY_PID_CHEK", "TD_B_PRODUCT_PACKAGE");
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取产品构成包信息出错！[%s]",ex.getCause()->what());
    }

    ConvertCodeToName(bufPackageByProd, "ProductName", "PRODUCT_ID", "PRODUCT_NAME");

    bufPackageByProd.SetInt("X_RECORDNUM", iCount);
    return iCount;
}


//产品必选包判断
void TradeCheckAfterTrade::CheckForcePackageByProduct(const int &iProductId, const string &strEparchyCode, const string &strTradeStaffId,
                                                      CFmlBuf &bufUserPackageByProd)
{
    //获取产品构成包信息
    bool bFound = false;
    CFmlBuf bufPackageByProd;
    int iCount = GetPackageByProduct(iProductId, strEparchyCode, strTradeStaffId, bufPackageByProd);
    int iPackageId = 0;
    string strErrorInfo = "";

    for(int i = 0; i < iCount; i++)
    {
        if(bufPackageByProd.GetString("FORCE_TAG", i) != "1")
            continue;

        //必选包标识
        iPackageId = bufPackageByProd.GetInt("PACKAGE_ID", i);
        bFound = false;

        for(int j = 0; j < bufUserPackageByProd.GetInt("X_RECORDNUM"); j++)
        {
            if(bufUserPackageByProd.GetInt("PRODUCT_ID", j) == iProductId &&
                bufUserPackageByProd.GetInt("PACKAGE_ID", j) == iPackageId)
            {
                bFound = true;
                break;
            }
        }

        if(!bFound)
        {
            if(strErrorInfo == "")
                strErrorInfo = bufPackageByProd.GetString("PACKAGE_NAME", i);
            else
                strErrorInfo += "、"+bufPackageByProd.GetString("PACKAGE_NAME", i);
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR, "产品：\"" +
                    bufPackageByProd.GetString("PRODUCT_NAME") + "\"，必选业务包：\"" +
                    strErrorInfo + "\"没有选择，业务无法继续！");
}


//产品包内元素的最大最小选择数判断
void TradeCheckAfterTrade::CheckPackageElementChoiceByProduct(const string &strEparchyCode, const string &strTradeStaffId, CFmlBuf &bufUserPackageByProd,
                                                              CFmlBuf &bufUserAllElements, CFmlBuf &bufNoChangePackage, CFmlBuf &bufDelPackage,
                                                              CFmlBuf &bufElementTimeSeries,
                                                              const int xTag)
{
    if(bufUserPackageByProd.GetInt("X_RECORDNUM") == 0)
        return;

    int iProductId = bufUserPackageByProd.GetInt("PRODUCT_ID");
    string strProductName = bufUserPackageByProd.GetString("PRODUCT_NAME");
    int iUserAllElementCount = bufUserAllElements.GetInt("X_RECORDNUM");

    //获取产品构成包信息
    CFmlBuf bufPackageByProd;
    int iPackageCount = GetPackageByProduct(iProductId, strEparchyCode, strTradeStaffId, bufPackageByProd, xTag);

    int iMaxElement = 0;
    int iMinElement = 0;
    int iUserElement = 0;
    string strErrorInfo = "";

    int iUserPackageCount = bufUserPackageByProd.GetInt("X_RECORDNUM");
    for(int i = 0; i < iUserPackageCount; i++)
    {
        //当前业务无变化的包不作判断
        bool bExist = false;
        int iNoChange = bufNoChangePackage.GetInt("X_RECORDNUM");
        for(int g = 0; g < iNoChange; g++)
        {
            if(bufNoChangePackage.GetInt("PRODUCT_ID", g) == bufUserPackageByProd.GetInt("PRODUCT_ID", i)
                && bufNoChangePackage.GetInt("PACKAGE_ID", g) == bufUserPackageByProd.GetInt("PACKAGE_ID", i))
            {
                bExist = true;
                break;
            }
        }
        if(bExist) continue;

        //当前业务完全删除的包不作判断
        bExist = false;
        int iDelPack = bufDelPackage.GetInt("X_RECORDNUM");
        for(int g = 0; g < iDelPack; g++)
        {
            if(bufDelPackage.GetString("RSRV_STR1", g) == to_string(bufUserPackageByProd.GetInt("PRODUCT_ID", i))
                && bufDelPackage.GetString("RSRV_STR3", g) == to_string(bufUserPackageByProd.GetInt("PACKAGE_ID", i)))
            {
                bExist = true;
                break;
            }
        }
        if(bExist) continue;


        iMaxElement = -1;
        iMinElement = -1;
        iUserElement = 0;
        for(int j = 0; j < iPackageCount; j++)
        {
            if(bufUserPackageByProd.GetInt("PACKAGE_ID", i) ==
                bufPackageByProd.GetInt("PACKAGE_ID", j))
            {
                iMaxElement = bufPackageByProd.GetInt("MAX_NUMBER", j);
                iMinElement = bufPackageByProd.GetInt("MIN_NUMBER", j);
                break;
            }
        }

        //产品包内元素最小、最大选择数判断
        if(iMinElement >= 0 || iMaxElement >= 0)
        {
            //获取产品包内用户元素选择数
            for(int z = 0; z < iUserAllElementCount; z++)
            {
                if(bufUserAllElements.GetInt("PRODUCT_ID", z) == iProductId
                    && bufUserAllElements.GetInt("PACKAGE_ID", z) == bufUserPackageByProd.GetInt("PACKAGE_ID", i))
                {
                    iUserElement++;
                }
            }

            //减去包内连续时间元素个数
            int iRecCount = bufElementTimeSeries.GetInt("X_RECORDNUM");
            int iSeriesCount = 0;
            for(int g = 0; g < iRecCount; g++)
            {
                if(bufElementTimeSeries.GetInt("PRODUCT_ID", g) == iProductId
                    && bufElementTimeSeries.GetInt("PACKAGE_ID", g) == bufUserPackageByProd.GetInt("PACKAGE_ID", i))
                {
                    iSeriesCount = bufElementTimeSeries.GetInt("X_TAG");
                    break;
                }
            }
            iUserElement -= iSeriesCount;

            //产品包内元素最小选择数判断
            if(iMinElement >= 0 && iUserElement < iMinElement)
            {
                if(strErrorInfo == "")
                {
                    strErrorInfo = "业务包：\""
                                 + bufUserPackageByProd.GetString("PACKAGE_NAME", i)
                                 + "\"最少选择" + to_string(iMinElement) + "个元素";
                }
                else
                {
                    strErrorInfo += "、业务包：\""
                                 + bufUserPackageByProd.GetString("PACKAGE_NAME", i)
                                 + "\"最少选择" + to_string(iMinElement) + "个元素";
                }
            }

            //产品包内元素最大选择数判断
            if(iMaxElement >= 0 && iUserElement > iMaxElement)
            {
                if(strErrorInfo == "")
                {
                    strErrorInfo = "业务包：\""
                                 + bufUserPackageByProd.GetString("PACKAGE_NAME", i)
                                 + "\"最多选择" + to_string(iMaxElement) + "个元素";
                }
                else
                {
                    strErrorInfo += "、业务包：\""
                                 + bufUserPackageByProd.GetString("PACKAGE_NAME", i)
                                 + "\"最多选择" + to_string(iMaxElement) + "个元素";
                }
            }
        }
    }

    if(strErrorInfo != "")
    {
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR, "产品：\"" +
                    strProductName + "\"" + strErrorInfo + "，业务无法继续！");
    }
}


//获取产品内包间关系
int TradeCheckAfterTrade::ExistsProdPackageLimitA(const int &iProductId, const int &iPackageIdA,
                                                  const string &strLimitTag,const string strEparchyCode,
                                                  CFmlBuf &bufPackageLimitByProd)
{
    bufPackageLimitByProd.ClearFmlValue();
    StaticDAO& dao = StaticDAO::getDAO();
    int iCount = 0;

    try
    {
        dao.Clear();
        dao.SetParam(":VPRODUCT_ID", iProductId);
        dao.SetParam(":VPACKAGE_ID_A", iPackageIdA);
        dao.SetParam(":VLIMIT_TAG", strLimitTag);
        dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
        iCount = dao.jselect(bufPackageLimitByProd, "JUDGE_A_LIMIT_EXISTS", "TD_B_PROD_PACKAGE_LIMIT");
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取包间关系信息出错！[%s]",ex.getCause()->what());
    }

    /*ConvertCodeToName(bufPackageLimitByProd, "ProductName", "PRODUCT_ID", "PRODUCT_NAME");
    ConvertCodeToName(bufPackageLimitByProd, "PackageName", "PACKAGE_ID_A", "PACKAGE_NAME_A");
    ConvertCodeToName(bufPackageLimitByProd, "PackageName", "PACKAGE_ID_B", "PACKAGE_NAME_B");*/

    bufPackageLimitByProd.SetInt("X_RECORDNUM", iCount);
    return iCount;
}


//产品内包间关系判断
void TradeCheckAfterTrade::CheckPackageLimitByProduct(const int &iProductId,CFmlBuf &bufUserPackageByProd,
                                                      const string strEparchyCode)
{
    int iUserPackageCount = bufUserPackageByProd.GetInt("X_RECORDNUM");
    CFmlBuf bufPackageLimitByProd;

    int iCount = 0;
    bool bFound = false;

    string strErrorInfo = "";
    //按用户包循环校验包间互斥关系
    for(int i = 0; i < iUserPackageCount; i++)
    {
        iCount = 0;
        bufPackageLimitByProd.ClearFmlValue();

        iCount = ExistsProdPackageLimitA(iProductId, bufUserPackageByProd.GetInt("PACKAGE_ID", i), "0", strEparchyCode, bufPackageLimitByProd);
        for(int j = 0; j < iCount; j++)
        {
            bFound = false;
            for(int z = 0; z < iUserPackageCount; z++)
            {
                if (i==z)continue; //自身不做互斥判断 added by tangz@2009-2-7 11:33
                if(bufUserPackageByProd.GetInt("PACKAGE_ID", z)
                    == bufPackageLimitByProd.GetInt("PACKAGE_ID_B", j))
                {
                    bFound = true;
                    break;
                }
            }

            if(bFound)
            {
                //用户包(i)与限制包(j)互斥
                if(strErrorInfo == "")
                    strErrorInfo = "业务包：\"" + bufUserPackageByProd.GetString("PACKAGE_NAME", i)
                        + "\"与业务包：\"" +  bufPackageLimitByProd.GetString("PACKAGE_NAME_B", j) + "\"";
                else
                    strErrorInfo += "、业务包：\"" + bufUserPackageByProd.GetString("PACKAGE_NAME", i)
                        + "\"与业务包：\"" +  bufPackageLimitByProd.GetString("PACKAGE_NAME_B", j) + "\"";
            }
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR, "产品：\"" +
                    bufUserPackageByProd.GetString("PRODUCT_NAME") + "\"，" +
                    strErrorInfo + "互斥，业务无法继续！");


    //按用户包循环校验包间部份依赖关系
    strErrorInfo = "";
    for(int i = 0; i < iUserPackageCount; i++)
    {
        iCount = 0;
        bFound = true;
        bufPackageLimitByProd.ClearFmlValue();

        iCount = ExistsProdPackageLimitA(iProductId, bufUserPackageByProd.GetInt("PACKAGE_ID", i), "1", strEparchyCode, bufPackageLimitByProd);
        int j = 0;
        for(j = 0; j < iCount; j++)
        {
            bFound = false;
            for(int z = 0; z < iUserPackageCount; z++)
            {
                if(bufUserPackageByProd.GetInt("PACKAGE_ID", z)
                    == bufPackageLimitByProd.GetInt("PACKAGE_ID_B", j))
                {
                    bFound = true;
                    break;
                }
            }

            if(bFound)
            {
                break;
            }
        }

        if(!bFound)
        {
            //用户包(i)部份依赖于限制包(j)
            //部份依赖业务包拼串
            string strInfo = "";
            for(int y = 0; y < iCount; y++)
            {
                if(strInfo == "")
                    strInfo = bufPackageLimitByProd.GetString("PACKAGE_NAME_B", y);
                else
                    strInfo += "、" + bufPackageLimitByProd.GetString("PACKAGE_NAME_B", y);
            }

            if(strErrorInfo == "")
                strErrorInfo = "业务包：\"" + bufUserPackageByProd.GetString("PACKAGE_NAME", i)
                    + "\"部份依赖于业务包：\"" +  strInfo + "\"";
            else
                strErrorInfo += "、业务包：\"" + bufUserPackageByProd.GetString("PACKAGE_NAME", i)
                    + "\"部份依赖于业务包：\"" +  strInfo + "\"";
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR, "产品：\"" +
                    bufUserPackageByProd.GetString("PRODUCT_NAME") + "\"，" +
                    strErrorInfo + "，业务无法继续！");


    //按用户包循环校验包间完全依赖关系
    strErrorInfo = "";
    for(int i = 0; i < iUserPackageCount; i++)
    {
        iCount = 0;
        bufPackageLimitByProd.ClearFmlValue();

        iCount = ExistsProdPackageLimitA(iProductId, bufUserPackageByProd.GetInt("PACKAGE_ID", i), "2", strEparchyCode, bufPackageLimitByProd);
        for(int j = 0; j < iCount; j++)
        {
            bFound = false;
            for(int z = 0; z < iUserPackageCount; z++)
            {
                if(bufUserPackageByProd.GetInt("PACKAGE_ID", z)
                    == bufPackageLimitByProd.GetInt("PACKAGE_ID_B", j))
                {
                    bFound = true;
                    break;
                }
            }

            if(!bFound)
            {
                //用户包(i)完全依赖于限制包(j)
                //完全依赖业务包拼串
                string strInfo = "";
                for(int y = 0; y < iCount; y++)
                {
                    if(strInfo == "")
                        strInfo = bufPackageLimitByProd.GetString("PACKAGE_NAME_B", y);
                    else
                        strInfo += "、" + bufPackageLimitByProd.GetString("PACKAGE_NAME_B", y);
                }

                if(strErrorInfo == "")
                    strErrorInfo = "业务包：\"" + bufUserPackageByProd.GetString("PACKAGE_NAME", i)
                        + "\"完全依赖于业务包：\"" +  strInfo + "\"";
                else
                    strErrorInfo += "、业务包：\"" + bufUserPackageByProd.GetString("PACKAGE_NAME", i)
                        + "\"完全依赖于业务包：\"" +  strInfo + "\"";

                break;
            }
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR, "产品：\"" +
                    bufUserPackageByProd.GetString("PRODUCT_NAME") + "\"，" +
                    strErrorInfo + "，业务无法继续！");
}


//获取全局包间关系
int TradeCheckAfterTrade::ExistsPackageLimitA(const int &iPackageIdA, const string &strLimitTag,
                                              const string strEparchyCode, CFmlBuf &bufPackageLimit)
{
    bufPackageLimit.ClearFmlValue();
    StaticDAO& dao = StaticDAO::getDAO();
    int iCount = 0;

    try
    {
        dao.Clear();
        dao.SetParam(":VPACKAGE_ID_A", iPackageIdA);
        dao.SetParam(":VLIMIT_TAG", strLimitTag);
        dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
        iCount = dao.jselect(bufPackageLimit, "JUDGE_A_LIMIT_EXISTS", "TD_B_PACKAGE_LIMIT");
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取包间关系信息出错！[%s]",ex.getCause()->what());
    }

    //ConvertCodeToName(bufPackageLimit, "PackageName", "PACKAGE_ID_A", "PACKAGE_NAME_A");
    //ConvertCodeToName(bufPackageLimit, "PackageName", "PACKAGE_ID_B", "PACKAGE_NAME_B");

    bufPackageLimit.SetInt("X_RECORDNUM", iCount);
    return iCount;
}


//全局包间关系判断
void TradeCheckAfterTrade::CheckPackageLimit(CFmlBuf &bufUserPackage, const string strEparchyCode)
{
    int iUserPackageCount = bufUserPackage.GetInt("X_RECORDNUM");
    CFmlBuf bufPackageLimit;

    int iCount = 0;
    bool bFound = false;

    string strErrorInfo = "";
    //按用户包循环校验包间互斥关系
    for(int i = 0; i < iUserPackageCount; i++)
    {
        iCount = 0;
        bufPackageLimit.ClearFmlValue();

        iCount = ExistsPackageLimitA(bufUserPackage.GetInt("PACKAGE_ID", i), "0", strEparchyCode, bufPackageLimit);
        for(int j = 0; j < iCount; j++)
        {
            bFound = false;
            for(int z = 0; z < iUserPackageCount; z++)
            {
                if (i==z)continue; //自身不做互斥判断 added by tangz@2009-2-7 11:33
                if(bufUserPackage.GetInt("PACKAGE_ID", z) == bufPackageLimit.GetInt("PACKAGE_ID_B", j)

                    //时间交集
                    && (bufUserPackage.GetString("START_DATE",z) <= bufUserPackage.GetString("START_DATE",i)
                    && bufUserPackage.GetString("START_DATE",i) <= bufUserPackage.GetString("END_DATE",z)
                    ||
                    bufUserPackage.GetString("START_DATE",i) <= bufUserPackage.GetString("START_DATE",z)
                    && bufUserPackage.GetString("START_DATE",z) <= bufUserPackage.GetString("END_DATE",i)))
                {
                    bFound = true;
                    break;
                }
            }

            if(bFound)
            {
                //用户包(i)与限制包(j)互斥
                if(strErrorInfo == "")
                    strErrorInfo = "业务包：\"" + bufUserPackage.GetString("PACKAGE_NAME", i)
                        + "\"与业务包：\"" +  bufPackageLimit.GetString("PACKAGE_NAME_B", j) + "\"";
                else
                    strErrorInfo += "、业务包：\"" + bufUserPackage.GetString("PACKAGE_NAME", i)
                        + "\"与业务包：\"" +  bufPackageLimit.GetString("PACKAGE_NAME_B", j) + "\"";
            }
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR, strErrorInfo + "互斥，业务无法继续！");


    //按用户包循环校验包间部份依赖关系
    strErrorInfo = "";
    for(int i = 0; i < iUserPackageCount; i++)
    {
        iCount = 0;
        bFound = true;
        bufPackageLimit.ClearFmlValue();

        iCount = ExistsPackageLimitA(bufUserPackage.GetInt("PACKAGE_ID", i), "1", strEparchyCode, bufPackageLimit);
        int j = 0;
        for(j = 0; j < iCount; j++)
        {
            bFound = false;
            for(int z = 0; z < iUserPackageCount; z++)
            {
                if(bufUserPackage.GetInt("PACKAGE_ID", z) == bufPackageLimit.GetInt("PACKAGE_ID_B", j)
                    //时间交集
                    && (bufUserPackage.GetString("START_DATE",z) <= bufUserPackage.GetString("START_DATE",i)
                    && bufUserPackage.GetString("START_DATE",i) <= bufUserPackage.GetString("END_DATE",z)
                    ||
                    bufUserPackage.GetString("START_DATE",i) <= bufUserPackage.GetString("START_DATE",z)
                    && bufUserPackage.GetString("START_DATE",z) <= bufUserPackage.GetString("END_DATE",i)))
                {
                    bFound = true;
                    break;
                }
            }

            if(bFound)
            {
                break;
            }
        }

        if(!bFound)
        {
            //用户包(i)部份依赖于限制包(j)
            //部份依赖业务包拼串
            string strInfo = "";
            for(int y = 0; y < iCount; y++)
            {
                if(strInfo == "")
                    strInfo = bufPackageLimit.GetString("PACKAGE_NAME_B", y);
                else
                    strInfo += "、" + bufPackageLimit.GetString("PACKAGE_NAME_B", y);
            }

            if(strErrorInfo == "")
                strErrorInfo = "业务包：\"" + bufUserPackage.GetString("PACKAGE_NAME", i)
                    + "\"部份依赖于业务包：\"" +  strInfo + "\"";
            else
                strErrorInfo += "、业务包：\"" + bufUserPackage.GetString("PACKAGE_NAME", i)
                    + "\"部份依赖于业务包：\"" +  strInfo + "\"";
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR, strErrorInfo + "，业务无法继续！");


    //按用户包循环校验包间完全依赖关系
    strErrorInfo = "";
    for(int i = 0; i < iUserPackageCount; i++)
    {
        iCount = 0;
        bufPackageLimit.ClearFmlValue();

        iCount = ExistsPackageLimitA(bufUserPackage.GetInt("PACKAGE_ID", i), "2", strEparchyCode, bufPackageLimit);
        for(int j = 0; j < iCount; j++)
        {
            bFound = false;
            for(int z = 0; z < iUserPackageCount; z++)
            {
                if(bufUserPackage.GetInt("PACKAGE_ID", z) == bufPackageLimit.GetInt("PACKAGE_ID_B", j)
                    //时间交集
                    && (bufUserPackage.GetString("START_DATE",z) <= bufUserPackage.GetString("START_DATE",i)
                    && bufUserPackage.GetString("START_DATE",i) <= bufUserPackage.GetString("END_DATE",z)
                    ||
                    bufUserPackage.GetString("START_DATE",i) <= bufUserPackage.GetString("START_DATE",z)
                    && bufUserPackage.GetString("START_DATE",z) <= bufUserPackage.GetString("END_DATE",i)))
                {
                    bFound = true;
                    break;
                }
            }

            if(!bFound)
            {
                //用户包(i)完全依赖于限制包(j)
                //完全依赖业务包拼串
                string strInfo = "";
                for(int y = 0; y < iCount; y++)
                {
                    if(strInfo == "")
                        strInfo = bufPackageLimit.GetString("PACKAGE_NAME_B", y);
                    else
                        strInfo += "、" + bufPackageLimit.GetString("PACKAGE_NAME_B", y);
                }

                if(strErrorInfo == "")
                    strErrorInfo = "业务包：\"" + bufUserPackage.GetString("PACKAGE_NAME", i)
                        + "\"完全依赖于业务包：\"" +  strInfo + "\"";
                else
                    strErrorInfo += "、业务包：\"" + bufUserPackage.GetString("PACKAGE_NAME", i)
                        + "\"完全依赖于业务包：\"" +  strInfo + "\"";

                break;
            }
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR, strErrorInfo + "，业务无法继续！");
}


//获取包内必选元素
int TradeCheckAfterTrade::GetPackageForceElements(const int &iPackageId,CFmlBuf &bufPackageForceElements)
{
    bufPackageForceElements.ClearFmlValue();
    StaticDAO& dao = StaticDAO::getDAO();
    int iCount = 0;

    try
    {
        dao.Clear();
        dao.SetParam(":VPACKAGE_ID", iPackageId);
        dao.SetParam(":VFORCE_TAG", "1");
        iCount = dao.jselect(bufPackageForceElements, "SEL_ELEMENTS_BY_PID_FTAG", "TD_B_PACKAGE_ELEMENT");
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取包内必选元素信息出错！[%s]",ex.getCause()->what());
    }

    /*ConvertCodeToName(bufPackageForceElements, "PackageName", "PACKAGE_ID", "PACKAGE_NAME");
    ConvertCodeToName(bufPackageForceElements, "DiscntName", "ELEMENT_ID", "DISCNT_NAME");
    ConvertCodeToName(bufPackageForceElements, "ServiceName", "ELEMENT_ID", "SERVICE_NAME");
    ConvertCodeToName(bufPackageForceElements, "ActionName", "ELEMENT_ID", "ACTION_NAME");
    ConvertCodeToName(bufPackageForceElements, "SpProductName", "ELEMENT_ID", "SP_PRODUCT_NAME");    */
    /*char cTemp[10];
    for(int i = 0; i < iCount; i++)
    {
        cTemp[0] = '\0';
        sprintf(cTemp,"%d",(int)bufPackageForceElements.GetInt("PACKAGE_ID", i));
        bufPackageForceElements.SetString("PACKAGE_NAME", CParamDAO::getParam("PackageName", cTemp),i);
        cTemp[0] = '\0';
        sprintf(cTemp,"%d",(int)bufPackageForceElements.GetInt("ELEMENT_ID", i));

        if(bufPackageForceElements.GetString("ELEMENT_TYPE_CODE", i) == "D")
        {
            bufPackageForceElements.SetString("DISCNT_NAME", CParamDAO::getParam("DiscntName", cTemp),i);
        }
        else if(bufPackageForceElements.GetString("ELEMENT_TYPE_CODE", i) == "S")
        {
            bufPackageForceElements.SetString("SERVICE_NAME", CParamDAO::getParam("ServiceName", cTemp),i);
        }
        else if(bufPackageForceElements.GetString("ELEMENT_TYPE_CODE", i) == "A" ||
            bufPackageForceElements.GetString("ELEMENT_TYPE_CODE", i) == "C")
        {
            bufPackageForceElements.SetString("ACTION_NAME", CParamDAO::getParam("ActionName", cTemp),i);
        }
        else if(bufPackageForceElements.GetString("ELEMENT_TYPE_CODE", i) == "X")
        {
            bufPackageForceElements.SetString("SP_PRODUCT_NAME", CParamDAO::getParam("SpProductName", cTemp), i);
        }
    }*/
    bufPackageForceElements.SetInt("X_RECORDNUM", iCount);
    return iCount;
}


//包内必选元素判断
void TradeCheckAfterTrade::CheckPackageForceElements(const int &iProductId, const string &strProductName,
                                                     CFmlBuf &bufUserPackageByProd, CFmlBuf &bufTradeDiscnt_AllDiscnt,
                                                     CFmlBuf &bufTradeSvc_UserAllSvc,
                                                     CFmlBuf &bufTradeSp_UserAllSp, CFmlBuf &bufTradeElement_UserAllElement,
                                                     CFmlBuf &bufNoChangePackage, CFmlBuf &bufDelPackage)
{
    CFmlBuf bufPackageForceElements;
    bool bFound = false;
    string strErrorInfo = "";

    int iCount = bufUserPackageByProd.GetInt("X_RECORDNUM");

    for(int i = 0; i < iCount; i++)
    {
        //当前业务无变化的包不作判断
        bool bExist = false;
        int iNoChange = bufNoChangePackage.GetInt("X_RECORDNUM");
        for(int g = 0; g < iNoChange; g++)
        {
            if(bufNoChangePackage.GetInt("PRODUCT_ID", g) == bufUserPackageByProd.GetInt("PRODUCT_ID", i)
                && bufNoChangePackage.GetInt("PACKAGE_ID", g) == bufUserPackageByProd.GetInt("PACKAGE_ID", i))
            {
                bExist = true;
                break;
            }
        }
        if(bExist) continue;

        //当前业务完全删除的包不作判断
        bExist = false;
        int iDelPack = bufDelPackage.GetInt("X_RECORDNUM");
        for(int g = 0; g < iDelPack; g++)
        {
            if(bufDelPackage.GetString("RSRV_STR1", g) == to_string(bufUserPackageByProd.GetInt("PRODUCT_ID", i))
                && bufDelPackage.GetString("RSRV_STR3", g) == to_string(bufUserPackageByProd.GetInt("PACKAGE_ID", i)))
            {
                bExist = true;
                break;
            }
        }
        if(bExist) continue;
        
        bFound = false;
        int iForceCount = GetPackageForceElements(bufUserPackageByProd.GetInt("PACKAGE_ID", i), bufPackageForceElements);

        for(int z = 0; z < iForceCount; z++)
        {
            //优惠
            if(bufPackageForceElements.GetString("ELEMENT_TYPE_CODE", z) == "D")
            {
                bFound = false;
                int iDiscntCount = bufTradeDiscnt_AllDiscnt.GetInt("X_RECORDNUM");
                for(int j = 0; j < iDiscntCount; j++)
                {
                    if(iProductId == bufTradeDiscnt_AllDiscnt.GetInt("PRODUCT_ID", j)
                        && bufUserPackageByProd.GetInt("PACKAGE_ID", i) == bufTradeDiscnt_AllDiscnt.GetInt("PACKAGE_ID", j)
                        && bufTradeDiscnt_AllDiscnt.GetInt("DISCNT_CODE", j) == bufPackageForceElements.GetInt("ELEMENT_ID", z))
                    {
                        bFound = true;
                        break;
                    }
                }

                if(!bFound)
                {
                    if(strErrorInfo == "")
                        strErrorInfo = "\"" + bufPackageForceElements.GetString("PACKAGE_NAME", z)
                            + " --> " + bufPackageForceElements.GetString("DISCNT_NAME", z) + "\"";
                    else
                        strErrorInfo += "、\"" + bufPackageForceElements.GetString("PACKAGE_NAME", z)
                            + " --> " + bufPackageForceElements.GetString("DISCNT_NAME", z) + "\"";
                    /*
                    if(strErrorInfo == "")
                        strErrorInfo = "业务包：\"" + bufPackageForceElements.GetString("PACKAGE_NAME", z)
                            + "\"的必选优惠：\"" + bufPackageForceElements.GetString("DISCNT_NAME", z) + "\"";
                    else
                        strErrorInfo += "、业务包：\"" + bufPackageForceElements.GetString("PACKAGE_NAME", z)
                            + "\"的必选优惠：\"" + bufPackageForceElements.GetString("DISCNT_NAME", z) + "\"";
                    */
                }

            }

            //服务
            if(bufPackageForceElements.GetString("ELEMENT_TYPE_CODE", z) == "S")
            {
                bFound = false;
                int iSvcCount = bufTradeSvc_UserAllSvc.GetInt("X_RECORDNUM");
                for(int j = 0; j < iSvcCount; j++)
                {
                    if(iProductId == bufTradeSvc_UserAllSvc.GetInt("PRODUCT_ID", j)
                        && bufUserPackageByProd.GetInt("PACKAGE_ID", i) == bufTradeSvc_UserAllSvc.GetInt("PACKAGE_ID", j)
                        && bufTradeSvc_UserAllSvc.GetInt("SERVICE_ID", j) == bufPackageForceElements.GetInt("ELEMENT_ID", z))
                    {
                        bFound = true;
                        break;
                    }
                }

                if(!bFound)
                {
                    if(strErrorInfo == "")
                        strErrorInfo = "\"" + bufPackageForceElements.GetString("PACKAGE_NAME", z)
                            + " --> " + bufPackageForceElements.GetString("SERVICE_NAME", z) + "\"";
                    else
                        strErrorInfo += "、\"" + bufPackageForceElements.GetString("PACKAGE_NAME", z)
                            + " --> " + bufPackageForceElements.GetString("SERVICE_NAME", z) + "\"";
                    /*
                    if(strErrorInfo == "")
                        strErrorInfo = "业务包：\"" + bufPackageForceElements.GetString("PACKAGE_NAME", z)
                            + "\"的必选服务：\"" + bufPackageForceElements.GetString("SERVICE_NAME", z) + "\"";
                    else
                        strErrorInfo += "、业务包：\"" + bufPackageForceElements.GetString("PACKAGE_NAME", z)
                            + "\"的必选服务：\"" + bufPackageForceElements.GetString("SERVICE_NAME", z) + "\"";
                    */
                }
            }

            //SP
            if(bufPackageForceElements.GetString("ELEMENT_TYPE_CODE", z) == "X")
            {
                bFound = false;
                int iSpCount = bufTradeSp_UserAllSp.GetInt("X_RECORDNUM");
                for(int j = 0; j < iSpCount; j++)
                {
                    if(iProductId == bufTradeSp_UserAllSp.GetInt("PRODUCT_ID", j)
                        && bufUserPackageByProd.GetInt("PACKAGE_ID", i) == bufTradeSp_UserAllSp.GetInt("PACKAGE_ID", j)
                        && bufTradeSp_UserAllSp.GetInt("SP_SERVICE_ID", j) == bufPackageForceElements.GetInt("ELEMENT_ID", z))
                    {
                        bFound = true;
                        break;
                    }
                }

                if(!bFound)
                {
                    if(strErrorInfo == "")
                        strErrorInfo = "\"" + bufPackageForceElements.GetString("PACKAGE_NAME", z)
                            + " --> " + bufPackageForceElements.GetString("SP_PRODUCT_NAME", z) + "\"";
                    else
                        strErrorInfo += "、\"" + bufPackageForceElements.GetString("PACKAGE_NAME", z)
                            + " --> " + bufPackageForceElements.GetString("SP_PRODUCT_NAME", z) + "\"";
                }
            }

            //物品类其他元素
            if(bufPackageForceElements.GetString("ELEMENT_TYPE_CODE", z) != "D" &&
                bufPackageForceElements.GetString("ELEMENT_TYPE_CODE", z) != "S" &&
                bufPackageForceElements.GetString("ELEMENT_TYPE_CODE", z) != "X")
            {
                bFound = false;
                int iElementCount = bufTradeElement_UserAllElement.GetInt("X_RECORDNUM");
                for(int j = 0; j < iElementCount; j++)
                {
                    if(iProductId == bufTradeElement_UserAllElement.GetInt("PRODUCT_ID", j)
                        && bufUserPackageByProd.GetInt("PACKAGE_ID", i) == bufTradeElement_UserAllElement.GetInt("PACKAGE_ID", j)
                        && bufTradeElement_UserAllElement.GetInt("ID", j) == bufPackageForceElements.GetInt("ELEMENT_ID", z))
                    {
                        bFound = true;
                        break;
                    }
                }

                if(!bFound)
                {
                    if(strErrorInfo == "")
                        strErrorInfo = "\"" + bufPackageForceElements.GetString("PACKAGE_NAME", z)
                            + " --> " + bufPackageForceElements.GetString("ACTION_NAME", z) + "\"";
                    else
                        strErrorInfo += "、\"" + bufPackageForceElements.GetString("PACKAGE_NAME", z)
                            + " --> " + bufPackageForceElements.GetString("ACTION_NAME", z) + "\"";
                }
            }
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR, "产品：\"" +
                    strProductName + "\"，" + strErrorInfo + "必需选择，业务无法继续！");


}


//用户元素整合
int TradeCheckAfterTrade::GeneUserAllElements(CFmlBuf &bufTradeDiscnt_AllDiscnt, CFmlBuf &bufTradeSvc_UserAllSvc,
                                              CFmlBuf &bufTradeSp_UserAllSp, CFmlBuf &bufTradeElement_UserAllElement,
                                              CFmlBuf &bufUserAllElements)
{
    int iBase = 0;
    bufUserAllElements.ClearFmlValue();

    int iDiscntCount = bufTradeDiscnt_AllDiscnt.GetInt("X_RECORDNUM");
    for(int i = 0; i < iDiscntCount; i++)
    {
        bufUserAllElements.SetString("USER_ID", bufTradeDiscnt_AllDiscnt.GetString("ID", i), i);
        bufUserAllElements.SetInt("PRODUCT_ID", bufTradeDiscnt_AllDiscnt.GetInt("PRODUCT_ID", i), i);
        bufUserAllElements.SetString("PRODUCT_NAME", bufTradeDiscnt_AllDiscnt.GetString("PRODUCT_NAME", i), i);

        bufUserAllElements.SetInt("PACKAGE_ID", bufTradeDiscnt_AllDiscnt.GetInt("PACKAGE_ID", i), i);
        bufUserAllElements.SetString("PACKAGE_NAME", bufTradeDiscnt_AllDiscnt.GetString("PACKAGE_NAME", i), i);

        bufUserAllElements.SetInt("ELEMENT_ID", bufTradeDiscnt_AllDiscnt.GetInt("DISCNT_CODE", i), i);
        bufUserAllElements.SetString("ELEMENT_NAME", bufTradeDiscnt_AllDiscnt.GetString("DISCNT_NAME", i), i);
        bufUserAllElements.SetString("ELEMENT_TYPE_CODE", "D", i);

        bufUserAllElements.SetString("START_DATE", bufTradeDiscnt_AllDiscnt.GetString("START_DATE", i), i);
        bufUserAllElements.SetString("END_DATE", bufTradeDiscnt_AllDiscnt.GetString("END_DATE", i), i);
        //以下用户判断订购依赖
        bufUserAllElements.SetString("TRADE_ID", bufTradeDiscnt_AllDiscnt.GetString("TRADE_ID", i), i);
        bufUserAllElements.SetString("MODIFY_TAG", bufTradeDiscnt_AllDiscnt.GetString("MODIFY_TAG", i), i);
    }

    iBase += iDiscntCount;
    int iSvcCount = bufTradeSvc_UserAllSvc.GetInt("X_RECORDNUM");
    for(int j = 0; j < iSvcCount; j++)
    {
        bufUserAllElements.SetString("USER_ID", bufTradeSvc_UserAllSvc.GetString("USER_ID", j), j+iBase);
        bufUserAllElements.SetInt("PRODUCT_ID", bufTradeSvc_UserAllSvc.GetInt("PRODUCT_ID", j), j+iBase);
        bufUserAllElements.SetString("PRODUCT_NAME", bufTradeSvc_UserAllSvc.GetString("PRODUCT_NAME", j), j+iBase);

        bufUserAllElements.SetInt("PACKAGE_ID", bufTradeSvc_UserAllSvc.GetInt("PACKAGE_ID", j), j+iBase);
        bufUserAllElements.SetString("PACKAGE_NAME", bufTradeSvc_UserAllSvc.GetString("PACKAGE_NAME", j), j+iBase);

        bufUserAllElements.SetInt("ELEMENT_ID", bufTradeSvc_UserAllSvc.GetInt("SERVICE_ID", j), j+iBase);
        bufUserAllElements.SetString("ELEMENT_NAME", bufTradeSvc_UserAllSvc.GetString("SERVICE_NAME", j), j+iBase);
        bufUserAllElements.SetString("ELEMENT_TYPE_CODE", "S", j+iBase);

        bufUserAllElements.SetString("START_DATE", bufTradeSvc_UserAllSvc.GetString("START_DATE", j), j+iBase);
        bufUserAllElements.SetString("END_DATE", bufTradeSvc_UserAllSvc.GetString("END_DATE", j), j+iBase);
        //以下用户判断订购依赖
        bufUserAllElements.SetString("TRADE_ID", bufTradeSvc_UserAllSvc.GetString("TRADE_ID", j), j+iBase);
        bufUserAllElements.SetString("MODIFY_TAG", bufTradeSvc_UserAllSvc.GetString("MODIFY_TAG", j), j+iBase);
    }

    iBase += iSvcCount;
    int iSPCount = bufTradeSp_UserAllSp.GetInt("X_RECORDNUM");
    for(int j = 0; j < iSPCount; j++)
    {
        bufUserAllElements.SetString("USER_ID", bufTradeSp_UserAllSp.GetString("USER_ID", j), j+iBase);
        bufUserAllElements.SetInt("PRODUCT_ID", bufTradeSp_UserAllSp.GetInt("PRODUCT_ID", j), j+iBase);
        bufUserAllElements.SetString("PRODUCT_NAME", bufTradeSp_UserAllSp.GetString("PRODUCT_NAME", j), j+iBase);

        bufUserAllElements.SetInt("PACKAGE_ID", bufTradeSp_UserAllSp.GetInt("PACKAGE_ID", j), j+iBase);
        bufUserAllElements.SetString("PACKAGE_NAME", bufTradeSp_UserAllSp.GetString("PACKAGE_NAME", j), j+iBase);

        bufUserAllElements.SetInt("ELEMENT_ID", bufTradeSp_UserAllSp.GetInt("SP_SERVICE_ID", j), j+iBase);
        bufUserAllElements.SetString("ELEMENT_NAME", bufTradeSp_UserAllSp.GetString("SP_PRODUCT_NAME", j), j+iBase);
        bufUserAllElements.SetString("ELEMENT_TYPE_CODE", "X", j+iBase);

        bufUserAllElements.SetString("START_DATE", bufTradeSp_UserAllSp.GetString("START_DATE", j), j+iBase);
        bufUserAllElements.SetString("END_DATE", bufTradeSp_UserAllSp.GetString("END_DATE", j), j+iBase);
        //以下用户判断订购依赖
        bufUserAllElements.SetString("TRADE_ID", bufTradeSp_UserAllSp.GetString("TRADE_ID", j), j+iBase);
        bufUserAllElements.SetString("MODIFY_TAG", bufTradeSp_UserAllSp.GetString("MODIFY_TAG", j), j+iBase);
    }

    iBase += iSPCount;
    int iElementCount = bufTradeElement_UserAllElement.GetInt("X_RECORDNUM");
    for(int j = 0; j < iElementCount; j++)
    {
        bufUserAllElements.SetString("USER_ID", bufTradeElement_UserAllElement.GetString("USER_ID", j), j+iBase);
        bufUserAllElements.SetInt("PRODUCT_ID", bufTradeElement_UserAllElement.GetInt("PRODUCT_ID", j), j+iBase);
        bufUserAllElements.SetString("PRODUCT_NAME", bufTradeElement_UserAllElement.GetString("PRODUCT_NAME", j), j+iBase);

        bufUserAllElements.SetInt("PACKAGE_ID", bufTradeElement_UserAllElement.GetInt("PACKAGE_ID", j), j+iBase);
        bufUserAllElements.SetString("PACKAGE_NAME", bufTradeElement_UserAllElement.GetString("PACKAGE_NAME", j), j+iBase);

        bufUserAllElements.SetInt("ELEMENT_ID", bufTradeElement_UserAllElement.GetInt("ID", j), j+iBase);
        if(bufTradeElement_UserAllElement.GetString("ID_TYPE", j)=="A" || bufTradeElement_UserAllElement.GetString("ID_TYPE", j)=="C")
            bufUserAllElements.SetString("ELEMENT_NAME", bufTradeElement_UserAllElement.GetString("ACTION_NAME", j), j+iBase);
        else
            bufUserAllElements.SetString("ELEMENT_NAME", bufTradeElement_UserAllElement.GetString("PARENT_ACTION_NAME", j), j+iBase);

        bufUserAllElements.SetString("ELEMENT_TYPE_CODE", bufTradeElement_UserAllElement.GetString("ID_TYPE", j), j+iBase);

        bufUserAllElements.SetString("START_DATE", bufTradeElement_UserAllElement.GetString("START_DATE", j), j+iBase);
        bufUserAllElements.SetString("END_DATE", bufTradeElement_UserAllElement.GetString("END_DATE", j), j+iBase);
        //以下用户判断订购依赖
        bufUserAllElements.SetString("TRADE_ID", bufTradeElement_UserAllElement.GetString("TRADE_ID", j), j+iBase);
        bufUserAllElements.SetString("MODIFY_TAG", bufTradeElement_UserAllElement.GetString("MODIFY_TAG", j), j+iBase);
    }

    bufUserAllElements.SetInt("X_RECORDNUM", iDiscntCount+iSvcCount+iSPCount+iElementCount);

    return iDiscntCount+iSvcCount+iSPCount+iElementCount;
}


//获取全局元素之间关系
int TradeCheckAfterTrade::ExistsElementLimitA(const string &strElementTypeA, const int &iElementIdA,
                                              const string &strLimitTag, const string strEparchyCode,
                                              CFmlBuf &bufElementLimit)
{
    bufElementLimit.ClearFmlValue();
    StaticDAO& dao = StaticDAO::getDAO();
    int iCount = 0;
    LOG_TRACE(logTradeCheckAfterTrade, "进入ExistsElementLimitA函数");
    try
    {
        dao.Clear();
        dao.SetParam(":VELEMENT_TYPE_CODE_A", strElementTypeA);
        dao.SetParam(":VELEMENT_ID_A", iElementIdA);
        dao.SetParam(":VLIMIT_TAG", strLimitTag);
        dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
        iCount = dao.jselect(bufElementLimit, "JUDGE_A_LIMIT_EXISTS", "TD_B_ELEMENT_LIMIT");
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取包内元素间关系信息出错！[%s]",ex.getCause()->what());
    }

    /*ConvertCodeToName(bufElementLimit, "DiscntName", "ELEMENT_ID_B", "DISCNT_NAME");
    ConvertCodeToName(bufElementLimit, "ServiceName", "ELEMENT_ID_B", "SERVICE_NAME");
    ConvertCodeToName(bufElementLimit, "ActionName", "ELEMENT_ID_B", "ACTION_NAME");
    ConvertCodeToName(bufElementLimit, "SpProductName", "ELEMENT_ID_B", "SP_PRODUCT_NAME");    */
    char cTemp[10];
    for(int i = 0; i < iCount; i++)
    {
        cTemp[0] = '\0';
        sprintf(cTemp,"%d",(int)bufElementLimit.GetInt("ELEMENT_ID_B", i));
        if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", i) == "D")
        {
            bufElementLimit.SetString("DISCNT_NAME", CParamDAO::getParam("DiscntName", cTemp), i);
        }
        else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", i) == "S")
        {
            bufElementLimit.SetString("SERVICE_NAME", CParamDAO::getParam("ServiceName", cTemp), i);
        }
        else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", i) == "A"
            || bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", i) == "C")
        {
            bufElementLimit.SetString("ACTION_NAME", CParamDAO::getParam("ActionName", cTemp), i);
        }
        else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", i) == "X")
        {
            bufElementLimit.SetString("SP_PRODUCT_NAME", CParamDAO::getParam("SpProductName", cTemp), i);
        }
    }

    bufElementLimit.SetInt("X_RECORDNUM", iCount);
    return iCount;
}

/**
 *获取业务办理取消元素
 *
 */
int TradeCheckAfterTrade::GetUserTradeDelElements(const string &strTradeId,CFmlBuf &bufTradeDelElement)
{
    bufTradeDelElement.ClearFmlValue();
    StaticDAO& dao = StaticDAO::getDAO();
    int iCount = 0;

    try
    {
        dao.Clear();
        dao.SetParam(":VTRADE_ID", strTradeId);
        dao.SetParam(":VACCEPT_MONTH", strTradeId.substr(4,2));
        iCount = dao.jselect(bufTradeDelElement, "SEL_DEL_TRADESUB","TF_B_TRADE");
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取业务办理取消元素出错！[%s]",ex.getCause()->what());
    }

    /*if(iCount > 0)
    {
        ConvertCodeToName(bufTradeDelElement, "ProductName", "PRODUCT_ID", "PRODUCT_NAME");
        ConvertCodeToName(bufTradeDelElement, "PackageName", "PACKAGE_ID", "PACKAGE_NAME");
    }*/

    bufTradeDelElement.SetInt("X_RECORDNUM", iCount);

    return iCount;
}

//全局元素之间限制关系判断
void TradeCheckAfterTrade::CheckElementLimit(CFmlBuf &bufUserAllElements, const string &strEparchyCode,const string strTradeId)
{
    CFmlBuf bufElementLimit;
    string strErrorInfo = "";
    int iCount = 0;
    bool bFound = false;
    int i,j,z;
    

    string strLastDate="";
    DualMgr objDualMgr;
    objDualMgr.GetLastDayOfCurMonth(strLastDate);    

    //按用户元素循环校验元素互斥关系
    int iUserElements = bufUserAllElements.GetInt("X_RECORDNUM");
    for(i = 0; i < iUserElements; i++)
    {
        iCount = 0;
        bufElementLimit.ClearFmlValue();
        
        /*
		    add by lirui@20090505	
		    月底终止的优惠判断问题(终止至月底的优惠，不参与互斥，部份依赖，完全依赖，重复元素的判断，即终止至月底的优惠
		    在判断时可以认为存在，也可以认为是不存在的元素)
		    */
		    if( bufUserAllElements.GetString("END_DATE",i)<=strLastDate&&bufUserAllElements.GetString("ELEMENT_TYPE_CODE", i)=="D")
			      continue;        

        //获取元素互斥信息
        iCount = ExistsElementLimitA(bufUserAllElements.GetString("ELEMENT_TYPE_CODE", i),
                                     bufUserAllElements.GetInt("ELEMENT_ID", i),
                                     "0", strEparchyCode, bufElementLimit);

        for(j = 0; j < iCount; j++)
        {
            bFound = false;
            for(z = 0; z < iUserElements; z++)
            {
                if (i==z)continue; //自身不做互斥判断 added by tangz@2009-2-7 11:33
                if(bufUserAllElements.GetInt("ELEMENT_ID", z) == bufElementLimit.GetInt("ELEMENT_ID_B", j)
                    && bufUserAllElements.GetString("ELEMENT_TYPE_CODE", z) == bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", j)
                    //时间交集
                    && (bufUserAllElements.GetString("START_DATE",z) <= bufUserAllElements.GetString("START_DATE",i)
                    && bufUserAllElements.GetString("START_DATE",i) <= bufUserAllElements.GetString("END_DATE",z)
                    ||
                    bufUserAllElements.GetString("START_DATE",i) <= bufUserAllElements.GetString("START_DATE",z)
                    && bufUserAllElements.GetString("START_DATE",z) <= bufUserAllElements.GetString("END_DATE",i)))
                {
                    bFound = true;
                    break;
                }
            }

            if(bFound)
            {
                if(strErrorInfo == "")
                    strErrorInfo = "\"" + bufUserAllElements.GetString("PRODUCT_NAME", i)
                        + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", i)
                        + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                        + "\"与\"" +  bufUserAllElements.GetString("PRODUCT_NAME", z)
                        + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", z)
                        + " --> " +  bufUserAllElements.GetString("ELEMENT_NAME", z) + "\"";
                else
                    strErrorInfo += "、\"" + bufUserAllElements.GetString("PRODUCT_NAME", i)
                        + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", i)
                        + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                        + "\"与\"" +  bufUserAllElements.GetString("PRODUCT_NAME", z)
                        + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", z)
                        + " --> " +  bufUserAllElements.GetString("ELEMENT_NAME", z) + "\"";
            }
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strErrorInfo + "互斥，业务无法继续！");

    //按用户元素循环校验元素部份依赖关系
    strErrorInfo = "";
    for(i = 0; i < iUserElements; i++)
    {
        iCount = 0;
        bufElementLimit.ClearFmlValue();

        /*
		    add by lirui@20090505	
		    月底终止的优惠判断问题(终止至月底的优惠，不参与互斥，部份依赖，完全依赖，重复元素的判断，即终止至月底的优惠
		    在判断时可以认为存在，也可以认为是不存在的元素)
		    */
		    if( bufUserAllElements.GetString("END_DATE",i)<=strLastDate&&bufUserAllElements.GetString("ELEMENT_TYPE_CODE", i)=="D")
			      continue;

        //获取元素部份依赖信息
        iCount = ExistsElementLimitA(bufUserAllElements.GetString("ELEMENT_TYPE_CODE", i),
                                     bufUserAllElements.GetInt("ELEMENT_ID", i),
                                     "1", strEparchyCode, bufElementLimit);
        bFound = true;
        for(j = 0; j < iCount; j++)
        {
            bFound = false;
            for(z = 0; z < iUserElements; z++)
            {
                if(bufUserAllElements.GetInt("ELEMENT_ID", z) == bufElementLimit.GetInt("ELEMENT_ID_B", j)
                    && bufUserAllElements.GetString("ELEMENT_TYPE_CODE", z) == bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", j)
                    //时间交集
                    && (bufUserAllElements.GetString("START_DATE",z) <= bufUserAllElements.GetString("START_DATE",i)
                    && bufUserAllElements.GetString("START_DATE",i) <= bufUserAllElements.GetString("END_DATE",z)
                    ||
                    bufUserAllElements.GetString("START_DATE",i) <= bufUserAllElements.GetString("START_DATE",z)
                    && bufUserAllElements.GetString("START_DATE",z) <= bufUserAllElements.GetString("END_DATE",i)))
                {
                    bFound = true;
                    break;
                }
            }

            if(bFound)
            {
                break;
            }
        }

        if(!bFound)
        {
            //(i)部份依赖于(j)
            //部份依赖元素拼串
            string strInfo = "";
            for(int y = 0; y < iCount; y++)
            {
                if(strInfo == "")
                {
                    if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "D")
                        strInfo = bufElementLimit.GetString("DISCNT_NAME", y);
                    else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "S")
                        strInfo = bufElementLimit.GetString("SERVICE_NAME", y);
                    else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "X")
                        strInfo = bufElementLimit.GetString("SP_PRODUCT_NAME", y);
                    else
                        strInfo = bufElementLimit.GetString("ACTION_NAME", y);
                }
                else
                {
                    if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "D")
                        strInfo += "、" + bufElementLimit.GetString("DISCNT_NAME", y);
                    else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "S")
                        strInfo += "、" + bufElementLimit.GetString("SERVICE_NAME", y);
                    else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "X")
                        strInfo += "、" + bufElementLimit.GetString("SP_PRODUCT_NAME", y);
                    else
                        strInfo += "、" + bufElementLimit.GetString("ACTION_NAME", y);
                }
            }

            if(strErrorInfo == "")
                strErrorInfo = "\"" + bufUserAllElements.GetString("PRODUCT_NAME", i)
                    + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", i)
                    + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                    + "\"部份依赖于元素：\"" +  strInfo + "\"";
            else
                strErrorInfo += "、\"" + bufUserAllElements.GetString("PRODUCT_NAME", i)
                    + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", i)
                    + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                    + "\"部份依赖于元素：\"" +  strInfo + "\"";
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strErrorInfo + "，业务无法继续！");


    //按用户元素循环校验包内元素完全依赖关系
    strErrorInfo = "";
    for(i = 0; i < iUserElements; i++)
    {
        iCount = 0;
        bufElementLimit.ClearFmlValue();

        /*
		    add by lirui@20090505	
		    月底终止的优惠判断问题(终止至月底的优惠，不参与互斥，部份依赖，完全依赖，重复元素的判断，即终止至月底的优惠
		    在判断时可以认为存在，也可以认为是不存在的元素)
		    */
		    if( bufUserAllElements.GetString("END_DATE",i)<=strLastDate&&bufUserAllElements.GetString("ELEMENT_TYPE_CODE", i)=="D")
			      continue;

        //获取元素完全依赖信息
        iCount = ExistsElementLimitA(bufUserAllElements.GetString("ELEMENT_TYPE_CODE", i),
                                     bufUserAllElements.GetInt("ELEMENT_ID", i),
                                     "2", strEparchyCode, bufElementLimit);

        for(j = 0; j < iCount; j++)
        {
            bFound = false;
            for(z = 0; z < iUserElements; z++)
            {
                if(bufUserAllElements.GetInt("ELEMENT_ID", z) == bufElementLimit.GetInt("ELEMENT_ID_B", j)
                    && bufUserAllElements.GetString("ELEMENT_TYPE_CODE", z) == bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", j)
                    //时间交集
                    && (bufUserAllElements.GetString("START_DATE",z) <= bufUserAllElements.GetString("START_DATE",i)
                    && bufUserAllElements.GetString("START_DATE",i) <= bufUserAllElements.GetString("END_DATE",z)
                    ||
                    bufUserAllElements.GetString("START_DATE",i) <= bufUserAllElements.GetString("START_DATE",z)
                    && bufUserAllElements.GetString("START_DATE",z) <= bufUserAllElements.GetString("END_DATE",i)))
                {
                    bFound = true;
                    break;
                }
            }

            if(!bFound)
            {
                //(i)完全依赖于(j)
                //完全依赖元素拼串
                string strInfo = "";
                for(int y = 0; y < iCount; y++)
                {
                    if(strInfo == "")
                    {
                        if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "D")
                            strInfo = bufElementLimit.GetString("DISCNT_NAME", y);
                        else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "S")
                            strInfo = bufElementLimit.GetString("SERVICE_NAME", y);
                        else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "X")
                            strInfo = bufElementLimit.GetString("SP_PRODUCT_NAME", y);
                        else
                            strInfo = bufElementLimit.GetString("ACTION_NAME", y);
                    }
                    else
                    {
                        if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "D")
                            strInfo += "、" + bufElementLimit.GetString("DISCNT_NAME", y);
                        else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "S")
                            strInfo += "、" + bufElementLimit.GetString("SERVICE_NAME", y);
                        else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "X")
                            strInfo += "、" + bufElementLimit.GetString("SP_PRODUCT_NAME", y);
                        else
                            strInfo += "、" + bufElementLimit.GetString("ACTION_NAME", y);
                    }
                }

                if(strErrorInfo == "")
                    strErrorInfo = "\"" + bufUserAllElements.GetString("PRODUCT_NAME", i)
                        + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", i)
                        + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                        + "\"完全依赖于元素：\"" +  strInfo + "\"";
                else
                    strErrorInfo += "、\"" + bufUserAllElements.GetString("PRODUCT_NAME", i)
                        + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", i)
                        + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                        + "\"完全依赖于元素：\"" +  strInfo + "\"";

                break;
            }
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strErrorInfo + "，业务无法继续！");

    //按用户元素循环校验包内元素订购依赖关系
    strErrorInfo = "";
    for(i = 0; i < iUserElements&&bufUserAllElements.GetString("TRADE_ID", i)!="0"
         &&(bufUserAllElements.GetString("MODIFY_TAG", i)=="A"||bufUserAllElements.GetString("MODIFY_TAG", i)=="0"); i++)//只检查本次订购的元素
    {
        iCount = 0;
        bufElementLimit.ClearFmlValue();

        //获取元素订购依赖信息
        iCount = ExistsElementLimitA(bufUserAllElements.GetString("ELEMENT_TYPE_CODE", i),
                                     bufUserAllElements.GetInt("ELEMENT_ID", i),
                                     "A", strEparchyCode, bufElementLimit);
        for(j = 0; j < iCount; j++)
        {
            bFound = false;
            for(z = 0; z < iUserElements; z++)
            {
                if(bufUserAllElements.GetInt("ELEMENT_ID", z) == bufElementLimit.GetInt("ELEMENT_ID_B", j)
                    && bufUserAllElements.GetString("ELEMENT_TYPE_CODE", z) == bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", j)
                    //时间交集
                    && (bufUserAllElements.GetString("START_DATE",z) <= bufUserAllElements.GetString("START_DATE",i)
                    && bufUserAllElements.GetString("START_DATE",i) <= bufUserAllElements.GetString("END_DATE",z)
                    ||
                    bufUserAllElements.GetString("START_DATE",i) <= bufUserAllElements.GetString("START_DATE",z)
                    && bufUserAllElements.GetString("START_DATE",z) <= bufUserAllElements.GetString("END_DATE",i)))
                {
                    bFound = true;
                    break;
                }
            }

            if(!bFound)
            {
                //(j)依赖于(i)
                //依赖元素拼串
                string strInfo = "";
                for(int y = 0; y < iCount; y++)
                {
                    if(strInfo == "")
                    {
                        if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "D")
                            strInfo = bufElementLimit.GetString("DISCNT_NAME", y);
                        else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "S")
                            strInfo = bufElementLimit.GetString("SERVICE_NAME", y);
                        else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "X")
                            strInfo = bufElementLimit.GetString("SP_PRODUCT_NAME", y);
                        else
                            strInfo = bufElementLimit.GetString("ACTION_NAME", y);
                    }
                    else
                    {
                        if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "D")
                            strInfo += "、" + bufElementLimit.GetString("DISCNT_NAME", y);
                        else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "S")
                            strInfo += "、" + bufElementLimit.GetString("SERVICE_NAME", y);
                        else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "X")
                            strInfo += "、" + bufElementLimit.GetString("SP_PRODUCT_NAME", y);
                        else
                            strInfo += "、" + bufElementLimit.GetString("ACTION_NAME", y);
                    }
                }

                if(strErrorInfo == "")
                    strErrorInfo = "\"" + bufUserAllElements.GetString("PRODUCT_NAME", i)
                        + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", i)
                        + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                        + "\"依赖于元素：\"" +  strInfo + "\"";
                else
                    strErrorInfo += "、\"" + bufUserAllElements.GetString("PRODUCT_NAME", i)
                        + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", i)
                        + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                        + "\"依赖于元素：\"" +  strInfo + "\"";

                break;
            }
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strErrorInfo + "，业务无法继续3！");

    //按用户元素循环校验包内元素取消订购依赖关系
    //有产品变更才判断此限制
    StaticDAO& dao = StaticDAO::getDAO();
    CFmlBuf bufTradeP;
    dao.SetParam(":VTRADE_ID",strTradeId);
    if(dao.jselect(bufTradeP,"SEL_BY_PK","TF_B_TRADE_PRODUCT")==0)
    {
        CFmlBuf bufTradeDelElement;
        int iDelEleCnt = GetUserTradeDelElements(strTradeId,bufTradeDelElement);
        strErrorInfo = "";
        for(i = 0; i < iDelEleCnt; i++)
        {
            iCount = 0;
            bufElementLimit.ClearFmlValue();

            //获取元素订购依赖信息
            iCount = ExistsElementLimitA(bufTradeDelElement.GetString("ELEMENT_TYPE_CODE", i),
                                         bufTradeDelElement.GetInt("ELEMENT_ID", i),
                                         "B", strEparchyCode, bufElementLimit);

            for(j = 0; j < iCount; j++)
            {
                bFound = false;
                for(z = 0; z < iUserElements; z++)
                {
                    if(bufUserAllElements.GetInt("ELEMENT_ID", z) == bufElementLimit.GetInt("ELEMENT_ID_B", j)
                        && bufUserAllElements.GetString("ELEMENT_TYPE_CODE", z) == bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", j)
                        //B的结束时间,后与A的结束时间
                        && bufUserAllElements.GetString("END_DATE",z) > bufTradeDelElement.GetString("END_DATE",i))
                    {
                        bFound = true;
                        break;
                    }
                }

                if(bFound)
                {
                    if(strErrorInfo == "")
                        strErrorInfo = "\"" + bufUserAllElements.GetString("PRODUCT_NAME", z)
                            + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", z)
                            + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", z)
                            + "\"依赖于元素：\"" +  bufTradeDelElement.GetString("PRODUCT_NAME", i)
                            + " --> " + bufTradeDelElement.GetString("PACKAGE_NAME", i)
                            + " --> " +  bufTradeDelElement.GetString("ELEMENT_NAME", i) + "\"";
                    else
                        strErrorInfo += "、\"" + bufUserAllElements.GetString("PRODUCT_NAME", z)
                            + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", z)
                            + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", z)
                            + "\"依赖于元素：\"" +  bufTradeDelElement.GetString("PRODUCT_NAME", i)
                            + " --> " + bufTradeDelElement.GetString("PACKAGE_NAME", i)
                            + " --> " +  bufTradeDelElement.GetString("ELEMENT_NAME", i) + "\"";
                }
            }
        }

        if(strErrorInfo != "")
            THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strErrorInfo + "，业务无法继续1！");
    }
}


//获取包内元素之间关系
int TradeCheckAfterTrade::ExistsPackageElementLimitA(const int &iPackageId, const string &strElementTypeA,
                                                     const int &iElementIdA, const string &strLimitTag,
                                                     const string strEparchyCode, CFmlBuf &bufElementLimitByPackage)
{
    bufElementLimitByPackage.ClearFmlValue();
    StaticDAO& dao = StaticDAO::getDAO();
    int iCount = 0;

    try
    {
        dao.Clear();
        dao.SetParam(":VPACKAGE_ID", iPackageId);
        dao.SetParam(":VELEMENT_TYPE_CODE_A", strElementTypeA);
        dao.SetParam(":VELEMENT_ID_A", iElementIdA);
        dao.SetParam(":VLIMIT_TAG", strLimitTag);
        dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
        iCount = dao.jselect(bufElementLimitByPackage, "JUDGE_A_LIMIT_EXISTS", "TD_B_PACKAGE_ELEMENT_LIMIT");
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取包内元素间关系信息出错！[%s]",ex.getCause()->what());
    }

    /*ConvertCodeToName(bufElementLimitByPackage, "PackageName", "PACKAGE_ID", "PACKAGE_NAME");
    ConvertCodeToName(bufElementLimitByPackage, "DiscntName", "ELEMENT_ID_B", "DISCNT_NAME");
    ConvertCodeToName(bufElementLimitByPackage, "ServiceName", "ELEMENT_ID_B", "SERVICE_NAME");
    ConvertCodeToName(bufElementLimitByPackage, "ActionName", "ELEMENT_ID_B", "ACTION_NAME");
    ConvertCodeToName(bufElementLimitByPackage, "ParentActionName", "ELEMENT_ID_B", "PARENT_ACTION_NAME");
    ConvertCodeToName(bufElementLimitByPackage, "SpProductName", "ELEMENT_ID_B", "SP_PRODUCT_NAME");    */
    /*char cTemp[10];
    for(int i = 0; i < iCount; i++)
    {
        cTemp[0] = '\0';
        sprintf(cTemp,"%d",(int)bufElementLimitByPackage.GetInt("PACKAGE_ID", i));
        bufElementLimitByPackage.SetString("PACKAGE_NAME", CParamDAO::getParam("PackageName", cTemp), i);
        cTemp[0] = '\0';
        sprintf(cTemp,"%d",(int)bufElementLimitByPackage.GetInt("ELEMENT_ID_B", i));
        if(bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", i) == "D")
        {
            bufElementLimitByPackage.SetString("DISCNT_NAME", CParamDAO::getParam("DiscntName", cTemp), i);
        }

        else if(bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", i) == "S")
        {
            bufElementLimitByPackage.SetString("SERVICE_NAME", CParamDAO::getParam("ServiceName", cTemp), i);
        }
        else if(bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", i) == "A" ||
            bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", i) == "C")
        {
            bufElementLimitByPackage.SetString("ACTION_NAME", CParamDAO::getParam("ActionName", cTemp), i);
        }
        else if(bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", i) == "X")
        {
            bufElementLimitByPackage.SetString("SP_PRODUCT_NAME", CParamDAO::getParam("SpProductName", cTemp), i);
        }
        else
        {
            bufElementLimitByPackage.SetString("PARENT_ACTION_NAME", CParamDAO::getParam("ParentActionName", cTemp), i);
        }
    }*/
    bufElementLimitByPackage.SetInt("X_RECORDNUM", iCount);
    return iCount;
}


//包内元素之间限制关系判断
void TradeCheckAfterTrade::CheckPackageElementLimit(CFmlBuf &bufUserAllElements, const string &strEparchyCode,
                                                    CFmlBuf &bufNoChangePackage, CFmlBuf &bufDelPackage)
{
    CFmlBuf bufElementLimitByPackage;
    string strErrorInfo = "";
    int iCount = 0;
    bool bFound = false;
    int i,j,z;


    string strLastDate="";
    DualMgr objDualMgr;
    objDualMgr.GetLastDayOfCurMonth(strLastDate);


    //按用户元素循环校验包内元素互斥关系
    int iUserElements = bufUserAllElements.GetInt("X_RECORDNUM");

    for(i = 0; i < iUserElements; i++)
    {
        //当前业务无变化的包不作判断
        bool bExist = false;
        int iNoChange = bufNoChangePackage.GetInt("X_RECORDNUM");
        for(int g = 0; g < iNoChange; g++)
        {
            if(bufNoChangePackage.GetInt("PRODUCT_ID", g) == bufUserAllElements.GetInt("PRODUCT_ID", i)
                && bufNoChangePackage.GetInt("PACKAGE_ID", g) == bufUserAllElements.GetInt("PACKAGE_ID", i))
            {
                bExist = true;
                break;
            }
        }
        if(bExist) continue;

        //当前业务完全删除的包不作判断
        bExist = false;
        int iDelPack = bufDelPackage.GetInt("X_RECORDNUM");
        for(int g = 0; g < iDelPack; g++)
        {
            if(bufDelPackage.GetString("RSRV_STR1", g) == to_string(bufUserAllElements.GetInt("PRODUCT_ID", i))
                && bufDelPackage.GetString("RSRV_STR3", g) == to_string(bufUserAllElements.GetInt("PACKAGE_ID", i)))
            {
                bExist = true;
                break;
            }
        }
        if(bExist) continue;

        /*
		    add by lirui@20090505	
		    月底终止的优惠判断问题(终止至月底的优惠，不参与互斥，部份依赖，完全依赖，重复元素的判断，即终止至月底的优惠
		    在判断时可以认为存在，也可以认为是不存在的元素)
		    */
		    if( bufUserAllElements.GetString("END_DATE",i)<=strLastDate&&bufUserAllElements.GetString("ELEMENT_TYPE_CODE", i)=="D")
			      continue;

        iCount = 0;
        bufElementLimitByPackage.ClearFmlValue();

        //获取元素互斥信息
        iCount = ExistsPackageElementLimitA(bufUserAllElements.GetInt("PACKAGE_ID", i),
                                            bufUserAllElements.GetString("ELEMENT_TYPE_CODE", i),
                                            bufUserAllElements.GetInt("ELEMENT_ID", i),
                                            "0", strEparchyCode, bufElementLimitByPackage);

        for(j = 0; j < iCount; j++)
        {
            bFound = false;
            for(z = 0; z < iUserElements; z++)
            {
                if (i==z) continue; //元素自身不做互斥判断 added by tangz@2009-2-7 11:33
                if(bufUserAllElements.GetInt("PACKAGE_ID", z) == bufElementLimitByPackage.GetInt("PACKAGE_ID", j)
                    && bufUserAllElements.GetInt("ELEMENT_ID", z) == bufElementLimitByPackage.GetInt("ELEMENT_ID_B", j)
                    && bufUserAllElements.GetString("ELEMENT_TYPE_CODE", z) == bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", j)
                    //时间交集
                    && (bufUserAllElements.GetString("START_DATE",z) <= bufUserAllElements.GetString("START_DATE",i)
                    && bufUserAllElements.GetString("START_DATE",i) <= bufUserAllElements.GetString("END_DATE",z)
                    ||
                    bufUserAllElements.GetString("START_DATE",i) <= bufUserAllElements.GetString("START_DATE",z)
                    && bufUserAllElements.GetString("START_DATE",z) <= bufUserAllElements.GetString("END_DATE",i)))
                {
                    bFound = true;
                    break;
                }
            }

            if(bFound)
            {
                //(i)与(z)互斥
                if(strErrorInfo == "")
                    strErrorInfo = "\"" + bufUserAllElements.GetString("PACKAGE_NAME", i)
                        + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                        + "\"与\"" + bufUserAllElements.GetString("PACKAGE_NAME", z)
                        + " --> " +  bufUserAllElements.GetString("ELEMENT_NAME", z) + "\"";
                else
                    strErrorInfo += "、\"" + bufUserAllElements.GetString("PACKAGE_NAME", i)
                        + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                        + "\"与\"" + bufUserAllElements.GetString("PACKAGE_NAME", z)
                        + " -->" +  bufUserAllElements.GetString("ELEMENT_NAME", z) + "\"";
            }
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strErrorInfo + "互斥，业务无法继续！");


    //按用户元素循环校验包内元素部份依赖关系
    strErrorInfo = "";
    for(i = 0; i < iUserElements; i++)
    {
        iCount = 0;
        bufElementLimitByPackage.ClearFmlValue();
        
        /*
		    add by lirui@20090505	
		    月底终止的优惠判断问题(终止至月底的优惠，不参与互斥，部份依赖，完全依赖，重复元素的判断，即终止至月底的优惠
		    在判断时可以认为存在，也可以认为是不存在的元素)
		    */
		    if( bufUserAllElements.GetString("END_DATE",i)<=strLastDate&&bufUserAllElements.GetString("ELEMENT_TYPE_CODE", i)=="D")
			      continue;        

        //获取元素部份依赖信息
        iCount = ExistsPackageElementLimitA(bufUserAllElements.GetInt("PACKAGE_ID", i),
                                            bufUserAllElements.GetString("ELEMENT_TYPE_CODE", i),
                                            bufUserAllElements.GetInt("ELEMENT_ID", i),
                                            "1", strEparchyCode, bufElementLimitByPackage);
        bFound = true;
        for(j = 0; j < iCount; j++)
        {
            bFound = false;
            for(z = 0; z < iUserElements; z++)
            {
                if(bufUserAllElements.GetInt("PACKAGE_ID", z) == bufElementLimitByPackage.GetInt("PACKAGE_ID", j)
                    && bufUserAllElements.GetInt("ELEMENT_ID", z) == bufElementLimitByPackage.GetInt("ELEMENT_ID_B", j)
                    && bufUserAllElements.GetString("ELEMENT_TYPE_CODE", z) == bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", j)
                    //时间交集
                    && (bufUserAllElements.GetString("START_DATE",z) <= bufUserAllElements.GetString("START_DATE",i)
                    && bufUserAllElements.GetString("START_DATE",i) <= bufUserAllElements.GetString("END_DATE",z)
                    ||
                    bufUserAllElements.GetString("START_DATE",i) <= bufUserAllElements.GetString("START_DATE",z)
                    && bufUserAllElements.GetString("START_DATE",z) <= bufUserAllElements.GetString("END_DATE",i)))
                {
                    bFound = true;
                    break;
                }
            }

            if(bFound)
            {
                break;
            }
        }

        if(!bFound)
        {
            //(i)部份依赖于(j)
            //部份依赖元素拼串
            string strInfo = "";
            for(int y = 0; y < iCount; y++)
            {
                if(strInfo == "")
                {
                    if(bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", y) == "D")
                        strInfo = bufElementLimitByPackage.GetString("DISCNT_NAME", y);
                    else if(bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", y) == "S")
                        strInfo = bufElementLimitByPackage.GetString("SERVICE_NAME", y);
                    else if(bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", y) == "X")
                        strInfo = bufElementLimitByPackage.GetString("SP_PRODUCT_NAME", y);
                    else if(bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", y) == "A")
                        strInfo = bufElementLimitByPackage.GetString("ACTION_NAME", y);
                    else if(bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", y) == "K")
                        strInfo = bufElementLimitByPackage.GetString("PARENT_ACTION_NAME", y);
                }
                else
                {
                    if(bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", y) == "D")
                        strInfo += "、" + bufElementLimitByPackage.GetString("DISCNT_NAME", y);
                    else if(bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", y) == "S")
                        strInfo += "、" + bufElementLimitByPackage.GetString("SERVICE_NAME", y);
                    else if(bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", y) == "X")
                        strInfo += "、" + bufElementLimitByPackage.GetString("SP_PRODUCT_NAME", y);
                    else if(bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", y) == "A")
                        strInfo += "、" + bufElementLimitByPackage.GetString("ACTION_NAME", y);
                    else if(bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", y) == "K")
                        strInfo = bufElementLimitByPackage.GetString("PARENT_ACTION_NAME", y);
                }
            }

            if(strErrorInfo == "")
                strErrorInfo = "\"" + bufUserAllElements.GetString("PACKAGE_NAME", i)
                    + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                    + "\"部份依赖于：\"" + bufElementLimitByPackage.GetString("PACKAGE_NAME")
                    + " --> " +  strInfo + "\"";
            else
                strErrorInfo += "、\"" + bufUserAllElements.GetString("PACKAGE_NAME", i)
                    + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                    + "\"部份依赖于：\"" + bufElementLimitByPackage.GetString("PACKAGE_NAME")
                    + " --> " +  strInfo + "\"";
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strErrorInfo + "，业务无法继续！");


    //按用户元素循环校验包内元素完全依赖关系
    strErrorInfo = "";
    for(i = 0; i < iUserElements; i++)
    {
        iCount = 0;
        bufElementLimitByPackage.ClearFmlValue();
        
        /*
		    add by lirui@20090505	
		    月底终止的优惠判断问题(终止至月底的优惠，不参与互斥，部份依赖，完全依赖，重复元素的判断，即终止至月底的优惠
		    在判断时可以认为存在，也可以认为是不存在的元素)
		    */
		    if( bufUserAllElements.GetString("END_DATE",i)<=strLastDate&&bufUserAllElements.GetString("ELEMENT_TYPE_CODE", i)=="D")
			      continue;        

        //获取元素完全依赖信息
        iCount = ExistsPackageElementLimitA(bufUserAllElements.GetInt("PACKAGE_ID", i),
                                            bufUserAllElements.GetString("ELEMENT_TYPE_CODE", i),
                                            bufUserAllElements.GetInt("ELEMENT_ID", i),
                                            "2", strEparchyCode, bufElementLimitByPackage);

        for(j = 0; j < iCount; j++)
        {
            bFound = false;
            for(z = 0; z < iUserElements; z++)
            {
                if(bufUserAllElements.GetInt("PACKAGE_ID", z) == bufElementLimitByPackage.GetInt("PACKAGE_ID", j)
                    && bufUserAllElements.GetInt("ELEMENT_ID", z) == bufElementLimitByPackage.GetInt("ELEMENT_ID_B", j)
                    && bufUserAllElements.GetString("ELEMENT_TYPE_CODE", z) == bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", j)
                    //时间交集
                    && (bufUserAllElements.GetString("START_DATE",z) <= bufUserAllElements.GetString("START_DATE",i)
                    && bufUserAllElements.GetString("START_DATE",i) <= bufUserAllElements.GetString("END_DATE",z)
                    ||
                    bufUserAllElements.GetString("START_DATE",i) <= bufUserAllElements.GetString("START_DATE",z)
                    && bufUserAllElements.GetString("START_DATE",z) <= bufUserAllElements.GetString("END_DATE",i)))
                {
                    bFound = true;
                    break;
                }
            }

            if(!bFound)
            {
                //(i)完全依赖于(j)
                //完全依赖元素拼串
                string strInfo = "";
                for(int y = 0; y < iCount; y++)
                {
                    if(strInfo == "")
                    {
                        if(bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", y) == "D")
                            strInfo = bufElementLimitByPackage.GetString("DISCNT_NAME", y);
                        else if(bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", y) == "S")
                            strInfo = bufElementLimitByPackage.GetString("SERVICE_NAME", y);
                        else if(bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", y) == "X")
                            strInfo = bufElementLimitByPackage.GetString("SP_PRODUCT_NAME", y);
                        else if(bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", y) == "A")
                            strInfo = bufElementLimitByPackage.GetString("ACTION_NAME", y);
                        else if(bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", y) == "K")
                            strInfo = bufElementLimitByPackage.GetString("PARENT_ACTION_NAME", y);
                    }
                    else
                    {
                        if(bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", y) == "D")
                            strInfo += "、" + bufElementLimitByPackage.GetString("DISCNT_NAME", y);
                        else if(bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", y) == "S")
                            strInfo += "、" + bufElementLimitByPackage.GetString("SERVICE_NAME", y);
                        else if(bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", y) == "X")
                            strInfo += "、" + bufElementLimitByPackage.GetString("SP_PRODUCT_NAME", y);
                        else if(bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", y) == "A")
                            strInfo += "、" + bufElementLimitByPackage.GetString("ACTION_NAME", y);
                        else if(bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", y) == "K")
                            strInfo = bufElementLimitByPackage.GetString("PARENT_ACTION_NAME", y);
                    }
                }

                if(strErrorInfo == "")
                    strErrorInfo = "\"" + bufUserAllElements.GetString("PACKAGE_NAME", i)
                        + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                        + "\"完全依赖于：\"" + bufElementLimitByPackage.GetString("PACKAGE_NAME")
                        + " --> " +  strInfo + "\"";
                else
                    strErrorInfo += "、\"" + bufUserAllElements.GetString("PACKAGE_NAME", i)
                        + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                        + "\"完全依赖于：\"" + bufElementLimitByPackage.GetString("PACKAGE_NAME")
                        + " --> " +  strInfo + "\"";

                break;
            }
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strErrorInfo + "，业务无法继续！");
}


//同一元素在一笔业务中只能选择一次
void TradeCheckAfterTrade::CheckSameElement(CFmlBuf &bufUserAllElements)
{
    int iCount = bufUserAllElements.GetInt("X_RECORDNUM");
    string strErrorInfo = "";

    string strLastDate="";
    DualMgr objDualMgr;
    objDualMgr.GetLastDayOfCurMonth(strLastDate);    

    for(int i = 0; i < iCount; i++)
    {
        if(bufUserAllElements.IsFldExist("X_GETMODE", i) && bufUserAllElements.GetInt("X_GETMODE", i) == 1)
            continue;
            

        /*
		    add by lirui@20090505	
		    月底终止的优惠判断问题(终止至月底的优惠，不参与互斥，部份依赖，完全依赖，重复元素的判断，即终止至月底的优惠
		    在判断时可以认为存在，也可以认为是不存在的元素)
		    */
		    if( bufUserAllElements.GetString("END_DATE",i)<=strLastDate&&bufUserAllElements.GetString("ELEMENT_TYPE_CODE", i)=="D")
			      continue;
			            
        for(int j = 0; j < iCount; j++)
        {
            if(i != j && bufUserAllElements.GetInt("ELEMENT_ID", i) == bufUserAllElements.GetInt("ELEMENT_ID", j)
                &&bufUserAllElements.GetString("ELEMENT_TYPE_CODE", i) == bufUserAllElements.GetString("ELEMENT_TYPE_CODE", j)
                //时间交集
                && (bufUserAllElements.GetString("START_DATE",i) <= bufUserAllElements.GetString("START_DATE",j)
                && bufUserAllElements.GetString("START_DATE",j) <= bufUserAllElements.GetString("END_DATE",i)
                ||
                bufUserAllElements.GetString("START_DATE",j) <= bufUserAllElements.GetString("START_DATE",i)
                && bufUserAllElements.GetString("START_DATE",i) <= bufUserAllElements.GetString("END_DATE",j)))
            {
                bufUserAllElements.SetInt("X_GETMODE", 1, j);
                if(strErrorInfo == "")
                {
                    strErrorInfo = "\"" + bufUserAllElements.GetString("PRODUCT_NAME",i)
                                   + " --> " + bufUserAllElements.GetString("PACKAGE_NAME",i)
                                   + " --> " + bufUserAllElements.GetString("ELEMENT_NAME",i)
                                   + "\"、\""
                                   + bufUserAllElements.GetString("PRODUCT_NAME",j)
                                   + " --> " + bufUserAllElements.GetString("PACKAGE_NAME",j)
                                   + " --> " + bufUserAllElements.GetString("ELEMENT_NAME",j)
                                   + "\"";
                }
                else
                {
                    strErrorInfo += "，\"" + bufUserAllElements.GetString("PRODUCT_NAME",i)
                                    + " --> " + bufUserAllElements.GetString("PACKAGE_NAME",i)
                                    + " --> " + bufUserAllElements.GetString("ELEMENT_NAME",i)
                                    + "\"、\""
                                    + bufUserAllElements.GetString("PRODUCT_NAME",j)
                                    + " --> " + bufUserAllElements.GetString("PACKAGE_NAME",j)
                                    + " --> " + bufUserAllElements.GetString("ELEMENT_NAME",j)
                                    + "\"";
                }
                break;
            }
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strErrorInfo + "重复选择，业务无法继续！");
}

//SP服务判断：同一个SP_ID只能选择一个
void TradeCheckAfterTrade::CheckSameSPID(CFmlBuf &bufTradeSp_UserAllSp)
{
    LOG_TRACE(logTradeCheckAfterTrade, "进入CheckSameSPID函数");

    int iCount = bufTradeSp_UserAllSp.GetInt("X_RECORDNUM");
    string strErrorInfo = "";
    StaticDAO& dao = StaticDAO::getDAO();
    CFmlBuf bufSp;
    int spCount = -1;

    //获取SP的SP_ID值
    for(int i = 0; i < iCount; i++)
    {
        try
        {
            bufSp.ClearFmlValue();
            spCount = -1;
            dao.Clear();
            dao.SetParam(":VSP_SERVICE_ID", bufTradeSp_UserAllSp.GetString("SP_SERVICE_ID",i));
            spCount = dao.jselect(bufSp, "SEL_BY_SERVICE_ID","TD_B_PARTY_PRODUCT");
        }
        catch (Exception &ex)
        {
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取办理业务后用户的所有SP的参数出错！[%s]",ex.getCause()->what());
        }

        if(spCount > 0)
        {
            bufTradeSp_UserAllSp.SetString("SP_ID",bufSp.GetString("SP_ID",0),i);
        }
        else
        {
            THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "获取办理业务后SP_SERVICE_ID=[%d]的参数出错！", bufTradeSp_UserAllSp.GetInt("SP_SERVICE_ID",i));
        }
    }

    //判断是否存在同一个SP_ID重复选择的数据
    for(int j = 0; j < iCount; j++)
    {
        if(bufTradeSp_UserAllSp.IsFldExist("X_GETMODE", j) && bufTradeSp_UserAllSp.GetInt("X_GETMODE", j) == 1)
            continue;
        for(int t = 0; t < iCount; t++)
        {
           if(j != t && bufTradeSp_UserAllSp.GetString("SP_ID", j) == bufTradeSp_UserAllSp.GetString("SP_ID", t)
                //时间交集
                && (bufTradeSp_UserAllSp.GetString("START_DATE",j) <= bufTradeSp_UserAllSp.GetString("START_DATE",t)
                && bufTradeSp_UserAllSp.GetString("START_DATE",t) <= bufTradeSp_UserAllSp.GetString("END_DATE",j)
                ||
                bufTradeSp_UserAllSp.GetString("START_DATE",t) <= bufTradeSp_UserAllSp.GetString("START_DATE",j)
                && bufTradeSp_UserAllSp.GetString("START_DATE",j) <= bufTradeSp_UserAllSp.GetString("END_DATE",t)))
                {
                    bufTradeSp_UserAllSp.SetInt("X_GETMODE", 1, t);
                    if(strErrorInfo == "")
                    {
                        strErrorInfo = "\"" + bufTradeSp_UserAllSp.GetString("PRODUCT_NAME",j)
                                       + " --> " + bufTradeSp_UserAllSp.GetString("PACKAGE_NAME",j)
                                       + " --> " + bufTradeSp_UserAllSp.GetString("SP_PRODUCT_NAME",j)
                                       + "\"、\""
                                       + bufTradeSp_UserAllSp.GetString("PRODUCT_NAME",t)
                                       + " --> " + bufTradeSp_UserAllSp.GetString("PACKAGE_NAME",t)
                                       + " --> " + bufTradeSp_UserAllSp.GetString("SP_PRODUCT_NAME",t)
                                       + "\"";
                    }
                    else
                    {
                        strErrorInfo += "，\"" + bufTradeSp_UserAllSp.GetString("PRODUCT_NAME",j)
                                        + " --> " + bufTradeSp_UserAllSp.GetString("PACKAGE_NAME",j)
                                        + " --> " + bufTradeSp_UserAllSp.GetString("SP_PRODUCT_NAME",j)
                                        + "\"、\""
                                        + bufTradeSp_UserAllSp.GetString("PRODUCT_NAME",t)
                                        + " --> " + bufTradeSp_UserAllSp.GetString("PACKAGE_NAME",t)
                                        + " --> " + bufTradeSp_UserAllSp.GetString("SP_PRODUCT_NAME",t)
                                        + "\"";
                    }
                    break;
                }
        }
    }

    LOG_TRACE_P1(logTradeCheckAfterTrade, "CheckSameSPID函数--校验sp:%s",strErrorInfo.c_str());
    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strErrorInfo + " 同一个SP代码重复选择，业务无法继续！");

    LOG_TRACE(logTradeCheckAfterTrade, "退出CheckSameSPID函数");
}

//获取包与元素之间关系
int TradeCheckAfterTrade::ExistsPackageElementsLimitA(const int &iPackageId, const string &strLimitTag,
                                                       CFmlBuf &bufPackageElementsLimit)
{
    bufPackageElementsLimit.ClearFmlValue();
    StaticDAO& dao = StaticDAO::getDAO();
    int iCount = 0;

    try
    {
        dao.Clear();
        dao.SetParam(":VPACKAGE_ID", iPackageId);
        dao.SetParam(":VLIMIT_TAG", strLimitTag);
        iCount = dao.jselect(bufPackageElementsLimit, "JUDGE_A_LIMIT_EXISTS", "TD_B_ELEMENT_PACKAGE_LIMIT");
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取包与元素间关系信息出错！[%s]",ex.getCause()->what());
    }

    /*ConvertCodeToName(bufPackageElementsLimit, "PackageName", "PACKAGE_ID", "PACKAGE_NAME");
    ConvertCodeToName(bufPackageElementsLimit, "DiscntName", "ELEMENT_ID", "DISCNT_NAME");
    ConvertCodeToName(bufPackageElementsLimit, "ServiceName", "ELEMENT_ID", "SERVICE_NAME");
    ConvertCodeToName(bufPackageElementsLimit, "ActionName", "ELEMENT_ID", "ACTION_NAME");
    ConvertCodeToName(bufPackageElementsLimit, "SpProductName", "ELEMENT_ID", "SP_PRODUCT_NAME");    */
    /*char cTemp[10];
    for(int i = 0; i < iCount; i++)
    {
        cTemp[0] = '\0';
        sprintf(cTemp,"%d",(int)bufPackageElementsLimit.GetInt("PACKAGE_ID", i));
        bufPackageElementsLimit.SetString("PACKAGE_NAME", CParamDAO::getParam("PackageName", cTemp), i);
        cTemp[0] = '\0';
        sprintf(cTemp,"%d",(int)bufPackageElementsLimit.GetInt("ELEMENT_ID", i));
        if(bufPackageElementsLimit.GetString("ELEMENT_TYPE_CODE", i) == "D")
        {
            bufPackageElementsLimit.SetString("DISCNT_NAME", CParamDAO::getParam("DiscntName", cTemp), i);
        }
        else if(bufPackageElementsLimit.GetString("ELEMENT_TYPE_CODE", i) == "S")
        {
            bufPackageElementsLimit.SetString("SERVICE_NAME", CParamDAO::getParam("ServiceName", cTemp), i);
        }
        else if(bufPackageElementsLimit.GetString("ELEMENT_TYPE_CODE", i) == "A" ||
            bufPackageElementsLimit.GetString("ELEMENT_TYPE_CODE", i) == "C")
        {
            bufPackageElementsLimit.SetString("ACTION_NAME", CParamDAO::getParam("ActionName", cTemp), i);
        }
        else if(bufPackageElementsLimit.GetString("ELEMENT_TYPE_CODE", i) == "X")
        {
            bufPackageElementsLimit.SetString("SP_PRODUCT_NAME", CParamDAO::getParam("SpProductName", cTemp), i);
        }
    }*/

    bufPackageElementsLimit.SetInt("X_RECORDNUM", iCount);
    return iCount;
}


//包与元素关系判断
void TradeCheckAfterTrade::CheckPackageAndElementsLimit(CFmlBuf &bufUserPackageByProd, CFmlBuf &bufUserAllElements)
{
    CFmlBuf bufPackageElementsLimit;
    int i,j,z;
    bool bFound = false;

    //互斥
    string strHintInfo = "";
    string strErrorInfo = "";
    string strDiscntInfo = "";
    string strSvcInfo = "";
    string strSpInfo = "";
    string strElementInfo = "";

    int iCount = bufUserPackageByProd.GetInt("X_RECORDNUM");
    for(i = 0; i < iCount; i++)
    {
        strErrorInfo = "";
        strDiscntInfo = "";
        strSvcInfo = "";
        string strSpInfo = "";
        string strElementInfo = "";

        int iLimitCount = ExistsPackageElementsLimitA(bufUserPackageByProd.GetInt("PACKAGE_ID", i),
                          "0", bufPackageElementsLimit);

        for(j = 0; j < iLimitCount; j++)
        {
            int iUserElements = bufUserAllElements.GetInt("X_RECORDNUM");
            bFound = false;
            for(z = 0; z < iUserElements; z++)
            {
                if(bufPackageElementsLimit.GetString("ELEMENT_TYPE_CODE", j) ==    //相同元素类型
                    bufUserAllElements.GetString("ELEMENT_TYPE_CODE", z) &&
                    bufPackageElementsLimit.GetInt("ELEMENT_ID", j) ==    //相同元素
                    bufUserAllElements.GetInt("ELEMENT_ID", z))
                {
                    bFound = true;
                    break;
                }
            }

            if(bFound)
            {
                //(i)与(j)互斥
                if(bufPackageElementsLimit.GetString("ELEMENT_TYPE_CODE", j) == "D")
                {
                    if(strDiscntInfo == "")
                        strDiscntInfo = bufPackageElementsLimit.GetString("DISCNT_NAME", j);
                    else
                        strDiscntInfo += "、" + bufPackageElementsLimit.GetString("DISCNT_NAME", j);
                }
                else if(bufPackageElementsLimit.GetString("ELEMENT_TYPE_CODE", j) == "S")
                {
                    if(strSvcInfo == "")
                        strSvcInfo = bufPackageElementsLimit.GetString("SERVICE_NAME", j);
                    else
                        strSvcInfo += "、" + bufPackageElementsLimit.GetString("SERVICE_NAME", j);
                }
                else if(bufPackageElementsLimit.GetString("ELEMENT_TYPE_CODE", j) == "X")
                {
                    if(strSpInfo == "")
                        strSpInfo = bufPackageElementsLimit.GetString("SP_PRODUCT_NAME", j);
                    else
                        strSpInfo += "、" + bufPackageElementsLimit.GetString("SP_PRODUCT_NAME", j);
                }
                else
                {
                    if(strElementInfo == "")
                        strElementInfo = bufPackageElementsLimit.GetString("ACTION_NAME", j);
                    else
                        strElementInfo += "、" + bufPackageElementsLimit.GetString("ACTION_NAME", j);
                }
            }
        }

        if(strDiscntInfo != "" || strSvcInfo != "" || strSpInfo != "" || strElementInfo != "")
        {
            //(i)与(j)互斥
            /*if(strDiscntInfo != "")
                strDiscntInfo = "优惠：\"" + strDiscntInfo + "\"";
            if(strSvcInfo != "")
                strSvcInfo = "服务：\"" + strSvcInfo + "\"";

            if(strDiscntInfo != "" && strSvcInfo != "")
                strErrorInfo = strDiscntInfo + "，" + strSvcInfo;
            else
            {
                if(strDiscntInfo != "")
                    strErrorInfo = strDiscntInfo;
                else
                    strErrorInfo = strSvcInfo;
            }*/

            if(strDiscntInfo != "")
            {
                if(strErrorInfo == "")  strErrorInfo += strDiscntInfo;
                else strErrorInfo += "、" + strDiscntInfo;
            }

            if(strSvcInfo != "")
            {
                if(strErrorInfo == "")  strErrorInfo += strSvcInfo;
                else strErrorInfo += "、" + strSvcInfo;
            }

            if(strSpInfo != "")
            {
                if(strErrorInfo == "")  strErrorInfo += strSpInfo;
                else strErrorInfo += "、" + strSpInfo;
            }

            if(strElementInfo != "")
            {
                if(strErrorInfo == "")  strErrorInfo += strElementInfo;
                else strErrorInfo += "、" + strElementInfo;
            }

            strErrorInfo = "业务包：\"" + bufUserPackageByProd.GetString("PACKAGE_NAME", i)
                    + "\"与元素：" + strErrorInfo + "互斥";
        }

        if(strErrorInfo != "")
        {
            if(strHintInfo == "")
                strHintInfo = strErrorInfo;
            else
                strHintInfo += "；" + strErrorInfo;
        }
    }

    if(strHintInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strHintInfo + "，业务无法继续！");


    //部份依赖
    strHintInfo = "";
    for(i = 0; i < iCount; i++)
    {
        strErrorInfo = "";
        strDiscntInfo = "";
        strSvcInfo = "";
        string strSpInfo = "";
        string strElementInfo = "";
        bFound = true;

        int iLimitCount = ExistsPackageElementsLimitA(bufUserPackageByProd.GetInt("PACKAGE_ID", i),
                          "1", bufPackageElementsLimit);

        for(j = 0; j < iLimitCount; j++)
        {
            int iUserElements = bufUserAllElements.GetInt("X_RECORDNUM");
            bFound = false;
            for(z = 0; z < iUserElements; z++)
            {
                if(bufPackageElementsLimit.GetString("ELEMENT_TYPE_CODE", j) ==    //相同元素类型
                    bufUserAllElements.GetString("ELEMENT_TYPE_CODE", z) &&
                    bufPackageElementsLimit.GetInt("ELEMENT_ID", j) ==    //相同元素
                    bufUserAllElements.GetInt("ELEMENT_ID", z))
                {
                    bFound = true;
                    break;
                }
            }

            if(bFound)
            {
                break;
            }
        }

        if(!bFound)
        {
            //(i)部份依赖j
            for(int y = 0; y < iLimitCount; y++)
            {
                if(bufPackageElementsLimit.GetString("ELEMENT_TYPE_CODE", y) == "D")
                {
                    if(strDiscntInfo == "")
                        strDiscntInfo = bufPackageElementsLimit.GetString("DISCNT_NAME", y);
                    else
                        strDiscntInfo += "、" + bufPackageElementsLimit.GetString("DISCNT_NAME", y);
                }
                else if(bufPackageElementsLimit.GetString("ELEMENT_TYPE_CODE", y) == "S")
                {
                    if(strSvcInfo == "")
                        strSvcInfo = bufPackageElementsLimit.GetString("SERVICE_NAME", y);
                    else
                        strSvcInfo += "、" + bufPackageElementsLimit.GetString("SERVICE_NAME", y);
                }
                else if(bufPackageElementsLimit.GetString("ELEMENT_TYPE_CODE", y) == "X")
                {
                    if(strSpInfo == "")
                        strSpInfo = bufPackageElementsLimit.GetString("SP_PRODUCT_NAME", y);
                    else
                        strSpInfo += "、" + bufPackageElementsLimit.GetString("SP_PRODUCT_NAME", y);
                }
                else
                {
                    if(strElementInfo == "")
                        strElementInfo = bufPackageElementsLimit.GetString("ACTION_NAME", y);
                    else
                        strElementInfo += "、" + bufPackageElementsLimit.GetString("ACTION_NAME", y);
                }
            }

            if(strDiscntInfo != "" || strSvcInfo != "" || strSpInfo != "" || strElementInfo != "")
            {
                //(i)部份依赖j
                /*if(strDiscntInfo != "")
                    strDiscntInfo = "优惠：\"" + strDiscntInfo + "\"";
                if(strSvcInfo != "")
                    strSvcInfo = "服务：\"" + strSvcInfo + "\"";

                if(strDiscntInfo != "" && strSvcInfo != "")
                    strErrorInfo = strDiscntInfo + "，" + strSvcInfo;
                else
                {
                    if(strDiscntInfo != "")
                        strErrorInfo = strDiscntInfo;
                    else
                        strErrorInfo = strSvcInfo;
                }*/

                if(strDiscntInfo != "")
                {
                    if(strErrorInfo == "")  strErrorInfo += strDiscntInfo;
                    else strErrorInfo += "、" + strDiscntInfo;
                }

                if(strSvcInfo != "")
                {
                    if(strErrorInfo == "")  strErrorInfo += strSvcInfo;
                    else strErrorInfo += "、" + strSvcInfo;
                }

                if(strSpInfo != "")
                {
                    if(strErrorInfo == "")  strErrorInfo += strSpInfo;
                    else strErrorInfo += "、" + strSpInfo;
                }

                if(strElementInfo != "")
                {
                    if(strErrorInfo == "")  strErrorInfo += strElementInfo;
                    else strErrorInfo += "、" + strElementInfo;
                }

                strErrorInfo = "业务包：\"" + bufUserPackageByProd.GetString("PACKAGE_NAME", i)
                        + "\"部份依赖于元素：" + strErrorInfo;
            }
        }

        if(strErrorInfo != "")
        {
            if(strHintInfo == "")
                strHintInfo = strErrorInfo;
            else
                strHintInfo += "；" + strErrorInfo;
        }
    }

    if(strHintInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strHintInfo + "，业务无法继续！");


    //完全依赖
    strHintInfo = "";
    for(i = 0; i < iCount; i++)
    {
        strErrorInfo = "";
        strDiscntInfo = "";
        strSvcInfo = "";
        string strSpInfo = "";
        string strElementInfo = "";

        int iLimitCount = ExistsPackageElementsLimitA(bufUserPackageByProd.GetInt("PACKAGE_ID", i),
                          "2", bufPackageElementsLimit);

        for(j = 0; j < iLimitCount; j++)
        {
            int iUserElements = bufUserAllElements.GetInt("X_RECORDNUM");
            bFound = false;
            for(z = 0; z < iUserElements; z++)
            {
                if(bufPackageElementsLimit.GetString("ELEMENT_TYPE_CODE", j) ==    //相同元素类型
                    bufUserAllElements.GetString("ELEMENT_TYPE_CODE", z) &&
                    bufPackageElementsLimit.GetInt("ELEMENT_ID", j) ==    //相同元素
                    bufUserAllElements.GetInt("ELEMENT_ID", z))
                {
                    bFound = true;
                    break;
                }
            }

            if(!bFound)
            {
                //(i)完全依赖j
                for(int y = 0; y < iLimitCount; y++)
                {
                    if(bufPackageElementsLimit.GetString("ELEMENT_TYPE_CODE", y) == "D")
                    {
                        if(strDiscntInfo == "")
                            strDiscntInfo = bufPackageElementsLimit.GetString("DISCNT_NAME", y);
                        else
                            strDiscntInfo += "、" + bufPackageElementsLimit.GetString("DISCNT_NAME", y);
                    }
                    else if(bufPackageElementsLimit.GetString("ELEMENT_TYPE_CODE", y) == "S")
                    {
                        if(strSvcInfo == "")
                            strSvcInfo = bufPackageElementsLimit.GetString("SERVICE_NAME", y);
                        else
                            strSvcInfo += "、" + bufPackageElementsLimit.GetString("SERVICE_NAME", y);
                    }
                    else if(bufPackageElementsLimit.GetString("ELEMENT_TYPE_CODE", y) == "X")
                    {
                        if(strSpInfo == "")
                            strSpInfo = bufPackageElementsLimit.GetString("SP_PRODUCT_NAME", y);
                        else
                            strSpInfo += "、" + bufPackageElementsLimit.GetString("SP_PRODUCT_NAME", y);
                    }
                    else
                    {
                        if(strElementInfo == "")
                            strElementInfo = bufPackageElementsLimit.GetString("ACTION_NAME", y);
                        else
                            strElementInfo += "、" + bufPackageElementsLimit.GetString("ACTION_NAME", y);
                    }
                }

                if(strDiscntInfo != "" || strSvcInfo != "" || strSpInfo != "" || strElementInfo != "")
                {
                    //(i)完全依赖j
                    /*if(strDiscntInfo != "")
                        strDiscntInfo = "优惠：\"" + strDiscntInfo + "\"";
                    if(strSvcInfo != "")
                        strSvcInfo = "服务：\"" + strSvcInfo + "\"";

                    if(strDiscntInfo != "" && strSvcInfo != "")
                        strErrorInfo = strDiscntInfo + "，" + strSvcInfo;
                    else
                    {
                        if(strDiscntInfo != "")
                            strErrorInfo = strDiscntInfo;
                        else
                            strErrorInfo = strSvcInfo;
                    }*/

                    if(strDiscntInfo != "")
                    {
                        if(strErrorInfo == "")  strErrorInfo += strDiscntInfo;
                        else strErrorInfo += "、" + strDiscntInfo;
                    }

                    if(strSvcInfo != "")
                    {
                        if(strErrorInfo == "")  strErrorInfo += strSvcInfo;
                        else strErrorInfo += "、" + strSvcInfo;
                    }

                    if(strSpInfo != "")
                    {
                        if(strErrorInfo == "")  strErrorInfo += strSpInfo;
                        else strErrorInfo += "、" + strSpInfo;
                    }

                    if(strElementInfo != "")
                    {
                        if(strErrorInfo == "")  strErrorInfo += strElementInfo;
                        else strErrorInfo += "、" + strElementInfo;
                    }

                    strErrorInfo = "业务包：\"" + bufUserPackageByProd.GetString("PACKAGE_NAME", i)
                            + "\"完全依赖于元素：" + strErrorInfo;
                }

                break;
            }
        }

        if(strErrorInfo != "")
        {
            if(strHintInfo == "")
                strHintInfo = strErrorInfo;
            else
                strHintInfo += "；" + strErrorInfo;
        }
    }

    if(strHintInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strHintInfo + "，业务无法继续！");
}


//获取产品之间限制关系
int TradeCheckAfterTrade::ExistsProductLimitA(const int &iProduct, const string &strLimitTag,
                                              CFmlBuf &bfProductLimit)
{
    bfProductLimit.ClearFmlValue();
    StaticDAO& dao = StaticDAO::getDAO();
    int iCount = 0;

    try
    {
        dao.Clear();
        dao.SetParam(":VPRODUCT_ID_A", iProduct);
        dao.SetParam(":VLIMIT_TAG", strLimitTag);
        iCount = dao.jselect(bfProductLimit, "JUDGE_A_LIMIT_EXISTS", "TD_S_PRODUCTLIMIT");
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取产品间关系信息出错！[%s]",ex.getCause()->what());
    }

    ConvertCodeToName(bfProductLimit, "ProductName", "PRODUCT_ID_B", "PRODUCT_NAME");

    bfProductLimit.SetInt("X_RECORDNUM", iCount);
    return iCount;
}


//产品间限制
void TradeCheckAfterTrade::CheckProductLimit(CFmlBuf &bfUserProd)
{
    CFmlBuf bfProductLimit;
    int i,j,z;
    bool bFound = false;

    //互斥
    string strHintInfo = "";
    string strErrorInfo = "";

    int iCount = bfUserProd.GetInt("X_RECORDNUM");
    for(i = 0; i < iCount; i++)
    {
        strErrorInfo = "";

        int iLimitCount = ExistsProductLimitA(bfUserProd.GetInt("PRODUCT_ID", i),
                          "0", bfProductLimit);

        for(j = 0; j < iLimitCount; j++)
        {
            int iUserProducts = bfUserProd.GetInt("X_RECORDNUM");
            bFound = false;
            for(z = 0; z < iUserProducts; z++)
            {
                if(bfProductLimit.GetString("PRODUCT_ID_B", j) ==
                    bfUserProd.GetString("PRODUCT_ID", z))
                {
                    bFound = true;
                    break;
                }
            }

            if(bFound)
            {
                //(i)与(j)互斥
                if(strErrorInfo == "")
                    strErrorInfo = bfProductLimit.GetString("PRODUCT_NAME", j);
                else
                    strErrorInfo += "、" + bfProductLimit.GetString("PRODUCT_NAME", j);
            }
        }

        if(strErrorInfo != "")
        {
            if(strHintInfo == "")
                strHintInfo = "产品：\"" + bfUserProd.GetString("PRODUCT_NAME", i)
                              + "\"与产品：\"" + strErrorInfo + "\"互斥";
            else
                strHintInfo += "；产品：\"" + bfUserProd.GetString("PRODUCT_NAME", i)
                               + "\"与产品：\"" + strErrorInfo + "\"互斥";
        }
    }

    if(strHintInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strHintInfo + "，业务无法继续！");


    //部份依赖
    strHintInfo = "";
    for(i = 0; i < iCount; i++)
    {
        strErrorInfo = "";
        bFound = true;

        int iLimitCount = ExistsProductLimitA(bfUserProd.GetInt("PRODUCT_ID", i),
                          "1", bfProductLimit);

        for(j = 0; j < iLimitCount; j++)
        {
            int iUserProducts = bfUserProd.GetInt("X_RECORDNUM");
            bFound = false;
            for(z = 0; z < iUserProducts; z++)
            {
                if(bfProductLimit.GetString("PRODUCT_ID_B", j) ==
                    bfUserProd.GetString("PRODUCT_ID", z))
                {
                    bFound = true;
                    break;
                }
            }

            if(bFound)
            {
                break;
            }
        }

        if(!bFound)
        {
            //(i)部份依赖j
            for(int y = 0; y < iLimitCount; y++)
            {
                if(strErrorInfo == "")
                    strErrorInfo = bfProductLimit.GetString("PRODUCT_NAME", y);
                else
                    strErrorInfo += "、" + bfProductLimit.GetString("PRODUCT_NAME", y);
            }
        }

        if(strErrorInfo != "")
        {
            if(strHintInfo == "")
                strHintInfo = "产品：\"" + bfUserProd.GetString("PRODUCT_NAME", i)
                              + "\"部份依赖于产品：\"" + strErrorInfo + "\"";
            else
                strHintInfo += "；产品：\"" + bfUserProd.GetString("PRODUCT_NAME", i)
                               + "\"部份依赖于产品：\"" + strErrorInfo + "\"";
        }
    }

    if(strHintInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strHintInfo + "，业务无法继续！");


    //完全依赖
    strHintInfo = "";
    for(i = 0; i < iCount; i++)
    {
        strErrorInfo = "";

        int iLimitCount = ExistsProductLimitA(bfUserProd.GetInt("PRODUCT_ID", i),
                          "2", bfProductLimit);

        for(j = 0; j < iLimitCount; j++)
        {
            int iUserProducts = bfUserProd.GetInt("X_RECORDNUM");
            bFound = false;
            for(z = 0; z < iUserProducts; z++)
            {
                if(bfProductLimit.GetString("PRODUCT_ID_B", j) ==
                    bfUserProd.GetString("PRODUCT_ID", z))
                {
                    bFound = true;
                    break;
                }
            }

            if(!bFound)
            {
                //(i)完全依赖j
                for(int y = 0; y < iLimitCount; y++)
                {
                    if(strErrorInfo == "")
                        strErrorInfo = bfProductLimit.GetString("PRODUCT_NAME", y);
                    else
                        strErrorInfo += "、" + bfProductLimit.GetString("PRODUCT_NAME", y);
                }

                break;
            }
        }

        if(strErrorInfo != "")
        {
            if(strHintInfo == "")
                strHintInfo = "产品：\"" + bfUserProd.GetString("PRODUCT_NAME", i)
                              + "完全依赖于产品：\"" +strErrorInfo + "\"";
            else
                strHintInfo += "；产品：\"" + bfUserProd.GetString("PRODUCT_NAME", i)
                               + "完全依赖于产品：\"" +strErrorInfo + "\"";
        }
    }

    if(strHintInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strHintInfo + "，业务无法继续！");
}

//获取删除包的信息
int TradeCheckAfterTrade::GetDelPackage(const string &strTradeId, CFmlBuf &bufDelPackage)
{
    StaticDAO& dao = StaticDAO::getDAO();
	int iCount = 0;

    try
    {
        bufDelPackage.ClearFmlValue();
        dao.Clear();
        dao.SetParam(":VTRADE_ID", strTradeId);
        dao.SetParam(":VRSRV_VALUE_CODE", "DLPK");
        iCount = dao.jselect(bufDelPackage, "SEL_BY_TRADEID_RSRVVLLUE", "TF_B_TRADE_OTHER");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:查询业务台账其他信息子表出错！");
    }

    bufDelPackage.SetInt("X_RECORDNUM", iCount);

    return iCount;
}

//获取当前业务未发生变化的包信息
int TradeCheckAfterTrade::GetNoChangePackage(const string &strTradeId,const string &strUserId, CFmlBuf &bufNoChangePackage)
{
    StaticDAO& dao = StaticDAO::getDAO();
	int iCount = 0;

    try
    {
        bufNoChangePackage.ClearFmlValue();
        dao.Clear();
        dao.SetParam(":VTRADE_ID", strTradeId);
        dao.SetParam(":VUSER_ID", strUserId);
        iCount = dao.jselect(bufNoChangePackage, "SEL_NOCHANG_PACKAGE", "TF_B_TRADE_PRODUCT");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取用户未变化包信息出错！");
    }

    bufNoChangePackage.SetInt("X_RECORDNUM", iCount);

    return iCount;
}


//获取包内元素时间连续的元素个数，用于计算包内元素最大最小数时，时间连续的两个元素计算为一个
int TradeCheckAfterTrade::GetElementTimeSeries(CFmlBuf &bufUserAllElements, CFmlBuf &bufElementTimeSeries)
{
    string strNextSecondTime = "";

    int iCount = bufUserAllElements.GetInt("X_RECORDNUM");
    for(int i = 0; i <iCount; i++)
    {
        bufUserAllElements.SetInt("X_DEAL_INDEX", 0, i);
    }

    for(int i = 0; i < iCount; i++)
    {
        strNextSecondTime = "";
        AddDays(bufUserAllElements.GetString("END_DATE", i), (float)1.0/24/3600, strNextSecondTime);

        bool bFound = false;
        for(int j = 0; j < iCount; j++)
        {
            //同一产品，同一包内，同一元素类型生效时间连续，（不判断是否为同一元素）
            if(i != j && bufUserAllElements.GetInt("X_DEAL_INDEX", j) != 1
                && bufUserAllElements.GetInt("PRODUCT_ID", i) == bufUserAllElements.GetInt("PRODUCT_ID", j)
                && bufUserAllElements.GetInt("PACKAGE_ID", i) == bufUserAllElements.GetInt("PACKAGE_ID", j)
                && bufUserAllElements.GetString("ELEMENT_TYPE_CODE", i) == bufUserAllElements.GetString("ELEMENT_TYPE_CODE", j)
                && strNextSecondTime == bufUserAllElements.GetString("START_DATE", j))
            {
                bufUserAllElements.SetInt("X_DEAL_INDEX", 1, j);
                bFound = true;
                break;
            }
        }

        if(bFound)
        {
            bool bExist = false;
            int iRecCount = 0;
            if(bufElementTimeSeries.IsFldExist("PRODUCT_ID"))
                iRecCount = bufElementTimeSeries.GetCount("PRODUCT_ID");

            for(int z = 0; z < iRecCount; z++)
            {
                if(bufElementTimeSeries.GetInt("PRODUCT_ID", z) == bufUserAllElements.GetInt("PRODUCT_ID", i)
                    && bufElementTimeSeries.GetInt("PACKAGE_ID", z) == bufUserAllElements.GetInt("PACKAGE_ID", i))
                {
                    bExist = true;

                    bufElementTimeSeries.SetInt("X_TAG", bufElementTimeSeries.GetInt("X_TAG", z)+1, z);
                    break;
                }
            }

            if(!bExist)
            {
                bufElementTimeSeries.SetInt("PRODUCT_ID", bufUserAllElements.GetInt("PRODUCT_ID", i), iRecCount);
                bufElementTimeSeries.SetInt("PACKAGE_ID", bufUserAllElements.GetInt("PACKAGE_ID", i), iRecCount);
                bufElementTimeSeries.SetInt("X_TAG", 1, iRecCount);
            }
        }
    }

    iCount = 0;
    if(bufElementTimeSeries.IsFldExist("PRODUCT_ID"))
        iCount = bufElementTimeSeries.GetCount("PRODUCT_ID");
    bufElementTimeSeries.SetInt("X_RECORDNUM", iCount);

    return iCount;
}


//产品资源约束
void TradeCheckAfterTrade::CheckProductRes(CFmlBuf &bfUserProd, const string &strTradeId, const string &strUserId,
                                           const string &strEparchyCode)
{
    int iCondCount = 0;
    bool bFound = false;
    CFmlBuf bfUserSn;
    CFmlBuf bfUserSimCard;
    CFmlBuf bfUserDeviceType;
    CFmlBuf bfProdResLimit;

    //获取用户号码
    GetUserSerialNumber(bfUserSn, strTradeId, strUserId);

    //获取用户SIM卡
    GetUserSimCard(bfUserSimCard, strTradeId, strUserId);

    //获取用户手机类型
    GetUserDeviceType(bfUserDeviceType, strTradeId, strUserId);

    //暂只实现单向判断（产品-->资源，产品强制规则）
    int iCount = bfUserProd.GetInt("X_RECORDNUM");
    for(int i = 0; i < iCount; i++)
    {
        if(bfUserSn.GetInt("X_RECORDNUM") > 0)
        {
            //获取产品资源约束(号码)
            iCondCount = GetProdResLimit(bfProdResLimit,bfUserProd.GetInt("PRODUCT_ID",i),"1","0","0",strEparchyCode);
            if(iCondCount > 0)
            {
                bFound = false;
                for(int j = 0; j < iCondCount; j++)
                {
                    if(bfProdResLimit.GetString("RES_NO_S", j) <= bfUserSn.GetString("RES_CODE") &&
                        bfProdResLimit.GetString("RES_NO_E", j) >= bfUserSn.GetString("RES_CODE"))
                    {
                        bFound = true;
                        break;
                    }
                }

                if(!bFound)
                    THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,
                        "产品：\"" + bfUserProd.GetString("PRODUCT_NAME", i) + "\"不能使用号码资源：\"" +
                         bfUserSn.GetString("RES_CODE") + "\"，业务无法继续！");
            }
        }

        if(bfUserSimCard.GetInt("X_RECORDNUM") > 0)
        {
            //获取产品资源约束(SIM卡)
            iCondCount = GetProdResLimit(bfProdResLimit,bfUserProd.GetInt("PRODUCT_ID",i),"1","0","1",strEparchyCode);
            if(iCondCount > 0)
            {
                bFound = false;
                for(int j = 0; j < iCondCount; j++)
                {
                    if(bfProdResLimit.GetString("RES_NO_S", j) <= bfUserSimCard.GetString("RES_CODE") &&
                        bfProdResLimit.GetString("RES_NO_E", j) >= bfUserSimCard.GetString("RES_CODE"))
                    {
                        bFound = true;
                        break;
                    }
                }

                if(!bFound)
                    THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,
                        "产品：\"" + bfUserProd.GetString("PRODUCT_NAME", i) + "\"不能使用SIM卡资源：\"" +
                         bfUserSimCard.GetString("RES_CODE") + "\"，业务无法继续！");
            }
        }

        for(int z = 0; z < bfUserDeviceType.GetInt("X_RECORDNUM"); z++)
        {
            //获取产品资源约束(终端)
            iCondCount = GetProdResLimit(bfProdResLimit,bfUserProd.GetInt("PRODUCT_ID",i),"1","0","4",strEparchyCode);
            if(iCondCount > 0)
            {
                bFound = false;
                for(int j = 0; j < iCondCount; j++)
                {
                    if(bfProdResLimit.GetString("RES_NO_S", j) <= bfUserDeviceType.GetString("DEVICE_TYPE", z) &&
                        bfProdResLimit.GetString("RES_NO_E", j) >= bfUserDeviceType.GetString("DEVICE_TYPE", z))
                    {
                        bFound = true;
                        break;
                    }
                }

                if(!bFound)
                    THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,
                        "产品：\"" + bfUserProd.GetString("PRODUCT_NAME", i) + "\"不能使用终端资源：\"" +
                         bfUserDeviceType.GetString("DEVICE_TYPE", z) + "\"，业务无法继续！");
            }
        }
    }
}


//获取用户号码
int TradeCheckAfterTrade::GetUserSerialNumber(CFmlBuf &bfUserSn, const string &strTradeId, const string &strUserId)
{
    StaticDAO& dao = StaticDAO::getDAO();
	int iCount = 0;

    try
    {
        bfUserSn.ClearFmlValue();
        dao.Clear();
        dao.SetParam(":VTRADE_ID", strTradeId);
        dao.SetParam(":VUSER_ID", strUserId);
        dao.SetParam(":VRES_TYPE_CODE", "0");
        iCount = dao.jselect(bfUserSn, "SEL_USERRES_BYID", "TF_B_TRADE_RES");
        if(bfUserSn.IsFldExist("RES_CODE")) iCount = bfUserSn.GetCount("RES_CODE");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取用户服务号码信息出错！");
    }

    bfUserSn.SetInt("X_RECORDNUM", iCount);

    return iCount;
}


//获取用户SIM卡
int TradeCheckAfterTrade::GetUserSimCard(CFmlBuf &bfUserSimCard, const string &strTradeId, const string &strUserId)
{
    StaticDAO& dao = StaticDAO::getDAO();
	int iCount = 0;

    try
    {
        bfUserSimCard.ClearFmlValue();
        dao.Clear();
        dao.SetParam(":VTRADE_ID", strTradeId);
        dao.SetParam(":VUSER_ID", strUserId);
        dao.SetParam(":VRES_TYPE_CODE", "1");
        iCount = dao.jselect(bfUserSimCard, "SEL_USERRES_BYID", "TF_B_TRADE_RES");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取用户SIM卡信息出错！");
    }

    bfUserSimCard.SetInt("X_RECORDNUM", iCount);

    return iCount;
}


//获取用户手机类型
int TradeCheckAfterTrade::GetUserDeviceType(CFmlBuf &bfUserDeviceType, const string &strTradeId, const string &strUserId)
{
    StaticDAO& dao = StaticDAO::getDAO();
	int iCount = 0;

    try
    {
        bfUserDeviceType.ClearFmlValue();
        dao.Clear();
        dao.SetParam(":VTRADE_ID", strTradeId);
        dao.SetParam(":VUSER_ID", strUserId);
        iCount = dao.jselect(bfUserDeviceType, "SEL_USER_DEVICETYPE_BYID", "TF_B_TRADE_PURCHASE");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取用户终端类型信息出错！");
    }

    bfUserDeviceType.SetInt("X_RECORDNUM", iCount);

    return iCount;
}


//获取产品资源约束
int TradeCheckAfterTrade::GetProdResLimit(CFmlBuf &bfProdResLimit, const int &iProduct, const string &strForceTag,
                                          const string &strCondTypeCode, const string &strResTypeCode,
                                          const string &strEparchyCode)
{
    StaticDAO& dao = StaticDAO::getDAO();
	int iCount = 0;

    try
    {
        bfProdResLimit.ClearFmlValue();
        dao.Clear();
        dao.SetParam(":VPRODUCT_ID", iProduct);
        dao.SetParam(":VCOND_TYPE_CODE", strCondTypeCode);
        dao.SetParam(":VRES_TYPE_CODE", strResTypeCode);
        dao.SetParam(":VFORCE_TAG", strForceTag);
        dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
        iCount = dao.jselect(bfProdResLimit, "SEL_PRODRES_LIMIT_BYTAG", "TD_B_PROD_RES_LIMIT");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取产品资源约束信息出错！");
    }

    ConvertCodeToName(bfProdResLimit, "ProductName", "PRODUCT_ID", "PRODUCT_NAME");

    bfProdResLimit.SetInt("X_RECORDNUM", iCount);

    return iCount;
}


/**
 *  山东代理商业务保证金判断
 */
void TradeCheckAfterTrade::CheckAgentTradeFee(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
    LOG_TRACE(logTradeCheckAfterTrade, "进入CheckAgentTradeFee函数");
    StaticDAO& dao = StaticDAO::getDAO();
    string tradeId = inBuf.GetString("TRADE_ID");
    int agentFee = inBuf.GetInt("MONEY");
    int iCount = -1;
    CFmlBuf tmpBuf;
    try
    {
        dao.Clear();
        dao.SetParam(":VTRADE_ID", tradeId);
        dao.SetParam(":VACCEPT_MONTH", tradeId.substr(4,2));
        iCount = dao.jselect(tmpBuf,"SEL_BY_TRADE_2","TF_B_TRADEFEE_SUB");
    }
    catch (Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, RECVDEPOSIT_ERR, "获取台帐费用子表资料异常！");
    }
    int iMoney = 0;
    for(int i=0;i<tmpBuf.GetCount("TRADE_ID");++i)
    {
        iMoney += atoi(tmpBuf.GetString("FEE",i).c_str());
    }
    if(iMoney>agentFee)
    		THROW_C(CRMException, CHECKAFTERTRADE_ERR, "代理商保证金额不够本次业务所收费用");
    LOG_TRACE(logTradeCheckAfterTrade, "退出CheckAgentTradeFee函数");
}

string TradeCheckAfterTrade::toTableCol(const string &strCentent)
{
    return "<td>" + strCentent + "</td>";
}

string TradeCheckAfterTrade::toTableRow(const string &strCentent1)
{
    return "<tr>" + toTableCol(strCentent1) + "</tr>";
}

string TradeCheckAfterTrade::toTableRow(const string &strCentent1, const string &strCentent2)
{
    return "<tr>" + toTableCol(strCentent1) + toTableCol(strCentent2) + "</tr>";
}

string TradeCheckAfterTrade::toTableRow(const string &strCentent1, const string &strCentent2,
                                        const string &strCentent3)
{
    return "<tr>" + toTableCol(strCentent1) + toTableCol(strCentent2) + toTableCol(strCentent3) + "</tr>";
}

string TradeCheckAfterTrade::toTableRow(const string &strCentent1, const string &strCentent2,
                                        const string &strCentent3, const string &strCentent4)
{
    return "<tr>" + toTableCol(strCentent1) + toTableCol(strCentent2) + toTableCol(strCentent3) + toTableCol(strCentent4) + "</tr>";
}

string TradeCheckAfterTrade::toTableRow(const string &strCentent1, const string &strCentent2,
                                        const string &strCentent3, const string &strCentent4,
                                        const string &strCentent5)
{
    return "<tr>" + toTableCol(strCentent1) + toTableCol(strCentent2) + toTableCol(strCentent3) + toTableCol(strCentent4) + toTableCol(strCentent5) + "</tr>";
}


void TradeCheckAfterTrade::AddDays(const string &strStartDateTime,const int &iDays, string &strResultDateTime)
{
    StaticDAO& dao = StaticDAO::getDAO();
    CFmlBuf bufTemp;

    try
    {
        dao.Clear();
        dao.SetParam(":VDATE", strStartDateTime);
        dao.SetParam(":VNUM", iDays);
        dao.jselect(bufTemp, "GET_DAY_ADD", "TD_S_CPARAM");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GETSYSDATE_ERR, "获取时间出错");
    }

    strResultDateTime = bufTemp.GetString("X_SYSDATE");
}

void TradeCheckAfterTrade::AddDays(const string &strStartDateTime,const float &fDays, string &strResultDateTime)
{
    StaticDAO& dao = StaticDAO::getDAO();
    CFmlBuf bufTemp;

    try
    {
        dao.Clear();
        dao.SetParam(":VDATE", strStartDateTime);
        dao.SetParam(":VNUM", to_string(fDays));
        dao.jselect(bufTemp, "GET_DAY_ADD", "TD_S_CPARAM");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GETSYSDATE_ERR, "获取时间出错");
    }

    strResultDateTime = bufTemp.GetString("X_SYSDATE");
}
//产品模型相关限制
void TradeCheckAfterTrade::CheckProductAllElementLimit(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
    string strSubscribeId = inBuf.GetString("SUBSCRIBE_ID");
    string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");
		int icount=0;
    CFmlBuf bufTradeDiscnt_AllDiscnt;   //用户优惠
    CFmlBuf bufTradeSvc_UserAllSvc;     //用户服务
    CFmlBuf bufTradeSp_UserAllSp;       //用户SP
    CFmlBuf bufTradeElement_UserAllElement; //用户物品等其他元素
    CFmlBuf bufUserAllElements;         //用户元素
    CFmlBuf bufElementTimeSeries;       //包内元素时间连续，用于计算包内元素最大最小数时，时间连续的两个元素计算为一个
		CFmlBuf tradeBuf;
		StaticDAO& dao = StaticDAO::getDAO();
    try{
    	  dao.Clear();
        dao.SetParam(":VSUBSCRIBE_ID", strSubscribeId);
        icount = dao.jselect(tradeBuf, "SEL_BY_SUBSCRIBEID","TF_B_TRADE");

        for (int i=0;i<icount;i++)
        {
        		//tradeBuf.printFmlBuffer();
        		if(tradeBuf.GetString("NET_TYPE_CODE",i)=="CP" && tradeBuf.GetString("TRADE_TYPE_CODE",i)=="192")
        				return;
        }

        GetAllDiscntAfterTradeComp(strSubscribeId,bufTradeDiscnt_AllDiscnt);
        GetUserAllServiceAfterTradeComp(strSubscribeId, bufTradeSvc_UserAllSvc);
        //获取用户物品等其他元素信息
        GetUserAllElementAfterTradeComp(strSubscribeId, bufTradeElement_UserAllElement);
        //获取用户SP信息
        GetUserAllSpAfterTradeComp(strSubscribeId, bufTradeSp_UserAllSp);
        //整合用户元素
        GeneUserAllElements(bufTradeDiscnt_AllDiscnt, bufTradeSvc_UserAllSvc, bufTradeSp_UserAllSp, bufTradeElement_UserAllElement, bufUserAllElements);
        //获取包内元素时间连续的元素个数，用于计算包内元素最大最小数时，时间连续的两个元素计算为一个
        GetElementTimeSeries(bufUserAllElements, bufElementTimeSeries);
        CheckElementLimitComp(bufUserAllElements, strEparchyCode,strSubscribeId);
    }
    catch (Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "产品模型相关限制出错！");
    }
}
/**
 *获取办理业务后的用户所有优惠和优惠的开始时间、结束时间、和修改状态（三户原有优惠的修改状态默认为'A')
 *
 */
int TradeCheckAfterTrade::GetAllDiscntAfterTradeComp(const string &strSubscribeId,
                                                 CFmlBuf &bufTradeDiscnt_AllDiscnt)
{
    bufTradeDiscnt_AllDiscnt.ClearFmlValue();
    CFmlBuf bufTradeDiscnt_AllDiscnt2;
    CFmlBuf tradeBuf;
    CFmlBuf compBuf;
    StaticDAO& dao = StaticDAO::getDAO();
    int tradeICount = 0;
    int compICount = 0;
    int iCount = 0;
    int iCount2 = 0;
    int iBase = 0;

    try
    {
        dao.Clear();
        dao.SetParam(":VSUBSCRIBE_ID", strSubscribeId);
        tradeICount = dao.jselect(tradeBuf, "SEL_BY_SUBSCRIBEID","TF_B_TRADE");
        string strUserId = "";
        string strEparchyCode = "";
        string strCityCode = "";
        string strBrandCode = "";
        if(tradeICount > 0 )
        {
            strUserId = tradeBuf.GetString("USER_ID",0);
            strEparchyCode = tradeBuf.GetString("TRADE_EPARCHY_CODE",0);
            strCityCode = tradeBuf.GetString("TRADE_CITY_CODE",0);
            strBrandCode = tradeBuf.GetString("BRAND_CODE",0);
        }

        //根据品牌判断是组合壳子的user_id或成员user_id，从而查询组合及成员信息
        if(strBrandCode != "" && strBrandCode == "COMP")
        {
            dao.Clear();
            dao.SetParam(":VUSER_ID", strUserId);
            dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
            dao.SetParam(":VCITY_CODE", strCityCode);
            compBuf.ClearFmlValue();
            compICount = dao.jselect(compBuf, "SEL_RELATION_BY_UIDA","TF_F_RELATION_UU");
        }
        else
        {
            dao.Clear();
            dao.SetParam(":VUSER_ID", strUserId);
            dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
            dao.SetParam(":VCITY_CODE", strCityCode);
            compBuf.ClearFmlValue();
            compICount = dao.jselect(compBuf, "SEL_RELATION_BY_UIDB","TF_F_RELATION_UU");
        }
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "查询订单[%s]相关记录出错！",strSubscribeId.c_str());
    }
    if(tradeICount <= 0 )
    {
        THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "没找到相关订单[%s]的记录！", strSubscribeId.c_str());
    }
    try
    {
        for(int i=0; i<tradeICount; i++)
        {
            dao.Clear();
            dao.SetParam(":VTRADE_ID", tradeBuf.GetString("TRADE_ID",i));
            dao.SetParam(":VACCEPT_MONTH", tradeBuf.GetString("TRADE_ID",i).substr(4,2));
            dao.SetParam(":VUSER_ID", tradeBuf.GetString("USER_ID",i));
            dao.SetParam(":VCUST_ID", tradeBuf.GetString("CUST_ID",i));
            dao.SetParam(":VACCT_ID", tradeBuf.GetString("ACCT_ID",i));

            iCount2 = dao.jselect(bufTradeDiscnt_AllDiscnt2, "SEL_ALLDISCNTS_AFTER_TRADE","TF_B_TRADE_DISCNT");
            //bufTradeDiscnt_AllDiscnt2.printFmlBuffer();
            int iDiscntCount = iCount2;
            for(int j = 0; j < iDiscntCount; j++)
            {
                bufTradeDiscnt_AllDiscnt.SetString("USER_ID", bufTradeDiscnt_AllDiscnt2.GetString("ID", j), iBase+j);
                bufTradeDiscnt_AllDiscnt.SetString("ID", bufTradeDiscnt_AllDiscnt2.GetString("ID", j), iBase+j);
                bufTradeDiscnt_AllDiscnt.SetInt("PRODUCT_ID", bufTradeDiscnt_AllDiscnt2.GetInt("PRODUCT_ID", j), iBase+j);
                //bufTradeDiscnt_AllDiscnt.SetString("PRODUCT_NAME", bufTradeDiscnt_AllDiscnt2.GetString("PRODUCT_NAME", j), iBase+j);
                bufTradeDiscnt_AllDiscnt.SetInt("PACKAGE_ID", bufTradeDiscnt_AllDiscnt2.GetInt("PACKAGE_ID", j), iBase+j);
                //bufTradeDiscnt_AllDiscnt.SetString("PACKAGE_NAME", bufTradeDiscnt_AllDiscnt2.GetString("PACKAGE_NAME", j), iBase+j);
                bufTradeDiscnt_AllDiscnt.SetInt("DISCNT_CODE", bufTradeDiscnt_AllDiscnt2.GetInt("DISCNT_CODE", j), iBase+j);
                //bufTradeDiscnt_AllDiscnt.SetString("ELEMENT_NAME", bufTradeDiscnt_AllDiscnt2.GetString("DISCNT_NAME", j), iBase+j);
                bufTradeDiscnt_AllDiscnt.SetString("ELEMENT_TYPE_CODE", "D", iBase+j);
                bufTradeDiscnt_AllDiscnt.SetString("START_DATE", bufTradeDiscnt_AllDiscnt2.GetString("START_DATE", j), iBase+j);
                bufTradeDiscnt_AllDiscnt.SetString("END_DATE", bufTradeDiscnt_AllDiscnt2.GetString("END_DATE", j), iBase+j);
                //以下用户判断订购依赖
                bufTradeDiscnt_AllDiscnt.SetString("TRADE_ID", bufTradeDiscnt_AllDiscnt2.GetString("TRADE_ID", j), iBase+j);
                bufTradeDiscnt_AllDiscnt.SetString("MODIFY_TAG", bufTradeDiscnt_AllDiscnt2.GetString("MODIFY_TAG", j), iBase+j);

                bufTradeDiscnt_AllDiscnt.SetString("PRODUCT_NAME", bufTradeDiscnt_AllDiscnt2.GetString("PRODUCT_NAME", j), iBase+j);
                bufTradeDiscnt_AllDiscnt.SetString("PACKAGE_NAME", bufTradeDiscnt_AllDiscnt2.GetString("PACKAGE_NAME", j), iBase+j);
                bufTradeDiscnt_AllDiscnt.SetString("DISCNT_NAME", bufTradeDiscnt_AllDiscnt2.GetString("DISCNT_NAME", j), iBase+j);
            }
            iBase += iDiscntCount;
        }
        for(int l=0; l<compICount; l++)
        {
            bool check = false;
            for(int n=0; n<tradeICount; n++)
            {
                if(tradeBuf.GetString("USER_ID",n) == compBuf.GetString("USER_ID",l))
                    check = true;
            }
            if(!check){
                dao.Clear();
                dao.SetParam(":VUSER_ID", compBuf.GetString("USER_ID",l));

                iCount2 = dao.jselect(bufTradeDiscnt_AllDiscnt2, "SEL_ALLDISCNTS_AFTER_TRADE2","TF_B_TRADE_DISCNT");
                //bufTradeDiscnt_AllDiscnt2.printFmlBuffer();
                int iDiscntCount = iCount2;
                for(int k = 0; k < iDiscntCount; k++)
                {
                    bufTradeDiscnt_AllDiscnt.SetString("USER_ID", bufTradeDiscnt_AllDiscnt2.GetString("ID", k), iBase+k);
                    bufTradeDiscnt_AllDiscnt.SetString("ID", bufTradeDiscnt_AllDiscnt2.GetString("ID", k), iBase+k);
                    bufTradeDiscnt_AllDiscnt.SetInt("PRODUCT_ID", bufTradeDiscnt_AllDiscnt2.GetInt("PRODUCT_ID", k), iBase+k);
                    //bufTradeDiscnt_AllDiscnt.SetString("PRODUCT_NAME", bufTradeDiscnt_AllDiscnt2.GetString("PRODUCT_NAME", j), iBase+j);
                    bufTradeDiscnt_AllDiscnt.SetInt("PACKAGE_ID", bufTradeDiscnt_AllDiscnt2.GetInt("PACKAGE_ID", k), iBase+k);
                    //bufTradeDiscnt_AllDiscnt.SetString("PACKAGE_NAME", bufTradeDiscnt_AllDiscnt2.GetString("PACKAGE_NAME", j), iBase+j);
                    bufTradeDiscnt_AllDiscnt.SetInt("DISCNT_CODE", bufTradeDiscnt_AllDiscnt2.GetInt("DISCNT_CODE", k), iBase+k);
                    //bufTradeDiscnt_AllDiscnt.SetString("ELEMENT_NAME", bufTradeDiscnt_AllDiscnt2.GetString("DISCNT_NAME", j), iBase+j);
                    bufTradeDiscnt_AllDiscnt.SetString("ELEMENT_TYPE_CODE", "D", iBase+k);
                    bufTradeDiscnt_AllDiscnt.SetString("START_DATE", bufTradeDiscnt_AllDiscnt2.GetString("START_DATE", k), iBase+k);
                    bufTradeDiscnt_AllDiscnt.SetString("END_DATE", bufTradeDiscnt_AllDiscnt2.GetString("END_DATE", k), iBase+k);
                    //以下用户判断订购依赖
                    bufTradeDiscnt_AllDiscnt.SetString("TRADE_ID", bufTradeDiscnt_AllDiscnt2.GetString("TRADE_ID", k), iBase+k);
                    bufTradeDiscnt_AllDiscnt.SetString("MODIFY_TAG", bufTradeDiscnt_AllDiscnt2.GetString("MODIFY_TAG", k), iBase+k);

                    bufTradeDiscnt_AllDiscnt.SetString("PRODUCT_NAME", bufTradeDiscnt_AllDiscnt2.GetString("PRODUCT_NAME", k), iBase+k);
                    bufTradeDiscnt_AllDiscnt.SetString("PACKAGE_NAME", bufTradeDiscnt_AllDiscnt2.GetString("PACKAGE_NAME", k), iBase+k);
                    bufTradeDiscnt_AllDiscnt.SetString("DISCNT_NAME", bufTradeDiscnt_AllDiscnt2.GetString("DISCNT_NAME", k), iBase+k);
                }
                iBase += iDiscntCount;
            }
        }
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取办理业务后三户的所有优惠出错！[%s]",ex.getCause()->what());
    }

    /*if(iBase > 0 )
    {
        ConvertCodeToName(bufTradeDiscnt_AllDiscnt, "ProductName", "PRODUCT_ID", "PRODUCT_NAME");
        ConvertCodeToName(bufTradeDiscnt_AllDiscnt, "PackageName", "PACKAGE_ID", "PACKAGE_NAME");
        ConvertCodeToName(bufTradeDiscnt_AllDiscnt, "DiscntName", "DISCNT_CODE", "DISCNT_NAME");
    }*/

    bufTradeDiscnt_AllDiscnt.SetInt("X_RECORDNUM", iBase);

    return iBase;
}
/**
 *获取办理业务后用户的所有服务
 *
 */
int TradeCheckAfterTrade::GetUserAllServiceAfterTradeComp(const string &strSubscribeId,
                                                 CFmlBuf &bufTradeSvc_UserAllSvc)
{
    bufTradeSvc_UserAllSvc.ClearFmlValue();
    CFmlBuf bufTradeSvc_UserAllSvc2;
    CFmlBuf tradeBuf;
    CFmlBuf compBuf;
    StaticDAO& dao = StaticDAO::getDAO();
    int tradeICount = 0;
    int compICount = 0;
    int iCount = 0;
    int iCount2 = 0;
    int iBase = 0;

    try
    {
        dao.Clear();
        dao.SetParam(":VSUBSCRIBE_ID", strSubscribeId);
        tradeICount = dao.jselect(tradeBuf, "SEL_BY_SUBSCRIBEID","TF_B_TRADE");
        string strUserId = "";
        string strEparchyCode = "";
        string strCityCode = "";
        string strBrandCode = "";
        if(tradeICount > 0 )
        {
            strUserId = tradeBuf.GetString("USER_ID",0);
            strEparchyCode = tradeBuf.GetString("TRADE_EPARCHY_CODE",0);
            strCityCode = tradeBuf.GetString("TRADE_CITY_CODE",0);
            strBrandCode = tradeBuf.GetString("BRAND_CODE",0);
        }

        //根据品牌判断是组合壳子的user_id或成员user_id，从而查询组合及成员信息
        if(strBrandCode != "" && strBrandCode == "COMP")
        {
            dao.Clear();
            dao.SetParam(":VUSER_ID", strUserId);
            dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
            dao.SetParam(":VCITY_CODE", strCityCode);
            compBuf.ClearFmlValue();
            compICount = dao.jselect(compBuf, "SEL_RELATION_BY_UIDA","TF_F_RELATION_UU");
        }
        else
        {
            dao.Clear();
            dao.SetParam(":VUSER_ID", strUserId);
            dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
            dao.SetParam(":VCITY_CODE", strCityCode);
            compBuf.ClearFmlValue();
            compICount = dao.jselect(compBuf, "SEL_RELATION_BY_UIDB","TF_F_RELATION_UU");
        }
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "查询订单[%s]相关记录出错！",strSubscribeId.c_str());
    }
    if(tradeICount <= 0 )
    {
        THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "没找到相关订单[%s]的记录！", strSubscribeId.c_str());
    }
    try
    {
        for(int i=0; i<tradeICount; i++)
        {
            dao.Clear();
            dao.SetParam(":VTRADE_ID", tradeBuf.GetString("TRADE_ID",i));
            dao.SetParam(":VACCEPT_MONTH", tradeBuf.GetString("TRADE_ID",i).substr(4,2));
            dao.SetParam(":VUSER_ID", tradeBuf.GetString("USER_ID",i));

            iCount2 = dao.jselect(bufTradeSvc_UserAllSvc2, "SEL_ALLSERVICES_AFTER_TRADE","TF_B_TRADE_SVC");
            //bufTradeSvc_UserAllSvc2.printFmlBuffer();
            int iSvcCount = iCount2;
            for(int j = 0; j < iSvcCount; j++)
            {
                bufTradeSvc_UserAllSvc.SetString("USER_ID", bufTradeSvc_UserAllSvc2.GetString("USER_ID", j), iBase+j);
                bufTradeSvc_UserAllSvc.SetInt("PRODUCT_ID", bufTradeSvc_UserAllSvc2.GetInt("PRODUCT_ID", j), iBase+j);
                //bufTradeSvc_UserAllSvc.SetString("PRODUCT_NAME", bufTradeSvc_UserAllSvc2.GetString("PRODUCT_NAME", j), iBase+j);
                bufTradeSvc_UserAllSvc.SetInt("PACKAGE_ID", bufTradeSvc_UserAllSvc2.GetInt("PACKAGE_ID", j), iBase+j);
                //bufTradeSvc_UserAllSvc.SetString("PACKAGE_NAME", bufTradeSvc_UserAllSvc2.GetString("PACKAGE_NAME", j), iBase+j);
                bufTradeSvc_UserAllSvc.SetInt("SERVICE_ID", bufTradeSvc_UserAllSvc2.GetInt("SERVICE_ID", j), iBase+j);
                //bufTradeSvc_UserAllSvc.SetString("SERVICE_NAME", bufTradeSvc_UserAllSvc2.GetString("SERVICE_NAME", j), iBase+j);
                bufTradeSvc_UserAllSvc.SetString("ELEMENT_TYPE_CODE", "S", iBase+j);
                bufTradeSvc_UserAllSvc.SetString("START_DATE", bufTradeSvc_UserAllSvc2.GetString("START_DATE", j), iBase+j);
                bufTradeSvc_UserAllSvc.SetString("END_DATE", bufTradeSvc_UserAllSvc2.GetString("END_DATE", j), iBase+j);
                //以下用户判断订购依赖
                bufTradeSvc_UserAllSvc.SetString("TRADE_ID", bufTradeSvc_UserAllSvc2.GetString("TRADE_ID", j), iBase+j);
                bufTradeSvc_UserAllSvc.SetString("MODIFY_TAG", bufTradeSvc_UserAllSvc2.GetString("MODIFY_TAG", j), iBase+j);

                bufTradeSvc_UserAllSvc.SetString("PRODUCT_NAME", bufTradeSvc_UserAllSvc2.GetString("PRODUCT_NAME", j), iBase+j);
                bufTradeSvc_UserAllSvc.SetString("PACKAGE_NAME", bufTradeSvc_UserAllSvc2.GetString("PACKAGE_NAME", j), iBase+j);
                bufTradeSvc_UserAllSvc.SetString("SERVICE_NAME", bufTradeSvc_UserAllSvc2.GetString("SERVICE_NAME", j), iBase+j);
            }
            iBase += iSvcCount;
        }
        bufTradeSvc_UserAllSvc.printFmlBuffer();
        for(int l=0; l<compICount; l++)
        {
            bool check = false;
            for(int n=0; n<tradeICount; n++)
            {
                if(tradeBuf.GetString("USER_ID",n) == compBuf.GetString("USER_ID",l))
                    check = true;
            }
            if(!check){
                dao.Clear();
                dao.SetParam(":VUSER_ID", compBuf.GetString("USER_ID",l));

                iCount2 = dao.jselect(bufTradeSvc_UserAllSvc2, "SEL_ALLSERVICES_AFTER_TRADE2","TF_B_TRADE_SVC");
                //bufTradeSvc_UserAllSvc2.printFmlBuffer();
                int iSvcCount = iCount2;
                for(int k = 0; k < iSvcCount; k++)
                {
                    bufTradeSvc_UserAllSvc.SetString("USER_ID", bufTradeSvc_UserAllSvc2.GetString("USER_ID", k), iBase+k);
                    bufTradeSvc_UserAllSvc.SetInt("PRODUCT_ID", bufTradeSvc_UserAllSvc2.GetInt("PRODUCT_ID", k), iBase+k);
                    //bufTradeSvc_UserAllSvc.SetString("PRODUCT_NAME", bufTradeSvc_UserAllSvc2.GetString("PRODUCT_NAME", k), iBase+k);
                    bufTradeSvc_UserAllSvc.SetInt("PACKAGE_ID", bufTradeSvc_UserAllSvc2.GetInt("PACKAGE_ID", k), iBase+k);
                    //bufTradeSvc_UserAllSvc.SetString("PACKAGE_NAME", bufTradeSvc_UserAllSvc2.GetString("PACKAGE_NAME", k), iBase+k);
                    bufTradeSvc_UserAllSvc.SetInt("SERVICE_ID", bufTradeSvc_UserAllSvc2.GetInt("SERVICE_ID", k), iBase+k);
                    //bufTradeSvc_UserAllSvc.SetString("SERVICE_NAME", bufTradeSvc_UserAllSvc2.GetString("SERVICE_NAME", k), iBase+k;
                    bufTradeSvc_UserAllSvc.SetString("ELEMENT_TYPE_CODE", "S", iBase+k);
                    bufTradeSvc_UserAllSvc.SetString("START_DATE", bufTradeSvc_UserAllSvc2.GetString("START_DATE", k), iBase+k);
                    bufTradeSvc_UserAllSvc.SetString("END_DATE", bufTradeSvc_UserAllSvc2.GetString("END_DATE", k), iBase+k);
                    //以下用户判断订购依赖
                    bufTradeSvc_UserAllSvc.SetString("TRADE_ID", bufTradeSvc_UserAllSvc2.GetString("TRADE_ID", k), iBase+k);
                    bufTradeSvc_UserAllSvc.SetString("MODIFY_TAG", bufTradeSvc_UserAllSvc2.GetString("MODIFY_TAG", k), iBase+k);

                    bufTradeSvc_UserAllSvc.SetString("PRODUCT_NAME", bufTradeSvc_UserAllSvc2.GetString("PRODUCT_NAME", k), iBase+k);
                    bufTradeSvc_UserAllSvc.SetString("PACKAGE_NAME", bufTradeSvc_UserAllSvc2.GetString("PACKAGE_NAME", k), iBase+k);
                    bufTradeSvc_UserAllSvc.SetString("SERVICE_NAME", bufTradeSvc_UserAllSvc2.GetString("SERVICE_NAME", k), iBase+k);
                }
                iBase += iSvcCount;
            }
        }
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取办理业务后三户的所有服务出错！[%s]",ex.getCause()->what());
    }

    /*if(iBase > 0 )
    {
        ConvertCodeToName(bufTradeSvc_UserAllSvc, "ProductName", "PRODUCT_ID", "PRODUCT_NAME");
        ConvertCodeToName(bufTradeSvc_UserAllSvc, "PackageName", "PACKAGE_ID", "PACKAGE_NAME");
        ConvertCodeToName(bufTradeSvc_UserAllSvc, "ServiceName", "SERVICE_ID", "SERVICE_NAME");
    }*/

    bufTradeSvc_UserAllSvc.SetInt("X_RECORDNUM", iBase);

    return iBase;
}
int TradeCheckAfterTrade::GetUserAllElementAfterTradeComp(const string &strSubscribeId,
                                                 CFmlBuf &bufTradeElement_UserAllElement)
{
    bufTradeElement_UserAllElement.ClearFmlValue();
    CFmlBuf bufTradeElement_UserAllElement2;
    CFmlBuf tradeBuf;
    CFmlBuf compBuf;
    StaticDAO& dao = StaticDAO::getDAO();
    int tradeICount = 0;
    int compICount = 0;
    int iCount = 0;
    int iCount2 = 0;
    int iBase = 0;

    try
    {
        dao.Clear();
        dao.SetParam(":VSUBSCRIBE_ID", strSubscribeId);
        tradeICount = dao.jselect(tradeBuf, "SEL_BY_SUBSCRIBEID","TF_B_TRADE");
        string strUserId = "";
        string strEparchyCode = "";
        string strCityCode = "";
        string strBrandCode = "";
        if(tradeICount > 0 )
        {
            strUserId = tradeBuf.GetString("USER_ID",0);
            strEparchyCode = tradeBuf.GetString("TRADE_EPARCHY_CODE",0);
            strCityCode = tradeBuf.GetString("TRADE_CITY_CODE",0);
            strBrandCode = tradeBuf.GetString("BRAND_CODE",0);
        }

        //根据品牌判断是组合壳子的user_id或成员user_id，从而查询组合及成员信息
        if(strBrandCode != "" && strBrandCode == "COMP")
        {
            dao.Clear();
            dao.SetParam(":VUSER_ID", strUserId);
            dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
            dao.SetParam(":VCITY_CODE", strCityCode);
            compBuf.ClearFmlValue();
            compICount = dao.jselect(compBuf, "SEL_RELATION_BY_UIDA","TF_F_RELATION_UU");
        }
        else
        {
            dao.Clear();
            dao.SetParam(":VUSER_ID", strUserId);
            dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
            dao.SetParam(":VCITY_CODE", strCityCode);
            compBuf.ClearFmlValue();
            compICount = dao.jselect(compBuf, "SEL_RELATION_BY_UIDB","TF_F_RELATION_UU");
        }
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "查询订单[%s]相关记录出错！",strSubscribeId.c_str());
    }
    if(tradeICount <= 0 )
    {
        THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "没找到相关订单[%s]的记录！", strSubscribeId.c_str());
    }
    try
    {
        for(int i=0; i<tradeICount; i++)
        {
            dao.Clear();
            dao.SetParam(":VTRADE_ID", tradeBuf.GetString("TRADE_ID",i));
            dao.SetParam(":VACCEPT_MONTH", tradeBuf.GetString("TRADE_ID",i).substr(4,2));
            dao.SetParam(":VUSER_ID", tradeBuf.GetString("USER_ID",i));

            iCount2 = dao.jselect(bufTradeElement_UserAllElement2, "SEL_ALLELEMENT_AFTER_TRADE","TF_B_TRADE_ELEMENT");
            //bufTradeElement_UserAllElement2.printFmlBuffer();
            int iElementCount = iCount2;
            for(int j = 0; j < iElementCount; j++)
            {
                bufTradeElement_UserAllElement.SetString("USER_ID", bufTradeElement_UserAllElement2.GetString("USER_ID", j), iBase+j);
                bufTradeElement_UserAllElement.SetInt("PRODUCT_ID", bufTradeElement_UserAllElement2.GetInt("PRODUCT_ID", j), iBase+j);
                //bufTradeElement_UserAllElement.SetString("PRODUCT_NAME", bufTradeElement_UserAllElement2.GetString("PRODUCT_NAME", j), iBase+j);
                bufTradeElement_UserAllElement.SetInt("PACKAGE_ID", bufTradeElement_UserAllElement2.GetInt("PACKAGE_ID", j), iBase+j);
                //bufTradeElement_UserAllElement.SetString("PACKAGE_NAME", bufTradeElement_UserAllElement2.GetString("PACKAGE_NAME", j), iBase+j);
                bufTradeElement_UserAllElement.SetInt("ID", bufTradeElement_UserAllElement2.GetInt("ID", j), iBase+j);
                //bufTradeElement_UserAllElement.SetString("ELEMENT_NAME", bufTradeElement_UserAllElement2.GetString("ELEMENT_NAME", j), iBase+j);
                bufTradeElement_UserAllElement.SetString("ID_TYPE", bufTradeElement_UserAllElement2.GetString("ID_TYPE", j), iBase+j);
                //bufTradeElement_UserAllElement.SetString("ACTION_NAME", bufTradeElement_UserAllElement2.GetString("ACTION_NAME", j), iBase+j);
                //bufTradeElement_UserAllElement.SetString("PARENT_ACTION_NAME", bufTradeElement_UserAllElement2.GetString("PARENT_ACTION_NAME", j), iBase+j);
                bufTradeElement_UserAllElement.SetString("START_DATE", bufTradeElement_UserAllElement2.GetString("START_DATE", j), iBase+j);
                bufTradeElement_UserAllElement.SetString("END_DATE", bufTradeElement_UserAllElement2.GetString("END_DATE", j), iBase+j);
                //以下用户判断订购依赖
                bufTradeElement_UserAllElement.SetString("TRADE_ID", bufTradeElement_UserAllElement2.GetString("TRADE_ID", j), iBase+j);
                bufTradeElement_UserAllElement.SetString("MODIFY_TAG", bufTradeElement_UserAllElement2.GetString("MODIFY_TAG", j), iBase+j);

                bufTradeElement_UserAllElement.SetString("PRODUCT_NAME", bufTradeElement_UserAllElement2.GetString("PRODUCT_NAME", j), iBase+j);
                bufTradeElement_UserAllElement.SetString("PACKAGE_NAME", bufTradeElement_UserAllElement2.GetString("PACKAGE_NAME", j), iBase+j);
            }
            iBase += iElementCount;
        }
        for(int l=0; l<compICount; l++)
        {
            bool check = false;
            for(int n=0; n<tradeICount; n++)
            {
                if(tradeBuf.GetString("USER_ID",n) == compBuf.GetString("USER_ID",l))
                    check = true;
            }
            if(!check){
                dao.Clear();
                dao.SetParam(":VUSER_ID", compBuf.GetString("USER_ID",l));

                iCount2 = dao.jselect(bufTradeElement_UserAllElement2, "SEL_ALLELEMENT_AFTER_TRADE2","TF_B_TRADE_ELEMENT");
                //bufTradeElement_UserAllElement2.printFmlBuffer();
                int iElementCount = iCount2;
                for(int k = 0; k < iElementCount; k++)
                {
                    bufTradeElement_UserAllElement.SetString("USER_ID", bufTradeElement_UserAllElement2.GetString("USER_ID", k), iBase+k);
                    bufTradeElement_UserAllElement.SetInt("PRODUCT_ID", bufTradeElement_UserAllElement2.GetInt("PRODUCT_ID", k), iBase+k);
                    //bufTradeElement_UserAllElement.SetString("PRODUCT_NAME", bufTradeElement_UserAllElement2.GetString("PRODUCT_NAME", k), iBase+k);
                    bufTradeElement_UserAllElement.SetInt("PACKAGE_ID", bufTradeElement_UserAllElement2.GetInt("PACKAGE_ID", k), iBase+k);
                    //bufTradeElement_UserAllElement.SetString("PACKAGE_NAME", bufTradeElement_UserAllElement2.GetString("PACKAGE_NAME", k), iBase+k);
                    bufTradeElement_UserAllElement.SetInt("ID", bufTradeElement_UserAllElement2.GetInt("ID", k), iBase+k);
                    //bufTradeElement_UserAllElement.SetString("ELEMENT_NAME", bufTradeElement_UserAllElement2.GetString("ELEMENT_NAME", k), iBase+k);
                    bufTradeElement_UserAllElement.SetString("ID_TYPE", bufTradeElement_UserAllElement2.GetString("ID_TYPE", k), iBase+k);
                    //bufTradeElement_UserAllElement.SetString("ACTION_NAME", bufTradeElement_UserAllElement2.GetString("ACTION_NAME", k), iBase+k);
                    //bufTradeElement_UserAllElement.SetString("PARENT_ACTION_NAME", bufTradeElement_UserAllElement2.GetString("PARENT_ACTION_NAME", k), iBase+k);
                    bufTradeElement_UserAllElement.SetString("START_DATE", bufTradeElement_UserAllElement2.GetString("START_DATE", k), iBase+k);
                    bufTradeElement_UserAllElement.SetString("END_DATE", bufTradeElement_UserAllElement2.GetString("END_DATE", k), iBase+k);
                    //以下用户判断订购依赖
                    bufTradeElement_UserAllElement.SetString("TRADE_ID", bufTradeElement_UserAllElement2.GetString("TRADE_ID", k), iBase+k);
                    bufTradeElement_UserAllElement.SetString("MODIFY_TAG", bufTradeElement_UserAllElement2.GetString("MODIFY_TAG", k), iBase+k);

                    bufTradeElement_UserAllElement.SetString("PRODUCT_NAME", bufTradeElement_UserAllElement2.GetString("PRODUCT_NAME", k), iBase+k);
                    bufTradeElement_UserAllElement.SetString("PACKAGE_NAME", bufTradeElement_UserAllElement2.GetString("PACKAGE_NAME", k), iBase+k);
                }
                iBase += iElementCount;
            }
        }
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取办理业务后三户的所有元素出错！[%s]",ex.getCause()->what());
    }

    if(iBase > 0 )
    {
        /*
        ConvertCodeToName(bufTradeElement_UserAllElement, "ProductName", "PRODUCT_ID", "PRODUCT_NAME");
        ConvertCodeToName(bufTradeElement_UserAllElement, "PackageName", "PACKAGE_ID", "PACKAGE_NAME");
        ConvertCodeToName(bufTradeElement_UserAllElement, "ActionName", "ID", "ACTION_NAME");
        ConvertCodeToName(bufTradeElement_UserAllElement, "ParentActionName", "ID", "PARENT_ACTION_NAME");
        */
        //char cTemp[10];
        for(int i = 0; i < iBase; i++)
        {
            /*cTemp[0] = '\0';
            sprintf(cTemp,"%d",(int)bufTradeElement_UserAllElement.GetInt("PRODUCT_ID", i));
            bufTradeElement_UserAllElement.SetString("PRODUCT_NAME", CParamDAO::getParam("ProductName", cTemp),i);
            cTemp[0] = '\0';
            sprintf(cTemp,"%d",(int)bufTradeElement_UserAllElement.GetInt("PACKAGE_ID", i));
            bufTradeElement_UserAllElement.SetString("PACKAGE_NAME", CParamDAO::getParam("PackageName", cTemp),i);
            */
            if(bufTradeElement_UserAllElement.GetString("ID_TYPE", i) == "A" ||
                bufTradeElement_UserAllElement.GetString("ID_TYPE", i) == "C")
            {
                bufTradeElement_UserAllElement.SetString("ACTION_NAME", CParamDAO::getParam("ActionName", bufTradeElement_UserAllElement.GetString("ID", i)),i);
            }
            else
            {
                bufTradeElement_UserAllElement.SetString("PARENT_ACTION_NAME", CParamDAO::getParam("ParentActionName", bufTradeElement_UserAllElement.GetString("ID", i)),i);
            }
        }
    }

    bufTradeElement_UserAllElement.SetInt("X_RECORDNUM", iBase);

    return iBase;
}
int TradeCheckAfterTrade::GetUserAllSpAfterTradeComp(const string &strSubscribeId,
                                                 CFmlBuf &bufTradeSp_UserAllSp)
{
    bufTradeSp_UserAllSp.ClearFmlValue();
    CFmlBuf bufTradeSp_UserAllSp2;
    CFmlBuf tradeBuf;
    CFmlBuf compBuf;
    StaticDAO& dao = StaticDAO::getDAO();
    int tradeICount = 0;
    int compICount = 0;
    int iCount = 0;
    int iCount2 = 0;
    int iBase = 0;

    try
    {
        dao.Clear();
        dao.SetParam(":VSUBSCRIBE_ID", strSubscribeId);
        tradeICount = dao.jselect(tradeBuf, "SEL_BY_SUBSCRIBEID","TF_B_TRADE");
        string strUserId = "";
        string strEparchyCode = "";
        string strCityCode = "";
        string strBrandCode = "";
        if(tradeICount > 0 )
        {
            strUserId = tradeBuf.GetString("USER_ID",0);
            strEparchyCode = tradeBuf.GetString("TRADE_EPARCHY_CODE",0);
            strCityCode = tradeBuf.GetString("TRADE_CITY_CODE",0);
            strBrandCode = tradeBuf.GetString("BRAND_CODE",0);
        }

        //根据品牌判断是组合壳子的user_id或成员user_id，从而查询组合及成员信息
        if(strBrandCode != "" && strBrandCode == "COMP")
        {
            dao.Clear();
            dao.SetParam(":VUSER_ID", strUserId);
            dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
            dao.SetParam(":VCITY_CODE", strCityCode);
            compBuf.ClearFmlValue();
            compICount = dao.jselect(compBuf, "SEL_RELATION_BY_UIDA","TF_F_RELATION_UU");
        }
        else
        {
            dao.Clear();
            dao.SetParam(":VUSER_ID", strUserId);
            dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
            dao.SetParam(":VCITY_CODE", strCityCode);
            compBuf.ClearFmlValue();
            compICount = dao.jselect(compBuf, "SEL_RELATION_BY_UIDB","TF_F_RELATION_UU");
        }
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "查询订单[%s]相关记录出错！",strSubscribeId.c_str());
    }
    if(tradeICount <= 0 )
    {
        THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "没找到相关订单[%s]的记录！", strSubscribeId.c_str());
    }
    try
    {
        for(int i=0; i<tradeICount; i++)
        {
            dao.Clear();
            dao.SetParam(":VTRADE_ID", tradeBuf.GetString("TRADE_ID",i));
            dao.SetParam(":VACCEPT_MONTH", tradeBuf.GetString("TRADE_ID",i).substr(4,2));
            dao.SetParam(":VUSER_ID", tradeBuf.GetString("USER_ID",i));

            iCount2 = dao.jselect(bufTradeSp_UserAllSp2, "SEL_ALLSP_AFTER_TRADE","TF_B_TRADE_SP");
            //bufTradeSp_UserAllSp2.printFmlBuffer();
            int iSPCount = iCount2;
            for(int j = 0; j < iSPCount; j++)
            {
                bufTradeSp_UserAllSp.SetString("USER_ID", bufTradeSp_UserAllSp2.GetString("USER_ID", j), iBase+j);
                bufTradeSp_UserAllSp.SetInt("PRODUCT_ID", bufTradeSp_UserAllSp2.GetInt("PRODUCT_ID", j), iBase+j);
                //bufTradeSp_UserAllSp.SetString("PRODUCT_NAME", bufTradeSp_UserAllSp2.GetString("PRODUCT_NAME", j), iBase+j);
                bufTradeSp_UserAllSp.SetInt("PACKAGE_ID", bufTradeSp_UserAllSp2.GetInt("PACKAGE_ID", j), iBase+j);
                //bufTradeSp_UserAllSp.SetString("PACKAGE_NAME", bufTradeSp_UserAllSp2.GetString("PACKAGE_NAME", j), iBase+j);
                bufTradeSp_UserAllSp.SetString("SP_SERVICE_ID", bufTradeSp_UserAllSp2.GetString("SP_SERVICE_ID", j), iBase+j);
                //bufTradeSp_UserAllSp.SetInt("SP_PRODUCT_NAME", bufTradeSp_UserAllSp2.GetInt("SP_PRODUCT_NAME", j), iBase+j);
                bufTradeSp_UserAllSp.SetString("ELEMENT_TYPE_CODE", "X", iBase+j);
                bufTradeSp_UserAllSp.SetString("START_DATE", bufTradeSp_UserAllSp2.GetString("START_DATE", j), iBase+j);
                bufTradeSp_UserAllSp.SetString("END_DATE", bufTradeSp_UserAllSp2.GetString("END_DATE", j), iBase+j);
                //以下用户判断订购依赖
                bufTradeSp_UserAllSp.SetString("TRADE_ID", bufTradeSp_UserAllSp2.GetString("TRADE_ID", j), iBase+j);
                bufTradeSp_UserAllSp.SetString("MODIFY_TAG", bufTradeSp_UserAllSp2.GetString("MODIFY_TAG", j), iBase+j);

                bufTradeSp_UserAllSp.SetString("PRODUCT_NAME", bufTradeSp_UserAllSp2.GetString("PRODUCT_NAME", j), iBase+j);
                bufTradeSp_UserAllSp.SetString("PACKAGE_NAME", bufTradeSp_UserAllSp2.GetString("PACKAGE_NAME", j), iBase+j);
                bufTradeSp_UserAllSp.SetString("SP_PRODUCT_NAME", bufTradeSp_UserAllSp2.GetString("SP_PRODUCT_NAME", j), iBase+j);
            }
            iBase += iSPCount;
        }
        for(int l=0; l<compICount; l++)
        {
            bool check = false;
            for(int n=0; n<tradeICount; n++)
            {
                if(tradeBuf.GetString("USER_ID",n) == compBuf.GetString("USER_ID",l))
                    check = true;
            }
            if(!check){
                dao.Clear();
                dao.SetParam(":VUSER_ID", compBuf.GetString("USER_ID",l));

                iCount2 = dao.jselect(bufTradeSp_UserAllSp2, "SEL_ALLSP_AFTER_TRADE2","TF_B_TRADE_SP");
                //bufTradeSp_UserAllSp2.printFmlBuffer();
                int iSPCount = iCount2;
                for(int k = 0; k < iSPCount; k++)
                {
                    bufTradeSp_UserAllSp.SetString("USER_ID", bufTradeSp_UserAllSp2.GetString("USER_ID", k), iBase+k);
                    bufTradeSp_UserAllSp.SetInt("PRODUCT_ID", bufTradeSp_UserAllSp2.GetInt("PRODUCT_ID", k), iBase+k);
                    //bufTradeSp_UserAllSp.SetString("PRODUCT_NAME", bufTradeSp_UserAllSp2.GetString("PRODUCT_NAME", k), iBase+k);
                    bufTradeSp_UserAllSp.SetInt("PACKAGE_ID", bufTradeSp_UserAllSp2.GetInt("PACKAGE_ID", k), iBase+k);
                    //bufTradeSp_UserAllSp.SetString("PACKAGE_NAME", bufTradeSp_UserAllSp2.GetString("PACKAGE_NAME", k), iBase+k);
                    bufTradeSp_UserAllSp.SetString("SP_SERVICE_ID", bufTradeSp_UserAllSp2.GetString("SP_SERVICE_ID", k), iBase+k);
                    //bufTradeSp_UserAllSp.SetInt("SP_PRODUCT_NAME", bufTradeSp_UserAllSp2.GetInt("SP_PRODUCT_NAME", k), iBase+k);
                    bufTradeSp_UserAllSp.SetString("ELEMENT_TYPE_CODE", "X", iBase+k);
                    bufTradeSp_UserAllSp.SetString("START_DATE", bufTradeSp_UserAllSp2.GetString("START_DATE", k), iBase+k);
                    bufTradeSp_UserAllSp.SetString("END_DATE", bufTradeSp_UserAllSp2.GetString("END_DATE", k), iBase+k);
                    //以下用户判断订购依赖
                    bufTradeSp_UserAllSp.SetString("TRADE_ID", bufTradeSp_UserAllSp2.GetString("TRADE_ID", k), iBase+k);
                    bufTradeSp_UserAllSp.SetString("MODIFY_TAG", bufTradeSp_UserAllSp2.GetString("MODIFY_TAG", k), iBase+k);

                    bufTradeSp_UserAllSp.SetString("PRODUCT_NAME", bufTradeSp_UserAllSp2.GetString("PRODUCT_NAME", k), iBase+k);
                    bufTradeSp_UserAllSp.SetString("PACKAGE_NAME", bufTradeSp_UserAllSp2.GetString("PACKAGE_NAME", k), iBase+k);
                    bufTradeSp_UserAllSp.SetString("SP_PRODUCT_NAME", bufTradeSp_UserAllSp2.GetString("SP_PRODUCT_NAME", k), iBase+k);
                }
                iBase += iSPCount;
            }
        }
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取办理业务后三户的所有SP出错！[%s]",ex.getCause()->what());
    }

    /*if(iBase > 0 )
    {
        ConvertCodeToName(bufTradeSp_UserAllSp, "ProductName", "PRODUCT_ID", "PRODUCT_NAME");
        ConvertCodeToName(bufTradeSp_UserAllSp, "PackageName", "PACKAGE_ID", "PACKAGE_NAME");
        ConvertCodeToName(bufTradeSp_UserAllSp, "SpProductName", "SP_SERVICE_ID", "SP_PRODUCT_NAME");
    }*/

    bufTradeSp_UserAllSp.SetInt("X_RECORDNUM", iBase);

    return iBase;
}
//全局元素之间限制关系判断
void TradeCheckAfterTrade::CheckElementLimitComp(CFmlBuf &bufUserAllElements, const string &strEparchyCode,const string &strSubscribeId)
{
    CFmlBuf bufElementLimit;
    string strErrorInfo = "";
    int iCount = 0;
    bool bFound = false;
    int i,j,z,k;

    //按用户元素循环校验元素互斥关系
    int iUserElements = bufUserAllElements.GetInt("X_RECORDNUM");
    LOG_TRACE_P1(logTradeCheckAfterTrade, "*********************iUserElements:%d-----", iUserElements);
    bufUserAllElements.printFmlBuffer();
    for(i = 0; i < iUserElements; i++)
    {
        iCount = 0;
        bufElementLimit.ClearFmlValue();

        //获取元素互斥信息
        iCount = ExistsElementLimitA(bufUserAllElements.GetString("ELEMENT_TYPE_CODE", i),
                                     bufUserAllElements.GetInt("ELEMENT_ID", i),
                                     "4", strEparchyCode, bufElementLimit);

        for(j = 0; j < iCount; j++)
        {
            bFound = false;
            for(z = 0; z < iUserElements; z++)
            {
                if (i==z)continue; //自身不做互斥判断 added by tangz@2009-2-7 11:33
                if(bufUserAllElements.GetInt("ELEMENT_ID", z) == bufElementLimit.GetInt("ELEMENT_ID_B", j)
                    && bufUserAllElements.GetString("ELEMENT_TYPE_CODE", z) == bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", j)
                    //时间交集
                    && (bufUserAllElements.GetString("START_DATE",z) <= bufUserAllElements.GetString("START_DATE",i)
                    && bufUserAllElements.GetString("START_DATE",i) <= bufUserAllElements.GetString("END_DATE",z)
                    ||
                    bufUserAllElements.GetString("START_DATE",i) <= bufUserAllElements.GetString("START_DATE",z)
                    && bufUserAllElements.GetString("START_DATE",z) <= bufUserAllElements.GetString("END_DATE",i)))
                {
                    bFound = true;
                    break;
                }
            }

            if(bFound)
            {
                if(strErrorInfo == "")
                    strErrorInfo = "\"" + bufUserAllElements.GetString("PRODUCT_NAME", i)
                        + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", i)
                        + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                        + "\"与\"" +  bufUserAllElements.GetString("PRODUCT_NAME", z)
                        + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", z)
                        + " --> " +  bufUserAllElements.GetString("ELEMENT_NAME", z) + "\"";
                else
                    strErrorInfo += "、\"" + bufUserAllElements.GetString("PRODUCT_NAME", i)
                        + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", i)
                        + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                        + "\"与\"" +  bufUserAllElements.GetString("PRODUCT_NAME", z)
                        + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", z)
                        + " --> " +  bufUserAllElements.GetString("ELEMENT_NAME", z) + "\"";
            }
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strErrorInfo + "互斥，业务无法继续！");

    StaticDAO& dao = StaticDAO::getDAO();
    CFmlBuf tradeBuf;
    int tradeICount;
    try
    {
        dao.Clear();
        dao.SetParam(":VSUBSCRIBE_ID", strSubscribeId);
        tradeICount = dao.jselect(tradeBuf, "SEL_BY_PROD_SUBSCRIBEID","TF_B_TRADE");
    }catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "获取订单出错!");
    }
    tradeBuf.printFmlBuffer();
    //按用户元素循环校验元素部份依赖关系
    strErrorInfo = "";
    for(i = 0; i < iUserElements; i++)
    {
        iCount = 0;
        bufElementLimit.ClearFmlValue();

        //获取元素部份依赖信息
        iCount = ExistsElementLimitA(bufUserAllElements.GetString("ELEMENT_TYPE_CODE", i),
                                     bufUserAllElements.GetInt("ELEMENT_ID", i),
                                     "5", strEparchyCode, bufElementLimit);

        bFound = true;
        for(j = 0; j < iCount; j++)
        {
            bFound = false;
            int ip=0;
            int ie=0;
            int strproid = 0;
            CFmlBuf tmpBuf;
            for(k = 0; k < tradeICount; k++)
            {
                strproid = tradeBuf.GetInt("PRODUCT_ID", k);

                int iRowCount;
                try
                {
                    dao.Clear();
                    dao.SetParam(":VPRODUCT_ID", (int)strproid);
                    dao.SetParam(":VELEMENT_ID", (int)bufElementLimit.GetInt("ELEMENT_ID_B", j));
                    dao.SetParam(":VELEMENT_TYPE_CODE", bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", j));
                    iRowCount = dao.jselect(tmpBuf, "SEL_BY_ELEMENT", "TD_B_PRODUCT");
                }catch(Exception &ex)
                {
                    THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "元素限制判断，查询产品出错!");
                }
                ip += iRowCount;
                LOG_TRACE_P1(logTradeCheckAfterTrade, "***************strproid**%d************", strproid);
            }

            for(z = 0; z < iUserElements; z++)
            {
                if(bufUserAllElements.GetInt("ELEMENT_ID", z) == bufElementLimit.GetInt("ELEMENT_ID_B", j)
                    && bufUserAllElements.GetString("ELEMENT_TYPE_CODE", z) == bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", j)
                    //时间交集
                    && (bufUserAllElements.GetString("START_DATE",z) <= bufUserAllElements.GetString("START_DATE",i)
                    && bufUserAllElements.GetString("START_DATE",i) <= bufUserAllElements.GetString("END_DATE",z)
                    ||
                    bufUserAllElements.GetString("START_DATE",i) <= bufUserAllElements.GetString("START_DATE",z)
                    && bufUserAllElements.GetString("START_DATE",z) <= bufUserAllElements.GetString("END_DATE",i)))
                {

                    ie++;
                    //break;
                }
            }
            if(ie == ip)
            {
                bFound = true;
                break;
            }
            /*if(bFound)
            {
                break;
            }*/
        }

        if(!bFound)
        {
            //(i)部份依赖于(j)
            //部份依赖元素拼串
            string strInfo = "";
            for(int y = 0; y < iCount; y++)
            {
                if(strInfo == "")
                {
                    if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "D")
                        strInfo = bufElementLimit.GetString("DISCNT_NAME", y);
                    else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "S")
                        strInfo = bufElementLimit.GetString("SERVICE_NAME", y);
                    else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "X")
                        strInfo = bufElementLimit.GetString("SP_PRODUCT_NAME", y);
                    else
                        strInfo = bufElementLimit.GetString("ACTION_NAME", y);
                }
                else
                {
                    if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "D")
                        strInfo += "、" + bufElementLimit.GetString("DISCNT_NAME", y);
                    else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "S")
                        strInfo += "、" + bufElementLimit.GetString("SERVICE_NAME", y);
                    else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "X")
                        strInfo += "、" + bufElementLimit.GetString("SP_PRODUCT_NAME", y);
                    else
                        strInfo += "、" + bufElementLimit.GetString("ACTION_NAME", y);
                }
            }

            if(strErrorInfo == "")
                strErrorInfo = "\"" + bufUserAllElements.GetString("PRODUCT_NAME", i)
                    + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", i)
                    + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                    + "\"部份依赖于元素：\"" +  strInfo + "\"";
            else
                strErrorInfo += "、\"" + bufUserAllElements.GetString("PRODUCT_NAME", i)
                    + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", i)
                    + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                    + "\"部份依赖于元素：\"" +  strInfo + "\"";
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strErrorInfo + "，业务无法继续！");


    //按用户元素循环校验包内元素完全依赖关系
    strErrorInfo = "";
    for(i = 0; i < iUserElements; i++)
    {
        iCount = 0;
        bufElementLimit.ClearFmlValue();

        //获取元素完全依赖信息
        iCount = ExistsElementLimitA(bufUserAllElements.GetString("ELEMENT_TYPE_CODE", i),
                                     bufUserAllElements.GetInt("ELEMENT_ID", i),
                                     "6", strEparchyCode, bufElementLimit);

        for(j = 0; j < iCount; j++)
        {
            bFound = false;
            int ip=0;
            int ie=0;
            int strproid = 0;
            CFmlBuf tmpBuf;
            for(k = 0; k < tradeICount; k++)
            {
                strproid = tradeBuf.GetInt("PRODUCT_ID", k);

                int iRowCount;
                try
                {
                    dao.Clear();
                    dao.SetParam(":VPRODUCT_ID", (int)strproid);
                    dao.SetParam(":VELEMENT_ID", (int)bufElementLimit.GetInt("ELEMENT_ID_B", j));
                    dao.SetParam(":VELEMENT_TYPE_CODE", bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", j));
                    iRowCount = dao.jselect(tmpBuf, "SEL_BY_ELEMENT", "TD_B_PRODUCT");
                }catch(Exception &ex)
                {
                    THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "元素限制判断，查询产品出错!");
                }
                ip += iRowCount;
                LOG_TRACE_P2(logTradeCheckAfterTrade, "***************strproid**%d******%d******", strproid,iRowCount);
            }
			LOG_TRACE_P1(logTradeCheckAfterTrade, "***************ip**%d************", ip);



            for(z = 0; z < iUserElements; z++)
            {
                if(bufUserAllElements.GetInt("ELEMENT_ID", z) == bufElementLimit.GetInt("ELEMENT_ID_B", j)
                    && bufUserAllElements.GetString("ELEMENT_TYPE_CODE", z) == bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", j)
                    //时间交集
                    && (bufUserAllElements.GetString("START_DATE",z) <= bufUserAllElements.GetString("START_DATE",i)
                    && bufUserAllElements.GetString("START_DATE",i) <= bufUserAllElements.GetString("END_DATE",z)
                    ||
                    bufUserAllElements.GetString("START_DATE",i) <= bufUserAllElements.GetString("START_DATE",z)
                    && bufUserAllElements.GetString("START_DATE",z) <= bufUserAllElements.GetString("END_DATE",i)))
                {
                    ie++;
                    //bFound = true;
                    //break;
                }
            }

            LOG_TRACE_P1(logTradeCheckAfterTrade, "***************ie**%d************", ie);

            if(ie == ip)
            {
               bFound = true;
                //break;
            }
            else
               bFound = false;

            if(!bFound||ie==0)//modify by zhangyangshuo  增加 ie ==0 条件
            {
                //(i)完全依赖于(j)
                //完全依赖元素拼串
                string strInfo = "";
                for(int y = 0; y < iCount; y++)
                {
                    if(strInfo == "")
                    {
                        if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "D")
                            strInfo = bufElementLimit.GetString("DISCNT_NAME", y);
                        else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "S")
                            strInfo = bufElementLimit.GetString("SERVICE_NAME", y);
                        else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "X")
                            strInfo = bufElementLimit.GetString("SP_PRODUCT_NAME", y);
                        else
                            strInfo = bufElementLimit.GetString("ACTION_NAME", y);
                    }
                    else
                    {
                        if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "D")
                            strInfo += "、" + bufElementLimit.GetString("DISCNT_NAME", y);
                        else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "S")
                            strInfo += "、" + bufElementLimit.GetString("SERVICE_NAME", y);
                        else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "X")
                            strInfo += "、" + bufElementLimit.GetString("SP_PRODUCT_NAME", y);
                        else
                            strInfo += "、" + bufElementLimit.GetString("ACTION_NAME", y);
                    }
                }

                if(strErrorInfo == "")
                    strErrorInfo = "\"" + bufUserAllElements.GetString("PRODUCT_NAME", i)
                        + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", i)
                        + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                        + "\"完全依赖于元素：\"" +  strInfo + "\"";
                else
                    strErrorInfo += "、\"" + bufUserAllElements.GetString("PRODUCT_NAME", i)
                        + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", i)
                        + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                        + "\"完全依赖于元素：\"" +  strInfo + "\"";

                break;
            }
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strErrorInfo + "，业务无法继续！");

    //按用户元素循环校验包内元素订购依赖关系
    strErrorInfo = "";
    for(i = 0; i < iUserElements&&bufUserAllElements.GetString("TRADE_ID", i)!="0"
         &&(bufUserAllElements.GetString("MODIFY_TAG", i)=="A"||bufUserAllElements.GetString("MODIFY_TAG", i)=="0"); i++)//只检查本次订购的元素
    {
        iCount = 0;
        bufElementLimit.ClearFmlValue();

        //获取元素订购依赖信息
        iCount = ExistsElementLimitA(bufUserAllElements.GetString("ELEMENT_TYPE_CODE", i),
                                     bufUserAllElements.GetInt("ELEMENT_ID", i),
                                     "3", strEparchyCode, bufElementLimit);
        for(j = 0; j < iCount; j++)
        {
            bFound = false;
            for(z = 0; z < iUserElements; z++)
            {
                if(bufUserAllElements.GetInt("ELEMENT_ID", z) == bufElementLimit.GetInt("ELEMENT_ID_B", j)
                    && bufUserAllElements.GetString("ELEMENT_TYPE_CODE", z) == bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", j)
                    //时间交集
                    && (bufUserAllElements.GetString("START_DATE",z) <= bufUserAllElements.GetString("START_DATE",i)
                    && bufUserAllElements.GetString("START_DATE",i) <= bufUserAllElements.GetString("END_DATE",z)
                    ||
                    bufUserAllElements.GetString("START_DATE",i) <= bufUserAllElements.GetString("START_DATE",z)
                    && bufUserAllElements.GetString("START_DATE",z) <= bufUserAllElements.GetString("END_DATE",i)))
                {
                    bFound = true;
                    break;
                }
            }

            if(!bFound)
            {
                //(j)依赖于(i)
                //依赖元素拼串
                string strInfo = "";
                for(int y = 0; y < iCount; y++)
                {
                    if(strInfo == "")
                    {
                        if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "D")
                            strInfo = bufElementLimit.GetString("DISCNT_NAME", y);
                        else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "S")
                            strInfo = bufElementLimit.GetString("SERVICE_NAME", y);
                        else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "X")
                            strInfo = bufElementLimit.GetString("SP_PRODUCT_NAME", y);
                        else
                            strInfo = bufElementLimit.GetString("ACTION_NAME", y);
                    }
                    else
                    {
                        if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "D")
                            strInfo += "、" + bufElementLimit.GetString("DISCNT_NAME", y);
                        else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "S")
                            strInfo += "、" + bufElementLimit.GetString("SERVICE_NAME", y);
                        else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "X")
                            strInfo += "、" + bufElementLimit.GetString("SP_PRODUCT_NAME", y);
                        else
                            strInfo += "、" + bufElementLimit.GetString("ACTION_NAME", y);
                    }
                }

                if(strErrorInfo == "")
                    strErrorInfo = "\"" + bufUserAllElements.GetString("PRODUCT_NAME", i)
                        + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", i)
                        + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                        + "\"依赖于元素：\"" +  strInfo + "\"";
                else
                    strErrorInfo += "、\"" + bufUserAllElements.GetString("PRODUCT_NAME", i)
                        + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", i)
                        + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                        + "\"依赖于元素：\"" +  strInfo + "\"";

                break;
            }
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strErrorInfo + "，业务无法继续3！");
}

//SP反向订购的产品校验
void TradeCheckAfterTrade::CheckSpProductModelLimit(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
    LOG_TRACE(logTradeCheckAfterTrade, "进入CheckSpProductModelLimit函数");
    string strTradeId = inBuf.GetString("TRADE_ID");
    string strUserId = inBuf.GetString("USER_ID");
    string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");
    string strModifyTag="";
    string strSpServiceId="";
    int iPackageId=-1;

    int iRowCount = -1;
  	StaticDAO& dao = StaticDAO::getDAO();
    CFmlBuf bufTradeSpSub;
    CFmlBuf checkBuf;
    CFmlBuf bufSpElementLimit;

    try
    {
        dao.Clear();
        bufTradeSpSub.ClearFmlValue();
        dao.SetParam(":VTRADE_ID", strTradeId);
        iRowCount = dao.jselect(bufTradeSpSub, "SEL_BY_TRADEID", "TF_B_TRADE_SP");
    }
    catch(Exception &e)
    {
        THROW_AGC(CRMException, e, Grade::WARNING, MODIFYUSERINFO_ERR, "获取台帐SP资料异常！");
    }
    LOG_TRACE_P1(logTradeCheckAfterTrade, "SP订购退订记录数：---[%d]",iRowCount);
    
    //增加tag，用来判断是否需要校验新订购的SP互斥
	CFmlBuf tmpBuf;
	int tmpcount = 0;
	try
	{
		dao.Clear();
		dao.SetParam(":VEPARCHY_CODE", "ZZZZ");
		dao.SetParam(":VTAG_CODE", "CS_CHECK_SP_TRADE_LIMIT");
		dao.SetParam(":VSUBSYS_CODE", "CSM");
		dao.SetParam(":VUSE_TAG", "0");
		tmpcount = dao.jselect(tmpBuf, "SEL_ALL_BY_TAGCODE","TD_S_TAG");
	}
	catch(Exception &ex)
	{
		THROW_AGC(CRMException, ex, Grade::WARNING, GETTRADEINFO_ERR, "获取是TD_S_TAG::CS_CHECK_SP_TRADE_LIMIT信息出错！");
	}
	LOG_TRACE_P1(logTradeCheckAfterTrade, "获取tag号为CS_CHECK_SP_TRADE_LIMIT记录数：---[%d]",tmpcount);
    if(iRowCount > 0)
    {
        for(int i = 0; i < iRowCount ;i++ )
        {
            int iCount = 0;
            string strModifyTag = bufTradeSpSub.GetString("MODIFY_TAG",i);
            string strSpServiceId = bufTradeSpSub.GetString("SP_SERVICE_ID",i);
            int iPackageId = bufTradeSpSub.GetInt("PACKAGE_ID",i);

            //判断包和包元素个数，必选限制
            try
            {
                dao.Clear();
                checkBuf.ClearFmlValue();
                dao.SetParam(":VUSER_ID", strUserId);
                dao.SetParam(":VPACKAGE_ID", iPackageId);
                dao.SetParam(":VSP_SERVICE_ID", strSpServiceId);
                dao.SetParam(":VMODIFY_TAG", strModifyTag);
                iCount = dao.jselect(checkBuf, "CHE_SP_PACKAGE_LIMIT","TF_F_USER_SP");
            }
            catch(Exception &ex)
            {
                THROW_AGC(CRMException,ex,Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:检查SP包限制出错！");
            }
            if( iCount > 0 )
            {
            	THROW_GC(CRMException,Grade::WARNING, CHECKAFTERTRADE_ERR, "该SP订购退订存在包限制！ 限制包ID为："+checkBuf.GetString("PACKAGE_ID",0));
            }

            //判断订购互斥依赖
            if(strModifyTag=="0")
            {
                try
                {
                    dao.Clear();
                    checkBuf.ClearFmlValue();
                    dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
                    dao.SetParam(":VUSER_ID", strUserId);
                    dao.SetParam(":VPACKAGE_ID", iPackageId);
                    dao.SetParam(":VSP_SERVICE_ID", strSpServiceId);
                    iCount = dao.jselect(checkBuf, "CHE_ORDER_LIMIT","TF_F_USER_SP");
                }
                catch(Exception &ex)
                {
                    THROW_AGC(CRMException,ex,Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:检查SP互斥出错！");
                }
                if( iCount > 0 )
                {
                	THROW_GC(CRMException,Grade::WARNING, SP_MUTEX_ERR, "该SP和用户原有的SP存在互斥或者还存在依赖！ a元素："+checkBuf.GetString("ELEMENT_ID_A")+"b元素："+checkBuf.GetString("ELEMENT_ID_B")+"限制关系类型："+checkBuf.GetString("LIMIT_TAG"));
                }
                
				//begin add by wangwp 20120420
				//校验台账里的SP互斥
				//获取与当前sp互斥的元素
				if (tmpcount > 0)
				{
					bufSpElementLimit.ClearFmlValue();
					int iTempCount = 0;
					try
	                {
	                    dao.Clear();
				        dao.SetParam(":VELEMENT_TYPE_CODE_A", "X");
				        dao.SetParam(":VELEMENT_ID_A", strSpServiceId);
				        dao.SetParam(":VLIMIT_TAG", "0");
				        dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
				        iTempCount = dao.jselect(bufSpElementLimit, "JUDGE_A_LIMIT_EXISTS", "TD_B_ELEMENT_LIMIT");
	                }
	                catch(Exception &ex)
	                {
	                    THROW_AGC(CRMException,ex,Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:检查SP互斥出错！");
	                }
	                LOG_TRACE_P1(logTradeCheckAfterTrade, "获取与当期订购的SP互斥的SP元素记录数：---[%d]",iTempCount);
	                if(iTempCount > 0)	//存在互斥
	                {
	                	for(int z=0; z<iTempCount; z++)//循环互斥的元素记录
						{
							for(int j=0; j<iRowCount; j++)//循环台账的sp元素
							{
								if(bufTradeSpSub.GetString("MODIFY_TAG",j)=="0")
								{
									if(bufSpElementLimit.GetString("ELEMENT_ID_B", z)==bufTradeSpSub.GetString("SP_SERVICE_ID", j) 
										&& (bufTradeSpSub.GetString("START_DATE",j) <= bufTradeSpSub.GetString("START_DATE",i)
					                    && bufTradeSpSub.GetString("START_DATE",i) <= bufTradeSpSub.GetString("END_DATE",j)
					                    ||
					                    bufTradeSpSub.GetString("START_DATE",i) <= bufTradeSpSub.GetString("START_DATE",j)
					                    && bufTradeSpSub.GetString("START_DATE",j) <= bufTradeSpSub.GetString("END_DATE",i)))
					                {
					                	THROW_GC(CRMException,Grade::WARNING, SP_MUTEX_ERR, "该用户新订购的SP存在互斥！ a元素："+bufTradeSpSub.GetString("SP_SERVICE_ID", i)+"b元素："+bufTradeSpSub.GetString("SP_SERVICE_ID", j)+"限制关系类型："+bufSpElementLimit.GetString("LIMIT_TAG", z));
					                }
								}	
							}
						}
	                }
	            }
				//end add by wangwp 20120420
            }
            //判断退订互斥依赖
            else if(strModifyTag=="1"){
                try
                {
                    dao.Clear();
                    checkBuf.ClearFmlValue();
                    dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
                    dao.SetParam(":VUSER_ID", strUserId);
                    dao.SetParam(":VPACKAGE_ID", iPackageId);
                    dao.SetParam(":VSP_SERVICE_ID", strSpServiceId);
                    iCount = dao.jselect(checkBuf, "CHE_CANCEL_ORDER_LIMIT","TF_F_USER_SP");
                }
                catch(Exception &ex)
                {
                    THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:检查SP依赖出错！");
                }
                if( iCount > 0 )
                {
                	THROW_GC(CRMException, Grade::WARNING, SP_DEPEND_ON_ERR, "该SP存在依赖关系，不能取消！ a元素："+checkBuf.GetString("ELEMENT_ID_A")+"b元素："+checkBuf.GetString("ELEMENT_ID_B")+"限制关系类型："+checkBuf.GetString("LIMIT_TAG"));
                }
            }
        }
        //获取用户SP信息
        CFmlBuf bufTradeSp_UserAllSp;       //用户SP
        GetUserAllSpAfterTrade(strTradeId, strUserId, bufTradeSp_UserAllSp);
        //SP服务判断：同一个SP_ID只能选择一个
        CheckSameSPID(bufTradeSp_UserAllSp);
    }


    LOG_TRACE(logTradeCheckAfterTrade, "退出CheckSpProductModelLimit函数");
}

/**
 *	成员沃家庭通信管家、电脑保姆和沃家庭关系类型、资费档次是否匹配
 *	author:suiq
 *	date:2011-06-20
 **/
void TradeCheckAfterTrade::ChkWoDisUserProdLimit(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
		LOG_TRACE(logTradeCheckAfterTrade, "进入ChkWoUserProdLimit函数！");
		StaticDAO& dao = StaticDAO::getDAO();
		CFmlBuf	tradeBuf;
		CFmlBuf	bufTradeDiscnt_AllDiscnt;
		CFmlBuf	bufTradeDiscnt_AllDiscntMem;
		CFmlBuf bufTradeSvc_UserAllSvcMem;
		CFmlBuf bufTradeSp_UserAllSpMem;
		CFmlBuf bufTradeElement_UserAllElementMem;
		CFmlBuf bufUserAllElementsMem;
		CFmlBuf	tradeRelationBuf;
		CFmlBuf	tradEleDisBuf;
		CFmlBuf	tradPlusDisBuf;
		CFmlBuf memTradBuf;
		CFmlBuf tradeCommparaBuf;
		int icount = -1;
		int	iRelationCount = -1;
		int iEleDisCount = -1;
    int iPlusDisCount = -1;
    int iMemTradeCount = -1;
    int iCommparaCount = -1;
    int matchTag = 0;
    string strInfo = "";
    string strErrorInfo = "";
    string strErrorInfoTemp = "";
    string strTradeId = "";
    if (inBuf.IsFldExist("SUBSCRIBE_ID"))
        strTradeId=inBuf.GetString("SUBSCRIBE_ID");

    try
    {

        dao.Clear();
        dao.SetParam(":VTRADE_ID", strTradeId);
        dao.SetParam(":VCANCEL_TAG", 0);
      	icount=dao.jselect(tradeBuf,"SEL_BY_PK","TF_B_TRADE");
        LOG_TRACE_P1(logTradeCheckAfterTrade, "tradeBuf--%s",tradeBuf.ToString().c_str());
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GETTRADEINFO_ERR, "获取虚拟用户台账！");
    }
		//获取用户优惠信息
		GetAllDiscntAfterTrade(strTradeId, tradeBuf.GetString("USER_ID"),	tradeBuf.GetString("CUST_ID"), tradeBuf.GetString("ACCT_ID"),	bufTradeDiscnt_AllDiscnt);
		LOG_TRACE_P1(logTradeCheckAfterTrade, "bufTradeDiscnt_AllDiscnt--%s",bufTradeDiscnt_AllDiscnt.ToString().c_str());

		//获取沃家庭成员信息
		try
		{
				dao.Clear();
				dao.SetParam(":VUSER_ID_A",	tradeBuf.GetString("USER_ID"));
				dao.SetParam(":VTRADE_ID", strTradeId);
				iRelationCount = dao.jselect(tradeRelationBuf, "SEL_ALLUUS_AFTER_SUBSCRIBE","TF_B_TRADE_RELATION");
		}
		catch(Exception	&ex)
		{
				THROW_AGC(CRMException,	ex,	Grade::WARNING,	CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取沃家庭成员信息出错！");
		}
    //判断每个成员的元素是否和沃家庭资费档次匹配
		for(int	i	=	0;i	<	iRelationCount;i++)
		{
			  strErrorInfoTemp = "";
		    //获取沃家庭成员的台账信息
		    try
				{
					  memTradBuf.ClearFmlValue();
						dao.Clear();
						dao.SetParam(":VUSER_ID",	tradeRelationBuf.GetString("USER_ID_B",i));
						dao.SetParam(":VSUBSCRIBE_ID", strTradeId);
						iMemTradeCount = dao.jselect(memTradBuf, "SEL_BY_SUBID_UID","TF_B_TRADE");
				}
				catch(Exception	&ex)
				{
						THROW_AGC(CRMException,	ex,	Grade::WARNING,	CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取沃家庭成员台账表信息出错！");
				}
	      bufTradeDiscnt_AllDiscntMem.ClearFmlValue();
        bufTradeSvc_UserAllSvcMem.ClearFmlValue();
        bufTradeSp_UserAllSpMem.ClearFmlValue();
        bufTradeElement_UserAllElementMem.ClearFmlValue();
        bufUserAllElementsMem.ClearFmlValue();
				if(iMemTradeCount > 0)
				{
						GetAllDiscntAfterTrade(memTradBuf.GetString("TRADE_ID"), tradeRelationBuf.GetString("USER_ID_B",i), memTradBuf.GetString("CUST_ID"), memTradBuf.GetString("ACCT_ID"), bufTradeDiscnt_AllDiscntMem);
						GetUserAllServiceAfterTrade(memTradBuf.GetString("TRADE_ID"), tradeRelationBuf.GetString("USER_ID_B",i), bufTradeSvc_UserAllSvcMem);
						GetUserAllSpAfterTrade(memTradBuf.GetString("TRADE_ID"), tradeRelationBuf.GetString("USER_ID_B",i), bufTradeSp_UserAllSpMem);
						GetUserAllElementAfterTrade(memTradBuf.GetString("TRADE_ID"), tradeRelationBuf.GetString("USER_ID_B",i), bufTradeElement_UserAllElementMem);
						GeneUserAllElements(bufTradeDiscnt_AllDiscntMem, bufTradeSvc_UserAllSvcMem, bufTradeSp_UserAllSpMem, bufTradeElement_UserAllElementMem, bufUserAllElementsMem);
						for(int j = 0;j < bufUserAllElementsMem.GetInt("X_RECORDNUM"); j++)
						{
							  //获取沃家庭成员元素是否需要和沃家庭资费档次匹配
								try
								{
									  tradEleDisBuf.ClearFmlValue();
										dao.Clear();
										dao.SetParam(":VKEY",	"COMPELEDIS");
										dao.SetParam(":VVALUE1", to_string(bufUserAllElementsMem.GetInt("ELEMENT_ID",j)));
										iEleDisCount = dao.jselect(tradEleDisBuf, "SEL_ALL_1","TD_S_CPARAM");
								}
								catch(Exception	&ex)
								{
										THROW_AGC(CRMException,	ex,	Grade::WARNING,	CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取沃家庭成员元素是否需要和资费档次匹配出错！");
								}

		            if(iEleDisCount > 0)//需要和沃家庭关系类型、资费档次匹配
		            {
		            	 matchTag = 0;
		            	 strInfo = "";
		            	 //(x)部分依赖于(y)
									 //部分依赖元素拼串
		            	 for(int x = 0;x < iEleDisCount; x++)
		            	 {
		            	     if(strInfo == "")
											 {
											     strInfo = CParamDAO::getParam("DiscntName", tradEleDisBuf.GetString("VRESULT",x));

											 }
											 else
											 {
											     strInfo += "、" + CParamDAO::getParam("DiscntName", tradEleDisBuf.GetString("VRESULT",x));
											 }
		            	 }

		            	 for(int y = 0; y < bufTradeDiscnt_AllDiscnt.GetInt("X_RECORDNUM"); y++)
		               {
		               	  iPlusDisCount = -1;
		               	  if(bufTradeDiscnt_AllDiscnt.GetString("MODIFY_TAG",y) == "0" || bufTradeDiscnt_AllDiscnt.GetString("MODIFY_TAG",y) == "A" || bufTradeDiscnt_AllDiscnt.GetString("MODIFY_TAG",y) == "O" )
		               	  {
						               //判断沃家庭成员通信管家、电脑保姆是否和沃家庭资费档次匹配
													try
													{
														  tradPlusDisBuf.ClearFmlValue();
															dao.Clear();
															dao.SetParam(":VKEY",	"COMPELEDIS");
															dao.SetParam(":VVALUE1", to_string(bufUserAllElementsMem.GetInt("ELEMENT_ID",j)));
															dao.SetParam(":VVRESULT", to_string(bufTradeDiscnt_AllDiscnt.GetInt("DISCNT_CODE",y)));
															iPlusDisCount = dao.jselect(tradPlusDisBuf, "SEL_BY_KEY_RES","TD_S_CPARAM");
													}
													catch(Exception	&ex)
													{
															THROW_AGC(CRMException,	ex,	Grade::WARNING,	CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取沃家庭成员附加产品是否需要和资费档次匹配出错！");
													}
										  }
											//沃家庭成员通信管家、电脑保姆和沃家庭资费档次匹配
									    if(iPlusDisCount > 0){
											    matchTag = 1;
											    break;
											}
									 }
									 if(matchTag == 0)
							     {
							         if(strErrorInfoTemp	== "")
									 		    strErrorInfoTemp = "["	+	tradeRelationBuf.GetString("SERIAL_NUMBER_B",i) + "]的元素\"" + bufUserAllElementsMem.GetString("ELEMENT_NAME",j)
									 				+	"\"部份依赖于沃家庭资费：\""+	strInfo	+	"\"";
									     else
									 		    strErrorInfoTemp +=	"、\"" + bufUserAllElementsMem.GetString("ELEMENT_NAME",j)
									 				+	"\"部份依赖于沃家庭资费：\""+	strInfo	+	"\"";
									 }
		            }
						}
						if(strErrorInfo == "" && strErrorInfoTemp != "")
				    		strErrorInfo = strErrorInfoTemp;
						else if(strErrorInfo != "" && strErrorInfoTemp != "")
					  		strErrorInfo += "|" + strErrorInfoTemp;
				}
		}

		if(strErrorInfo	!= "")
		    THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,	 strErrorInfo	+	"，业务无法继续！请取消成员通信管家、电脑保姆或者点击【上一步】变更沃家庭资费！");

		LOG_TRACE(logTradeCheckAfterTrade, "退出ChkWoUserProdLimit函数！");
}


//协议与资费、组合产品之间限制关系判断
void TradeCheckAfterTrade::CheckAttrElementLimit(CFmlBuf &bufTradeDiscnt_AllDiscnt, const string &strEparchyCode,const string strTradeId, const string &strUserId,const string &strCustId, const string &strAcctId)
{
	LOG_TRACE(logTradeCheckAfterTrade, "进入CheckAttrElementLimit函数");
	CFmlBuf bufTrade;
    CFmlBuf bufAttrElementLimit;
    CFmlBuf bufTradeAttr_AllAttr;
    CFmlBuf bufTradeComp;
    CFmlBuf bufTradeCompProt;
    string strErrorInfo = "";
    int iCountTrade = 0;
    int iCountComp = 0;
    int iCountCompLimit = 0;
    int iCount = 0;
    bool bFound = false;
    int i,j,z;
    StaticDAO& dao = StaticDAO::getDAO();
	string strSysdate;
	DualMgr objDualMgr;
	objDualMgr.GetSysDate(strSysdate);

	//获取用户所有用户属性信息
	int iUserAttrs = GetAllAttrAfterTrade(strTradeId, strUserId, bufTradeAttr_AllAttr);
	int iUserDiscnts = bufTradeDiscnt_AllDiscnt.GetInt("X_RECORDNUM");

	//获取用户台账主表
	try
    {
        dao.Clear();
        dao.SetParam(":VTRADE_ID", strTradeId);
        dao.SetParam(":VCANCEL_TAG", "0");
        iCountTrade = dao.jselect(bufTrade, "SEL_BY_PK","TF_B_TRADE");
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取用户台账主表出错！[%s]",ex.getCause()->what());
    }

    //组合用户：判断录入的协议号必须是当前组合产品要求的协议 start
    //获取用户是否组合用户
    try
    {
        dao.Clear();
        dao.SetParam(":VTRADE_ID", strTradeId);
        iCountComp = dao.jselect(bufTradeComp, "SEL_COMP_TRADE_INFO2","TF_B_TRADE");
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取用户是否办理的组合业务出错！[%s]",ex.getCause()->what());
    }

	//判断用户协议与资费、组合产品依赖关系 start
    //按用户属性循环校验资费、组合产品依赖关系
    strErrorInfo = "";
    for(i = 0; i < iUserAttrs; i++)
    {
    	//获取协议与业务类型（根据协议获取业务类型）之间互斥关系
    	iCount = 0;
        bufAttrElementLimit.ClearFmlValue();
    	iCount = ExistsAttrElementLimitA(bufTradeAttr_AllAttr.GetString("ATTR_CODE",i),bufTradeAttr_AllAttr.GetString("ATTR_VALUE",i), "2", "0", strEparchyCode, bufAttrElementLimit);
		bFound = true;
		string attrReleaseDate = "";
		for(j = 0; j < iCount; j++)
        {

        	objDualMgr.AddMonths(bufTradeAttr_AllAttr.GetString("START_DATE",i), bufAttrElementLimit.GetInt("MONTHS",j) <= 0 ? 0:bufAttrElementLimit.GetInt("MONTHS",j), attrReleaseDate);
            if(iCountTrade > 0 && bufTrade.GetString("TRADE_TYPE_CODE") == bufAttrElementLimit.GetString("ELEMENT_ID",j)
            	&& attrReleaseDate >= strSysdate)
            {
            	bFound = false;
            	break;
            }
        }
        if(!bFound)
        {
        	string attrName = getAttrName(bufTradeAttr_AllAttr.GetString("ATTR_CODE",i));
        	string attrValueName = "";      	
        	if(bufTradeAttr_AllAttr.GetString("ATTR_CODE",i) == "PROTOCOL_ID")
        		attrValueName = CParamDAO::getParam("ProtocolName",  bufTradeAttr_AllAttr.GetString("ATTR_VALUE", i));

        	string tradeTypeName = CParamDAO::getParam("TradeType",  bufTrade.GetString("TRADE_TYPE_CODE"));
        	strErrorInfo = "于"+bufTradeAttr_AllAttr.GetString("START_DATE",i)+"选择的\""+attrName+"："+attrValueName+"\""+"，"
        	               + to_string(bufAttrElementLimit.GetInt("MONTHS",j))+"个月内不能办理\""+tradeTypeName
        	               +"\"，请于"+attrReleaseDate+"之后再来办理";
        	break;
        }

    	//获取协议与组合产品(根据协议获取组合产品)之间限制关系
    	if(iCountComp > 0)
    	{
    		iCountCompLimit = 0;
        	bufAttrElementLimit.ClearFmlValue();
    		iCountCompLimit = ExistsAttrElementLimitA(bufTradeAttr_AllAttr.GetString("ATTR_CODE",i),bufTradeAttr_AllAttr.GetString("ATTR_VALUE",i), "1", "1", strEparchyCode, bufAttrElementLimit);
    		bFound = true;
	        for(j = 0; j < iCountCompLimit; j++)
	        {
	            bFound = false;
	            for(z = 0; z < iCountComp; z++)
	            {
	                if(bufTradeComp.GetInt("PRODUCT_ID", z) == bufAttrElementLimit.GetInt("ELEMENT_ID", j)
	                    && "1" == bufAttrElementLimit.GetString("ID_TYPE_CODE", j)
	                    && bufTradeAttr_AllAttr.GetString("START_DATE",i) <= strSysdate
	                    && strSysdate < bufTradeAttr_AllAttr.GetString("END_DATE",i))
	                {
	                    bFound = true;
	                    break;
	                }
	            }
	    		if(bFound)
	    		{
					break;
				}
	        }
	        if(!bFound)
	        {
	            string strInfo = "";
	            for(int y = 0; y < iCountCompLimit; y++)
	            {
	                if(strInfo == "")
	                {
	                    strInfo = bufAttrElementLimit.GetString("PRODUCT_NAME", y);
	                }
	                else
	                {
	 					strInfo += "、" + bufAttrElementLimit.GetString("PRODUCT_NAME", y);
	                }
	            }
				string attrName = getAttrName(bufTradeAttr_AllAttr.GetString("ATTR_CODE",i));
        		string attrValueName = "";      	
        		if(bufTradeAttr_AllAttr.GetString("ATTR_CODE",i) == "PROTOCOL_ID")
        			attrValueName = CParamDAO::getParam("ProtocolName",  bufTradeAttr_AllAttr.GetString("ATTR_VALUE", i));

	            if(strErrorInfo == "")
	                strErrorInfo = "\"" +attrName + "：" + attrValueName + "\"部分依赖于组合产品：\"" +  strInfo + "\"";
	            else
	                strErrorInfo += "、\"" +attrName + "：" + attrValueName + "\"部分依赖于组合产品：\"" +  strInfo + "\"";

	            break;
	        }
    	}

    	//获取协议与资费(根据协议获取资费)之间限制关系
    	//判断属性依赖的元素，LIMIT_TAG为1
        iCount = 0;
        bufAttrElementLimit.ClearFmlValue();
        iCount = ExistsAttrElementLimitA(bufTradeAttr_AllAttr.GetString("ATTR_CODE",i),bufTradeAttr_AllAttr.GetString("ATTR_VALUE",i), "0", "1", strEparchyCode, bufAttrElementLimit);
		bFound = true;
        for(j = 0; j < iCount; j++)
        {
            bFound = false;
            for(z = 0; z < iUserDiscnts; z++)
            {
                if(bufTradeDiscnt_AllDiscnt.GetInt("DISCNT_CODE", z) == bufAttrElementLimit.GetInt("ELEMENT_ID", j)
                    && "0" == bufAttrElementLimit.GetString("ID_TYPE_CODE", j)
                    //时间交集
                    && (bufTradeDiscnt_AllDiscnt.GetString("START_DATE",z) <= bufTradeAttr_AllAttr.GetString("START_DATE",i)
                    && bufTradeAttr_AllAttr.GetString("START_DATE",i) <= bufTradeDiscnt_AllDiscnt.GetString("END_DATE",z)
                    ||
                    bufTradeAttr_AllAttr.GetString("START_DATE",i) <= bufTradeDiscnt_AllDiscnt.GetString("START_DATE",z)
                    && bufTradeDiscnt_AllDiscnt.GetString("START_DATE",z) <= bufTradeAttr_AllAttr.GetString("END_DATE",i)))
                {
                    bFound = true;
                    break;
                }
            }
    		if(bFound)
    		{
				break;
			}
        }
        if(!bFound)
        {
            string strInfo = "";
            for(int y = 0; y < iCount; y++)
            {
                if(strInfo == "")
                {
                    strInfo = bufAttrElementLimit.GetString("DISCNT_NAME", y);
                }
                else
                {
                    strInfo += "、" + bufAttrElementLimit.GetString("DISCNT_NAME", y);
                }
            }

			string attrName = getAttrName(bufTradeAttr_AllAttr.GetString("ATTR_CODE",i));
        	string attrValueName = "";      	
        	if(bufTradeAttr_AllAttr.GetString("ATTR_CODE",i) == "PROTOCOL_ID")
        		attrValueName = CParamDAO::getParam("ProtocolName",  bufTradeAttr_AllAttr.GetString("ATTR_VALUE", i));
            
            if(strErrorInfo == "")
                strErrorInfo = "\"" +attrName + "：" + attrValueName + "\"部分依赖于元素：\"" +  strInfo + "\"";
            else
                strErrorInfo += "、\"" +attrName + "：" + attrValueName + "\"部分依赖于元素：\"" +  strInfo + "\"";

            break;
        }
    }
	if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  "号码\"" + bufTrade.GetString("SERIAL_NUMBER") + "\":" + strErrorInfo + "，业务无法继续！");

    //获取协议与组合产品(根据组合产品获取协议)之间限制关系
    strErrorInfo = "";
    for(i = 0; i < iCountComp; i++)
	{
		iCountCompLimit = 0;
    	bufAttrElementLimit.ClearFmlValue();
		iCountCompLimit = ExistsAttrElementLimitB(bufTradeComp.GetString("PRODUCT_ID",i), "1", "1", strEparchyCode, bufAttrElementLimit);

		bFound = true;
        for(j = 0; j < iCountCompLimit; j++)
        {
            bFound = false;
            for(z = 0; z < iUserAttrs; z++)
            {
                if(bufTradeAttr_AllAttr.GetString("ATTR_CODE", z) == bufAttrElementLimit.GetString("ATTR_CODE", j)
                	&& bufTradeAttr_AllAttr.GetString("ATTR_VALUE", z) == bufAttrElementLimit.GetString("ATTR_VALUE", j)
                    && "1" == bufAttrElementLimit.GetString("ID_TYPE_CODE", j)
                    && strSysdate < bufTradeAttr_AllAttr.GetString("END_DATE",z)
                    && bufTradeAttr_AllAttr.GetString("START_DATE",z) <= strSysdate)
                {
                    bFound = true;
                    break;
                }
            }
    		if(bFound)
    		{
				break;
			}
        }

        if(!bFound)
        {
            string strInfo = "";
            for(int y = 0; y < iCountCompLimit; y++)
            {
            	string attrName = getAttrName(bufAttrElementLimit.GetString("ATTR_CODE",y));
        		string attrValueName = "";      	
        		if(bufAttrElementLimit.GetString("ATTR_CODE",y) == "PROTOCOL_ID")
        			attrValueName = CParamDAO::getParam("ProtocolName",  bufAttrElementLimit.GetString("ATTR_VALUE", y));
             	
                if(strInfo == "")
                {
                    strInfo = "\"" +attrName + "：" +  attrValueName;
                }
                else
                {
                    strInfo += "、\"" +attrName + "：" +  attrValueName;
                }
            }
            string compProductName = CParamDAO::getParam("ProductName",  bufTradeComp.GetString("PRODUCT_ID", i));
            if(strErrorInfo == "")
                strErrorInfo = "组合产品：\"" + compProductName
                    + "\"部分依赖于元素：\"" +  strInfo + "\"";
            else
                strErrorInfo += "、组合产品：\"" + compProductName
                    + "\"部分依赖于元素：\"" +  strInfo + "\"";

            break;
        }
	}
    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  "号码\"" + bufTrade.GetString("SERIAL_NUMBER") + "\":" + strErrorInfo + "，业务无法继续！");

 	//获取协议与资费(根据资费获取协议)之间限制关系，
 	//modfiy by lirui 判断元素依赖的属性，LIMIT_TAG为2
 	//是否校验用户所有元素判断, 如果存在TAG:CS_ONLY_ADDELM_CHANGE并且TAG_CHAR为1,则只判断新增的元素,否则判断所有
 	CFmlBuf tagBuf;
    int count = -1;
    try
    {
        dao.Clear();
        dao.SetParam(":VEPARCHY_CODE",strEparchyCode);
        dao.SetParam(":VTAG_CODE","CS_ONLY_ADDELM_CHANGE");
        dao.SetParam(":VSUBSYS_CODE","CSM");
        dao.SetParam(":VUSE_TAG","0");
        count = dao.jselect(tagBuf,"SEL_BY_TAGCODE_1","TD_S_TAG");
    }
    catch(Exception &e)
    {
        THROW_AGC(CRMException, e, Grade::WARNING, 8888, "获取是否只判断新增元素标识异常！");
    }
    if (count>0&&tagBuf.GetString("TAG_CHAR")=="1")
    {
		bufTradeDiscnt_AllDiscnt.ClearFmlValue();
		
		int iCount = 0;
		
		try
		{
		    dao.Clear();
		    dao.SetParam(":VTRADE_ID", strTradeId);
		    dao.SetParam(":VACCEPT_MONTH", strTradeId.substr(4,2));
		    dao.SetParam(":VUSER_ID", strUserId);
		    dao.SetParam(":VCUST_ID", strCustId);
		    dao.SetParam(":VACCT_ID", strAcctId);
		
		    iCount = dao.jselect(bufTradeDiscnt_AllDiscnt, "SEL_ADDDISCNTS_AFTER_TRADE","TF_B_TRADE_DISCNT");
		}
		catch (Exception &ex)
		{
		    THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取办理业务后三户的所有优惠出错！[%s]",ex.getCause()->what());
		}
		
		
		iUserDiscnts=iCount;
	}
    strErrorInfo = "";
	for(i = 0; i < iUserDiscnts; i++)
    {
        iCount = 0;
        bufAttrElementLimit.ClearFmlValue();
        // modfiy by lirui@202120213 
        iCount = ExistsAttrElementLimitB(bufTradeDiscnt_AllDiscnt.GetString("DISCNT_CODE",i), "0", "2", strEparchyCode, bufAttrElementLimit);
		bFound = true;
        for(j = 0; j < iCount; j++)
        {
            bFound = false;
            for(z = 0; z < iUserAttrs; z++)
            {
                if(bufTradeAttr_AllAttr.GetString("ATTR_CODE", z) == bufAttrElementLimit.GetString("ATTR_CODE", j)
                	&& bufTradeAttr_AllAttr.GetString("ATTR_VALUE", z) == bufAttrElementLimit.GetString("ATTR_VALUE", j)
                    && "0" == bufAttrElementLimit.GetString("ID_TYPE_CODE", j)
                    //时间交集
                    && (bufTradeAttr_AllAttr.GetString("START_DATE",z) <= bufTradeDiscnt_AllDiscnt.GetString("START_DATE",i)
                    && bufTradeDiscnt_AllDiscnt.GetString("START_DATE",i) <= bufTradeAttr_AllAttr.GetString("END_DATE",z)
                    ||
                    bufTradeDiscnt_AllDiscnt.GetString("START_DATE",i) <= bufTradeAttr_AllAttr.GetString("START_DATE",z)
                    && bufTradeAttr_AllAttr.GetString("START_DATE",z) <= bufTradeDiscnt_AllDiscnt.GetString("END_DATE",i)))
                {
                    bFound = true;
                    break;
                }
            }
    		if(bFound)
    		{
				break;
			}
        }

        if(!bFound)
        {
            string strInfo = "";
            for(int y = 0; y < iCount; y++)
            {				
				string attrName = getAttrName(bufAttrElementLimit.GetString("ATTR_CODE",y));
        		string attrValueName = "";      	
        		if(bufAttrElementLimit.GetString("ATTR_CODE",y) == "PROTOCOL_ID")
        			attrValueName = CParamDAO::getParam("ProtocolName",  bufAttrElementLimit.GetString("ATTR_VALUE", y));
        		else
        			// modfiy by lirui@202120213 
        			attrValueName = getAttrValueName(bufAttrElementLimit.GetString("ATTR_CODE",y),bufAttrElementLimit.GetString("ATTR_VALUE", y));
             		
                if(strInfo == "")
                {
                    strInfo = "\"" +attrName + "：" +  attrValueName;
                }
                else
                {
                    strInfo += "、\"" +attrName + "：" +  attrValueName;
                }
            }

            if(strErrorInfo == "")
                strErrorInfo = "\"" + bufTradeDiscnt_AllDiscnt.GetString("PRODUCT_NAME", i)
                    + " --> " + bufTradeDiscnt_AllDiscnt.GetString("PACKAGE_NAME", i)
                    + " --> " + bufTradeDiscnt_AllDiscnt.GetString("DISCNT_NAME", i)
                    + "\"部分依赖于元素：\"" +  strInfo + "\"";
            else
                strErrorInfo += "、\"" + bufTradeDiscnt_AllDiscnt.GetString("PRODUCT_NAME", i)
                    + " --> " + bufTradeDiscnt_AllDiscnt.GetString("PACKAGE_NAME", i)
                    + " --> " + bufTradeDiscnt_AllDiscnt.GetString("DISCNT_NAME", i)
                    + "\"部分依赖于元素：\"" +  strInfo + "\"";

            break;
        }
    }
    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  "号码\"" + bufTrade.GetString("SERIAL_NUMBER") + "\":" + strErrorInfo + "，业务无法继续！");
    //判断用户协议与资费、组合产品完全依赖关系 end

	LOG_TRACE(logTradeCheckAfterTrade, "退出CheckAttrElementLimit函数");
}

//获取办理业务后的用户所有属性信息
int TradeCheckAfterTrade::GetAllAttrAfterTrade(const string &strTradeId, const string &strUserId, CFmlBuf &bufTradeAttr_AllAttr)
{
	LOG_TRACE(logTradeCheckAfterTrade, "进入GetAllAttrAfterTrade函数");
    bufTradeAttr_AllAttr.ClearFmlValue();
    StaticDAO& dao = StaticDAO::getDAO();
    int iCount = 0;

    try
    {
        dao.Clear();
        dao.SetParam(":VTRADE_ID", strTradeId);
        dao.SetParam(":VACCEPT_MONTH", strTradeId.substr(4,2));
        dao.SetParam(":VUSER_ID", strUserId);

        iCount = dao.jselect(bufTradeAttr_AllAttr, "SEL_ALLATTRS_AFTER_TRADE","TF_B_TRADE_SUB_ITEM");
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取用户所有用户属性信息出错！[%s]",ex.getCause()->what());
    }

    bufTradeAttr_AllAttr.SetInt("X_RECORDNUM", iCount);

    return iCount;
}

//获取协议和资费、组合产品之间关系(根据协议获取资费、组合产品)
int TradeCheckAfterTrade::ExistsAttrElementLimitA(const string &strAttrCode, const string &strAttrValue,const string &strIdTypeCode,
                                              const string &strLimitTag, const string strEparchyCode,
                                              CFmlBuf &bufAttrElementLimit)
{
    bufAttrElementLimit.ClearFmlValue();
    StaticDAO& dao = StaticDAO::getDAO();
    int iCount = 0;
    LOG_TRACE(logTradeCheckAfterTrade, "进入ExistsAttrElementLimitA函数");
    try
    {
        dao.Clear();
        dao.SetParam(":VATTR_CODE", strAttrCode);
        dao.SetParam(":VATTR_VALUE", strAttrValue);
        dao.SetParam(":VID_TYPE_CODE", strIdTypeCode);
        dao.SetParam(":VLIMIT_TAG", strLimitTag);
        dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
        iCount = dao.jselect(bufAttrElementLimit, "JUDGE_ATTR_LIMIT_EXISTSA", "TD_B_ATTR_ELEMENT_LIMIT");
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取协议和资费(根据协议获取资费)限制关系信息出错！[%s]",ex.getCause()->what());
    }

    char cTemp[10];
    for(int i = 0; i < iCount; i++)
    {
        cTemp[0] = '\0';
        sprintf(cTemp,"%d",(int)bufAttrElementLimit.GetInt("ELEMENT_ID", i));
        if(bufAttrElementLimit.GetString("ID_TYPE_CODE", i) == "0")
        {
            bufAttrElementLimit.SetString("DISCNT_NAME", CParamDAO::getParam("DiscntName", cTemp), i);
        }
        else if(bufAttrElementLimit.GetString("ID_TYPE_CODE", i) == "1")
        {
            bufAttrElementLimit.SetString("PRODUCT_NAME", CParamDAO::getParam("ProductName", cTemp), i);
        }
    }

    bufAttrElementLimit.SetInt("X_RECORDNUM", iCount);
    return iCount;
}

//获取协议和资费、组合产品之间关系(根据资费、组合产品获取协议)
int TradeCheckAfterTrade::ExistsAttrElementLimitB(const string &strElementId, const string &strIdTypeCode,
                                              const string &strLimitTag, const string strEparchyCode,
                                              CFmlBuf &bufAttrElementLimit)
{
    bufAttrElementLimit.ClearFmlValue();
    StaticDAO& dao = StaticDAO::getDAO();
    int iCount = 0;
    LOG_TRACE(logTradeCheckAfterTrade, "进入ExistsAttrElementLimitB函数");
    try
    {
        dao.Clear();
        dao.SetParam(":VELEMENT_ID", strElementId);
        dao.SetParam(":VID_TYPE_CODE", strIdTypeCode);
        dao.SetParam(":VLIMIT_TAG", strLimitTag);
        dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
        iCount = dao.jselect(bufAttrElementLimit, "JUDGE_ATTR_LIMIT_EXISTSB", "TD_B_ATTR_ELEMENT_LIMIT");
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取协议和资费(根据资费获取协议)限制关系信息出错！[%s]",ex.getCause()->what());
    }

    char cTemp[10];
    for(int i = 0; i < iCount; i++)
    {
        cTemp[0] = '\0';
        sprintf(cTemp,"%d",(int)bufAttrElementLimit.GetInt("ELEMENT_ID", i));
        if(bufAttrElementLimit.GetString("ID_TYPE_CODE", i) == "0")
        {
            bufAttrElementLimit.SetString("DISCNT_NAME", CParamDAO::getParam("DiscntName", cTemp), i);
        }
        else if(bufAttrElementLimit.GetString("ID_TYPE_CODE", i) == "1")
        {
            bufAttrElementLimit.SetString("PRODUCT_NAME", CParamDAO::getParam("ProductName", cTemp), i);
        }
    }

    bufAttrElementLimit.SetInt("X_RECORDNUM", iCount);
    return iCount;
}

//获取属性名称
string TradeCheckAfterTrade::getAttrName(const string &strAttrCode)
{
    CFmlBuf bufAttrName;
    StaticDAO& dao = StaticDAO::getDAO();
    int iCount = 0;
    string strAttrName = "";
    LOG_TRACE(logTradeCheckAfterTrade, "进入getAttrName函数");
    try
    {
        dao.Clear();
        dao.SetParam(":VATTR_CODE", strAttrCode);
        iCount = dao.jselect(bufAttrName, "SQL_ITEMS_INFO", "TD_B_ITEMS");
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取协议和资费(根据资费获取协议)限制关系信息出错！[%s]",ex.getCause()->what());
    }
    if(iCount > 0)
    	strAttrName = bufAttrName.GetString("ATTR_NAME");
    else
    	strAttrName = "元素";
    	
    return strAttrName;
}


//获取属性值名称 add by lirui@20120213
string TradeCheckAfterTrade::getAttrValueName(const string &strAttrCode,const string &strAttrValueCode)
{
	LOG_TRACE(logTradeCheckAfterTrade, "进入getAttrValueName函数");
    CFmlBuf bufAttrName;
    StaticDAO& dao = StaticDAO::getDAO();
    int iCount = 0;
    string strAttrValueName = "";

    try
    {
        dao.Clear();
        dao.SetParam(":VATTR_CODE", strAttrCode);
        dao.SetParam(":VATTR_VALUE", strAttrValueCode);
        iCount = dao.jselect(bufAttrName, "SEL_ENUMNAME_BY_ATTRCODE", "TD_S_ENUMERATE");
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取协议和资费(根据资费获取协议)限制关系信息出错！[%s]",ex.getCause()->what());
    }
    if(iCount > 0)
    	strAttrValueName = bufAttrName.GetString("ENUM_FIELD_NAME");
    else
    	strAttrValueName = "";
    	
    LOG_TRACE(logTradeCheckAfterTrade, "退出getAttrValueName函数");
    return strAttrValueName;
}

