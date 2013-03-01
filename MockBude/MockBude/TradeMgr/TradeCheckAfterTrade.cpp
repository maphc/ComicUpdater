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

/*�ж�ʱ���Ƿ�����  Add by tz@2005-12-31 09:45*/
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
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:�ж�ʱ���Ƿ�������");
    }
}

/*�ж��Ƿ���ԤԼ�Ĳ�Ʒ���(ע�⣺ԤԼ�Ĳ�Ʒ���ֻ��һ����¼) Add by tz@2006-01-18 19:33*/
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
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:�ж�ʱ���Ƿ�������");
    }

    strTradeId = bufTrade.GetString("TRADE_ID");
    strStartDate = string(bufTrade.GetString("EXEC_TIME")).substr(0,10) + " 00:00:00";

    return true;
}

/**
 *  ��ϲ�Ʒ����˲�Ʒ����У��
 *  author:Yangf
 *  date:2008-12-27
 **/
void TradeCheckAfterTrade::ChkRelaUserProductLimit(const string &tradeId,
    const string &userId, const string &eparchyCode)
{
    LOG_TRACE(logTradeCheckAfterTrade, "����ChkRelaUserProductLimit������");
    StaticDAO& dao = StaticDAO::getDAO();
    CFmlBuf tradRelaBuf;
    //��ȡ̨�˹�ϵ�ӱ�������¼
    try
    {
        dao.Clear();
        dao.SetParam(":VTRADE_ID", tradeId);
        dao.SetParam(":VMODIFY_TAG", "0");
        dao.jselect(tradRelaBuf, "SEL_BY_TRADEID_TAG","TF_B_TRADE_RELATION");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ̨�˹�ϵ�ӱ����");
    }

    if(tradRelaBuf.GetCount("ID_A") == 0)
    {
        return;
    }
    //��ȡ̨�˲�Ʒ�ӱ��¼
    CFmlBuf tradProdBuf;
    try
    {
        dao.Clear();
        dao.SetParam(":VTRADE_ID", tradeId);
        dao.jselect(tradProdBuf, "SEL_BY_PK","TF_B_TRADE_PRODUCT");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ̨�˲�Ʒ�ӱ����");
    }
    //��ȡ�����û���Ч�˻�
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
                    //THROW_C(CRMException, UPDATECUSTINFO_ERR, "ҵ��ǼǺ������ж�:�û����ѹ�ϵ���ϲ����ڣ�");
                }
                else
                {
                    acctId = tempBuf.GetString("ACCT_ID");
                }
            }
            catch(Exception &ex)
            {
                THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ̨�˸��ѹ�ϵ�ӱ����");
            }
        }

    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ���ѹ�ϵ���ϳ���");
    }

    for(int i=0; i<tradRelaBuf.GetCount("ID_A"); ++i)
    {
        for(int j=0; j<tradProdBuf.GetCount("USER_ID_A"); ++j)
        {
            //added by Yangf@2009-1-14 begin
            //����У��
            CFmlBuf tempBuf;
            try
            {
                dao.Clear();
                dao.SetParam(":VPRODUCT_ID", (int)tradProdBuf.GetInt("PRODUCT_ID"));
                dao.SetParam(":VFORCE_TAG", "3");   //�˻��Żݰ�
                dao.SetParam(":VEPARCHY_CODE", eparchyCode);
                dao.jselect(tempBuf, "SEL_BY_PID_FORCETAG","TD_B_PRODUCT_PACKAGE");
            }
            catch(Exception &ex)
            {
                THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ���ѹ�ϵ���ϳ���");
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
                        THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR,"ҵ��ǼǺ������ж�:����%s���������ĺ����û���",tradRelaBuf.GetString("SERIAL_NUMBER_B", i).c_str());
                    }
                }
                catch(Exception &ex)
                {
                    THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ���ѹ�ϵ���ϳ���");
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
                    THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ�û���Ʒ���ϱ����");
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
                            THROW_C_P3(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:���룺%s���˲�Ʒ��\"%s\"����ϲ�Ʒ��\"%s\"���⣡",
                                        tradRelaBuf.GetString("SERIAL_NUMBER_B", i).c_str(), userProduct.c_str(), compProduct.c_str());
                        }
                    }
                    catch(Exception &ex)
                    {
                        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ��Ʒ�����������");
                    }
                }
            }
        }
    }
    LOG_TRACE(logTradeCheckAfterTrade, "�˳�ChkRelaUserProductLimit������");
}

//�ж�Ԫ�غ�������֮������ƹ�ϵ
void TradeCheckAfterTrade::CheckElmentsAndItsDiscnt(CFmlBuf &inBuf)
{
    LOG_TRACE(logTradeCheckAfterTrade, "����CheckElmentsAndItsDiscnt������");
    //�Ż�����-------------------
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
        THROW_AGC(CRMException, ex, Grade::WARNING, GETCOMMPARA_ERR, "��������Ԫ��:��ȡ̨���Ż�Ԫ�ع�ϵ�쳣!");
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
        THROW_AGC(CRMException, ex, Grade::WARNING, GETCOMMPARA_ERR, "��������Ԫ��:��ȡ�û��Ż�Ԫ�ع�ϵ�쳣!");
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
            THROW_AGC(CRMException, ex, Grade::WARNING, GETCOMMPARA_ERR, "��������Ԫ��:��ȡ̨������Ԫ�ع�ϵ�쳣!");
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
            THROW_AGC(CRMException, ex, Grade::WARNING, GETCOMMPARA_ERR, "��������Ԫ��:��ȡ�û�����Ԫ�ع�ϵ�쳣!");
        }
        if(iUserTcount > 0)
            bufTradeItem.Append(bufUserItem);
        iTcount = bufTradeItem.GetCount("TRADE_ID");
        if(iTcount > 0)
        {
            for(int j = 0; j < iCount; j ++)
            {
                exStr = bufCommpara.GetString("PARA_CODE5",j);  //��ʾ
                LOG_TRACE_P1(logTradeCheckAfterTrade, "bufCommpara@cccc2222DDcccc@@@@@@@@@@@]:%s",exStr.c_str());

                for(int i = 0; i < iTcount; i ++)
                {
                    if(bufCommpara.GetString("PARA_CODE6",j)=="1")  //ȡ��
                    {
                        if(bufCommpara.GetString("PARA_CODE2", j) == bufTradeItem.GetString("ATTR_CODE", i))
                        {
                            if(bufCommpara.GetString("PARA_CODE3", j).find("|" + bufTradeItem.GetString("ATTR_VALUE", i) + "|") != string::npos)
                            {
                                THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "�Żݺ����������ж�:%s!", exStr.c_str());
                            }
                        }
                    }
                    else
                    {
                        if(bufCommpara.GetString("PARA_CODE2", j) == bufTradeItem.GetString("ATTR_CODE", i))
                        {
                            if(bufCommpara.GetString("PARA_CODE3", j).find("|" + bufTradeItem.GetString("ATTR_VALUE", i) + "|") == string::npos)
                            {
                                THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "�Żݺ����������ж�:%s!", exStr.c_str());
                            }
                        }
                    }
                }
            }
        }
    }
    LOG_TRACE(logTradeCheckAfterTrade, "�˳�CheckElmentsAndItsDiscnt������");
}

//�ж�Ԫ�غ�������֮������ƹ�ϵ add by zhouf@2009-10-18
void TradeCheckAfterTrade::CheckElmentsAndItsAttr(CFmlBuf &inBuf,CFmlBuf &bufUserAllElements)
{
    LOG_TRACE(logTradeCheckAfterTrade, "����CheckElmentsAndItsAttr������");
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
        THROW_AGC(CRMException, ex, Grade::WARNING, GETCOMMPARA_ERR, "��������Ԫ��:��ȡ��������Ԫ�ع�ϵ�쳣!");
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
            LOG_TRACE_P2(logTradeCheckAfterTrade, "bufUserAllElements�Ż�:%s->%s",bufUserAllElements.GetString("ELEMENT_NAME", i).c_str(),bufCommpara.GetString("PARA_CODE5",j).c_str());

            if(bufCommpara.GetString("PARA_CODE6",j)=="1")  //ȡ������PARA_CODE6Ϊ1ʱ��ʾ���Ժ��Ż��б�����Żݲ���ͬʱ����
            {
                if(tmpStr.find("|" + bufUserAllElements.GetString("ELEMENT_ID", i) + "|") != string::npos)
                {
                    if(bufCommpara.GetString("PARA_CODE1",j).find("|" + bufCommpara.GetString("ATTR_VALUE",j) + "|") != string::npos)
                    {

                        THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "�Żݺ����������ж�:%s!", exStr.c_str());
                    }
                }
            }
            else
            {
                if(tmpStr.find("|" + bufUserAllElements.GetString("ELEMENT_ID", i) + "|") != string::npos)
                {
                    if(bufCommpara.GetString("PARA_CODE1",j).find("|" + bufCommpara.GetString("ATTR_VALUE",j) + "|") == string::npos)
                    {

                        THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "�Żݺ����������ж�:%s!", exStr.c_str());
                    }
                }
            }
        }

    }

    LOG_TRACE(logTradeCheckAfterTrade, "�˳�CheckElmentsAndItsAttr������");

    /*
    //��ȡ����̨�����ӵ����Ժ����Ʊ������Ժ�Ԫ��֮�����ƹ�ϵ����
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
        THROW_AGC(CRMException, ex, Grade::WARNING, GETCOMMPARA_ERR, "��������Ԫ��:��ȡ��������Ԫ�ع�ϵ�쳣!");
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
        if(bufCommpara.GetString("PARA_CODE6",j)=="1") //ȡ������PARA_CODE6Ϊ1ʱ��ʾ���Ժ��Ż��б�����Żݲ���ͬʱ����
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
                        THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "���Ժ��Żݵ������ж�:%s!",iter->second.second.second.c_str());
                }
            }
        }
    }
    for(iter = mapAttrInfo.begin(); iter != mapAttrInfo.end(); ++iter)
    {
        if(iter->second.second.first)
        {
            THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "���Ժ��Żݵ������ж�:%s!",iter->second.second.second.c_str());
        }
    }*/
}
/**
 * ҵ��ǼǺ������ж�
 * @param inBuf ����CFmlBuf���������
 * @param outBuf ���CFmlBuf���������
 * @return �ޣ�ʧ��ʱ�׳�����
 */
//##ModelId=41CA6C2A01D5
void TradeCheckAfterTrade::CheckAfterTrade(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
    //0.���������Ϣ
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
	string strSerialNumber;//add by zhangzh@2005-12-10 ����ҵ������
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
    //Add By WangRc һ��˫�Ÿ�����������ʶ Begin

    else strFamilyCount = "1";

    if (inBuf.IsFldExist("BRAND_CODE"))
	    strBrandCode = inBuf.GetString("BRAND_CODE");//Modify by Zhoush @ 2005-07-29
	if (inBuf.IsFldExist("SERIAL_NUMBER"))
	    strSerialNumber = inBuf.GetString("SERIAL_NUMBER");//add by zhangzh@2005-12-10 ����ҵ������
    if (inBuf.IsFldExist("PROCESS_TAG_SET"))
        strProcessTagSet = inBuf.GetString("PROCESS_TAG_SET");

	string strInModeCode = inBuf.IsFldExist("IN_MODE_CODE")?inBuf.GetString("IN_MODE_CODE"):"Z"; //Add by tz@2005-12-06 17:27
	string strNetTypeCode = inBuf.IsFldExist("NET_TYPE_CODE")?inBuf.GetString("NET_TYPE_CODE"):"00"; //Add by jianghp@2006-11-15 10:33
  StaticDAO& dao = StaticDAO::getDAO();
  	
  //�����жϲ���Ҫ�� CheckAfterTrade У��
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
      THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ����ֵ����");
  }
  for(int i=0; i < specialC; i++)
  {
      if(bufItemSpecial.GetString("ATTR_CODE", i) == "rightCode")
      {
          attrValue = bufItemSpecial.GetString("ATTR_VALUE", i);
      }
  }
  
  if(attrValue == "csCreateWileBossYUserTrade"){//�ܲ�Ԥ������������������ ����Ҫ��CheckAfterTrade
  	LOG_TRACE_P1(logTradeCheckAfterTrade, "attrValue=%s", attrValue.c_str());
  	return;	
  }
	//begin jianghp@2006-12-20 10:07 add
	//GtoC�ֻ����Ų����ж�
    if(strTradeTypeCode == "189")
	{
		return;
	}
	//end jianghp@2006-12-20 10:07 add


    //begin jianghp@2006-11-15 10:35 add
    //����˫ģ����ѡ��˫ģ��������ʾ����  2018(˫ģ20),1023(˫ģ10)

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
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ�Ƿ�ѡ����˫ģ����1ʱ����");
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
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ�Ƿ�ѡ����˫ģ����2ʱ����");
        }

		//2018(˫ģ20) iRecCount1,1023(˫ģ10) iRecCount2
   		if(strProcessTagSet.length() > 12)
   		{
	   		if(strProcessTagSet.substr(12,1)=="1")
	   		{
				if(strNetTypeCode == "10" && iRecCount1 > 0)
				{
		           THROW_C(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:���û���GSM�û�������ѡ��CDMA˫ģ����!");
				}

				if(strNetTypeCode == "20" && iRecCount2 > 0)
				{
		           THROW_C(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:���û���CDMA�û�������ѡ��GSM˫ģ����!");
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
		            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ�û��Ƿ�ѡ����˫ģ�Ż�ʱ����");
		        }
		        if(iRecCount3 == 0)
		        {
		           THROW_C(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��˫ģ���û�û��ѡ��˫ģ�Żݣ�����!");
		        }
	   		}
	   		else
	   		{
		        if(iRecCount1 + iRecCount2 > 0)
		        {
		           THROW_C(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:���û�����˫ģ��������ѡ��˫ģ����!");
		        }
	   		}
   		}
   		else
   		{
	        if(iRecCount1 + iRecCount2 > 0)
	        {
	           THROW_C(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:���û�����˫ģ��������ѡ��˫ģ����!");
	        }
   		}
    }
    //end jianghp@2006-11-15 10:35 add

    //begin jianghp@2006-12-27 11:20 add
    //˫ģ����Ʒ�������ѡ��˫ģ�Ż�
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
		            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ�û��Ƿ�ѡ����˫ģ�Ż�ʱ����");
		        }
		        if(iRecCount3 == 0)
		        {
		           THROW_C(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��˫ģ���û�û��ѡ��˫ģ�Żݣ�����!");
		        }
	   		}
   		}
    }
    //end jianghp@2006-12-27 11:19 add

    CFmlBuf bufUser;

    //Ѻ��ҵ��
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
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ�û����ϳ���");
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
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ̨�ʷ����ӱ����");
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
                THROW_AGC(CRMException, ex, Grade::WARNING, GETUSERFOREGIFT_ERR, "ҵ��ǼǺ������ж�:��ѯ�û�Ѻ�����");
            }

            for (int i=0;i<count;i++)
            {
                if(atol(tradeFeeSubBuf.GetString("FEE",i).c_str()) <0 && tradeFeeSubBuf.GetString("FEE_MODE",i) == "1") //���ˣ��ж��Ƿ��ܽ���Ѻ������
                {
                    string foregiftCode = tradeFeeSubBuf.GetString("FEE_TYPE_CODE",i);
                    string foregiftName = "";
                    int  foregiftFee = atol(tradeFeeSubBuf.GetString("FEE",i).c_str());
                    LOG_TRACE_P1(logTradeCheckAfterTrade, "---foregiftFee---%d��",foregiftFee);

                    int money = 0;
                    for (int j=0;j<foreGiftCount;j++)
                    {
                        if(userForeGift.GetString("FOREGIFT_CODE",j) == foregiftCode)
                        {
                           money = userForeGift.GetInt("MONEY",j)*100;
                           LOG_TRACE_P1(logTradeCheckAfterTrade, "---money---%d��",money);
                           foregiftName = userForeGift.GetString("FOREGIFT_NAME",j);
                        }
                    }
                    //��鹺��Ѻ���Ƿ�����˴���
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
                        THROW_AGC(CRMException, e, Grade::WARNING, 8888, "Ѻ�������쳣����ȡ������Ϣ�쳣��");
                    }
                    int canRefundFee = int(atof(tempBuf.GetString("PARA_CODE1").c_str())*100);
                    LOG_TRACE_P1(logTradeCheckAfterTrade, "---canRefundFee---%d��",canRefundFee);
                    string refundDate = tempBuf.GetString("END_DATE");
                    if(canRefundFee < 0)
                    {
                        THROW_C_P2(CRMException, 8888, "Ѻ������:%s���ڲ��������ˣ��뵽����ʱ��:%s�����������ˣ�",foregiftName.c_str(),refundDate.c_str());
                    }
                    if( (canRefundFee + foregiftFee) < 0)
                    {
                        THROW_C_P2(CRMException, 8888, "Ѻ������:%s���ڿ������˽��Ϊ: %d Ԫ��",foregiftName.c_str(),canRefundFee/100);
                    }

                    //����Ѻ�������ж�
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
                        THROW_AGC(CRMException, e, Grade::WARNING, 8888, "Ѻ�������쳣����ȡ����Ѻ����Ϣ�쳣��");
                    }

                    int fee = 0;
                    for (int j=0;j<tempBuf.GetCount("PARA_CODE1");j++)
                    {
                        if(tempBuf.GetString("PARA_CODE2",j) == foregiftCode)
                        {
                            fee = int(atof(tempBuf.GetString("PARA_CODE1",j).c_str())*100);
                            canRefundFee = money - fee; //�������˵Ĺ���Ѻ����
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
                                        THROW_AGC(CRMException, e, Grade::WARNING, 8888, "Ѻ�������쳣����ȡѺ�����������쳣��");
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
                                    THROW_C_P4(CRMException, 8888, "Ѻ������:Ŀǰֻ������%s���Ϊ: %d Ԫ,������������%s,��ȡ���������λ���ʳ�;����%s������",foregiftName.c_str(),canRefundFee/100,foregiftName.c_str(),day.c_str());
                                }
                                if (canRefundFee < 0)
                                {
                                     THROW_C_P1(CRMException, 8888, "Ѻ������:%sĿǰ�������ˣ�",foregiftName.c_str());
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
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ��ǰʡ�������");
    }
    if(iMainCount == 0)
    {
        THROW_C(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ��ǰʡ��������Ч���ݣ�");
    }
    else if(iMainCount == 1)
    {
        strProvince = bufTag.GetString("TAG_INFO");
    }
    if (strTradeTypeCode != "381")//sp�������Żݲ�У��Ȩ��
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
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ѯҵ��̨���Ż��ӱ����[%s]",ex.getCause()->what());
        }

    	for(int i = 0;i<iDiscntAdd; i++)
    	{
    	    int iDiscntCode = (int)bufVTradeDiscntAdd.GetInt("DISCNT_CODE",i);
    	    LOG_TRACE_P1(logTradeCheckAfterTrade, "iDiscntCode===��%d��",iDiscntCode);
    	    LOG_TRACE_P1(logTradeCheckAfterTrade, "strTradeStaffId===��%s��",strTradeStaffId.c_str());
    	    CFmlBuf bufVStaffdataright;
    	    int iRightCount = -1;
    	    try
    	    {
                char chIdA[10];
                sprintf(chIdA,"%d",iDiscntCode);
                dao.Clear();
    	        dao.SetParam(":VDATA_CODE", chIdA); //jianghp@2006-04-28 16:56 modify �������ͱ���
    	        dao.SetParam(":VDATA_TYPE", "D");
    	        dao.SetParam(":VSTAFF_ID", strTradeStaffId);
    	        iRightCount = dao.jselect(bufVStaffdataright, "SEL_STAFF_EXISTS_DATACODE_BY_TYPE","TF_M_STAFFDATARIGHT");
    	    }
    	    catch (Exception &ex)
            {
                THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ѯԱ������Ȩ�ޱ����[%s]",ex.getCause()->what());
            }
            if(iRightCount<1)
            {
                char chIdA[10];
                sprintf(chIdA,"%d",iDiscntCode);
                string strDiscntName = CParamDAO::getParam("DiscntName", chIdA);
                THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "�Բ�������Ȩ����%s���Żݣ�", strDiscntName.c_str());
            }
    	}
    }

    //Added by Qiumb@ 2006-11-27
    if(strTradeTypeCode == "330" || strTradeTypeCode == "350" )//���ֶһ������ֵ�������ֹ��Χ�ӿ�δ��ʣ�����ǿ����ҵ��
    {
        try
        {
            dao.Clear();
            bufUser.ClearFmlValue();
            dao.SetParam(":VUSER_ID", strUserId);
            if ( dao.jselect(bufUser, "SEL_BY_PK","TF_F_USER") == 0)
            {
                THROW_C(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ�û������޼�¼��");
            }
        }
        catch(Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ�û����ϳ���");
        }

        CFmlBuf bufTradeScore;
        try
        {
            dao.Clear();
            dao.SetParam(":VTRADE_ID", strTradeId);
            if (0 == dao.jselect(bufTradeScore, "SEL_BY_TRADE","TF_B_TRADE_SCORE"))
            {
                THROW_C(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:����ҵ��δ�����̨������");
            }
        }
        catch (CRMException &ex)
        {
        	throw;
        }
        catch (Exception &ex)
        {
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ѯ����̨������[%s]",ex.getCause()->what());
        }

        if ( atoi(string(bufUser.GetString("SCORE_VALUE")).c_str()) + atoi(string(bufTradeScore.GetString("SCORE_CHANGED")).c_str()) < 0)
        {
            //THROW_C_P2(CRMException, CHECKAFTERTRADE_ERR,"ҵ��ǼǺ������ж�:�û����ֲ����ۼ�����ǰֵ[%s]���춯ֵ[%s]",string(bufUser.GetString("SCORE_VALUE")).c_str(),string(bufTradeScore.GetString("SCORE_CHANGED")).c_str());
        }

        //ADD by Liujun 2011-05-07 ESS�����ֹ������ύǰ�ж�
        if(strTradeTypeCode == "350")
        {
            LOG_TRACE(logTradeCheckAfterTrade, "��������ֹ��������麯��");

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
            	  THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADESCOREMAIN_ERR, "��ȡ̨�����Ա����");
            }

            string scoreRight;
            string scoreChange;
            LOG_TRACE_P1(logTradeCheckAfterTrade, "����Ԥ�ύ������̨�����Ա�RowCount: %d:", iRowCount);
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
            LOG_TRACE_P1(logTradeCheckAfterTrade, "����Ԥ�ύ�������Ƿ����޸����Ƶ��ж�: %s", scoreRight.c_str());
            LOG_TRACE_P1(logTradeCheckAfterTrade, "����Ԥ�ύ������tradeId: %s", strTradeId.c_str());
            LOG_TRACE_P1(logTradeCheckAfterTrade, "����Ԥ�ύ������SerialNumber: %s", strSerialNumber.c_str());
            LOG_TRACE_P1(logTradeCheckAfterTrade, "����Ԥ�ύ������ҵ��Ա�����޸Ļ���ֵ: %s", scoreChange.c_str());

            if(scoreRight=="0")
            {
                LOG_TRACE(logTradeCheckAfterTrade, "����Ԥ�ύ������Ȩ����0����Ҫ�����жϣ�");
            try
            {
                dao.Clear();
                dao.SetParam(":VSERIAL_NUMBER",strSerialNumber);
                iRowCount = dao.jselect(userInfoBuf, "SEL_MONTH_TOTAL_SCORE_BY_NUMBER", "TF_B_TRADE_SCORE");
            }
            catch (Exception &ex)
            {
                THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADESCOREMAIN_ERR, "��ȡ̨�ʻ����������");
            }

            //���㵱���ܹ������Ļ��ֽ��
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

            LOG_TRACE_P1(logTradeCheckAfterTrade, "����Ԥ�ύ���������µ����Ļ����ܶ���: %d:", scoreValue);

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
        	      THROW_AGC(CRMException, ex, Grade::WARNING, 8888, "У����غ��룺��ȡ�û�Ƿ����Ϣ�쳣��");
            }

            //���ε����޶�
            string oneChgLimit;

            //�¶ȵ�����
            string chgMonthLimit;

            if(iRowCount>=0)
            {
            //��ȡ��������������¶���������ܶ�
            oneChgLimit=userInfoBuf.GetString("PARA_CODE1",0);
            LOG_TRACE_P1(logTradeCheckAfterTrade, "����Ԥ�ύ����oneChgLimit: %s:", oneChgLimit.c_str());

            chgMonthLimit = userInfoBuf.GetString("PARA_CODE2",0);
            LOG_TRACE_P1(logTradeCheckAfterTrade, "����Ԥ�ύ����chgMonthLimit: %s:", chgMonthLimit.c_str());

            //��ʾ�������ܴ��ڵ��ε�����!
            if(atoi(oneChgLimit.c_str())- atoi(scoreChange.c_str())<0)
                {
        	          LOG_TRACE(logTradeCheckAfterTrade, "����Ԥ�ύ������һ�ε�����ܴ����������");

                    THROW_C(CRMException, 8888, "�������ܴ��ڵ��ε����");
                }

            if(atoi(chgMonthLimit.c_str())-scoreValue-atoi(scoreChange.c_str())<0)
                {
        	          LOG_TRACE(logTradeCheckAfterTrade, "����Ԥ�ύ������������ܴ��ڵ����������");

                    //��������������
                    int leftScore = atoi(chgMonthLimit.c_str())-scoreValue;

                    LOG_TRACE_P1(logTradeCheckAfterTrade, "����Ԥ�ύ���������»����Ե������ֶ�: %d:", leftScore);

                    //�����Ļ��ֲ��ܴ����µ���������Ե����Ľ��ΪleftScore;
                    THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR,"�����Ļ��ֲ��ܴ��ڵ��µ���������Ե���[%d]���֣�", leftScore);
                }
            }

            LOG_TRACE(logTradeCheckAfterTrade, "�˳�CheckUserRight����");

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
				THROW_AGC(CRMException, ex, Grade::WARNING, ADDUSERDISCNT_ERR, "���ݳ�ԱƷ�Ʋ�ѯ�����Ż��쳣��");
			}
			if (count > 1)
			{
				string strDateTime;//�Ƿ���������Ч�Ż�
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
					THROW_AGC(CRMException, ex, Grade::WARNING, ADDUSERDISCNT_ERR, "���ݳ�ԱƷ�Ʋ�ѯ����������Ч�Ż��쳣��");
				}
				if (count == 0)
				{
					THROW_C(CRMException, ADDUSERDISCNT_ERR, "���ݳ�ԱƷ�ƻ�ȡ�����Ż���Ϣ�쳣�����飡");
				}
				else if (count > 1)
				{
					THROW_C(CRMException, ADDUSERDISCNT_ERR, "���ݳ�ԱƷ��ѡ�����Ż�ȡ��������¼�����飡");
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
					THROW_AGC(CRMException, ex, Grade::WARNING, ADDUSERDISCNT_ERR, "��ѯ��Ա��ѡ�����Ż��쳣��");
				}
				if (count >1)
				{
					string strDateTime;//�Ƿ���������Ч�Ż�
					DualMgr::GetFirstDayOfNextMonth(strDateTime);

					try{
						dao.Clear();
						dao.SetParam(":VUSER_ID_A", strUserIdAV);
						dao.SetParam(":VEND_DATE", strDateTime);
						count = dao.jselect(bufVUserDiscnt, "SEL_GRP_DISCNT_BY_BRAND_NEXT1","TF_F_USER_DISCNT");
					}
					catch(Exception &ex)
					{
						THROW_AGC(CRMException, ex, Grade::WARNING, ADDUSERDISCNT_ERR, "���ݳ�ԱƷ�Ʋ�ѯ����������Ч�Ż��쳣��");
					}
					if (count == 0)
					{
						THROW_C(CRMException, ADDUSERDISCNT_ERR, "���ݳ�ԱƷ��û�в�ѯ�����ſ�ѡ�Ż�1������ӣ�");
					}
					else if (count >1)
					{
						THROW_C(CRMException, ADDUSERDISCNT_ERR, "���ݳ�ԱƷ��û�в�ѯ�����ſ�ѡ�Żݶ�����¼����ȷ�ϣ�");
					}
				}
				else if (count == 0)
				{
					THROW_C(CRMException, ADDUSERDISCNT_ERR, "���ݳ�ԱƷ��û�в�ѯ�����ſ�ѡ�Żݣ�����ӣ�");
				}
			}
		}
    }
	//add by zhangzh 2006-01-13 end
	/*
    //add by liudx2@2011-04-29 begin �μ���������ĺ��룬����Ʒ������ܵ���66Ԫ�ײ������Ͷ��2��ҪС���ײͷ���
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
            THROW_AGC(CRMException, e, Grade::WARNING, GENEOLCOM_ERR, "��ȡ���������Ʒ�����Ϣʧ��");
        }
        if ( countGift > 0)
        {
            int productFee = bufGiftFor3G.GetInt("PARA_CODE1");
            int productFeeDeal = bufGiftFor3G.GetInt("PARA_CODE3");
            int GiftFee = bufGiftFor3G.GetInt("BFEE");
            if ( productFee < 66 )
                THROW_C(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:�������͵�3G�����ײ�ֵ���õ���66Ԫ��");
            if ( GiftFee/100 > productFeeDeal)
                THROW_C_P2(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:�ײ��趨���Ͷ�%dԪ����С���Ѳμ����ͽ��%dԪ��",productFeeDeal,GiftFee);
        }
    }*/
    //add by liudx2@2011-04-29 end
	//add by zhangzh@2006-03-14 begin ������ű�����267�������ã����жϲ���������ϵ
	 CFmlBuf bufCommpara;
	 if (strTradeTypeCode != "381")//sp������У��
   {
        try
        {
            int count = 0;
            dao.Clear();
            dao.SetParam(":VSUBSYS_CODE", "CSM");
            dao.SetParam(":VPARAM_ATTR", 267);
            dao.SetParam(":VPARAM_CODE", strBrandCodeV);//���Ų�Ʒ����
            dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
            count = dao.jselect(bufCommpara, "SEL1_PK_TD_S_COMMPARA","TD_S_COMMPARA");
        }
        catch(Exception &e)
        {
            THROW_AGC(CRMException, e, Grade::WARNING, GENEOLCOM_ERR, "ȡ��Ʒ������־����ͨ�ò���267��ȡ��Ʒ����������־ʧ��");
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
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ���������Ʊ�ǳ���");
        }
        if(iMainCount == 1)
        {
            strLimitType = bufTag.GetString("TAG_CHAR");
        }
   }

    //�����û�����  �ж��Ƿ���������������(�Ƿ���ڲ�Ʒ����)
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
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ��ǰʡ�������");
        }
        if(iTradeRela == 0)
        {
            THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ̨�ʹ�ϵ�ӱ��쳣[TRADE_ID:%s]��", inBuf.GetString("TRADE_ID").c_str());
        }

        string strUserIdA = "";
        if(iTradeRela > 0)
        {
            strUserIdA = bufTradeRelaTemp.GetString("ID_A");
            strTradeRelaTypeCode = bufTradeRelaTemp.GetString("RELATION_TYPE_CODE");
LOG_TRACE_P1(logTradeCheckAfterTrade, "----strTradeRelaTypeCode:[%s]-----", strTradeRelaTypeCode.c_str());
        }

        CFmlBuf bufUser;//ȡ���ŵĲ�Ʒ
        try
        {
            dao.Clear();
            dao.SetParam(":VUSER_ID", strUserIdA);
            if (dao.jselect(bufUser, "SEL_BY_PK","TF_F_USER") == 0)
            {
                THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:�����ļ����û�������[ID:%s]��", strUserIdA.c_str());
            }
        }
        catch(Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ��ǰʡ�������");
        }

        CFmlBuf bufUserRoleB;//ȡ�û��Ĳ�Ʒ
        try
        {
            dao.Clear();
            dao.SetParam(":VUSER_ID",  inBuf.GetString("USER_ID"));
            if (dao.jselect(bufUserRoleB, "SEL_BY_PK","TF_F_USER") == 0)
            {
                THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:�����ļ����û�������[ID:%s]��", strUserIdA.c_str());
            }
        }
        catch(Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ��ǰʡ�������");
        }

        CFmlBuf bufVProductLimit;
        int iProductRoleCodeA = -1;
        iProductRoleCodeA = (int)bufUser.GetInt("PRODUCT_ID");

        int iProductLimit = -1;
        try
        {
            dao.Clear();
            bufVProductLimit.ClearFmlValue();
            dao.SetParam(":VPRODUCT_ID_A", iProductRoleCodeA);//���Ų�Ʒ
            dao.SetParam(":VPRODUCT_ID_B", (int)bufUserRoleB.GetInt("PRODUCT_ID"));//�û���Ʒ
            iProductLimit = dao.jselect(bufVProductLimit, "SEL_CHECK_LIMIT_0_NEW","TD_S_PRODUCTLIMIT");
        }
        catch(Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ѯ��Ʒ�����ƹ�ϵ����");
        }

        if ( iProductLimit > 0)//��ʾ������
        {
            char chIdA[10];
            sprintf(chIdA,"%d",(int)bufVProductLimit.GetInt("PRODUCT_ID_B"));
            string strNameA = CParamDAO::getParam("ProductName", chIdA);
            THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:�û���Ʒ����[%s]�����ܼ���ü��ţ�", strNameA.c_str());
        }

        if (iProductLimit == 0)//��ʾ����,�����´��߼��ж�
        {
            //����û���������Ч��Ʒ������������Ч��Ʒ�ͼ��Ų�Ʒ�����ڻ��⣬���������
            CFmlBuf bufUserPreProd;
            try
            {
                dao.Clear();
                dao.SetParam(":VUSER_ID", inBuf.GetString("USER_ID"));
                iMainCount = dao.jselect(bufUserPreProd, "SEL_PRE_PROD_NEW","TF_F_USER_PRODUCT");
            }
            catch(Exception &ex)
            {
                THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:����û��Ƿ����²�Ʒ����");
            }

            if (iMainCount == 0)//��������Ч����Ʒ
            {
                //char chIdA[10];
                //sprintf(chIdA,"%d",(int)bufVProductLimit.GetInt("PRODUCT_ID_B"));
                //string strNameA = CParamDAO::getParam("ProductName", chIdA);
                //THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:�û�����[%s]�����ܼ���ü��ţ�", strNameA.c_str());
            }
            else if (iMainCount > 0)
            {   //�ж��²�Ʒ�Ƿ�������뼯��
                int iProductId = bufUserPreProd.GetInt("PRODUCT_ID");
                CFmlBuf bufVProductLimitPre;
                int iProductLimitPre = -1;
                try
                {
                    dao.Clear();
                    bufVProductLimitPre.ClearFmlValue();
                    dao.SetParam(":VPRODUCT_ID_A", iProductRoleCodeA);  //���Ų�Ʒ
                    dao.SetParam(":VPRODUCT_ID_B", iProductId);         //�û����²�Ʒ
                    iProductLimitPre=dao.jselect(bufVProductLimitPre, "SEL_CHECK_LIMIT_0_NEW","TD_S_PRODUCTLIMIT");
                }
                catch(Exception &ex)
                {
                    THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ѯ��Ʒ�����ƹ�ϵ����");
                }
                if (iProductLimitPre > 0)
                {
                    char chIdA[10];
                    sprintf(chIdA,"%d",(int)bufVProductLimitPre.GetInt("PRODUCT_ID_B"));
                    string strNameA = CParamDAO::getParam("ProductName", chIdA);
                    THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:�û�ԤԼ�Ĳ�Ʒ����[%s]�����ܼ���ü��ţ�", strNameA.c_str());
                }
                else
                {
                    ;
                }
            }
        }
    	//MODIFY BY HUANGM@2006-04-06 END

     	/*
        //VPMN��Ա���������������,�ж��Ƿ��б���vpmnע���ļ�¼�������ֻ��������Ч�����ܱ�����Ч
        if (inBuf.GetString("EXEC_TIME") < strFirstDayOfNextMonth && inBuf.GetString("RSRV_STR2") == "20")
        {
            //�ж��Ƿ��б���ȡ���ģ�����Ǳ���ȡ������������
            CFmlBuf bufUserRelation;
            try
            {
                dao.Clear();
                dao.SetParam(":VUSER_ID_B", strUserId);
                dao.SetParam(":VRELATION_TYPE_CODE", "20");
                if (dao.jselect(bufUserRelation, "SEL_USERRELA_BY_IDB","TF_F_RELATION_UU") > 0)
                {
                    //THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:�ú���Ŀǰ�ڼ���[%s]�У�", string(bufUserRelation.GetString("SERIAL_NUMBER_A")).c_str());
                }
            }
            catch(Exception &ex)
            {
                THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:����û���ϵ�����");
            }
        }
        */
    }

	//Modify by Zhoush @ 2005-07-09
	if(strTradeTypeCode=="241")
	{
		/*//�Ƿ���Թ���ȡ��
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
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ����ҵ���������");
        }

		if(iCount > 0)
		{
			if(string(bufVPurchasetrade.GetString("PARA_CODE1")).substr(0,1) != "1")
			{
				THROW_C(CRMException, CHECKAFTERTRADE_ERR, "�˹���ҵ�񲻿���ȡ����");
			}
		}
		else
		{
            THROW_C(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ����ҵ������޼�¼��");
		}
		*/
	}

	if(strTradeTypeCode == "192")
	{
		//20110628 add by wuxg for ��������
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
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "����û���������ҵ������Ϣʧ�ܣ�");
        }

        if(iCount>0)
        {
			THROW_C(CRMException, CHECKAFTERTRADE_ERR, "�û������˹������λ�ȡ���������β����������ѳ����ڣ���������������");
        }*/
	}

	if (strTradeTypeCode != "381" && strTradeStaffId!="Z0000VAC")//sp������У��
  {
	//Modify by Zhoush @ 2005-07-09
    if (strTradeTypeCode == "121")//��������
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
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "����û��Ƿ���������Ż�ʧ�ܣ�");
        }

        if(iCount>0)
        {
        	  THROW_C(CRMException, CHECKAFTERTRADE_ERR, "�������û���ǰ�������Żݣ����ܼ�������");
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
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡͨ�ò���������������������[%s]",ex.getCause()->what());
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
                THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡͨ�ò���������������������[%s]",ex.getCause()->what());
            }

            if(iCount>0)
            {
            	  THROW_C(CRMException, CHECKAFTERTRADE_ERR, "�û������ܷ������ͣ����ܰ�����Żݣ�");
            }
        }
    }


        //1.��ȡ̨�˲�Ʒ�ӱ���Ϣ
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
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ѯҵ��̨�˲�Ʒ�ӱ����[%s]",ex.getCause()->what());
        }

        //2.��ȡ̨�˷����ӱ���Ϣ
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
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ѯҵ��̨�˷����ӱ����[%s]",ex.getCause()->what());
        }
         /*
        if (strInModeCode.find_first_of("03") == string::npos)         //Add by tz@2005-12-06 17:30 �ų� "��ͨӪҵ��"��"���Ͽͷ�"
        {
            //2.1����Ƿ��в�Ʒ������ķ��� Add by Anj@2005-12-02
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
                THROW_AGC(CRMException, ex, Grade::WARNING, GETUSERALLSERVICE_ERR, "��ѯ��Ʒ��������");
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
                    THROW_C_P2(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��Ʒ��%s�����ܰ������%s�������޸ĺ����ύ��", strNameB.c_str(), strNameA.c_str());
                }
            }
        }

        //������ı�������Ƿ����� td_b_serv_itema.item_can_null
        CFmlBuf bufVServItema;
        int iServItemaCount;
        for(int i=0; i<iSvcCnt; i++)
        {
            if (bufVTradeSvc.GetString("MODIFY_TAG",i) == "1" ) continue;

            //���÷����Ƿ��б���������
            bufVServItema.ClearFmlValue();
            dao.Clear();
            dao.SetParam(":VID", (int)bufVTradeSvc.GetInt("SERVICE_ID",i));
            dao.SetParam(":VITEM_CAN_NULL", "0");  //����Ϊ��
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
                        THROW_C_P2(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:����%s���Ĳ�����%s��Ϊ���������������ύ��", strNameA.c_str(), string(bufVServItema.GetString("ITEM_LABLE",i)).c_str());
                    }
                }
            }
        }

        //��2010-12-31 23:59:59����Ϊ����Զʱ��
        for ( int i=0; i<iSvcCnt; i++)
        {
            if (bufVTradeSvc.GetString("END_DATE",i) > "2010-12-31 23:59:59")
                bufVTradeSvc.SetString("END_DATE","2010-12-31 23:59:59",i);
        }

        */
        //3.��ȡ̨���Ż��ӱ���Ϣ
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
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ѯҵ��̨���Ż��ӱ����[%s]",ex.getCause()->what());
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
                THROW_C(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡͨ�ò�����Ϣ����!");
            }
            if (count >0 && bufVTradeDiscnt.GetString("MODIFY_TAG",i)=="0")
            {
                char ch[10];
                string strDiscntCode;
                sprintf(ch,"%d",(int)bufVTradeDiscnt.GetInt("DISCNT_CODE",i));
                strDiscntCode=ch;
                string strDiscntName = CParamDAO::getParam("DiscntName", strDiscntCode);
                THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:[%s]�Żݲ������ڴ˽�������!", strDiscntName.c_str());
            }
            else
            if (count >0 && bufVTradeDiscnt.GetString("MODIFY_TAG",i)=="1")
            {
            	  char ch[10];
              	string strDiscntCode;
              	sprintf(ch,"%d",(int)bufVTradeDiscnt.GetInt("DISCNT_CODE",i));
              	strDiscntCode=ch;
              	string strDiscntName = CParamDAO::getParam("DiscntName", strDiscntCode);
                THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:[%s]�Żݲ������ڴ˽���ȡ��!", strDiscntName.c_str());
            }
        }

        //��2010-12-31 23:59:59����Ϊ����Զʱ��
        for ( int i=0; i<iDstCnt; i++)
        {
            if (bufVTradeDiscnt.GetString("END_DATE",i) > "2050-12-31 23:59:59")
                bufVTradeDiscnt.SetString("END_DATE","2050-12-31 23:59:59",i);
        }

        LOG_TRACE_P1(logTradeCheckAfterTrade, "----0.0[%d]-----", iPdtCnt);
        LOG_TRACE_P1(logTradeCheckAfterTrade, "----0.1[%d]-----", iSvcCnt);
        LOG_TRACE_P1(logTradeCheckAfterTrade, "----0.2[%d]-----", iDstCnt);

        //4.��ȡδչ��̨���ӱ���Ϣ
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
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ѯҵ��̨��������Ϣ�ӱ����[%s]",ex.getCause()->what());
        }


        //5.��ȡSP̨���ӱ���Ϣ
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
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ѯҵ��̨��SP��Ϣ�ӱ����[%s]",ex.getCause()->what());
        }

        //6.��ȡ��Ʒ������Ԫ��̨���ӱ���Ϣ
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
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ѯҵ��̨������Ԫ����Ϣ�ӱ����[%s]",ex.getCause()->what());
        }

        //7.���в�Ʒ��������Żݷ������ʱ����������ж�
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
					LOG_TRACE(logTradeCheckAfterTrade, "�˳������У�顣����");
					return;
				}else{
					//��Ʒ���Ƽ��
		            CheckProductModelLimit(inBuf, outBuf);
		            CheckElmentsAndItsDiscnt(inBuf);
				}
		    }
		    catch(Exception &ex)
		    {
		        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:�˳���ƷУ��TAGУ���쳣��");
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
                THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:���Һ���󶨲�Ʒ��Ϣʧ��[%s]",ex.getCause()->what());
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
                    THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:���Һ���󶨲�Ʒ��Ϣʧ��",ex.getCause()->what());
                }
                if (tempBuf.GetInt("X_SELCOUNT")==0)
                {
                    THROW_C(CRMException, CHECKAFTERTRADE_ERR, "�������Ʒ��У��:�������Ʒ��ƥ��");
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
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ѯҵ��̨�˲�Ʒ�ӱ����[%s]",ex.getCause()->what());
        }

        for (int i=0;i<iPdtCnt;i++)
        {
            if ((bufVTradeProduct.GetString("MODIFY_TAG",i)=="0"||bufVTradeProduct.GetString("MODIFY_TAG",i)=="2")&&(strTradeTypeCode!="250"))
            {
                CFmlBuf tempBuf;
                int iRowCount ;
                bool bFound = false ;
                 //�����Ʒ�໥����
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
                    THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:���Һ���󶨲�Ʒ��Ϣʧ��[%s]",ex.getCause()->what());
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
                        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:���Һ���󶨲�Ʒ��Ϣʧ��",ex.getCause()->what());
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
                    	    THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:���Һ���󶨲�Ʒ��Ϣʧ��",ex.getCause()->what());
                    	}
                    	if (tempBuf.GetInt("X_SELCOUNT")==0)
                    	{
                    	    THROW_C(CRMException, CHECKAFTERTRADE_ERR, "�������Ʒ��У��[��Ʒ��Ŷλ���ǿ��]:�������Ʒ��ƥ��");
                    	}
                    	else  bFound = true ;
                    }
                    else  bFound = true ;
                }
                if( bFound ) continue ;

                //��Ʒ���ƺ���� add by digy@20070908 start
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
                    THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:���Һ���󶨲�Ʒ��Ϣʧ��[%s]",ex.getCause()->what());
                }
				// �ų�1060������֤ update by dangsw@20120206 xizhaosi
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
                        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:���Һ���󶨲�Ʒ��Ϣʧ��",ex.getCause()->what());
                    }
                    if (tempBuf.GetInt("X_SELCOUNT")==0)
                    {
                        THROW_C(CRMException, CHECKAFTERTRADE_ERR, "�������Ʒ���ƣ��ú��벻������˲�Ʒ");
                    }
                    else bFound = true ;
                }				
                if( bFound ) continue ;
                //��Ʒ���ƺ����
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
                    THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:���Һ���󶨲�Ʒ��Ϣʧ��1[%s]",ex.getCause()->what());
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
                        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:���Һ���󶨲�Ʒ��Ϣʧ��",ex.getCause()->what());
                    }
                    if (tempBuf.GetInt("X_SELCOUNT")==0)
                    {
                        THROW_C(CRMException, CHECKAFTERTRADE_ERR, "�������Ʒ��У��[��Ʒǿ�ƺŶ�]:��Ʒ����벻ƥ��");
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
                    THROW_AGC(CRMException, ex, Grade::WARNING, GETSVCSTATE_ERR, "��ȡ��Ʒ��Ϣ�쳣����");
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
		//��������3G���Ų��ܱ���ʷѵ��������ײ͵��ײ�
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
            	   THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "��ȡʵ�����汾��ǳ���");
        	}
					if(iTagCount > 0) 
					{
						  //qc:8651 Begin
							int lastMonthCnt12 = -1;
							CFmlBuf bufIsLuckyNumber;
              dao.Clear();
              dao.SetParam(":VUSER_ID", strUserId);
              lastMonthCnt12 = dao.jselect(bufIsLuckyNumber, "CHECK_LUCKEY_NUMBER_LASTMONTH_BJ","TD_S_CPARAM");
              //��������88888888��88888881�ʷ��Ż�ʱ���ж�Ϊ���ţ�������������������ߣ�
              if(lastMonthCnt12 > 0)
              {
              	LOG_TRACE(logTradeCheckAfterTrade, "���û�����88888888�ʷѣ���û�й���");
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
		                		//LOG_TRACE_P1(logTradeCheckAfterTrade, "��ѯ���ſɰ����ײ͵�dao:%s",dao.parToString().c_str());
		                		LOG_TRACE_P1(logTradeCheckAfterTrade, "+++++++++++++��ѯ�������ſ��԰�����ײ͸���++++++++++++=[%d]",phoneProductCnt);
		                }
					 				}
					 			}
              }
              else
              {
              	LOG_TRACE(logTradeCheckAfterTrade, "���û�������88888888�ʷѣ������Ѿ����ڣ������ñ�־���������ж�");
              	//�������ã����������ж�
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
                	//�������һ���¿ɰ�����
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
            		THROW_C_P1(CRMException, 8888, "�˺���Ϊ���ţ���������Ϊ�˲�Ʒ���ɱ����ƷΪ:%s��",checkErrorInfo.c_str());
        	}
        	
        }

        //5.�ͷ���״̬�йص��ж�
        //5.0��ȡҵ��̨�˷���״̬�ӱ�
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
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ѯҵ��̨�˷���״̬�ӱ����[%s]",ex.getCause()->what());
        }

        //5.1����״̬������
        if (iSvcStateCnt != 0)
        {
            //��ȡҵ�������û����еķ���״̬
            CFmlBuf bufVTradeSvcState_UserAllSvcState;
            GetUserAllSvcstateAfterTrade(strTradeId, strUserId, bufVTradeSvcState_UserAllSvcState);
            int iUserAllSvcState = bufVTradeSvcState_UserAllSvcState.GetCount("SERVICE_ID");
            //�����ж�
            CFmlBuf bufVSvcstateLimit;
            for (int i = 0 ; i<iSvcStateCnt; i++) //it
            {
                if (bufVTradeSvcState.GetString("MODIFY_TAG",i) == "0" || bufVTradeSvcState.GetString("MODIFY_TAG",i) == "A")  //��
                {
                    //�����ж�
                    if (ExistsSvcstateLimitA((int)bufVTradeSvcState.GetInt("SERVICE_ID",i), bufVTradeSvcState.GetString("STATE_CODE",i), strEparchyCode, "0", bufVSvcstateLimit))
                    {
                        //
                        int iSvcstateLimit = bufVSvcstateLimit.GetCount("SERVICE_ID");
                        for (int j=0; j<iUserAllSvcState; j++) //itAllSvcstate
                        {
                            //�ų��Լ���������¼�������ų��û����е�ͬһ������
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
                                    THROW_C_P4(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��������״̬[%s.%s]�ͷ���״̬[%s.%s]���⣬����ͬʱ��Ч��ҵ���ܼ�������", strName.c_str(), strNameA.c_str(), strName.c_str(), strNameB.c_str());
                                }
                            }
                        }
                    }

                    //���������ж�
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
                                if (k != 0) strNameB += " �� ";
                                strNameB += CParamDAO::getParam("SvcstateName", chId, bufVSvcstateLimit.GetString("STATE_CODE_B",k));
                            }
                            THROW_C_P3(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��������״̬[%s.%s]������Ч����Ϊ���������ķ���״̬[%s]�����ڡ�ҵ���ܼ�������", strName.c_str(), strNameA.c_str(), strNameB.c_str() );
                        }
                    }

                    //��ȫ����
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
                                THROW_C_P4(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��������״̬[%s.%s]������Ч����Ϊ���������ķ���״̬[%s.%s]�����ڡ�ҵ���ܼ�������", strName.c_str(), strNameA.c_str(), strName.c_str(), strNameB.c_str() );
                            }
                        }
                    }
                }
                else if (bufVTradeSvcState.GetString("MODIFY_TAG",i) == "1" || bufVTradeSvcState.GetString("MODIFY_TAG",i) == "B")  //ɾ
                {
                    //����������
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
                                        THROW_C_P4(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:����״̬[%s.%s]������Ч����Ϊ���������ķ���״̬[%s.%s]��ɾ����ҵ���ܼ�������", strName.c_str(), strNameA.c_str(), strName.c_str(), strNameB.c_str() );
                                    }
                                    ////////////
                                }
                            }
                        }
                    }
                    //����ȫ����
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
                                    THROW_C_P4(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:����״̬[%s.%s]���ܱ�ɾ������Ϊ�����û�����һ������״̬[%s.%s]��������ҵ���ܼ�������", strName.c_str(), strNameA.c_str(), strName.c_str(), strNameB.c_str() );
                                }
                            }
                        }
                    }
                }
                else if (bufVTradeSvcState.GetString("MODIFY_TAG",i) == "2")  //��
                {
                    ;
                }
                else if (bufVTradeSvcState.GetString("MODIFY_TAG",i) == "3")  //����
                {
                    ;
                }
                else
                {
                    THROW_C(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:����״̬���޸ı�ǲ���ȷ��");
                }
            }
        }
        //�˻��������
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
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ѯҵ��̨���˻��ӱ����");
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
    			    THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ѯҵ��̨���˻��ӱ����");
    			}
    			if (iRelationUu>0)
    				THROW_C(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��Ա�û����ܸ����˻�");
    		}
        }
  }
  else
  {
    CheckSpProductModelLimit(inBuf, outBuf);
  }
    //Added by Yangf@2008-12-27����ϲ�Ʒ����˲�Ʒ����У��(������У��)
    ChkRelaUserProductLimit(strTradeId, strUserId, strEparchyCode);
}


/**
 *������iServiceId��״̬strStateCode������״̬�Ƿ���ڻ��⡢������������ȫ�����Ĺ�ϵ
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ѯ����״̬���Ʊ����[%s]",ex.getCause()->what());
    }
    return false;
}

/**
 *������iServiceId��״̬strStateCode������״̬�Ƿ���ڱ���������������ȫ�����Ĺ�ϵ
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ѯ����״̬���Ʊ����[%s]",ex.getCause()->what());
    }
    return false;
}

/**
 *��ȡ����ҵ����û������з���״̬
 *
 */
void TradeCheckAfterTrade::GetUserAllSvcstateAfterTrade(const string &strTradeId, const string &strUserId, CFmlBuf &bufVTradeSvcState_UserAllSvcState)
{
    bufVTradeSvcState_UserAllSvcState.ClearFmlValue();

    StaticDAO& dao = StaticDAO::getDAO();
    CFmlBuf bufTag;
    string strTag="";
    try  //�����м��cpp�л�ȡ�ò���
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
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "����״̬���:��ȡ��ǰ��������");
    }

	if (strTag != "1")
    {
    	//��ȡ����ҵ����û����з���״̬
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
		    THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ����ҵ����û��ķ���״̬����");
		}
	}
	else //������ˮ̨���ӱ���Ϊ���ձ�
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
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ����ҵ����û��ķ���״̬����");
        }
	}
}


//�ж��Ƿ��ǵ绰����
inline bool IsPhone(const string &strPhone, string &strMsg)
{
    StaticDAO& dao = StaticDAO::getDAO();
    if ((strPhone.substr(0,2) == "13" || strPhone.substr(0,3) == "159"))
    {
        if (strPhone.length() != 11)
        {
            strMsg = "�ֻ�����ĳ��ȱ���Ϊ11λ��";
            return false;
        }
        else
            return true; //�ֻ�����
    }
    else if (strPhone.substr(strPhone.length()-5) == "12580") //�������
    {
        //���̻���ͷ�Ƿ�Ϊ���������� �Լ������Ƿ�Ϸ�
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
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ��ǰʡ�������");
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
    	    strMsg = "����12580ǰ����ϵ绰����";
    	}

        return bFind;
    }
    else
    {
        //���̻���ͷ�Ƿ�Ϊ���������� �Լ������Ƿ�Ϸ�
        if (strPhone.substr(0,1) != "0" || strPhone.substr(0,2)=="00")
        {
            strMsg = "�̶��绰ǰ�����������ţ�";
            return false;
        }
        else if (strPhone.length() < 11)
        {
            strMsg = "�̶��绰���Ȳ���С��11λ��";
            return false;
        }
        else if (strPhone.length() > 12)
        {
            strMsg = "�̶��绰���Ȳ��ܴ���12λ��";
            return false;
        }
        else if (strPhone.substr(strPhone.length()-8) == "00000000")
        {
            strMsg = "�̶��绰����Ϊ00000000��";
            return false;
        }

        return true;
    }
}


/**
 * ���󶨵ĺ����Ƿ����
 */
void TradeCheckAfterTrade::CheckBindPhone(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
    LOG_TRACE(logTradeCheckAfterTrade, "����CheckBindPhone����");

    string strTradeId = inBuf.GetString("TRADE_ID");

    int iCnt(0);
    StaticDAO& dao = StaticDAO::getDAO();
    //��������ת���Ժ�ת���������
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
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ����ҵ����û��ķ���״̬����[%s]");
    }

    if (iCnt > 0 && bufTradeSvc.GetString("MODIFY_TAG") != "1")
    {
        string strSN = bufTradeSvc.GetString("SERV_PARA1");
        string strMsg;
        if (!IsPhone(strSN, strMsg))
        {
            string strName = CParamDAO::getParam("ServiceName", "12");
            THROW_C_P2(CRMException, CHECKAFTERTRADE_ERR, "����[%s]�ĺ�ת�����ʽ����ȷ:%s��", strName.c_str(), strMsg.c_str());
        }
    }

    LOG_TRACE(logTradeCheckAfterTrade, "�˳�CheckBindPhone����");
}

//��������������� QAM_YEARFEEQRY_OS ��ȡ���û�����Э���Ѷ�󣬴������񷵻ؽ��
void TradeCheckAfterTrade::CheckPurchaseAcctReturn(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckAfterTrade, "���� CheckPurchaseAcctReturn ����");
	//�жϵ��ý��
    int resultCode = inBuf.GetInt("X_RESULTCODE");
    string resultInfo = inBuf.GetString("X_RESULTINFO");
    if (resultCode != 0)
    {
        THROW_C(CRMException, CHECKAFTERTRADE_ERR, "����������Э���Ѳ�ѯ�ӿ�ʧ��:" + resultInfo);
    }

    int iAcctCount = inBuf.GetInt("X_RECORDNUM");
    long lUsedFee = 0;
    int iFoundCount = 0;
	float iTotalFee = 0;
	float iCurrFee  = 0;

    string strAddupValue;
    StaticDAO& dao = StaticDAO::getDAO();
	CFmlBuf ibuf;
	//��ȡ������ʱ�䣬�Լ����Ѷ��
	string strUserId = inBuf.GetString("USER_ID");
    try
    {
        dao.Clear();
        ibuf.ClearFmlValue();
        dao.SetParam(":VUSER_ID", strUserId);
        dao.SetParam(":VBINDSALE_ATTR", "0"); //1��Ѻ�𹺻�
        //��ѯ�����û�����1��Ѻ��Ĺ�����¼�������յ�������,modify by chenzm@2007-3-9
        dao.jselect(ibuf, "SEL_PURCHASE_BY_USERID_BINDSALE_ATTR","TF_F_USER_PURCHASE");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:�ж��Ƿ�����Ѻ�����");
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
	if (iFoundCount == 0)//δ��ѯ����Э�ۼ�����ֵ,����ʵЧʱ�����ж�
	{
		THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "δ�����Ѷ��,��������Ѻ����߰�����������ҵ��:���Ѷ��[%.2f],�Ѿ����� 0 Ԫ",iTotalFee);
	}

	if (lUsedFee >= atol(strConsumeValue.c_str())) //���Ѷ���Ѿ�����
	{
		outBuf.SetInt("X_RESULTCODE",0);
		outBuf.SetString("X_RESULTINFO","Acct Purchase check ok!");
		return;
	}
	//δ����,���Ѷ��Ҳδ��,����
	iCurrFee  = (float)lUsedFee/(float)100;

    THROW_C_P2(CRMException, CHECKAFTERTRADE_ERR, "δ�����Ѷ��,��������Ѻ����߰�����������ҵ��:���Ѷ��:%.2fԪ,�Ѿ�����:%.2fԪ",iTotalFee,iCurrFee);

    LOG_TRACE(logTradeCheckAfterTrade, "�˳� CheckPurchaseAcctReturn ����");
}


//��Ʒģ���������
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
    CFmlBuf bfUserProd;                 //�û���Ʒ
    CFmlBuf bufUserPackageByProd;       //�û���������Ʒ��
    CFmlBuf bufUserPackage;             //�û��������У�
    CFmlBuf bufTradeDiscnt_AllDiscnt;   //�û��Ż�
    CFmlBuf bufTradeSvc_UserAllSvc;     //�û�����
    CFmlBuf bufTradeSp_UserAllSp;       //�û�SP
    CFmlBuf bufTradeElement_UserAllElement; //�û���Ʒ������Ԫ��
    CFmlBuf bufUserAllElements;         //�û�Ԫ��
    CFmlBuf bufDelPackage;              //ɾ���İ�
    CFmlBuf bufNoChangePackage;         //��ǰҵ��δ�����仯�İ�
    CFmlBuf bufElementTimeSeries;       //����Ԫ��ʱ�����������ڼ������Ԫ�������С��ʱ��ʱ������������Ԫ�ؼ���Ϊһ��

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
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ��ǰʡ�������");
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
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ��ǰʡ�������");
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

    if(strTradeTypeCode == "190" || strTradeTypeCode == "191" || strTradeTypeCode == "192"||strTradeTypeCode == "241"||strTradeTypeCode == "310")//add ȡ��ԤԼ�������У�� 310
        return;

    bool bCheck = true;
    if (strTradeTypeCode == "7230" || strTradeTypeCode == "7240" || strTradeTypeCode == "7302" ||
        strTradeTypeCode == "1025" || strTradeTypeCode == "1028" || strTradeTypeCode == "1029" ||
        strTradeTypeCode == "112" || strTradeTypeCode == "70")
    {
        bCheck = false;
    }

    //У���û���Ʒ�Ƿ�ﵽ�����
    //CheckUserProductMaxSale(strTradeId,strUserId);

 
	LOG_TRACE_P1(logTradeCheckAfterTrade, "--bufTradeSvc_UserAllSvc:%s",bufTradeSvc_UserAllSvc.ToString().c_str());
    //��ȡ��ǰҵ��δ�仯�İ���Ϣ
    GetNoChangePackage(strTradeId, strUserId, bufNoChangePackage);

    //��ȡɾ���İ�(�û�ԭ�а�����ֱ��ɾ��)
    GetDelPackage(strTradeId, bufDelPackage);

    //��ȡ�û��Ż���Ϣ
    if(hbtag == "1" )
    	GetAllDiscntAfterTradehb(strTradeId, strUserId, strCustId, strAcctId, bufTradeDiscnt_AllDiscnt);
    else
    	GetAllDiscntAfterTrade(strTradeId, strUserId, strCustId, strAcctId, bufTradeDiscnt_AllDiscnt);
	LOG_TRACE_P1(logTradeCheckAfterTrade, "--bufTradeDiscnt_AllDiscnt:%s",bufTradeDiscnt_AllDiscnt.ToString().c_str());
    //��ȡ�û�������Ϣ
    if(hbtag == "1" )
    	GetUserAllServiceAfterTradehb(strTradeId, strUserId, bufTradeSvc_UserAllSvc);
    else
    	GetUserAllServiceAfterTrade(strTradeId, strUserId, bufTradeSvc_UserAllSvc);
	LOG_TRACE_P1(logTradeCheckAfterTrade, "--bufTradeSvc_UserAllSvc:%s",bufTradeSvc_UserAllSvc.ToString().c_str());
    //��ȡ�û���Ʒ������Ԫ����Ϣ
    GetUserAllElementAfterTrade(strTradeId, strUserId, bufTradeElement_UserAllElement);
LOG_TRACE_P1(logTradeCheckAfterTrade, "--bufTradeElement_UserAllElement:%s",bufTradeElement_UserAllElement.ToString().c_str());
    //��ȡ�û�SP��Ϣ
    GetUserAllSpAfterTrade(strTradeId, strUserId, bufTradeSp_UserAllSp);
	LOG_TRACE_P1(logTradeCheckAfterTrade, "--bufTradeSp_UserAllSp:%s",bufTradeSp_UserAllSp.ToString().c_str());
    //�����û�Ԫ��
    GeneUserAllElements(bufTradeDiscnt_AllDiscnt, bufTradeSvc_UserAllSvc, bufTradeSp_UserAllSp, bufTradeElement_UserAllElement, bufUserAllElements);
	LOG_TRACE_P1(logTradeCheckAfterTrade, "--bufUserAllElements:%s",bufUserAllElements.ToString().c_str());
    //��ȡ�û��������У�
    GetUserPackageByProduct(-1, bufTradeDiscnt_AllDiscnt, bufTradeSp_UserAllSp, bufTradeElement_UserAllElement, bufTradeSvc_UserAllSvc, bufUserPackage);
	LOG_TRACE_P1(logTradeCheckAfterTrade, "--bufUserPackage:%s",bufUserPackage.ToString().c_str());
    //��ȡ����Ԫ��ʱ��������Ԫ�ظ��������ڼ������Ԫ�������С��ʱ��ʱ������������Ԫ�ؼ���Ϊһ��
    GetElementTimeSeries(bufUserAllElements, bufElementTimeSeries);
	LOG_TRACE_P1(logTradeCheckAfterTrade, "--bufElementTimeSeries:%s",bufElementTimeSeries.ToString().c_str());
    //��ȡ�û���Ʒ��Ϣ
    iCount = GetUserProductInfo(strTradeId, strUserId, bfUserProd);
	LOG_TRACE_P1(logTradeCheckAfterTrade, "--bfUserProd:%s",bfUserProd.ToString().c_str());
	  //У���û�Ԫ���Ƿ�ﵽ���������
    CheckUserElement(strTradeId,strUserId,strEparchyCode,bfUserProd);
    //�ж�Ԫ�غ�������֮������ƹ�ϵ
    //CheckElmentsAndItsAttr(inBuf,bufUserAllElements);
    //���û���Ʒ�ֱ�У������
    for(int i = 0; i < iCount; i++)
    {
        iProductId = bfUserProd.GetInt("PRODUCT_ID", i);
        strProductName = bfUserProd.GetString("PRODUCT_NAME", i);
        strProductMode = bfUserProd.GetString("PRODUCT_MODE", i);
        iMinPackage = bfUserProd.GetInt("MIN_NUMBER", i);
        iMaxPackage = bfUserProd.GetInt("MAX_NUMBER", i);
        strProductEndDate = bfUserProd.GetString("END_DATE", i);
        if(strTradeTypeCode == "1060" && strProductMode=="20")  continue;
        //��ȡ�û�ĳһ��Ʒ��ҵ���
        iPackageCount = GetUserPackageByProduct(iProductId, bufTradeDiscnt_AllDiscnt, bufTradeSp_UserAllSp, bufTradeElement_UserAllElement, bufTradeSvc_UserAllSvc, bufUserPackageByProd, strProductEndDate);
	LOG_TRACE_P1(logTradeCheckAfterTrade, "--bufUserPackageByProd:%s",bufUserPackageByProd.ToString().c_str());
        //��Ʒ�ڰ���Сѡ�����ж�
        if(iMinPackage >= 0 && iPackageCount < iMinPackage)
            THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR, "��Ʒ��\"" +
                        strProductName + "\"����ѡ��" + to_string(iMinPackage) + "��ҵ�����ҵ���޷�������");

        //��Ʒ�ڰ����ѡ�����ж�
        if(iMaxPackage >= 0 && iPackageCount > iMaxPackage)
            THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR, "��Ʒ��\"" +
                        strProductName + "\"���ѡ��" + to_string(iMaxPackage) + "��ҵ�����ҵ���޷�������");

        //��Ʒ��ѡ���ж�
        if(bCheck)
            CheckForcePackageByProduct(iProductId, strEparchyCode, strTradeStaffId, bufUserPackageByProd);

        //��Ʒ�ڰ����ϵ�ж�(TD_B_PROD_PACKAGE_LIMIT)
        CheckPackageLimitByProduct(iProductId, bufUserPackageByProd, strEparchyCode);

        //����Ԫ�ر�ѡ�ж�
        if(bCheck)
            CheckPackageForceElements(iProductId, strProductName, bufUserPackageByProd,
                bufTradeDiscnt_AllDiscnt, bufTradeSvc_UserAllSvc,
                bufTradeSp_UserAllSp, bufTradeElement_UserAllElement,
                bufNoChangePackage, bufDelPackage);

        //��Ʒ����Ԫ�ص������Сѡ�����ж�(TD_B_PACKAGE)
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
        //��Ʒ����Ԫ�ص������Сѡ�����ж�(TD_B_PRODUCT_PACKAGE)
        CheckPackageElementChoiceByProduct(strEparchyCode, strTradeStaffId, bufUserPackageByProd, bufUserAllElements, bufNoChangePackage, bufDelPackage, bufElementTimeSeries, 1);

        //����Ԫ�ع�ϵ�ж�
        CheckPackageAndElementsLimit(bufUserPackageByProd, bufUserAllElements);
    }

    //Э�����ʷѡ���ϲ�Ʒ֮�����ƹ�ϵ�ж�
    if(inBuf.GetString("NET_TYPE_CODE")!="CP")
    	CheckAttrElementLimit(bufTradeDiscnt_AllDiscnt, strEparchyCode, strTradeId, strUserId, strCustId, strAcctId);

    //ȫ�ְ����ϵ�ж�(TD_B_PACKAGE_LIMIT)
     if(hbtag != "1" )
    	CheckPackageLimit(bufUserPackage, strEparchyCode);

    //����Ԫ�ع�ϵ�ж�
    CheckPackageElementLimit(bufUserAllElements, strEparchyCode, bufNoChangePackage, bufDelPackage);

    //ȫ��Ԫ�ع�ϵ�ж�
    CheckElementLimit(bufUserAllElements, strEparchyCode,strTradeId);

    //ͬһԪ����һ��ҵ����ֻ��ѡ��һ��
    if(checkSameEleTag != "1")
    	CheckSameElement(bufUserAllElements);

    //SP�����жϣ�ͬһ��SP_IDֻ��ѡ��һ��
    CheckSameSPID(bufTradeSp_UserAllSp);

    //��Ʒ������
    if (strTradeTypeCode != "70" && strTradeTypeCode != "1060")
    {
        CheckProductLimit(bfUserProd);
    }

    //��Ʒ��ԴԼ��
    //CheckProductRes(bfUserProd, strTradeId, strUserId, strEparchyCode);

    //��Ʒ�ͻ�����
    CheckCustProductLimit(inBuf);
}

void TradeCheckAfterTrade::CheckCustProductLimit(CFmlBuf &bufTrade)
{
    LOG_TRACE(logTradeCheckAfterTrade,"����CheckCustProductLimit����....");
    //ȡtag����Ƿ���ҪУ��
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
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ�Ƿ�У���Ʒ��ͻ�����ʧ�ܣ�");
    }

    CFmlBuf tempBuf;
    //У���Ƿ�ÿ�����������
    if(iCount>0)
    {
        if(bufTag.GetString("TAG_CHAR")=="1")//��ҪУ��
        {
            //��ȡҵ����������Ʒ
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
                THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "������ˮ��ȡ�����Ʒ��Ϣ�쳣��");
            }

            //��ͻ���Ϣ
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
                    THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "���ݸ��ݿͻ���ʾ��ȡ�ͻ������쳣��");
                }

                if(iCount<1)
                {
                    return;
                    //THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:�����û���ʾ[%s]δ��ȡ���ͻ�����!", bufTrade.GetStr("CUST_ID"));
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
                else continue;//����������Ʒ�����´�ѭ��
                //��Ʒ�ͻ����ƹ���
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
                    THROW_AGC(CRMException, ex, Grade::WARNING, GETCOMMPARA_ERR, "��ȡ��Ʒ��ͻ���������쳣��");
                }
                LOG_TRACE_P1(logTradeCheckAfterTrade,"iCount====%d",iCount);
                if(iCount > 0)//û������Ĭ�ϲ�У��
                {
                    //0:����, 1:�ҿ�, 2:����
                    string strUserTypeCode = bufParam.GetString("PARA_CODE4");
                    string strAllTag = bufParam.GetString("PARA_CODE5");
                    if(strAllTag == "1")//PARA_CODE5 Ϊ1��ʾ����PARA_CODE4�ж�,Ϊ0��ʾ�ҿͼ��Ͷ��ɰ���
                    {
                        if(bufCust.GetString("CUST_TYPE")=="1"&&strUserTypeCode!="2")
                        {
                            THROW_C(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:�ͻ�Ϊ���ſͻ�,���ܰ�����˿ͻ���Ʒ!");
                        }
                        if(bufCust.GetString("CUST_TYPE")=="0"&&strUserTypeCode!="0"&&strUserTypeCode!="1")
                        {
                            THROW_C(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:�ͻ�Ϊ���˿ͻ�,���ܰ����ſͻ���Ʒ!");
                        }
                    }
                }
            }
        }
    }
    LOG_TRACE(logTradeCheckAfterTrade,"�˳�CheckCustProductLimit����....");
}

//��ȡ�û���Ʒ��Ϣ��ҵ�������
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
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ�û���Ʒ��Ϣ����");
    }

    ConvertCodeToName(bfUserProd, "ProductName", "PRODUCT_ID", "PRODUCT_NAME");
    bfUserProd.SetInt("X_RECORDNUM", iCount);

    return iCount;
}

/**
 *У���û���Ʒ�Ƿ�ﵽ�����
 *
 */
int TradeCheckAfterTrade::CheckUserProductMaxSale(const string &strTradeId, const string &strUserId)
{
    StaticDAO& dao = StaticDAO::getDAO();
    //1.��ȡ̨�˲�Ʒ�ӱ���Ϣ
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ѯҵ��̨�˲�Ʒ�ӱ����[%s]",ex.getCause()->what());
    }

    CFmlBuf tmpBuff;
    int iCount = 0;
    for(int i = 0;i<iPdtCnt;i++)
    {
        iCount = 0;
        if(bufTradeProduct.GetString("MODIFY_TAG")!="0")
        	continue;

        //У���Ƿ�ﵽ���������
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
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ����ҵ��������������Żݳ���[%s]",ex.getCause()->what());
    	}

        if((int)tmpBuff.GetInt("RECORDCOUNT") > 0 )
    	{
            char chIdA[10];
            sprintf(chIdA,"%d",(int)bufTradeProduct.GetInt("PRODUCT_ID"));
            string strNameA = CParamDAO::getParam("ProductName", chIdA);
            THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:[%s]�ò�Ʒ�Ѿ��ﵽ���������,�����Զ���!", strNameA.c_str());
    	}

        //У���Ƿ��������Ʒ�����������
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
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ����ҵ��������������Żݳ���[%s]",ex.getCause()->what());
    	}

        if((int)tmpBuff.GetInt("RECORDCOUNT") > 0 )
    	{
            char chIdA[10];
            sprintf(chIdA,"%d",(int)bufTradeProduct.GetInt("PRODUCT_ID"));
            string strNameA = CParamDAO::getParam("ProductName", chIdA);
            THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:[%s]�������Ʒ�Ѿ��ﵽ��������������߶����ڲ���������,�����Զ���!", strNameA.c_str());
    	}
    }

    return iCount;
}

/**
 *У���û�Ԫ���Ƿ�ﵽ�����
 *0������
 *1���Ż�
 *2��sp
 */
void TradeCheckAfterTrade::CheckUserElement(const string &strTradeId, const string &strUserId, const string &strEparchyCode,CFmlBuf &bfUserProd)
{
    StaticDAO& dao = StaticDAO::getDAO();
    //�����û������������tag��������ڣ�����ʾ�������������
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
            strGDTag = "1";   //����һ�� modify hhx
        }
        else
            strGDTag = bufGDTag.GetString("TAG_CHAR");
    	intGDTag = atoi(strGDTag.c_str());
     }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GETTRADEINFO_ERR, "����̨������:��ȡtag����");
    }
    //0.��ȡ̨�˷����ӱ���Ϣ
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ѯҵ��̨�˷����ӱ����[%s]",ex.getCause()->what());
    }

    CFmlBuf tmpBuff;
    int iCount = 0;
    for(int i = 0;i<iSvcCnt;i++)
    {

        //У���Ƿ���������������������
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
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ����ҵ������������з������[%s]",ex.getCause()->what());
    	}

        if((int)tmpBuff.GetInt("RECORDCOUNT") >=  intGDTag  )
    	{
            char chIdA[10];
            sprintf(chIdA,"%d",(int)bufTradeSvc.GetInt("SERVICE_ID",i));
            string strNameA = CParamDAO::getParam("ServiceName", chIdA);
            THROW_C_P2(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:[%s]���������ֻ������[%d]��!", strNameA.c_str(),intGDTag);
    	}
    }
    
   
    //1.��ȡ̨���Ż��ӱ���Ϣ
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ѯҵ��̨���Ż��ӱ����[%s]",ex.getCause()->what());
    }
    LOG_TRACE_P1(logTradeCheckAfterTrade, "����ChkRelaUserProductLimit����--[%d]��",iDisCnt);

    iCount = 0;
    for(int i = 0;i<iDisCnt;i++)
    {

        //У���Ƿ��������Ż������������
        iCount = 0;
        try
    	{
            LOG_TRACE_P2(logTradeCheckAfterTrade, "����ChkRelaUserProductLimit����--[%s,%d]��",strUserId.c_str(),(int)bufTradeDis.GetInt("DISCNT_CODE",i));
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
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ����ҵ��������������Żݳ���[%s]",ex.getCause()->what());
    	}
    LOG_TRACE_P1(logTradeCheckAfterTrade, "����ChkRelaUserProductLimit����--[%d]��",(int)tmpBuff.GetInt("RECORDCOUNT"));
        if((int)tmpBuff.GetInt("RECORDCOUNT")>=  intGDTag )
    	{
            char chIdA[10];
            sprintf(chIdA,"%d",(int)bufTradeDis.GetInt("DISCNT_CODE",i));
            string strNameA = CParamDAO::getParam("DiscntName", chIdA);
            THROW_C_P2(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:[%s]�������Ż�ֻ������[%d]��!", strNameA.c_str(),intGDTag);
    	}
    }

    //2.��ȡ̨��sp�ӱ���Ϣ
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ѯҵ��̨��sp�ӱ����[%s]",ex.getCause()->what());
    }


    iCount = 0;
    for(int i = 0;i<iSpCnt;i++)
    {

        if(bufTradeSp.GetString("MODIFY_TAG")!="0")
        	continue;
        //У���Ƿ�������sp�����������
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
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ����ҵ�������������sp����[%s]",ex.getCause()->what());
    	}

        if((int)tmpBuff.GetInt("RECORDCOUNT") >=  intGDTag )
    	{
            string strNameA = CParamDAO::getParam("SpProductName", bufTradeSp.GetString("SP_PRODUCT_ID", i));
            THROW_C_P2(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:[%s]������spֻ������[%d]��!", strNameA.c_str(), intGDTag);
    	}
    }
    
    
    //У��Ԫ���Ƿ񳬳���Ʒ��Χ
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
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ѯҵ��̨�˷����ӱ����[%s]",ex.getCause()->what());
        }
        if(iSvcCntTemp> 0)
        {
            for(int i = 0;i<iSvcCntTemp;i++)
            {
                bufTradeSvc.CoverLine(iSvcCnt+i,bufTradeSvcTemp,i);
            }
        }
        //����
        for(int i = 0;i<iSvcCnt+iSvcCntTemp;i++)
        { 
             for(int j = 0;j<ProdCnt;j++)
             {
             
                 if (bufTradeSvc.GetString("PRODUCT_ID",i) == bfUserProd.GetString("PRODUCT_ID",j) 
                    &&(bufTradeSvc.GetString("START_DATE",i).substr(0,10)< bfUserProd.GetString("START_DATE",j).substr(0,10)) )
                 {
                     string serviceName = CParamDAO::getParam("ServiceName", bufTradeSvc.GetString("SERVICE_ID",i));
                     THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:[%s]�÷��񳬳���Ʒ��Χ��������񶩹�ʱ��!", serviceName.c_str());
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
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ѯҵ��̨���Ż��ӱ����[%s]",ex.getCause()->what());
        }
        
        if(iDisCntTemp> 0)
        {
            for(int i = 0;i<iDisCntTemp;i++)
            {
                bufTradeDis.CoverLine(iDisCnt+i,bufTradeDisTemp,i);
            }
        }
        
        
        //�ʷ�
        for(int i = 0;i<iDisCnt+iDisCntTemp;i++)
        {
             for(int j = 0;j<ProdCnt;j++)
             {
                 if (bufTradeDis.GetString("PRODUCT_ID",i) == bfUserProd.GetString("PRODUCT_ID",j) 
                    &&(bufTradeDis.GetString("START_DATE",i).substr(0,10)< bfUserProd.GetString("START_DATE",j).substr(0,10)) )
                 {
                     string discntName = CParamDAO::getParam("DiscntName", bufTradeDis.GetString("DISCNT_CODE",i));
                     THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:[%s]���Żݳ�����Ʒ��Χ�������Żݶ���ʱ��!", discntName.c_str());
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
                     THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:[%s]��sp������Ʒ��Χ������sp����ʱ��!", spName.c_str());
                 }
            } 
        }
    }
}

/**
 *��ȡ����ҵ�����û������Żݺ��ŻݵĿ�ʼʱ�䡢����ʱ�䡢���޸�״̬������ԭ���Żݵ��޸�״̬Ĭ��Ϊ'A')
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ����ҵ��������������Żݳ���[%s]",ex.getCause()->what());
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
 *��ȡ����ҵ����û������з���
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ����ҵ����û������з������[%s]",ex.getCause()->what());
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
 *��ȡ����ҵ�����û������Żݺ��ŻݵĿ�ʼʱ�䡢����ʱ�䡢���޸�״̬������ԭ���Żݵ��޸�״̬Ĭ��Ϊ'A')
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ����ҵ��������������Żݳ���[%s]",ex.getCause()->what());
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
 *��ȡ����ҵ����û������з���
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ����ҵ����û������з������[%s]",ex.getCause()->what());
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
 *��ȡ����ҵ����û�������SP
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ����ҵ����û�������SP����[%s]",ex.getCause()->what());
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
 *��ȡ����ҵ����û���������Ʒ������Ԫ��
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ����ҵ����û�����������Ԫ�س���[%s]",ex.getCause()->what());
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


//����ȡ�û���ҵ��������ҵ�����Ϣ
//iProductId:-1ʱȡ���в�Ʒ�����û����а���Ϣ
int TradeCheckAfterTrade::GetUserPackageByProduct(const int &iProductId, CFmlBuf &bufTradeDiscnt_AllDiscnt,
                                                  CFmlBuf &bufTradeSp_UserAllSp, CFmlBuf &bufTradeElement_UserAllElement,
                                                  CFmlBuf &bufTradeSvc_UserAllSvc, CFmlBuf &bufUserPackageByProd, const string &strProdEndDate)
{
	LOG_TRACE(logTradeCheckAfterTrade, "���� GetUserPackageByProduct ����");
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
        strProductEndDate="2015-12-31"; //����Ϊ�����. �е�ʡʱ2019(ɽ��)��������2015Ϊ�����
	//modfiy by lirui 20120220 �µ׽����İ�����Ϊ�û����а�
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

                if(bufUserPackageByProd.GetString("END_DATE", j) < bufTradeDiscnt_AllDiscnt.GetString("END_DATE", i)&&strProductEndDate<=bufTradeDiscnt_AllDiscnt.GetString("END_DATE", i))//���жϽ������µ׵��ʷ�  modify by tz@2009-5-27 09:47����
                    bufUserPackageByProd.SetString("END_DATE", bufTradeDiscnt_AllDiscnt.GetString("END_DATE", i), j);

                break;
            }
        }
        //&&strProductEndDate<=bufTradeDiscnt_AllDiscnt.GetString("END_DATE", i)
        //if(!bFound&&bufTradeDiscnt_AllDiscnt.GetString("END_DATE", i)>strLastDate)  //���жϽ������µ׵��ʷ�  modify by tz@2009-5-27 09:47����
        //liuhj 2012-03-22 QC_30587 �µ�ʧЧ�Ĳ�Ʒ���Żݣ����Ӧ�İ�ҲӦ�üӽ�ȥ������һ��||��ͬʱ�޸�SQL:SEL_ALLPRODUCTS_AFTER_TRADE
        //���ָĳ�strLastDate���쵼Ҫ����QC�������ѸĻ�ȥ�ɣ�˭����������˭�ٰ�strCurrentDate�ĳ� strLastDate
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


//��ȡ��Ʒ���ɰ���Ϣ
//xTag:0-����Ԫ�������СֵȡTD_B_PACKAGE  1-����Ԫ�������СֵȡTD_B_PRODUCT_PACKAGE
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ��Ʒ���ɰ���Ϣ����[%s]",ex.getCause()->what());
    }

    ConvertCodeToName(bufPackageByProd, "ProductName", "PRODUCT_ID", "PRODUCT_NAME");

    bufPackageByProd.SetInt("X_RECORDNUM", iCount);
    return iCount;
}


//��Ʒ��ѡ���ж�
void TradeCheckAfterTrade::CheckForcePackageByProduct(const int &iProductId, const string &strEparchyCode, const string &strTradeStaffId,
                                                      CFmlBuf &bufUserPackageByProd)
{
    //��ȡ��Ʒ���ɰ���Ϣ
    bool bFound = false;
    CFmlBuf bufPackageByProd;
    int iCount = GetPackageByProduct(iProductId, strEparchyCode, strTradeStaffId, bufPackageByProd);
    int iPackageId = 0;
    string strErrorInfo = "";

    for(int i = 0; i < iCount; i++)
    {
        if(bufPackageByProd.GetString("FORCE_TAG", i) != "1")
            continue;

        //��ѡ����ʶ
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
                strErrorInfo += "��"+bufPackageByProd.GetString("PACKAGE_NAME", i);
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR, "��Ʒ��\"" +
                    bufPackageByProd.GetString("PRODUCT_NAME") + "\"����ѡҵ�����\"" +
                    strErrorInfo + "\"û��ѡ��ҵ���޷�������");
}


//��Ʒ����Ԫ�ص������Сѡ�����ж�
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

    //��ȡ��Ʒ���ɰ���Ϣ
    CFmlBuf bufPackageByProd;
    int iPackageCount = GetPackageByProduct(iProductId, strEparchyCode, strTradeStaffId, bufPackageByProd, xTag);

    int iMaxElement = 0;
    int iMinElement = 0;
    int iUserElement = 0;
    string strErrorInfo = "";

    int iUserPackageCount = bufUserPackageByProd.GetInt("X_RECORDNUM");
    for(int i = 0; i < iUserPackageCount; i++)
    {
        //��ǰҵ���ޱ仯�İ������ж�
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

        //��ǰҵ����ȫɾ���İ������ж�
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

        //��Ʒ����Ԫ����С�����ѡ�����ж�
        if(iMinElement >= 0 || iMaxElement >= 0)
        {
            //��ȡ��Ʒ�����û�Ԫ��ѡ����
            for(int z = 0; z < iUserAllElementCount; z++)
            {
                if(bufUserAllElements.GetInt("PRODUCT_ID", z) == iProductId
                    && bufUserAllElements.GetInt("PACKAGE_ID", z) == bufUserPackageByProd.GetInt("PACKAGE_ID", i))
                {
                    iUserElement++;
                }
            }

            //��ȥ��������ʱ��Ԫ�ظ���
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

            //��Ʒ����Ԫ����Сѡ�����ж�
            if(iMinElement >= 0 && iUserElement < iMinElement)
            {
                if(strErrorInfo == "")
                {
                    strErrorInfo = "ҵ�����\""
                                 + bufUserPackageByProd.GetString("PACKAGE_NAME", i)
                                 + "\"����ѡ��" + to_string(iMinElement) + "��Ԫ��";
                }
                else
                {
                    strErrorInfo += "��ҵ�����\""
                                 + bufUserPackageByProd.GetString("PACKAGE_NAME", i)
                                 + "\"����ѡ��" + to_string(iMinElement) + "��Ԫ��";
                }
            }

            //��Ʒ����Ԫ�����ѡ�����ж�
            if(iMaxElement >= 0 && iUserElement > iMaxElement)
            {
                if(strErrorInfo == "")
                {
                    strErrorInfo = "ҵ�����\""
                                 + bufUserPackageByProd.GetString("PACKAGE_NAME", i)
                                 + "\"���ѡ��" + to_string(iMaxElement) + "��Ԫ��";
                }
                else
                {
                    strErrorInfo += "��ҵ�����\""
                                 + bufUserPackageByProd.GetString("PACKAGE_NAME", i)
                                 + "\"���ѡ��" + to_string(iMaxElement) + "��Ԫ��";
                }
            }
        }
    }

    if(strErrorInfo != "")
    {
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR, "��Ʒ��\"" +
                    strProductName + "\"" + strErrorInfo + "��ҵ���޷�������");
    }
}


//��ȡ��Ʒ�ڰ����ϵ
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ�����ϵ��Ϣ����[%s]",ex.getCause()->what());
    }

    /*ConvertCodeToName(bufPackageLimitByProd, "ProductName", "PRODUCT_ID", "PRODUCT_NAME");
    ConvertCodeToName(bufPackageLimitByProd, "PackageName", "PACKAGE_ID_A", "PACKAGE_NAME_A");
    ConvertCodeToName(bufPackageLimitByProd, "PackageName", "PACKAGE_ID_B", "PACKAGE_NAME_B");*/

    bufPackageLimitByProd.SetInt("X_RECORDNUM", iCount);
    return iCount;
}


//��Ʒ�ڰ����ϵ�ж�
void TradeCheckAfterTrade::CheckPackageLimitByProduct(const int &iProductId,CFmlBuf &bufUserPackageByProd,
                                                      const string strEparchyCode)
{
    int iUserPackageCount = bufUserPackageByProd.GetInt("X_RECORDNUM");
    CFmlBuf bufPackageLimitByProd;

    int iCount = 0;
    bool bFound = false;

    string strErrorInfo = "";
    //���û���ѭ��У����以���ϵ
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
                if (i==z)continue; //�����������ж� added by tangz@2009-2-7 11:33
                if(bufUserPackageByProd.GetInt("PACKAGE_ID", z)
                    == bufPackageLimitByProd.GetInt("PACKAGE_ID_B", j))
                {
                    bFound = true;
                    break;
                }
            }

            if(bFound)
            {
                //�û���(i)�����ư�(j)����
                if(strErrorInfo == "")
                    strErrorInfo = "ҵ�����\"" + bufUserPackageByProd.GetString("PACKAGE_NAME", i)
                        + "\"��ҵ�����\"" +  bufPackageLimitByProd.GetString("PACKAGE_NAME_B", j) + "\"";
                else
                    strErrorInfo += "��ҵ�����\"" + bufUserPackageByProd.GetString("PACKAGE_NAME", i)
                        + "\"��ҵ�����\"" +  bufPackageLimitByProd.GetString("PACKAGE_NAME_B", j) + "\"";
            }
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR, "��Ʒ��\"" +
                    bufUserPackageByProd.GetString("PRODUCT_NAME") + "\"��" +
                    strErrorInfo + "���⣬ҵ���޷�������");


    //���û���ѭ��У����䲿��������ϵ
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
            //�û���(i)�������������ư�(j)
            //��������ҵ���ƴ��
            string strInfo = "";
            for(int y = 0; y < iCount; y++)
            {
                if(strInfo == "")
                    strInfo = bufPackageLimitByProd.GetString("PACKAGE_NAME_B", y);
                else
                    strInfo += "��" + bufPackageLimitByProd.GetString("PACKAGE_NAME_B", y);
            }

            if(strErrorInfo == "")
                strErrorInfo = "ҵ�����\"" + bufUserPackageByProd.GetString("PACKAGE_NAME", i)
                    + "\"����������ҵ�����\"" +  strInfo + "\"";
            else
                strErrorInfo += "��ҵ�����\"" + bufUserPackageByProd.GetString("PACKAGE_NAME", i)
                    + "\"����������ҵ�����\"" +  strInfo + "\"";
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR, "��Ʒ��\"" +
                    bufUserPackageByProd.GetString("PRODUCT_NAME") + "\"��" +
                    strErrorInfo + "��ҵ���޷�������");


    //���û���ѭ��У�������ȫ������ϵ
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
                //�û���(i)��ȫ���������ư�(j)
                //��ȫ����ҵ���ƴ��
                string strInfo = "";
                for(int y = 0; y < iCount; y++)
                {
                    if(strInfo == "")
                        strInfo = bufPackageLimitByProd.GetString("PACKAGE_NAME_B", y);
                    else
                        strInfo += "��" + bufPackageLimitByProd.GetString("PACKAGE_NAME_B", y);
                }

                if(strErrorInfo == "")
                    strErrorInfo = "ҵ�����\"" + bufUserPackageByProd.GetString("PACKAGE_NAME", i)
                        + "\"��ȫ������ҵ�����\"" +  strInfo + "\"";
                else
                    strErrorInfo += "��ҵ�����\"" + bufUserPackageByProd.GetString("PACKAGE_NAME", i)
                        + "\"��ȫ������ҵ�����\"" +  strInfo + "\"";

                break;
            }
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR, "��Ʒ��\"" +
                    bufUserPackageByProd.GetString("PRODUCT_NAME") + "\"��" +
                    strErrorInfo + "��ҵ���޷�������");
}


//��ȡȫ�ְ����ϵ
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ�����ϵ��Ϣ����[%s]",ex.getCause()->what());
    }

    //ConvertCodeToName(bufPackageLimit, "PackageName", "PACKAGE_ID_A", "PACKAGE_NAME_A");
    //ConvertCodeToName(bufPackageLimit, "PackageName", "PACKAGE_ID_B", "PACKAGE_NAME_B");

    bufPackageLimit.SetInt("X_RECORDNUM", iCount);
    return iCount;
}


//ȫ�ְ����ϵ�ж�
void TradeCheckAfterTrade::CheckPackageLimit(CFmlBuf &bufUserPackage, const string strEparchyCode)
{
    int iUserPackageCount = bufUserPackage.GetInt("X_RECORDNUM");
    CFmlBuf bufPackageLimit;

    int iCount = 0;
    bool bFound = false;

    string strErrorInfo = "";
    //���û���ѭ��У����以���ϵ
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
                if (i==z)continue; //�����������ж� added by tangz@2009-2-7 11:33
                if(bufUserPackage.GetInt("PACKAGE_ID", z) == bufPackageLimit.GetInt("PACKAGE_ID_B", j)

                    //ʱ�佻��
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
                //�û���(i)�����ư�(j)����
                if(strErrorInfo == "")
                    strErrorInfo = "ҵ�����\"" + bufUserPackage.GetString("PACKAGE_NAME", i)
                        + "\"��ҵ�����\"" +  bufPackageLimit.GetString("PACKAGE_NAME_B", j) + "\"";
                else
                    strErrorInfo += "��ҵ�����\"" + bufUserPackage.GetString("PACKAGE_NAME", i)
                        + "\"��ҵ�����\"" +  bufPackageLimit.GetString("PACKAGE_NAME_B", j) + "\"";
            }
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR, strErrorInfo + "���⣬ҵ���޷�������");


    //���û���ѭ��У����䲿��������ϵ
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
                    //ʱ�佻��
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
            //�û���(i)�������������ư�(j)
            //��������ҵ���ƴ��
            string strInfo = "";
            for(int y = 0; y < iCount; y++)
            {
                if(strInfo == "")
                    strInfo = bufPackageLimit.GetString("PACKAGE_NAME_B", y);
                else
                    strInfo += "��" + bufPackageLimit.GetString("PACKAGE_NAME_B", y);
            }

            if(strErrorInfo == "")
                strErrorInfo = "ҵ�����\"" + bufUserPackage.GetString("PACKAGE_NAME", i)
                    + "\"����������ҵ�����\"" +  strInfo + "\"";
            else
                strErrorInfo += "��ҵ�����\"" + bufUserPackage.GetString("PACKAGE_NAME", i)
                    + "\"����������ҵ�����\"" +  strInfo + "\"";
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR, strErrorInfo + "��ҵ���޷�������");


    //���û���ѭ��У�������ȫ������ϵ
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
                    //ʱ�佻��
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
                //�û���(i)��ȫ���������ư�(j)
                //��ȫ����ҵ���ƴ��
                string strInfo = "";
                for(int y = 0; y < iCount; y++)
                {
                    if(strInfo == "")
                        strInfo = bufPackageLimit.GetString("PACKAGE_NAME_B", y);
                    else
                        strInfo += "��" + bufPackageLimit.GetString("PACKAGE_NAME_B", y);
                }

                if(strErrorInfo == "")
                    strErrorInfo = "ҵ�����\"" + bufUserPackage.GetString("PACKAGE_NAME", i)
                        + "\"��ȫ������ҵ�����\"" +  strInfo + "\"";
                else
                    strErrorInfo += "��ҵ�����\"" + bufUserPackage.GetString("PACKAGE_NAME", i)
                        + "\"��ȫ������ҵ�����\"" +  strInfo + "\"";

                break;
            }
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR, strErrorInfo + "��ҵ���޷�������");
}


//��ȡ���ڱ�ѡԪ��
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ���ڱ�ѡԪ����Ϣ����[%s]",ex.getCause()->what());
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


//���ڱ�ѡԪ���ж�
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
        //��ǰҵ���ޱ仯�İ������ж�
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

        //��ǰҵ����ȫɾ���İ������ж�
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
            //�Ż�
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
                        strErrorInfo += "��\"" + bufPackageForceElements.GetString("PACKAGE_NAME", z)
                            + " --> " + bufPackageForceElements.GetString("DISCNT_NAME", z) + "\"";
                    /*
                    if(strErrorInfo == "")
                        strErrorInfo = "ҵ�����\"" + bufPackageForceElements.GetString("PACKAGE_NAME", z)
                            + "\"�ı�ѡ�Żݣ�\"" + bufPackageForceElements.GetString("DISCNT_NAME", z) + "\"";
                    else
                        strErrorInfo += "��ҵ�����\"" + bufPackageForceElements.GetString("PACKAGE_NAME", z)
                            + "\"�ı�ѡ�Żݣ�\"" + bufPackageForceElements.GetString("DISCNT_NAME", z) + "\"";
                    */
                }

            }

            //����
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
                        strErrorInfo += "��\"" + bufPackageForceElements.GetString("PACKAGE_NAME", z)
                            + " --> " + bufPackageForceElements.GetString("SERVICE_NAME", z) + "\"";
                    /*
                    if(strErrorInfo == "")
                        strErrorInfo = "ҵ�����\"" + bufPackageForceElements.GetString("PACKAGE_NAME", z)
                            + "\"�ı�ѡ����\"" + bufPackageForceElements.GetString("SERVICE_NAME", z) + "\"";
                    else
                        strErrorInfo += "��ҵ�����\"" + bufPackageForceElements.GetString("PACKAGE_NAME", z)
                            + "\"�ı�ѡ����\"" + bufPackageForceElements.GetString("SERVICE_NAME", z) + "\"";
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
                        strErrorInfo += "��\"" + bufPackageForceElements.GetString("PACKAGE_NAME", z)
                            + " --> " + bufPackageForceElements.GetString("SP_PRODUCT_NAME", z) + "\"";
                }
            }

            //��Ʒ������Ԫ��
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
                        strErrorInfo += "��\"" + bufPackageForceElements.GetString("PACKAGE_NAME", z)
                            + " --> " + bufPackageForceElements.GetString("ACTION_NAME", z) + "\"";
                }
            }
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR, "��Ʒ��\"" +
                    strProductName + "\"��" + strErrorInfo + "����ѡ��ҵ���޷�������");


}


//�û�Ԫ������
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
        //�����û��ж϶�������
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
        //�����û��ж϶�������
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
        //�����û��ж϶�������
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
        //�����û��ж϶�������
        bufUserAllElements.SetString("TRADE_ID", bufTradeElement_UserAllElement.GetString("TRADE_ID", j), j+iBase);
        bufUserAllElements.SetString("MODIFY_TAG", bufTradeElement_UserAllElement.GetString("MODIFY_TAG", j), j+iBase);
    }

    bufUserAllElements.SetInt("X_RECORDNUM", iDiscntCount+iSvcCount+iSPCount+iElementCount);

    return iDiscntCount+iSvcCount+iSPCount+iElementCount;
}


//��ȡȫ��Ԫ��֮���ϵ
int TradeCheckAfterTrade::ExistsElementLimitA(const string &strElementTypeA, const int &iElementIdA,
                                              const string &strLimitTag, const string strEparchyCode,
                                              CFmlBuf &bufElementLimit)
{
    bufElementLimit.ClearFmlValue();
    StaticDAO& dao = StaticDAO::getDAO();
    int iCount = 0;
    LOG_TRACE(logTradeCheckAfterTrade, "����ExistsElementLimitA����");
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ����Ԫ�ؼ��ϵ��Ϣ����[%s]",ex.getCause()->what());
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
 *��ȡҵ�����ȡ��Ԫ��
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡҵ�����ȡ��Ԫ�س���[%s]",ex.getCause()->what());
    }

    /*if(iCount > 0)
    {
        ConvertCodeToName(bufTradeDelElement, "ProductName", "PRODUCT_ID", "PRODUCT_NAME");
        ConvertCodeToName(bufTradeDelElement, "PackageName", "PACKAGE_ID", "PACKAGE_NAME");
    }*/

    bufTradeDelElement.SetInt("X_RECORDNUM", iCount);

    return iCount;
}

//ȫ��Ԫ��֮�����ƹ�ϵ�ж�
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

    //���û�Ԫ��ѭ��У��Ԫ�ػ����ϵ
    int iUserElements = bufUserAllElements.GetInt("X_RECORDNUM");
    for(i = 0; i < iUserElements; i++)
    {
        iCount = 0;
        bufElementLimit.ClearFmlValue();
        
        /*
		    add by lirui@20090505	
		    �µ���ֹ���Ż��ж�����(��ֹ���µ׵��Żݣ������뻥�⣬������������ȫ�������ظ�Ԫ�ص��жϣ�����ֹ���µ׵��Ż�
		    ���ж�ʱ������Ϊ���ڣ�Ҳ������Ϊ�ǲ����ڵ�Ԫ��)
		    */
		    if( bufUserAllElements.GetString("END_DATE",i)<=strLastDate&&bufUserAllElements.GetString("ELEMENT_TYPE_CODE", i)=="D")
			      continue;        

        //��ȡԪ�ػ�����Ϣ
        iCount = ExistsElementLimitA(bufUserAllElements.GetString("ELEMENT_TYPE_CODE", i),
                                     bufUserAllElements.GetInt("ELEMENT_ID", i),
                                     "0", strEparchyCode, bufElementLimit);

        for(j = 0; j < iCount; j++)
        {
            bFound = false;
            for(z = 0; z < iUserElements; z++)
            {
                if (i==z)continue; //�����������ж� added by tangz@2009-2-7 11:33
                if(bufUserAllElements.GetInt("ELEMENT_ID", z) == bufElementLimit.GetInt("ELEMENT_ID_B", j)
                    && bufUserAllElements.GetString("ELEMENT_TYPE_CODE", z) == bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", j)
                    //ʱ�佻��
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
                        + "\"��\"" +  bufUserAllElements.GetString("PRODUCT_NAME", z)
                        + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", z)
                        + " --> " +  bufUserAllElements.GetString("ELEMENT_NAME", z) + "\"";
                else
                    strErrorInfo += "��\"" + bufUserAllElements.GetString("PRODUCT_NAME", i)
                        + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", i)
                        + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                        + "\"��\"" +  bufUserAllElements.GetString("PRODUCT_NAME", z)
                        + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", z)
                        + " --> " +  bufUserAllElements.GetString("ELEMENT_NAME", z) + "\"";
            }
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strErrorInfo + "���⣬ҵ���޷�������");

    //���û�Ԫ��ѭ��У��Ԫ�ز���������ϵ
    strErrorInfo = "";
    for(i = 0; i < iUserElements; i++)
    {
        iCount = 0;
        bufElementLimit.ClearFmlValue();

        /*
		    add by lirui@20090505	
		    �µ���ֹ���Ż��ж�����(��ֹ���µ׵��Żݣ������뻥�⣬������������ȫ�������ظ�Ԫ�ص��жϣ�����ֹ���µ׵��Ż�
		    ���ж�ʱ������Ϊ���ڣ�Ҳ������Ϊ�ǲ����ڵ�Ԫ��)
		    */
		    if( bufUserAllElements.GetString("END_DATE",i)<=strLastDate&&bufUserAllElements.GetString("ELEMENT_TYPE_CODE", i)=="D")
			      continue;

        //��ȡԪ�ز���������Ϣ
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
                    //ʱ�佻��
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
            //(i)����������(j)
            //��������Ԫ��ƴ��
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
                        strInfo += "��" + bufElementLimit.GetString("DISCNT_NAME", y);
                    else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "S")
                        strInfo += "��" + bufElementLimit.GetString("SERVICE_NAME", y);
                    else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "X")
                        strInfo += "��" + bufElementLimit.GetString("SP_PRODUCT_NAME", y);
                    else
                        strInfo += "��" + bufElementLimit.GetString("ACTION_NAME", y);
                }
            }

            if(strErrorInfo == "")
                strErrorInfo = "\"" + bufUserAllElements.GetString("PRODUCT_NAME", i)
                    + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", i)
                    + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                    + "\"����������Ԫ�أ�\"" +  strInfo + "\"";
            else
                strErrorInfo += "��\"" + bufUserAllElements.GetString("PRODUCT_NAME", i)
                    + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", i)
                    + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                    + "\"����������Ԫ�أ�\"" +  strInfo + "\"";
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strErrorInfo + "��ҵ���޷�������");


    //���û�Ԫ��ѭ��У�����Ԫ����ȫ������ϵ
    strErrorInfo = "";
    for(i = 0; i < iUserElements; i++)
    {
        iCount = 0;
        bufElementLimit.ClearFmlValue();

        /*
		    add by lirui@20090505	
		    �µ���ֹ���Ż��ж�����(��ֹ���µ׵��Żݣ������뻥�⣬������������ȫ�������ظ�Ԫ�ص��жϣ�����ֹ���µ׵��Ż�
		    ���ж�ʱ������Ϊ���ڣ�Ҳ������Ϊ�ǲ����ڵ�Ԫ��)
		    */
		    if( bufUserAllElements.GetString("END_DATE",i)<=strLastDate&&bufUserAllElements.GetString("ELEMENT_TYPE_CODE", i)=="D")
			      continue;

        //��ȡԪ����ȫ������Ϣ
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
                    //ʱ�佻��
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
                //(i)��ȫ������(j)
                //��ȫ����Ԫ��ƴ��
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
                            strInfo += "��" + bufElementLimit.GetString("DISCNT_NAME", y);
                        else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "S")
                            strInfo += "��" + bufElementLimit.GetString("SERVICE_NAME", y);
                        else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "X")
                            strInfo += "��" + bufElementLimit.GetString("SP_PRODUCT_NAME", y);
                        else
                            strInfo += "��" + bufElementLimit.GetString("ACTION_NAME", y);
                    }
                }

                if(strErrorInfo == "")
                    strErrorInfo = "\"" + bufUserAllElements.GetString("PRODUCT_NAME", i)
                        + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", i)
                        + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                        + "\"��ȫ������Ԫ�أ�\"" +  strInfo + "\"";
                else
                    strErrorInfo += "��\"" + bufUserAllElements.GetString("PRODUCT_NAME", i)
                        + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", i)
                        + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                        + "\"��ȫ������Ԫ�أ�\"" +  strInfo + "\"";

                break;
            }
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strErrorInfo + "��ҵ���޷�������");

    //���û�Ԫ��ѭ��У�����Ԫ�ض���������ϵ
    strErrorInfo = "";
    for(i = 0; i < iUserElements&&bufUserAllElements.GetString("TRADE_ID", i)!="0"
         &&(bufUserAllElements.GetString("MODIFY_TAG", i)=="A"||bufUserAllElements.GetString("MODIFY_TAG", i)=="0"); i++)//ֻ��鱾�ζ�����Ԫ��
    {
        iCount = 0;
        bufElementLimit.ClearFmlValue();

        //��ȡԪ�ض���������Ϣ
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
                    //ʱ�佻��
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
                //(j)������(i)
                //����Ԫ��ƴ��
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
                            strInfo += "��" + bufElementLimit.GetString("DISCNT_NAME", y);
                        else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "S")
                            strInfo += "��" + bufElementLimit.GetString("SERVICE_NAME", y);
                        else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "X")
                            strInfo += "��" + bufElementLimit.GetString("SP_PRODUCT_NAME", y);
                        else
                            strInfo += "��" + bufElementLimit.GetString("ACTION_NAME", y);
                    }
                }

                if(strErrorInfo == "")
                    strErrorInfo = "\"" + bufUserAllElements.GetString("PRODUCT_NAME", i)
                        + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", i)
                        + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                        + "\"������Ԫ�أ�\"" +  strInfo + "\"";
                else
                    strErrorInfo += "��\"" + bufUserAllElements.GetString("PRODUCT_NAME", i)
                        + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", i)
                        + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                        + "\"������Ԫ�أ�\"" +  strInfo + "\"";

                break;
            }
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strErrorInfo + "��ҵ���޷�����3��");

    //���û�Ԫ��ѭ��У�����Ԫ��ȡ������������ϵ
    //�в�Ʒ������жϴ�����
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

            //��ȡԪ�ض���������Ϣ
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
                        //B�Ľ���ʱ��,����A�Ľ���ʱ��
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
                            + "\"������Ԫ�أ�\"" +  bufTradeDelElement.GetString("PRODUCT_NAME", i)
                            + " --> " + bufTradeDelElement.GetString("PACKAGE_NAME", i)
                            + " --> " +  bufTradeDelElement.GetString("ELEMENT_NAME", i) + "\"";
                    else
                        strErrorInfo += "��\"" + bufUserAllElements.GetString("PRODUCT_NAME", z)
                            + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", z)
                            + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", z)
                            + "\"������Ԫ�أ�\"" +  bufTradeDelElement.GetString("PRODUCT_NAME", i)
                            + " --> " + bufTradeDelElement.GetString("PACKAGE_NAME", i)
                            + " --> " +  bufTradeDelElement.GetString("ELEMENT_NAME", i) + "\"";
                }
            }
        }

        if(strErrorInfo != "")
            THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strErrorInfo + "��ҵ���޷�����1��");
    }
}


//��ȡ����Ԫ��֮���ϵ
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ����Ԫ�ؼ��ϵ��Ϣ����[%s]",ex.getCause()->what());
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


//����Ԫ��֮�����ƹ�ϵ�ж�
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


    //���û�Ԫ��ѭ��У�����Ԫ�ػ����ϵ
    int iUserElements = bufUserAllElements.GetInt("X_RECORDNUM");

    for(i = 0; i < iUserElements; i++)
    {
        //��ǰҵ���ޱ仯�İ������ж�
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

        //��ǰҵ����ȫɾ���İ������ж�
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
		    �µ���ֹ���Ż��ж�����(��ֹ���µ׵��Żݣ������뻥�⣬������������ȫ�������ظ�Ԫ�ص��жϣ�����ֹ���µ׵��Ż�
		    ���ж�ʱ������Ϊ���ڣ�Ҳ������Ϊ�ǲ����ڵ�Ԫ��)
		    */
		    if( bufUserAllElements.GetString("END_DATE",i)<=strLastDate&&bufUserAllElements.GetString("ELEMENT_TYPE_CODE", i)=="D")
			      continue;

        iCount = 0;
        bufElementLimitByPackage.ClearFmlValue();

        //��ȡԪ�ػ�����Ϣ
        iCount = ExistsPackageElementLimitA(bufUserAllElements.GetInt("PACKAGE_ID", i),
                                            bufUserAllElements.GetString("ELEMENT_TYPE_CODE", i),
                                            bufUserAllElements.GetInt("ELEMENT_ID", i),
                                            "0", strEparchyCode, bufElementLimitByPackage);

        for(j = 0; j < iCount; j++)
        {
            bFound = false;
            for(z = 0; z < iUserElements; z++)
            {
                if (i==z) continue; //Ԫ�������������ж� added by tangz@2009-2-7 11:33
                if(bufUserAllElements.GetInt("PACKAGE_ID", z) == bufElementLimitByPackage.GetInt("PACKAGE_ID", j)
                    && bufUserAllElements.GetInt("ELEMENT_ID", z) == bufElementLimitByPackage.GetInt("ELEMENT_ID_B", j)
                    && bufUserAllElements.GetString("ELEMENT_TYPE_CODE", z) == bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", j)
                    //ʱ�佻��
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
                //(i)��(z)����
                if(strErrorInfo == "")
                    strErrorInfo = "\"" + bufUserAllElements.GetString("PACKAGE_NAME", i)
                        + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                        + "\"��\"" + bufUserAllElements.GetString("PACKAGE_NAME", z)
                        + " --> " +  bufUserAllElements.GetString("ELEMENT_NAME", z) + "\"";
                else
                    strErrorInfo += "��\"" + bufUserAllElements.GetString("PACKAGE_NAME", i)
                        + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                        + "\"��\"" + bufUserAllElements.GetString("PACKAGE_NAME", z)
                        + " -->" +  bufUserAllElements.GetString("ELEMENT_NAME", z) + "\"";
            }
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strErrorInfo + "���⣬ҵ���޷�������");


    //���û�Ԫ��ѭ��У�����Ԫ�ز���������ϵ
    strErrorInfo = "";
    for(i = 0; i < iUserElements; i++)
    {
        iCount = 0;
        bufElementLimitByPackage.ClearFmlValue();
        
        /*
		    add by lirui@20090505	
		    �µ���ֹ���Ż��ж�����(��ֹ���µ׵��Żݣ������뻥�⣬������������ȫ�������ظ�Ԫ�ص��жϣ�����ֹ���µ׵��Ż�
		    ���ж�ʱ������Ϊ���ڣ�Ҳ������Ϊ�ǲ����ڵ�Ԫ��)
		    */
		    if( bufUserAllElements.GetString("END_DATE",i)<=strLastDate&&bufUserAllElements.GetString("ELEMENT_TYPE_CODE", i)=="D")
			      continue;        

        //��ȡԪ�ز���������Ϣ
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
                    //ʱ�佻��
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
            //(i)����������(j)
            //��������Ԫ��ƴ��
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
                        strInfo += "��" + bufElementLimitByPackage.GetString("DISCNT_NAME", y);
                    else if(bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", y) == "S")
                        strInfo += "��" + bufElementLimitByPackage.GetString("SERVICE_NAME", y);
                    else if(bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", y) == "X")
                        strInfo += "��" + bufElementLimitByPackage.GetString("SP_PRODUCT_NAME", y);
                    else if(bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", y) == "A")
                        strInfo += "��" + bufElementLimitByPackage.GetString("ACTION_NAME", y);
                    else if(bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", y) == "K")
                        strInfo = bufElementLimitByPackage.GetString("PARENT_ACTION_NAME", y);
                }
            }

            if(strErrorInfo == "")
                strErrorInfo = "\"" + bufUserAllElements.GetString("PACKAGE_NAME", i)
                    + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                    + "\"���������ڣ�\"" + bufElementLimitByPackage.GetString("PACKAGE_NAME")
                    + " --> " +  strInfo + "\"";
            else
                strErrorInfo += "��\"" + bufUserAllElements.GetString("PACKAGE_NAME", i)
                    + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                    + "\"���������ڣ�\"" + bufElementLimitByPackage.GetString("PACKAGE_NAME")
                    + " --> " +  strInfo + "\"";
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strErrorInfo + "��ҵ���޷�������");


    //���û�Ԫ��ѭ��У�����Ԫ����ȫ������ϵ
    strErrorInfo = "";
    for(i = 0; i < iUserElements; i++)
    {
        iCount = 0;
        bufElementLimitByPackage.ClearFmlValue();
        
        /*
		    add by lirui@20090505	
		    �µ���ֹ���Ż��ж�����(��ֹ���µ׵��Żݣ������뻥�⣬������������ȫ�������ظ�Ԫ�ص��жϣ�����ֹ���µ׵��Ż�
		    ���ж�ʱ������Ϊ���ڣ�Ҳ������Ϊ�ǲ����ڵ�Ԫ��)
		    */
		    if( bufUserAllElements.GetString("END_DATE",i)<=strLastDate&&bufUserAllElements.GetString("ELEMENT_TYPE_CODE", i)=="D")
			      continue;        

        //��ȡԪ����ȫ������Ϣ
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
                    //ʱ�佻��
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
                //(i)��ȫ������(j)
                //��ȫ����Ԫ��ƴ��
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
                            strInfo += "��" + bufElementLimitByPackage.GetString("DISCNT_NAME", y);
                        else if(bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", y) == "S")
                            strInfo += "��" + bufElementLimitByPackage.GetString("SERVICE_NAME", y);
                        else if(bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", y) == "X")
                            strInfo += "��" + bufElementLimitByPackage.GetString("SP_PRODUCT_NAME", y);
                        else if(bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", y) == "A")
                            strInfo += "��" + bufElementLimitByPackage.GetString("ACTION_NAME", y);
                        else if(bufElementLimitByPackage.GetString("ELEMENT_TYPE_CODE_B", y) == "K")
                            strInfo = bufElementLimitByPackage.GetString("PARENT_ACTION_NAME", y);
                    }
                }

                if(strErrorInfo == "")
                    strErrorInfo = "\"" + bufUserAllElements.GetString("PACKAGE_NAME", i)
                        + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                        + "\"��ȫ�����ڣ�\"" + bufElementLimitByPackage.GetString("PACKAGE_NAME")
                        + " --> " +  strInfo + "\"";
                else
                    strErrorInfo += "��\"" + bufUserAllElements.GetString("PACKAGE_NAME", i)
                        + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                        + "\"��ȫ�����ڣ�\"" + bufElementLimitByPackage.GetString("PACKAGE_NAME")
                        + " --> " +  strInfo + "\"";

                break;
            }
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strErrorInfo + "��ҵ���޷�������");
}


//ͬһԪ����һ��ҵ����ֻ��ѡ��һ��
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
		    �µ���ֹ���Ż��ж�����(��ֹ���µ׵��Żݣ������뻥�⣬������������ȫ�������ظ�Ԫ�ص��жϣ�����ֹ���µ׵��Ż�
		    ���ж�ʱ������Ϊ���ڣ�Ҳ������Ϊ�ǲ����ڵ�Ԫ��)
		    */
		    if( bufUserAllElements.GetString("END_DATE",i)<=strLastDate&&bufUserAllElements.GetString("ELEMENT_TYPE_CODE", i)=="D")
			      continue;
			            
        for(int j = 0; j < iCount; j++)
        {
            if(i != j && bufUserAllElements.GetInt("ELEMENT_ID", i) == bufUserAllElements.GetInt("ELEMENT_ID", j)
                &&bufUserAllElements.GetString("ELEMENT_TYPE_CODE", i) == bufUserAllElements.GetString("ELEMENT_TYPE_CODE", j)
                //ʱ�佻��
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
                                   + "\"��\""
                                   + bufUserAllElements.GetString("PRODUCT_NAME",j)
                                   + " --> " + bufUserAllElements.GetString("PACKAGE_NAME",j)
                                   + " --> " + bufUserAllElements.GetString("ELEMENT_NAME",j)
                                   + "\"";
                }
                else
                {
                    strErrorInfo += "��\"" + bufUserAllElements.GetString("PRODUCT_NAME",i)
                                    + " --> " + bufUserAllElements.GetString("PACKAGE_NAME",i)
                                    + " --> " + bufUserAllElements.GetString("ELEMENT_NAME",i)
                                    + "\"��\""
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
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strErrorInfo + "�ظ�ѡ��ҵ���޷�������");
}

//SP�����жϣ�ͬһ��SP_IDֻ��ѡ��һ��
void TradeCheckAfterTrade::CheckSameSPID(CFmlBuf &bufTradeSp_UserAllSp)
{
    LOG_TRACE(logTradeCheckAfterTrade, "����CheckSameSPID����");

    int iCount = bufTradeSp_UserAllSp.GetInt("X_RECORDNUM");
    string strErrorInfo = "";
    StaticDAO& dao = StaticDAO::getDAO();
    CFmlBuf bufSp;
    int spCount = -1;

    //��ȡSP��SP_IDֵ
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
            THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ����ҵ����û�������SP�Ĳ�������[%s]",ex.getCause()->what());
        }

        if(spCount > 0)
        {
            bufTradeSp_UserAllSp.SetString("SP_ID",bufSp.GetString("SP_ID",0),i);
        }
        else
        {
            THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "��ȡ����ҵ���SP_SERVICE_ID=[%d]�Ĳ�������", bufTradeSp_UserAllSp.GetInt("SP_SERVICE_ID",i));
        }
    }

    //�ж��Ƿ����ͬһ��SP_ID�ظ�ѡ�������
    for(int j = 0; j < iCount; j++)
    {
        if(bufTradeSp_UserAllSp.IsFldExist("X_GETMODE", j) && bufTradeSp_UserAllSp.GetInt("X_GETMODE", j) == 1)
            continue;
        for(int t = 0; t < iCount; t++)
        {
           if(j != t && bufTradeSp_UserAllSp.GetString("SP_ID", j) == bufTradeSp_UserAllSp.GetString("SP_ID", t)
                //ʱ�佻��
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
                                       + "\"��\""
                                       + bufTradeSp_UserAllSp.GetString("PRODUCT_NAME",t)
                                       + " --> " + bufTradeSp_UserAllSp.GetString("PACKAGE_NAME",t)
                                       + " --> " + bufTradeSp_UserAllSp.GetString("SP_PRODUCT_NAME",t)
                                       + "\"";
                    }
                    else
                    {
                        strErrorInfo += "��\"" + bufTradeSp_UserAllSp.GetString("PRODUCT_NAME",j)
                                        + " --> " + bufTradeSp_UserAllSp.GetString("PACKAGE_NAME",j)
                                        + " --> " + bufTradeSp_UserAllSp.GetString("SP_PRODUCT_NAME",j)
                                        + "\"��\""
                                        + bufTradeSp_UserAllSp.GetString("PRODUCT_NAME",t)
                                        + " --> " + bufTradeSp_UserAllSp.GetString("PACKAGE_NAME",t)
                                        + " --> " + bufTradeSp_UserAllSp.GetString("SP_PRODUCT_NAME",t)
                                        + "\"";
                    }
                    break;
                }
        }
    }

    LOG_TRACE_P1(logTradeCheckAfterTrade, "CheckSameSPID����--У��sp:%s",strErrorInfo.c_str());
    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strErrorInfo + " ͬһ��SP�����ظ�ѡ��ҵ���޷�������");

    LOG_TRACE(logTradeCheckAfterTrade, "�˳�CheckSameSPID����");
}

//��ȡ����Ԫ��֮���ϵ
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ����Ԫ�ؼ��ϵ��Ϣ����[%s]",ex.getCause()->what());
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


//����Ԫ�ع�ϵ�ж�
void TradeCheckAfterTrade::CheckPackageAndElementsLimit(CFmlBuf &bufUserPackageByProd, CFmlBuf &bufUserAllElements)
{
    CFmlBuf bufPackageElementsLimit;
    int i,j,z;
    bool bFound = false;

    //����
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
                if(bufPackageElementsLimit.GetString("ELEMENT_TYPE_CODE", j) ==    //��ͬԪ������
                    bufUserAllElements.GetString("ELEMENT_TYPE_CODE", z) &&
                    bufPackageElementsLimit.GetInt("ELEMENT_ID", j) ==    //��ͬԪ��
                    bufUserAllElements.GetInt("ELEMENT_ID", z))
                {
                    bFound = true;
                    break;
                }
            }

            if(bFound)
            {
                //(i)��(j)����
                if(bufPackageElementsLimit.GetString("ELEMENT_TYPE_CODE", j) == "D")
                {
                    if(strDiscntInfo == "")
                        strDiscntInfo = bufPackageElementsLimit.GetString("DISCNT_NAME", j);
                    else
                        strDiscntInfo += "��" + bufPackageElementsLimit.GetString("DISCNT_NAME", j);
                }
                else if(bufPackageElementsLimit.GetString("ELEMENT_TYPE_CODE", j) == "S")
                {
                    if(strSvcInfo == "")
                        strSvcInfo = bufPackageElementsLimit.GetString("SERVICE_NAME", j);
                    else
                        strSvcInfo += "��" + bufPackageElementsLimit.GetString("SERVICE_NAME", j);
                }
                else if(bufPackageElementsLimit.GetString("ELEMENT_TYPE_CODE", j) == "X")
                {
                    if(strSpInfo == "")
                        strSpInfo = bufPackageElementsLimit.GetString("SP_PRODUCT_NAME", j);
                    else
                        strSpInfo += "��" + bufPackageElementsLimit.GetString("SP_PRODUCT_NAME", j);
                }
                else
                {
                    if(strElementInfo == "")
                        strElementInfo = bufPackageElementsLimit.GetString("ACTION_NAME", j);
                    else
                        strElementInfo += "��" + bufPackageElementsLimit.GetString("ACTION_NAME", j);
                }
            }
        }

        if(strDiscntInfo != "" || strSvcInfo != "" || strSpInfo != "" || strElementInfo != "")
        {
            //(i)��(j)����
            /*if(strDiscntInfo != "")
                strDiscntInfo = "�Żݣ�\"" + strDiscntInfo + "\"";
            if(strSvcInfo != "")
                strSvcInfo = "����\"" + strSvcInfo + "\"";

            if(strDiscntInfo != "" && strSvcInfo != "")
                strErrorInfo = strDiscntInfo + "��" + strSvcInfo;
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
                else strErrorInfo += "��" + strDiscntInfo;
            }

            if(strSvcInfo != "")
            {
                if(strErrorInfo == "")  strErrorInfo += strSvcInfo;
                else strErrorInfo += "��" + strSvcInfo;
            }

            if(strSpInfo != "")
            {
                if(strErrorInfo == "")  strErrorInfo += strSpInfo;
                else strErrorInfo += "��" + strSpInfo;
            }

            if(strElementInfo != "")
            {
                if(strErrorInfo == "")  strErrorInfo += strElementInfo;
                else strErrorInfo += "��" + strElementInfo;
            }

            strErrorInfo = "ҵ�����\"" + bufUserPackageByProd.GetString("PACKAGE_NAME", i)
                    + "\"��Ԫ�أ�" + strErrorInfo + "����";
        }

        if(strErrorInfo != "")
        {
            if(strHintInfo == "")
                strHintInfo = strErrorInfo;
            else
                strHintInfo += "��" + strErrorInfo;
        }
    }

    if(strHintInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strHintInfo + "��ҵ���޷�������");


    //��������
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
                if(bufPackageElementsLimit.GetString("ELEMENT_TYPE_CODE", j) ==    //��ͬԪ������
                    bufUserAllElements.GetString("ELEMENT_TYPE_CODE", z) &&
                    bufPackageElementsLimit.GetInt("ELEMENT_ID", j) ==    //��ͬԪ��
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
            //(i)��������j
            for(int y = 0; y < iLimitCount; y++)
            {
                if(bufPackageElementsLimit.GetString("ELEMENT_TYPE_CODE", y) == "D")
                {
                    if(strDiscntInfo == "")
                        strDiscntInfo = bufPackageElementsLimit.GetString("DISCNT_NAME", y);
                    else
                        strDiscntInfo += "��" + bufPackageElementsLimit.GetString("DISCNT_NAME", y);
                }
                else if(bufPackageElementsLimit.GetString("ELEMENT_TYPE_CODE", y) == "S")
                {
                    if(strSvcInfo == "")
                        strSvcInfo = bufPackageElementsLimit.GetString("SERVICE_NAME", y);
                    else
                        strSvcInfo += "��" + bufPackageElementsLimit.GetString("SERVICE_NAME", y);
                }
                else if(bufPackageElementsLimit.GetString("ELEMENT_TYPE_CODE", y) == "X")
                {
                    if(strSpInfo == "")
                        strSpInfo = bufPackageElementsLimit.GetString("SP_PRODUCT_NAME", y);
                    else
                        strSpInfo += "��" + bufPackageElementsLimit.GetString("SP_PRODUCT_NAME", y);
                }
                else
                {
                    if(strElementInfo == "")
                        strElementInfo = bufPackageElementsLimit.GetString("ACTION_NAME", y);
                    else
                        strElementInfo += "��" + bufPackageElementsLimit.GetString("ACTION_NAME", y);
                }
            }

            if(strDiscntInfo != "" || strSvcInfo != "" || strSpInfo != "" || strElementInfo != "")
            {
                //(i)��������j
                /*if(strDiscntInfo != "")
                    strDiscntInfo = "�Żݣ�\"" + strDiscntInfo + "\"";
                if(strSvcInfo != "")
                    strSvcInfo = "����\"" + strSvcInfo + "\"";

                if(strDiscntInfo != "" && strSvcInfo != "")
                    strErrorInfo = strDiscntInfo + "��" + strSvcInfo;
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
                    else strErrorInfo += "��" + strDiscntInfo;
                }

                if(strSvcInfo != "")
                {
                    if(strErrorInfo == "")  strErrorInfo += strSvcInfo;
                    else strErrorInfo += "��" + strSvcInfo;
                }

                if(strSpInfo != "")
                {
                    if(strErrorInfo == "")  strErrorInfo += strSpInfo;
                    else strErrorInfo += "��" + strSpInfo;
                }

                if(strElementInfo != "")
                {
                    if(strErrorInfo == "")  strErrorInfo += strElementInfo;
                    else strErrorInfo += "��" + strElementInfo;
                }

                strErrorInfo = "ҵ�����\"" + bufUserPackageByProd.GetString("PACKAGE_NAME", i)
                        + "\"����������Ԫ�أ�" + strErrorInfo;
            }
        }

        if(strErrorInfo != "")
        {
            if(strHintInfo == "")
                strHintInfo = strErrorInfo;
            else
                strHintInfo += "��" + strErrorInfo;
        }
    }

    if(strHintInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strHintInfo + "��ҵ���޷�������");


    //��ȫ����
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
                if(bufPackageElementsLimit.GetString("ELEMENT_TYPE_CODE", j) ==    //��ͬԪ������
                    bufUserAllElements.GetString("ELEMENT_TYPE_CODE", z) &&
                    bufPackageElementsLimit.GetInt("ELEMENT_ID", j) ==    //��ͬԪ��
                    bufUserAllElements.GetInt("ELEMENT_ID", z))
                {
                    bFound = true;
                    break;
                }
            }

            if(!bFound)
            {
                //(i)��ȫ����j
                for(int y = 0; y < iLimitCount; y++)
                {
                    if(bufPackageElementsLimit.GetString("ELEMENT_TYPE_CODE", y) == "D")
                    {
                        if(strDiscntInfo == "")
                            strDiscntInfo = bufPackageElementsLimit.GetString("DISCNT_NAME", y);
                        else
                            strDiscntInfo += "��" + bufPackageElementsLimit.GetString("DISCNT_NAME", y);
                    }
                    else if(bufPackageElementsLimit.GetString("ELEMENT_TYPE_CODE", y) == "S")
                    {
                        if(strSvcInfo == "")
                            strSvcInfo = bufPackageElementsLimit.GetString("SERVICE_NAME", y);
                        else
                            strSvcInfo += "��" + bufPackageElementsLimit.GetString("SERVICE_NAME", y);
                    }
                    else if(bufPackageElementsLimit.GetString("ELEMENT_TYPE_CODE", y) == "X")
                    {
                        if(strSpInfo == "")
                            strSpInfo = bufPackageElementsLimit.GetString("SP_PRODUCT_NAME", y);
                        else
                            strSpInfo += "��" + bufPackageElementsLimit.GetString("SP_PRODUCT_NAME", y);
                    }
                    else
                    {
                        if(strElementInfo == "")
                            strElementInfo = bufPackageElementsLimit.GetString("ACTION_NAME", y);
                        else
                            strElementInfo += "��" + bufPackageElementsLimit.GetString("ACTION_NAME", y);
                    }
                }

                if(strDiscntInfo != "" || strSvcInfo != "" || strSpInfo != "" || strElementInfo != "")
                {
                    //(i)��ȫ����j
                    /*if(strDiscntInfo != "")
                        strDiscntInfo = "�Żݣ�\"" + strDiscntInfo + "\"";
                    if(strSvcInfo != "")
                        strSvcInfo = "����\"" + strSvcInfo + "\"";

                    if(strDiscntInfo != "" && strSvcInfo != "")
                        strErrorInfo = strDiscntInfo + "��" + strSvcInfo;
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
                        else strErrorInfo += "��" + strDiscntInfo;
                    }

                    if(strSvcInfo != "")
                    {
                        if(strErrorInfo == "")  strErrorInfo += strSvcInfo;
                        else strErrorInfo += "��" + strSvcInfo;
                    }

                    if(strSpInfo != "")
                    {
                        if(strErrorInfo == "")  strErrorInfo += strSpInfo;
                        else strErrorInfo += "��" + strSpInfo;
                    }

                    if(strElementInfo != "")
                    {
                        if(strErrorInfo == "")  strErrorInfo += strElementInfo;
                        else strErrorInfo += "��" + strElementInfo;
                    }

                    strErrorInfo = "ҵ�����\"" + bufUserPackageByProd.GetString("PACKAGE_NAME", i)
                            + "\"��ȫ������Ԫ�أ�" + strErrorInfo;
                }

                break;
            }
        }

        if(strErrorInfo != "")
        {
            if(strHintInfo == "")
                strHintInfo = strErrorInfo;
            else
                strHintInfo += "��" + strErrorInfo;
        }
    }

    if(strHintInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strHintInfo + "��ҵ���޷�������");
}


//��ȡ��Ʒ֮�����ƹ�ϵ
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ��Ʒ���ϵ��Ϣ����[%s]",ex.getCause()->what());
    }

    ConvertCodeToName(bfProductLimit, "ProductName", "PRODUCT_ID_B", "PRODUCT_NAME");

    bfProductLimit.SetInt("X_RECORDNUM", iCount);
    return iCount;
}


//��Ʒ������
void TradeCheckAfterTrade::CheckProductLimit(CFmlBuf &bfUserProd)
{
    CFmlBuf bfProductLimit;
    int i,j,z;
    bool bFound = false;

    //����
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
                //(i)��(j)����
                if(strErrorInfo == "")
                    strErrorInfo = bfProductLimit.GetString("PRODUCT_NAME", j);
                else
                    strErrorInfo += "��" + bfProductLimit.GetString("PRODUCT_NAME", j);
            }
        }

        if(strErrorInfo != "")
        {
            if(strHintInfo == "")
                strHintInfo = "��Ʒ��\"" + bfUserProd.GetString("PRODUCT_NAME", i)
                              + "\"���Ʒ��\"" + strErrorInfo + "\"����";
            else
                strHintInfo += "����Ʒ��\"" + bfUserProd.GetString("PRODUCT_NAME", i)
                               + "\"���Ʒ��\"" + strErrorInfo + "\"����";
        }
    }

    if(strHintInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strHintInfo + "��ҵ���޷�������");


    //��������
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
            //(i)��������j
            for(int y = 0; y < iLimitCount; y++)
            {
                if(strErrorInfo == "")
                    strErrorInfo = bfProductLimit.GetString("PRODUCT_NAME", y);
                else
                    strErrorInfo += "��" + bfProductLimit.GetString("PRODUCT_NAME", y);
            }
        }

        if(strErrorInfo != "")
        {
            if(strHintInfo == "")
                strHintInfo = "��Ʒ��\"" + bfUserProd.GetString("PRODUCT_NAME", i)
                              + "\"���������ڲ�Ʒ��\"" + strErrorInfo + "\"";
            else
                strHintInfo += "����Ʒ��\"" + bfUserProd.GetString("PRODUCT_NAME", i)
                               + "\"���������ڲ�Ʒ��\"" + strErrorInfo + "\"";
        }
    }

    if(strHintInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strHintInfo + "��ҵ���޷�������");


    //��ȫ����
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
                //(i)��ȫ����j
                for(int y = 0; y < iLimitCount; y++)
                {
                    if(strErrorInfo == "")
                        strErrorInfo = bfProductLimit.GetString("PRODUCT_NAME", y);
                    else
                        strErrorInfo += "��" + bfProductLimit.GetString("PRODUCT_NAME", y);
                }

                break;
            }
        }

        if(strErrorInfo != "")
        {
            if(strHintInfo == "")
                strHintInfo = "��Ʒ��\"" + bfUserProd.GetString("PRODUCT_NAME", i)
                              + "��ȫ�����ڲ�Ʒ��\"" +strErrorInfo + "\"";
            else
                strHintInfo += "����Ʒ��\"" + bfUserProd.GetString("PRODUCT_NAME", i)
                               + "��ȫ�����ڲ�Ʒ��\"" +strErrorInfo + "\"";
        }
    }

    if(strHintInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strHintInfo + "��ҵ���޷�������");
}

//��ȡɾ��������Ϣ
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
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ѯҵ��̨��������Ϣ�ӱ����");
    }

    bufDelPackage.SetInt("X_RECORDNUM", iCount);

    return iCount;
}

//��ȡ��ǰҵ��δ�����仯�İ���Ϣ
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
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ�û�δ�仯����Ϣ����");
    }

    bufNoChangePackage.SetInt("X_RECORDNUM", iCount);

    return iCount;
}


//��ȡ����Ԫ��ʱ��������Ԫ�ظ��������ڼ������Ԫ�������С��ʱ��ʱ������������Ԫ�ؼ���Ϊһ��
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
            //ͬһ��Ʒ��ͬһ���ڣ�ͬһԪ��������Чʱ�������������ж��Ƿ�ΪͬһԪ�أ�
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


//��Ʒ��ԴԼ��
void TradeCheckAfterTrade::CheckProductRes(CFmlBuf &bfUserProd, const string &strTradeId, const string &strUserId,
                                           const string &strEparchyCode)
{
    int iCondCount = 0;
    bool bFound = false;
    CFmlBuf bfUserSn;
    CFmlBuf bfUserSimCard;
    CFmlBuf bfUserDeviceType;
    CFmlBuf bfProdResLimit;

    //��ȡ�û�����
    GetUserSerialNumber(bfUserSn, strTradeId, strUserId);

    //��ȡ�û�SIM��
    GetUserSimCard(bfUserSimCard, strTradeId, strUserId);

    //��ȡ�û��ֻ�����
    GetUserDeviceType(bfUserDeviceType, strTradeId, strUserId);

    //��ֻʵ�ֵ����жϣ���Ʒ-->��Դ����Ʒǿ�ƹ���
    int iCount = bfUserProd.GetInt("X_RECORDNUM");
    for(int i = 0; i < iCount; i++)
    {
        if(bfUserSn.GetInt("X_RECORDNUM") > 0)
        {
            //��ȡ��Ʒ��ԴԼ��(����)
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
                        "��Ʒ��\"" + bfUserProd.GetString("PRODUCT_NAME", i) + "\"����ʹ�ú�����Դ��\"" +
                         bfUserSn.GetString("RES_CODE") + "\"��ҵ���޷�������");
            }
        }

        if(bfUserSimCard.GetInt("X_RECORDNUM") > 0)
        {
            //��ȡ��Ʒ��ԴԼ��(SIM��)
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
                        "��Ʒ��\"" + bfUserProd.GetString("PRODUCT_NAME", i) + "\"����ʹ��SIM����Դ��\"" +
                         bfUserSimCard.GetString("RES_CODE") + "\"��ҵ���޷�������");
            }
        }

        for(int z = 0; z < bfUserDeviceType.GetInt("X_RECORDNUM"); z++)
        {
            //��ȡ��Ʒ��ԴԼ��(�ն�)
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
                        "��Ʒ��\"" + bfUserProd.GetString("PRODUCT_NAME", i) + "\"����ʹ���ն���Դ��\"" +
                         bfUserDeviceType.GetString("DEVICE_TYPE", z) + "\"��ҵ���޷�������");
            }
        }
    }
}


//��ȡ�û�����
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
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ�û����������Ϣ����");
    }

    bfUserSn.SetInt("X_RECORDNUM", iCount);

    return iCount;
}


//��ȡ�û�SIM��
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
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ�û�SIM����Ϣ����");
    }

    bfUserSimCard.SetInt("X_RECORDNUM", iCount);

    return iCount;
}


//��ȡ�û��ֻ�����
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
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ�û��ն�������Ϣ����");
    }

    bfUserDeviceType.SetInt("X_RECORDNUM", iCount);

    return iCount;
}


//��ȡ��Ʒ��ԴԼ��
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
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ��Ʒ��ԴԼ����Ϣ����");
    }

    ConvertCodeToName(bfProdResLimit, "ProductName", "PRODUCT_ID", "PRODUCT_NAME");

    bfProdResLimit.SetInt("X_RECORDNUM", iCount);

    return iCount;
}


/**
 *  ɽ��������ҵ��֤���ж�
 */
void TradeCheckAfterTrade::CheckAgentTradeFee(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
    LOG_TRACE(logTradeCheckAfterTrade, "����CheckAgentTradeFee����");
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
        THROW_AGC(CRMException, ex, Grade::WARNING, RECVDEPOSIT_ERR, "��ȡ̨�ʷ����ӱ������쳣��");
    }
    int iMoney = 0;
    for(int i=0;i<tmpBuf.GetCount("TRADE_ID");++i)
    {
        iMoney += atoi(tmpBuf.GetString("FEE",i).c_str());
    }
    if(iMoney>agentFee)
    		THROW_C(CRMException, CHECKAFTERTRADE_ERR, "�����̱�֤��������ҵ�����շ���");
    LOG_TRACE(logTradeCheckAfterTrade, "�˳�CheckAgentTradeFee����");
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
        THROW_AGC(CRMException, ex, Grade::WARNING, GETSYSDATE_ERR, "��ȡʱ�����");
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
        THROW_AGC(CRMException, ex, Grade::WARNING, GETSYSDATE_ERR, "��ȡʱ�����");
    }

    strResultDateTime = bufTemp.GetString("X_SYSDATE");
}
//��Ʒģ���������
void TradeCheckAfterTrade::CheckProductAllElementLimit(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
    string strSubscribeId = inBuf.GetString("SUBSCRIBE_ID");
    string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");
		int icount=0;
    CFmlBuf bufTradeDiscnt_AllDiscnt;   //�û��Ż�
    CFmlBuf bufTradeSvc_UserAllSvc;     //�û�����
    CFmlBuf bufTradeSp_UserAllSp;       //�û�SP
    CFmlBuf bufTradeElement_UserAllElement; //�û���Ʒ������Ԫ��
    CFmlBuf bufUserAllElements;         //�û�Ԫ��
    CFmlBuf bufElementTimeSeries;       //����Ԫ��ʱ�����������ڼ������Ԫ�������С��ʱ��ʱ������������Ԫ�ؼ���Ϊһ��
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
        //��ȡ�û���Ʒ������Ԫ����Ϣ
        GetUserAllElementAfterTradeComp(strSubscribeId, bufTradeElement_UserAllElement);
        //��ȡ�û�SP��Ϣ
        GetUserAllSpAfterTradeComp(strSubscribeId, bufTradeSp_UserAllSp);
        //�����û�Ԫ��
        GeneUserAllElements(bufTradeDiscnt_AllDiscnt, bufTradeSvc_UserAllSvc, bufTradeSp_UserAllSp, bufTradeElement_UserAllElement, bufUserAllElements);
        //��ȡ����Ԫ��ʱ��������Ԫ�ظ��������ڼ������Ԫ�������С��ʱ��ʱ������������Ԫ�ؼ���Ϊһ��
        GetElementTimeSeries(bufUserAllElements, bufElementTimeSeries);
        CheckElementLimitComp(bufUserAllElements, strEparchyCode,strSubscribeId);
    }
    catch (Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "��Ʒģ��������Ƴ���");
    }
}
/**
 *��ȡ����ҵ�����û������Żݺ��ŻݵĿ�ʼʱ�䡢����ʱ�䡢���޸�״̬������ԭ���Żݵ��޸�״̬Ĭ��Ϊ'A')
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

        //����Ʒ���ж�����Ͽ��ӵ�user_id���Աuser_id���Ӷ���ѯ��ϼ���Ա��Ϣ
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "��ѯ����[%s]��ؼ�¼����",strSubscribeId.c_str());
    }
    if(tradeICount <= 0 )
    {
        THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "û�ҵ���ض���[%s]�ļ�¼��", strSubscribeId.c_str());
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
                //�����û��ж϶�������
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
                    //�����û��ж϶�������
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ����ҵ��������������Żݳ���[%s]",ex.getCause()->what());
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
 *��ȡ����ҵ����û������з���
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

        //����Ʒ���ж�����Ͽ��ӵ�user_id���Աuser_id���Ӷ���ѯ��ϼ���Ա��Ϣ
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "��ѯ����[%s]��ؼ�¼����",strSubscribeId.c_str());
    }
    if(tradeICount <= 0 )
    {
        THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "û�ҵ���ض���[%s]�ļ�¼��", strSubscribeId.c_str());
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
                //�����û��ж϶�������
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
                    //�����û��ж϶�������
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ����ҵ������������з������[%s]",ex.getCause()->what());
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

        //����Ʒ���ж�����Ͽ��ӵ�user_id���Աuser_id���Ӷ���ѯ��ϼ���Ա��Ϣ
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "��ѯ����[%s]��ؼ�¼����",strSubscribeId.c_str());
    }
    if(tradeICount <= 0 )
    {
        THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "û�ҵ���ض���[%s]�ļ�¼��", strSubscribeId.c_str());
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
                //�����û��ж϶�������
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
                    //�����û��ж϶�������
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ����ҵ�������������Ԫ�س���[%s]",ex.getCause()->what());
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

        //����Ʒ���ж�����Ͽ��ӵ�user_id���Աuser_id���Ӷ���ѯ��ϼ���Ա��Ϣ
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "��ѯ����[%s]��ؼ�¼����",strSubscribeId.c_str());
    }
    if(tradeICount <= 0 )
    {
        THROW_C_P1(CRMException, CHECKAFTERTRADE_ERR, "û�ҵ���ض���[%s]�ļ�¼��", strSubscribeId.c_str());
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
                //�����û��ж϶�������
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
                    //�����û��ж϶�������
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ����ҵ�������������SP����[%s]",ex.getCause()->what());
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
//ȫ��Ԫ��֮�����ƹ�ϵ�ж�
void TradeCheckAfterTrade::CheckElementLimitComp(CFmlBuf &bufUserAllElements, const string &strEparchyCode,const string &strSubscribeId)
{
    CFmlBuf bufElementLimit;
    string strErrorInfo = "";
    int iCount = 0;
    bool bFound = false;
    int i,j,z,k;

    //���û�Ԫ��ѭ��У��Ԫ�ػ����ϵ
    int iUserElements = bufUserAllElements.GetInt("X_RECORDNUM");
    LOG_TRACE_P1(logTradeCheckAfterTrade, "*********************iUserElements:%d-----", iUserElements);
    bufUserAllElements.printFmlBuffer();
    for(i = 0; i < iUserElements; i++)
    {
        iCount = 0;
        bufElementLimit.ClearFmlValue();

        //��ȡԪ�ػ�����Ϣ
        iCount = ExistsElementLimitA(bufUserAllElements.GetString("ELEMENT_TYPE_CODE", i),
                                     bufUserAllElements.GetInt("ELEMENT_ID", i),
                                     "4", strEparchyCode, bufElementLimit);

        for(j = 0; j < iCount; j++)
        {
            bFound = false;
            for(z = 0; z < iUserElements; z++)
            {
                if (i==z)continue; //�����������ж� added by tangz@2009-2-7 11:33
                if(bufUserAllElements.GetInt("ELEMENT_ID", z) == bufElementLimit.GetInt("ELEMENT_ID_B", j)
                    && bufUserAllElements.GetString("ELEMENT_TYPE_CODE", z) == bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", j)
                    //ʱ�佻��
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
                        + "\"��\"" +  bufUserAllElements.GetString("PRODUCT_NAME", z)
                        + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", z)
                        + " --> " +  bufUserAllElements.GetString("ELEMENT_NAME", z) + "\"";
                else
                    strErrorInfo += "��\"" + bufUserAllElements.GetString("PRODUCT_NAME", i)
                        + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", i)
                        + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                        + "\"��\"" +  bufUserAllElements.GetString("PRODUCT_NAME", z)
                        + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", z)
                        + " --> " +  bufUserAllElements.GetString("ELEMENT_NAME", z) + "\"";
            }
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strErrorInfo + "���⣬ҵ���޷�������");

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
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "��ȡ��������!");
    }
    tradeBuf.printFmlBuffer();
    //���û�Ԫ��ѭ��У��Ԫ�ز���������ϵ
    strErrorInfo = "";
    for(i = 0; i < iUserElements; i++)
    {
        iCount = 0;
        bufElementLimit.ClearFmlValue();

        //��ȡԪ�ز���������Ϣ
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
                    THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "Ԫ�������жϣ���ѯ��Ʒ����!");
                }
                ip += iRowCount;
                LOG_TRACE_P1(logTradeCheckAfterTrade, "***************strproid**%d************", strproid);
            }

            for(z = 0; z < iUserElements; z++)
            {
                if(bufUserAllElements.GetInt("ELEMENT_ID", z) == bufElementLimit.GetInt("ELEMENT_ID_B", j)
                    && bufUserAllElements.GetString("ELEMENT_TYPE_CODE", z) == bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", j)
                    //ʱ�佻��
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
            //(i)����������(j)
            //��������Ԫ��ƴ��
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
                        strInfo += "��" + bufElementLimit.GetString("DISCNT_NAME", y);
                    else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "S")
                        strInfo += "��" + bufElementLimit.GetString("SERVICE_NAME", y);
                    else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "X")
                        strInfo += "��" + bufElementLimit.GetString("SP_PRODUCT_NAME", y);
                    else
                        strInfo += "��" + bufElementLimit.GetString("ACTION_NAME", y);
                }
            }

            if(strErrorInfo == "")
                strErrorInfo = "\"" + bufUserAllElements.GetString("PRODUCT_NAME", i)
                    + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", i)
                    + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                    + "\"����������Ԫ�أ�\"" +  strInfo + "\"";
            else
                strErrorInfo += "��\"" + bufUserAllElements.GetString("PRODUCT_NAME", i)
                    + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", i)
                    + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                    + "\"����������Ԫ�أ�\"" +  strInfo + "\"";
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strErrorInfo + "��ҵ���޷�������");


    //���û�Ԫ��ѭ��У�����Ԫ����ȫ������ϵ
    strErrorInfo = "";
    for(i = 0; i < iUserElements; i++)
    {
        iCount = 0;
        bufElementLimit.ClearFmlValue();

        //��ȡԪ����ȫ������Ϣ
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
                    THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "Ԫ�������жϣ���ѯ��Ʒ����!");
                }
                ip += iRowCount;
                LOG_TRACE_P2(logTradeCheckAfterTrade, "***************strproid**%d******%d******", strproid,iRowCount);
            }
			LOG_TRACE_P1(logTradeCheckAfterTrade, "***************ip**%d************", ip);



            for(z = 0; z < iUserElements; z++)
            {
                if(bufUserAllElements.GetInt("ELEMENT_ID", z) == bufElementLimit.GetInt("ELEMENT_ID_B", j)
                    && bufUserAllElements.GetString("ELEMENT_TYPE_CODE", z) == bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", j)
                    //ʱ�佻��
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

            if(!bFound||ie==0)//modify by zhangyangshuo  ���� ie ==0 ����
            {
                //(i)��ȫ������(j)
                //��ȫ����Ԫ��ƴ��
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
                            strInfo += "��" + bufElementLimit.GetString("DISCNT_NAME", y);
                        else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "S")
                            strInfo += "��" + bufElementLimit.GetString("SERVICE_NAME", y);
                        else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "X")
                            strInfo += "��" + bufElementLimit.GetString("SP_PRODUCT_NAME", y);
                        else
                            strInfo += "��" + bufElementLimit.GetString("ACTION_NAME", y);
                    }
                }

                if(strErrorInfo == "")
                    strErrorInfo = "\"" + bufUserAllElements.GetString("PRODUCT_NAME", i)
                        + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", i)
                        + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                        + "\"��ȫ������Ԫ�أ�\"" +  strInfo + "\"";
                else
                    strErrorInfo += "��\"" + bufUserAllElements.GetString("PRODUCT_NAME", i)
                        + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", i)
                        + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                        + "\"��ȫ������Ԫ�أ�\"" +  strInfo + "\"";

                break;
            }
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strErrorInfo + "��ҵ���޷�������");

    //���û�Ԫ��ѭ��У�����Ԫ�ض���������ϵ
    strErrorInfo = "";
    for(i = 0; i < iUserElements&&bufUserAllElements.GetString("TRADE_ID", i)!="0"
         &&(bufUserAllElements.GetString("MODIFY_TAG", i)=="A"||bufUserAllElements.GetString("MODIFY_TAG", i)=="0"); i++)//ֻ��鱾�ζ�����Ԫ��
    {
        iCount = 0;
        bufElementLimit.ClearFmlValue();

        //��ȡԪ�ض���������Ϣ
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
                    //ʱ�佻��
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
                //(j)������(i)
                //����Ԫ��ƴ��
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
                            strInfo += "��" + bufElementLimit.GetString("DISCNT_NAME", y);
                        else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "S")
                            strInfo += "��" + bufElementLimit.GetString("SERVICE_NAME", y);
                        else if(bufElementLimit.GetString("ELEMENT_TYPE_CODE_B", y) == "X")
                            strInfo += "��" + bufElementLimit.GetString("SP_PRODUCT_NAME", y);
                        else
                            strInfo += "��" + bufElementLimit.GetString("ACTION_NAME", y);
                    }
                }

                if(strErrorInfo == "")
                    strErrorInfo = "\"" + bufUserAllElements.GetString("PRODUCT_NAME", i)
                        + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", i)
                        + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                        + "\"������Ԫ�أ�\"" +  strInfo + "\"";
                else
                    strErrorInfo += "��\"" + bufUserAllElements.GetString("PRODUCT_NAME", i)
                        + " --> " + bufUserAllElements.GetString("PACKAGE_NAME", i)
                        + " --> " + bufUserAllElements.GetString("ELEMENT_NAME", i)
                        + "\"������Ԫ�أ�\"" +  strInfo + "\"";

                break;
            }
        }
    }

    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  strErrorInfo + "��ҵ���޷�����3��");
}

//SP���򶩹��Ĳ�ƷУ��
void TradeCheckAfterTrade::CheckSpProductModelLimit(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
    LOG_TRACE(logTradeCheckAfterTrade, "����CheckSpProductModelLimit����");
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
        THROW_AGC(CRMException, e, Grade::WARNING, MODIFYUSERINFO_ERR, "��ȡ̨��SP�����쳣��");
    }
    LOG_TRACE_P1(logTradeCheckAfterTrade, "SP�����˶���¼����---[%d]",iRowCount);
    
    //����tag�������ж��Ƿ���ҪУ���¶�����SP����
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
		THROW_AGC(CRMException, ex, Grade::WARNING, GETTRADEINFO_ERR, "��ȡ��TD_S_TAG::CS_CHECK_SP_TRADE_LIMIT��Ϣ����");
	}
	LOG_TRACE_P1(logTradeCheckAfterTrade, "��ȡtag��ΪCS_CHECK_SP_TRADE_LIMIT��¼����---[%d]",tmpcount);
    if(iRowCount > 0)
    {
        for(int i = 0; i < iRowCount ;i++ )
        {
            int iCount = 0;
            string strModifyTag = bufTradeSpSub.GetString("MODIFY_TAG",i);
            string strSpServiceId = bufTradeSpSub.GetString("SP_SERVICE_ID",i);
            int iPackageId = bufTradeSpSub.GetInt("PACKAGE_ID",i);

            //�жϰ��Ͱ�Ԫ�ظ�������ѡ����
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
                THROW_AGC(CRMException,ex,Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:���SP�����Ƴ���");
            }
            if( iCount > 0 )
            {
            	THROW_GC(CRMException,Grade::WARNING, CHECKAFTERTRADE_ERR, "��SP�����˶����ڰ����ƣ� ���ư�IDΪ��"+checkBuf.GetString("PACKAGE_ID",0));
            }

            //�ж϶�����������
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
                    THROW_AGC(CRMException,ex,Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:���SP�������");
                }
                if( iCount > 0 )
                {
                	THROW_GC(CRMException,Grade::WARNING, SP_MUTEX_ERR, "��SP���û�ԭ�е�SP���ڻ�����߻����������� aԪ�أ�"+checkBuf.GetString("ELEMENT_ID_A")+"bԪ�أ�"+checkBuf.GetString("ELEMENT_ID_B")+"���ƹ�ϵ���ͣ�"+checkBuf.GetString("LIMIT_TAG"));
                }
                
				//begin add by wangwp 20120420
				//У��̨�����SP����
				//��ȡ�뵱ǰsp�����Ԫ��
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
	                    THROW_AGC(CRMException,ex,Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:���SP�������");
	                }
	                LOG_TRACE_P1(logTradeCheckAfterTrade, "��ȡ�뵱�ڶ�����SP�����SPԪ�ؼ�¼����---[%d]",iTempCount);
	                if(iTempCount > 0)	//���ڻ���
	                {
	                	for(int z=0; z<iTempCount; z++)//ѭ�������Ԫ�ؼ�¼
						{
							for(int j=0; j<iRowCount; j++)//ѭ��̨�˵�spԪ��
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
					                	THROW_GC(CRMException,Grade::WARNING, SP_MUTEX_ERR, "���û��¶�����SP���ڻ��⣡ aԪ�أ�"+bufTradeSpSub.GetString("SP_SERVICE_ID", i)+"bԪ�أ�"+bufTradeSpSub.GetString("SP_SERVICE_ID", j)+"���ƹ�ϵ���ͣ�"+bufSpElementLimit.GetString("LIMIT_TAG", z));
					                }
								}	
							}
						}
	                }
	            }
				//end add by wangwp 20120420
            }
            //�ж��˶���������
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
                    THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:���SP��������");
                }
                if( iCount > 0 )
                {
                	THROW_GC(CRMException, Grade::WARNING, SP_DEPEND_ON_ERR, "��SP����������ϵ������ȡ���� aԪ�أ�"+checkBuf.GetString("ELEMENT_ID_A")+"bԪ�أ�"+checkBuf.GetString("ELEMENT_ID_B")+"���ƹ�ϵ���ͣ�"+checkBuf.GetString("LIMIT_TAG"));
                }
            }
        }
        //��ȡ�û�SP��Ϣ
        CFmlBuf bufTradeSp_UserAllSp;       //�û�SP
        GetUserAllSpAfterTrade(strTradeId, strUserId, bufTradeSp_UserAllSp);
        //SP�����жϣ�ͬһ��SP_IDֻ��ѡ��һ��
        CheckSameSPID(bufTradeSp_UserAllSp);
    }


    LOG_TRACE(logTradeCheckAfterTrade, "�˳�CheckSpProductModelLimit����");
}

/**
 *	��Ա�ּ�ͥͨ�Źܼҡ����Ա�ķ���ּ�ͥ��ϵ���͡��ʷѵ����Ƿ�ƥ��
 *	author:suiq
 *	date:2011-06-20
 **/
void TradeCheckAfterTrade::ChkWoDisUserProdLimit(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
		LOG_TRACE(logTradeCheckAfterTrade, "����ChkWoUserProdLimit������");
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
        THROW_AGC(CRMException, ex, Grade::WARNING, GETTRADEINFO_ERR, "��ȡ�����û�̨�ˣ�");
    }
		//��ȡ�û��Ż���Ϣ
		GetAllDiscntAfterTrade(strTradeId, tradeBuf.GetString("USER_ID"),	tradeBuf.GetString("CUST_ID"), tradeBuf.GetString("ACCT_ID"),	bufTradeDiscnt_AllDiscnt);
		LOG_TRACE_P1(logTradeCheckAfterTrade, "bufTradeDiscnt_AllDiscnt--%s",bufTradeDiscnt_AllDiscnt.ToString().c_str());

		//��ȡ�ּ�ͥ��Ա��Ϣ
		try
		{
				dao.Clear();
				dao.SetParam(":VUSER_ID_A",	tradeBuf.GetString("USER_ID"));
				dao.SetParam(":VTRADE_ID", strTradeId);
				iRelationCount = dao.jselect(tradeRelationBuf, "SEL_ALLUUS_AFTER_SUBSCRIBE","TF_B_TRADE_RELATION");
		}
		catch(Exception	&ex)
		{
				THROW_AGC(CRMException,	ex,	Grade::WARNING,	CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ�ּ�ͥ��Ա��Ϣ����");
		}
    //�ж�ÿ����Ա��Ԫ���Ƿ���ּ�ͥ�ʷѵ���ƥ��
		for(int	i	=	0;i	<	iRelationCount;i++)
		{
			  strErrorInfoTemp = "";
		    //��ȡ�ּ�ͥ��Ա��̨����Ϣ
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
						THROW_AGC(CRMException,	ex,	Grade::WARNING,	CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ�ּ�ͥ��Ա̨�˱���Ϣ����");
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
							  //��ȡ�ּ�ͥ��ԱԪ���Ƿ���Ҫ���ּ�ͥ�ʷѵ���ƥ��
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
										THROW_AGC(CRMException,	ex,	Grade::WARNING,	CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ�ּ�ͥ��ԱԪ���Ƿ���Ҫ���ʷѵ���ƥ�����");
								}

		            if(iEleDisCount > 0)//��Ҫ���ּ�ͥ��ϵ���͡��ʷѵ���ƥ��
		            {
		            	 matchTag = 0;
		            	 strInfo = "";
		            	 //(x)����������(y)
									 //��������Ԫ��ƴ��
		            	 for(int x = 0;x < iEleDisCount; x++)
		            	 {
		            	     if(strInfo == "")
											 {
											     strInfo = CParamDAO::getParam("DiscntName", tradEleDisBuf.GetString("VRESULT",x));

											 }
											 else
											 {
											     strInfo += "��" + CParamDAO::getParam("DiscntName", tradEleDisBuf.GetString("VRESULT",x));
											 }
		            	 }

		            	 for(int y = 0; y < bufTradeDiscnt_AllDiscnt.GetInt("X_RECORDNUM"); y++)
		               {
		               	  iPlusDisCount = -1;
		               	  if(bufTradeDiscnt_AllDiscnt.GetString("MODIFY_TAG",y) == "0" || bufTradeDiscnt_AllDiscnt.GetString("MODIFY_TAG",y) == "A" || bufTradeDiscnt_AllDiscnt.GetString("MODIFY_TAG",y) == "O" )
		               	  {
						               //�ж��ּ�ͥ��Աͨ�Źܼҡ����Ա�ķ�Ƿ���ּ�ͥ�ʷѵ���ƥ��
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
															THROW_AGC(CRMException,	ex,	Grade::WARNING,	CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ�ּ�ͥ��Ա���Ӳ�Ʒ�Ƿ���Ҫ���ʷѵ���ƥ�����");
													}
										  }
											//�ּ�ͥ��Աͨ�Źܼҡ����Ա�ķ���ּ�ͥ�ʷѵ���ƥ��
									    if(iPlusDisCount > 0){
											    matchTag = 1;
											    break;
											}
									 }
									 if(matchTag == 0)
							     {
							         if(strErrorInfoTemp	== "")
									 		    strErrorInfoTemp = "["	+	tradeRelationBuf.GetString("SERIAL_NUMBER_B",i) + "]��Ԫ��\"" + bufUserAllElementsMem.GetString("ELEMENT_NAME",j)
									 				+	"\"�����������ּ�ͥ�ʷѣ�\""+	strInfo	+	"\"";
									     else
									 		    strErrorInfoTemp +=	"��\"" + bufUserAllElementsMem.GetString("ELEMENT_NAME",j)
									 				+	"\"�����������ּ�ͥ�ʷѣ�\""+	strInfo	+	"\"";
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
		    THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,	 strErrorInfo	+	"��ҵ���޷���������ȡ����Աͨ�Źܼҡ����Ա�ķ���ߵ������һ��������ּ�ͥ�ʷѣ�");

		LOG_TRACE(logTradeCheckAfterTrade, "�˳�ChkWoUserProdLimit������");
}


//Э�����ʷѡ���ϲ�Ʒ֮�����ƹ�ϵ�ж�
void TradeCheckAfterTrade::CheckAttrElementLimit(CFmlBuf &bufTradeDiscnt_AllDiscnt, const string &strEparchyCode,const string strTradeId, const string &strUserId,const string &strCustId, const string &strAcctId)
{
	LOG_TRACE(logTradeCheckAfterTrade, "����CheckAttrElementLimit����");
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

	//��ȡ�û������û�������Ϣ
	int iUserAttrs = GetAllAttrAfterTrade(strTradeId, strUserId, bufTradeAttr_AllAttr);
	int iUserDiscnts = bufTradeDiscnt_AllDiscnt.GetInt("X_RECORDNUM");

	//��ȡ�û�̨������
	try
    {
        dao.Clear();
        dao.SetParam(":VTRADE_ID", strTradeId);
        dao.SetParam(":VCANCEL_TAG", "0");
        iCountTrade = dao.jselect(bufTrade, "SEL_BY_PK","TF_B_TRADE");
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ�û�̨���������[%s]",ex.getCause()->what());
    }

    //����û����ж�¼���Э��ű����ǵ�ǰ��ϲ�ƷҪ���Э�� start
    //��ȡ�û��Ƿ�����û�
    try
    {
        dao.Clear();
        dao.SetParam(":VTRADE_ID", strTradeId);
        iCountComp = dao.jselect(bufTradeComp, "SEL_COMP_TRADE_INFO2","TF_B_TRADE");
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ�û��Ƿ��������ҵ�����[%s]",ex.getCause()->what());
    }

	//�ж��û�Э�����ʷѡ���ϲ�Ʒ������ϵ start
    //���û�����ѭ��У���ʷѡ���ϲ�Ʒ������ϵ
    strErrorInfo = "";
    for(i = 0; i < iUserAttrs; i++)
    {
    	//��ȡЭ����ҵ�����ͣ�����Э���ȡҵ�����ͣ�֮�以���ϵ
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
        	strErrorInfo = "��"+bufTradeAttr_AllAttr.GetString("START_DATE",i)+"ѡ���\""+attrName+"��"+attrValueName+"\""+"��"
        	               + to_string(bufAttrElementLimit.GetInt("MONTHS",j))+"�����ڲ��ܰ���\""+tradeTypeName
        	               +"\"������"+attrReleaseDate+"֮����������";
        	break;
        }

    	//��ȡЭ������ϲ�Ʒ(����Э���ȡ��ϲ�Ʒ)֮�����ƹ�ϵ
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
	 					strInfo += "��" + bufAttrElementLimit.GetString("PRODUCT_NAME", y);
	                }
	            }
				string attrName = getAttrName(bufTradeAttr_AllAttr.GetString("ATTR_CODE",i));
        		string attrValueName = "";      	
        		if(bufTradeAttr_AllAttr.GetString("ATTR_CODE",i) == "PROTOCOL_ID")
        			attrValueName = CParamDAO::getParam("ProtocolName",  bufTradeAttr_AllAttr.GetString("ATTR_VALUE", i));

	            if(strErrorInfo == "")
	                strErrorInfo = "\"" +attrName + "��" + attrValueName + "\"������������ϲ�Ʒ��\"" +  strInfo + "\"";
	            else
	                strErrorInfo += "��\"" +attrName + "��" + attrValueName + "\"������������ϲ�Ʒ��\"" +  strInfo + "\"";

	            break;
	        }
    	}

    	//��ȡЭ�����ʷ�(����Э���ȡ�ʷ�)֮�����ƹ�ϵ
    	//�ж�����������Ԫ�أ�LIMIT_TAGΪ1
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
                    //ʱ�佻��
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
                    strInfo += "��" + bufAttrElementLimit.GetString("DISCNT_NAME", y);
                }
            }

			string attrName = getAttrName(bufTradeAttr_AllAttr.GetString("ATTR_CODE",i));
        	string attrValueName = "";      	
        	if(bufTradeAttr_AllAttr.GetString("ATTR_CODE",i) == "PROTOCOL_ID")
        		attrValueName = CParamDAO::getParam("ProtocolName",  bufTradeAttr_AllAttr.GetString("ATTR_VALUE", i));
            
            if(strErrorInfo == "")
                strErrorInfo = "\"" +attrName + "��" + attrValueName + "\"����������Ԫ�أ�\"" +  strInfo + "\"";
            else
                strErrorInfo += "��\"" +attrName + "��" + attrValueName + "\"����������Ԫ�أ�\"" +  strInfo + "\"";

            break;
        }
    }
	if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  "����\"" + bufTrade.GetString("SERIAL_NUMBER") + "\":" + strErrorInfo + "��ҵ���޷�������");

    //��ȡЭ������ϲ�Ʒ(������ϲ�Ʒ��ȡЭ��)֮�����ƹ�ϵ
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
                    strInfo = "\"" +attrName + "��" +  attrValueName;
                }
                else
                {
                    strInfo += "��\"" +attrName + "��" +  attrValueName;
                }
            }
            string compProductName = CParamDAO::getParam("ProductName",  bufTradeComp.GetString("PRODUCT_ID", i));
            if(strErrorInfo == "")
                strErrorInfo = "��ϲ�Ʒ��\"" + compProductName
                    + "\"����������Ԫ�أ�\"" +  strInfo + "\"";
            else
                strErrorInfo += "����ϲ�Ʒ��\"" + compProductName
                    + "\"����������Ԫ�أ�\"" +  strInfo + "\"";

            break;
        }
	}
    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  "����\"" + bufTrade.GetString("SERIAL_NUMBER") + "\":" + strErrorInfo + "��ҵ���޷�������");

 	//��ȡЭ�����ʷ�(�����ʷѻ�ȡЭ��)֮�����ƹ�ϵ��
 	//modfiy by lirui �ж�Ԫ�����������ԣ�LIMIT_TAGΪ2
 	//�Ƿ�У���û�����Ԫ���ж�, �������TAG:CS_ONLY_ADDELM_CHANGE����TAG_CHARΪ1,��ֻ�ж�������Ԫ��,�����ж�����
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
        THROW_AGC(CRMException, e, Grade::WARNING, 8888, "��ȡ�Ƿ�ֻ�ж�����Ԫ�ر�ʶ�쳣��");
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
		    THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ����ҵ��������������Żݳ���[%s]",ex.getCause()->what());
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
                    //ʱ�佻��
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
                    strInfo = "\"" +attrName + "��" +  attrValueName;
                }
                else
                {
                    strInfo += "��\"" +attrName + "��" +  attrValueName;
                }
            }

            if(strErrorInfo == "")
                strErrorInfo = "\"" + bufTradeDiscnt_AllDiscnt.GetString("PRODUCT_NAME", i)
                    + " --> " + bufTradeDiscnt_AllDiscnt.GetString("PACKAGE_NAME", i)
                    + " --> " + bufTradeDiscnt_AllDiscnt.GetString("DISCNT_NAME", i)
                    + "\"����������Ԫ�أ�\"" +  strInfo + "\"";
            else
                strErrorInfo += "��\"" + bufTradeDiscnt_AllDiscnt.GetString("PRODUCT_NAME", i)
                    + " --> " + bufTradeDiscnt_AllDiscnt.GetString("PACKAGE_NAME", i)
                    + " --> " + bufTradeDiscnt_AllDiscnt.GetString("DISCNT_NAME", i)
                    + "\"����������Ԫ�أ�\"" +  strInfo + "\"";

            break;
        }
    }
    if(strErrorInfo != "")
        THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR,  "����\"" + bufTrade.GetString("SERIAL_NUMBER") + "\":" + strErrorInfo + "��ҵ���޷�������");
    //�ж��û�Э�����ʷѡ���ϲ�Ʒ��ȫ������ϵ end

	LOG_TRACE(logTradeCheckAfterTrade, "�˳�CheckAttrElementLimit����");
}

//��ȡ����ҵ�����û�����������Ϣ
int TradeCheckAfterTrade::GetAllAttrAfterTrade(const string &strTradeId, const string &strUserId, CFmlBuf &bufTradeAttr_AllAttr)
{
	LOG_TRACE(logTradeCheckAfterTrade, "����GetAllAttrAfterTrade����");
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ�û������û�������Ϣ����[%s]",ex.getCause()->what());
    }

    bufTradeAttr_AllAttr.SetInt("X_RECORDNUM", iCount);

    return iCount;
}

//��ȡЭ����ʷѡ���ϲ�Ʒ֮���ϵ(����Э���ȡ�ʷѡ���ϲ�Ʒ)
int TradeCheckAfterTrade::ExistsAttrElementLimitA(const string &strAttrCode, const string &strAttrValue,const string &strIdTypeCode,
                                              const string &strLimitTag, const string strEparchyCode,
                                              CFmlBuf &bufAttrElementLimit)
{
    bufAttrElementLimit.ClearFmlValue();
    StaticDAO& dao = StaticDAO::getDAO();
    int iCount = 0;
    LOG_TRACE(logTradeCheckAfterTrade, "����ExistsAttrElementLimitA����");
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡЭ����ʷ�(����Э���ȡ�ʷ�)���ƹ�ϵ��Ϣ����[%s]",ex.getCause()->what());
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

//��ȡЭ����ʷѡ���ϲ�Ʒ֮���ϵ(�����ʷѡ���ϲ�Ʒ��ȡЭ��)
int TradeCheckAfterTrade::ExistsAttrElementLimitB(const string &strElementId, const string &strIdTypeCode,
                                              const string &strLimitTag, const string strEparchyCode,
                                              CFmlBuf &bufAttrElementLimit)
{
    bufAttrElementLimit.ClearFmlValue();
    StaticDAO& dao = StaticDAO::getDAO();
    int iCount = 0;
    LOG_TRACE(logTradeCheckAfterTrade, "����ExistsAttrElementLimitB����");
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡЭ����ʷ�(�����ʷѻ�ȡЭ��)���ƹ�ϵ��Ϣ����[%s]",ex.getCause()->what());
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

//��ȡ��������
string TradeCheckAfterTrade::getAttrName(const string &strAttrCode)
{
    CFmlBuf bufAttrName;
    StaticDAO& dao = StaticDAO::getDAO();
    int iCount = 0;
    string strAttrName = "";
    LOG_TRACE(logTradeCheckAfterTrade, "����getAttrName����");
    try
    {
        dao.Clear();
        dao.SetParam(":VATTR_CODE", strAttrCode);
        iCount = dao.jselect(bufAttrName, "SQL_ITEMS_INFO", "TD_B_ITEMS");
    }
    catch (Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡЭ����ʷ�(�����ʷѻ�ȡЭ��)���ƹ�ϵ��Ϣ����[%s]",ex.getCause()->what());
    }
    if(iCount > 0)
    	strAttrName = bufAttrName.GetString("ATTR_NAME");
    else
    	strAttrName = "Ԫ��";
    	
    return strAttrName;
}


//��ȡ����ֵ���� add by lirui@20120213
string TradeCheckAfterTrade::getAttrValueName(const string &strAttrCode,const string &strAttrValueCode)
{
	LOG_TRACE(logTradeCheckAfterTrade, "����getAttrValueName����");
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
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡЭ����ʷ�(�����ʷѻ�ȡЭ��)���ƹ�ϵ��Ϣ����[%s]",ex.getCause()->what());
    }
    if(iCount > 0)
    	strAttrValueName = bufAttrName.GetString("ENUM_FIELD_NAME");
    else
    	strAttrValueName = "";
    	
    LOG_TRACE(logTradeCheckAfterTrade, "�˳�getAttrValueName����");
    return strAttrValueName;
}

