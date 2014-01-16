#include "stdafx.h"
#include "TradeCheckBeforeTrade.h"
#include "../commondao/StaticDAO.h"
#include "../log4cpp/Logger.h"
#include "../commondao/DualDAO.h"
#include "../common/DualMgr.h"
#include "../UniCommonMgr/UniTradeDao.h"
#include "../UniCommonMgr/UniDataMgr.h"
#include "../common/StrTable.h"
#include "../UniCommonMgr/Utility.h"
#include "../commondao/CParamDAO.h"

using namespace MiscDom;
using namespace log4cpp;
static Logger& logTradeCheckBeforeTrade = Logger::getLogger("gboss.crm.custserv.TradeMgr.TradeCheckBeforeTrade");

//�ú���������ǰ��,���ڲ����ã�������
void getTradeTypeInfo(string strNetTypeCode,string strEparchyCode,string strTradeTypeCode,CFmlBuf &bufTradetype)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "���� getTradeTypeInfo");
	StaticDAO& dao = StaticDAO::getDAO();
    int count=0;
    if(strTradeTypeCode!="-1")
    {
        try
        {
            dao.Clear();
            dao.SetParam(":VEPARCHY_CODE",strEparchyCode);
            dao.SetParam(":VTRADE_TYPE_CODE",strTradeTypeCode);
            dao.SetParam(":VNET_TYPE_CODE",strNetTypeCode);  
            count = dao.jselect(bufTradetype,"SEL_BY_PK_NETTYPE","TD_S_TRADETYPE");  //�Ѿ������Ҳ�����¼�������ж�
        }
        catch (Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "getTradeTypeInfo:��ѯҵ�����Ͳ��������");
        }
        if (count == 0)
        {
        	THROW_C_P3(CRMException, CHECKBEFORETRADE_ERR, "ҵ������ǰ�����ж�:δ��ȡ��ҵ������������Ϣ.NetTypeCode=%s,TradeTypeCode=%s,EparchyCode=%s",strNetTypeCode.c_str(),strTradeTypeCode.c_str(),strEparchyCode.c_str());
        }
    }

	LOG_TRACE(logTradeCheckBeforeTrade, "�˳� getTradeTypeInfo");
}

/**
 *  ҵ������ǰ�����ж�:ҵ�����У�飬�����û�״̬��Ƿ�ѡ�ҵ���Ƿ����޵ȼ��
 */
void TradeCheckBeforeTrade::CheckChinagoDelay(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
    LOG_TRACE(logTradeCheckBeforeTrade, "����CheckChinagoDelay����");

    string strTradeId="";
    string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");
    inBuf.SetString("USER_ID", "-1");
    inBuf.SetInt("TRADE_TYPE_CODE",570);
    inBuf.SetString("IN_MODE_CODE", "5");
    inBuf.SetString("FOREGIFT", "0");
    inBuf.SetString("ADVANCE_PAY", "0");
    inBuf.SetString("PROCESS_TAG_SET", "0");  
    
    CheckChinagoDelay(strTradeId,strEparchyCode);
    
    outBuf.SetString("TRADE_ID",strTradeId);

    LOG_TRACE(logTradeCheckBeforeTrade, "�˳�CheckChinagoDelay����");
}

void TradeCheckBeforeTrade::CheckChinagoDelay(string &strTradeId,string &strEparchyCode)
{
	try
    {
        DualMgr objDualMgr;
        string sequenceName = "SEQ_TRADE_ID";
        objDualMgr.GenerateID(strEparchyCode, sequenceName, strTradeId);
    }
    catch(Exception &e)
    {
        THROW_AGC(CRMException, e, Grade::WARNING, GENEOLCOM_ERR, "����ҵ����ˮ��:��ȡҵ����ˮ��ʧ��2:");
    }
	
}
/**
 *  ҵ������ǰ�����ж�:ҵ�����У�飬�����û�״̬��Ƿ�ѡ�ҵ���Ƿ����޵ȼ��
 */
void TradeCheckBeforeTrade::CheckBeforeTrade(CFmlBuf &inBuf, CFmlBuf &outBuf)//ubss
{
	LOG_TRACE(logTradeCheckBeforeTrade, "���� CheckBeforeTrade ����");

    //0.����Check���صĳ�ʼֵ Ĭ�ϼ��ͨ��
    int ierrFag = 0;
    outBuf.SetInt("X_CHECK_TAG", 0);
    outBuf.SetString("X_CHECK_INFO","");
    StrTable stbOutPrompt;
    stbOutPrompt.AddTable("Prmp", 1);
            
    string strIdType = "1"; //����Ĭ��IDΪ�û���ʶ
    if (inBuf.IsFldExist("ID_TYPE")) strIdType = inBuf.GetString("ID_TYPE");

	string strId=inBuf.GetString("ID");
	string strNetTypeCode;
	if (inBuf.IsFldExist("NET_TYPE_CODE"))
	    strNetTypeCode = inBuf.GetString("NET_TYPE_CODE");
	
    bool hasCustInfo = false;
    bool hasUserInfo = false;
    //bool hasUserPurchaseInfo = false;
        
    //��ȡ�û� or �ͻ���Ϣ
    int iCount=0;
    iCount=TradeCheck_GetCustUserInfoIntoBuf(inBuf,outBuf);
    if (strIdType=="1")	hasUserInfo = iCount >0;
    if (strIdType=="0")	hasCustInfo = iCount >0;
    
    if (strIdType=="1" && !hasUserInfo)//add by tz@2008-1-23 16:12
        outBuf.SetString("BRAND_CODE","");
    
    if (strNetTypeCode !="")
        outBuf.SetString("NET_TYPE_CODE",strNetTypeCode);
    
    //�������þ����Ƿ���Ҫ�ж�Ա��������Ȩ��
    int iNeedCheckOptrDataRight=0;//1:check data right,0:not check data right
    iNeedCheckOptrDataRight=TradeCheck_NeedCheckOptrDataRight(inBuf,outBuf);
    
    //����Ա��ҵ�����ϵͳ����Ȩ��(���԰���ʲô����Χ��ҵ��)�жϡ��Ƿ���԰���
    if (iNeedCheckOptrDataRight == 1)
    	TradeCheck_CheckStaffDataRight(inBuf,outBuf);	
    
    //�û��������ԤԼ��Ʒ�����
    //�����û���ԤԼ������ҵ��,������ϸ��ʾ(����ʱ�䣺****��**��**�գ��û�ԤԼ��**��Ʒ�ĳ�**��Ʒ��*����Ч��)
	TradeCheck_CheckHintChgProduct(inBuf,outBuf);

	//�û���ǰ�����ҵ�������Ƿ����δ�깤ҵ�������
	TradeCheck_CheckNotCompletedTrade(inBuf,outBuf);
	
	
	//�������ͻ�����ҵ���жϡ��Ƿ������
	if (TradeCheck_CheckBlackUser(inBuf,outBuf)==1)
	{
		ierrFag=1;
        stbOutPrompt.AddField("Prmp", "TradeCheck_CheckBlackUser:�������ͻ���������ֹҵ��İ���\n�Ƿ�Ҫ����ҵ��İ���ѡ��ȷ������������ҵ��ѡ��ȡ������ֹ����ҵ��");
	}
	//�ж��û��Ƿ������
	bool isRedUser=false;
	if (strIdType == "1")   //��������û�
        isRedUser = (inBuf.GetString("REDUSER_TAG") == "1"); //cclib::isRedUser(strId);

	//Ƿ���ж�
	if (TradeCheck_CheckOweFee(inBuf,outBuf)==1)
	{
		ierrFag=1;
		stbOutPrompt.AddField("Prmp", "TradeCheck_CheckOweFee:�û��Ѿ�Ƿ�ѣ�������ֹҵ��İ���\n�Ƿ�Ҫ����ҵ��İ���ѡ��ȷ������������ҵ��ѡ��ȡ������ֹ����ҵ��");
	}
	
	//�ж��û��Ƿ����������ۣ�����ǣ���û�е���Э���ڣ�û�е�Э�����ʱ�䲻�ܰ������ҵ��
	outBuf.SetString("X_CHECK_INFO", "");
	TradeCheck_CheckBindSaleExpDate(inBuf,outBuf);
	string strPrompt = outBuf.GetString("X_CHECK_INFO");
	if ( strPrompt != "")
	{
	    ierrFag=1;
		stbOutPrompt.AddField("Prmp", strPrompt);
	}
	
	//�ж��û��Ƿ��в��ܰ���ҵ��ķ���״̬���Ƿ������
	TradeCheck_SvcstateTradeLimit(inBuf,outBuf);
	
	//�ж��û��Ƿ����ҵ������
	TradeCheck_UserTradelimit(inBuf,outBuf);
	
	//�ж�ҵ���������������ķ����Ƿ�ͨ��
	TradeCheck_CheckTradeTypeCodeDependSVC(inBuf,outBuf);
	
	//�ж��û��Ƿ���ڲ�Ʒҵ�����ơ��Ƿ������
	TradeCheck_ProdTradeLimit(inBuf,outBuf);
	
	//����Ƿ������ذ���ҵ��
	TradeCheck_CheckEparchyCode(inBuf,outBuf);
	
	//�ж������û��Ƿ���԰���
	TradeCheck_CheckPreOpen(inBuf,outBuf);
	
	//У���Ƿ���sp��Ϣ���Լ��Ƿ���԰���
	TradeCheck_CheckUserSP(inBuf,outBuf);//Ŀǰû��sp��Ԥ���ӿ��ڴ�
	
	//�û���Ԥ���Ƿ���԰���ҵ��
	if (TradeCheck_CheckPreDeposit(inBuf,outBuf)==1)
	{
		ierrFag=1;
        stbOutPrompt.AddField("Prmp", "TradeCheck_CheckPreDeposit:���û��н��࣬�Ƿ�Ҫ����ҵ��İ���\nѡ���ǡ���������ҵ��ѡ�񡾷���ֹ����ҵ��");
	}
	
	//��ǰ�û���Ѻ���Ƿ���԰����Ƿ������
    if (TradeCheck_CheckForegift(inBuf,outBuf)==1)
    {
    	ierrFag=1;
    	stbOutPrompt.AddField("Prmp", "TradeCheck_CheckForegift:���û���Ѻ���Ƿ�Ҫ����ҵ��İ���\nѡ���ǡ���������ҵ��ѡ�񡾷���ֹ����ҵ��");
	}
	//��ʼ�����û����ܽ���ҵ������
	TradeCheck_CheckInitPasswd(inBuf,outBuf);
	TradeCheck_CheckPrdAttrLimit(inBuf,outBuf);
	
	//У�����ҵ��ĳ���������(���ƴ���Χ�ӿڴ�����ֵ���)
    TradeCheck_CheckScoreTradeLimit(inBuf,outBuf);
	
    //���ǵ��ӿڵȷ�Ӫҵǰ̨����ʱ,buff��û��FEE1/FEE2/FEE3�ֶΣ��ٴ�Ϊbuff�������Щ�ֶΣ���ֹ������ñ���
    if (!inBuf.IsFldExist("FEE1")) inBuf.SetString("FEE1", "0");
    if (!inBuf.IsFldExist("FEE2")) inBuf.SetString("FEE2", "0");
    if (!inBuf.IsFldExist("FEE3")) inBuf.SetString("FEE3", "0");
    
    //Ӫҵǰ̨����ʱ��buff��û��IN_MODE_CODE�ֶΣ��ڴ�Ϊbuff���������ֶ�
    if (!inBuf.IsFldExist("IN_MODE_CODE")) inBuf.SetString("IN_MODE_CODE", "0");
    
    //�ܽ�
    if ( ierrFag != 0)
    {
        outBuf.SetInt("X_CHECK_TAG", ierrFag);
        string strResult;
        if ( stbOutPrompt.ToString(strResult) != 0)
            THROW_C(CRMException, CHECKBEFORETRADE_ERR, "ҵ������ǰ�����ж�:�����ִ������");
        outBuf.SetString("X_CHECK_INFO", strResult);
    }
	LOG_TRACE(logTradeCheckBeforeTrade, "�˳� CheckBeforeTrade ����");
} 
int TradeCheckBeforeTrade::TradeCheck_GetCustUserInfoIntoBuf(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "���� TradeCheck_GetCustUserInfoIntoBuf");

	if (!inBuf.IsFldExist("ID"))
	{
		THROW_C(CRMException, CHECKAFTERTRADE_ERR, "TradeCheck_GetCustUserInfo:������δ���� ID ��(�û���ͻ���ʶ)��");
	}

    string strId = inBuf.GetString("ID");
    string strIdType = "1"; //����Ĭ��IDΪ�û���ʶ
    if (inBuf.IsFldExist("ID_TYPE"))
        strIdType = inBuf.GetString("ID_TYPE");	
    int iRowCount=0;
    
    bool bPutIntoBuf=true;//�Ƿ񽫲�ѯ����������д��������
    
    string strNetType = inBuf.GetString("NET_TYPE_CODE");
    
    StaticDAO& dao = StaticDAO::getDAO();
    if (strIdType == "0")   //��ȡ�ͻ�����
    {
		CFmlBuf bufCust;
        try
        {
            dao.Clear();
            dao.SetParam(":VCUST_ID", strId);
            iRowCount = dao.jselect(bufCust, "SEL_BY_PK","TF_F_CUSTOMER");
            if (bPutIntoBuf)
            	outBuf.CoverLine(0,bufCust,0);
            if(iRowCount==0)
            	outBuf.SetString("NET_TYPE_CODE",strNetType);
        }
        catch (Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "TradeCheck_GetCustUserInfo:��ȡ�ͻ��������ϳ���");
        }
    }
    else if (strIdType == "1")    //��ȡ�û�����
    {
        CFmlBuf bufUser;
        try
        {
            dao.Clear();
            dao.SetParam(":VUSER_ID", strId);
            iRowCount = dao.jselect(bufUser, "SEL_BY_PK","TF_F_USER") > 0;
            if (bPutIntoBuf)
	            outBuf.CoverLine(0,bufUser,0);
	        if(iRowCount==0)
            	outBuf.SetString("NET_TYPE_CODE",strNetType);
        }
        catch (Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "TradeCheck_GetCustUserInfo:��ȡ�û��������ϳ���");
        }
    }
    LOG_TRACE(logTradeCheckBeforeTrade, "�˳� TradeCheck_GetCustUserInfoIntoBuf");
    return iRowCount;
}
void TradeCheckBeforeTrade::TradeCheck_CheckTradeTypeCodeDependSVC(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "���� TradeCheck_CheckTradeTypeCodeDependSVC ����");
	
    string strIdType = "0";
    if (inBuf.IsFldExist("ID_TYPE"))
        strIdType = inBuf.GetString("ID_TYPE");
    if (strIdType != "1")
    	return;
	string strTradeTypeCode = inBuf.GetString("TRADE_TYPE_CODE");
 	string strId = inBuf.GetString("ID");
	string strNetTypeCode = inBuf.GetString("NET_TYPE_CODE");
	string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");
	
	StaticDAO& dao = StaticDAO::getDAO();
 	CFmlBuf iBuf;
 	int iRowCount = 0;
	try
	{
		dao.Clear();
		iBuf.ClearFmlValue();
		dao.SetParam(":VPARAM_ATTR",3350);
		dao.SetParam(":VSUBSYS_CODE","CSM");
		dao.SetParam(":VNET_TYPE_CODE",strNetTypeCode);
		dao.SetParam(":VTRADE_TYPE_CODE",strTradeTypeCode);
		dao.SetParam(":VEPARCHY_CODE",strEparchyCode);
		
		iRowCount = dao.jselect(iBuf,"SEL_TRADETYPECODE_DEPEND_SERVICEID","TD_S_COMMPARA");		
	}
    catch (Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "��ѯҵ�������ķ������");
    }
	if (iRowCount == 0)
		return ;
	string strServiceID;
	for (int i = 0; i<iRowCount; i++)
	{
		strServiceID=iBuf.GetString("SERVICE_ID",i);
		try
		{
			dao.Clear();
			iBuf.ClearFmlValue();
			dao.SetParam(":VUSER_ID",strId);
			dao.SetParam(":VSERVICE_ID",strServiceID);
			
			dao.jselect(iBuf,"SEL_SVC_COUNT","TF_F_USER_SVC");		
		}
	    catch (Exception &ex)
	    {
	        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "��ѯ�û��Ƿ�ͨ�˷���[%s]����",strServiceID.c_str());
	    }
		if (atoi(iBuf.GetString("ROW_COUNT").c_str()) == 0)
		{
			string strServiceName = CParamDAO::getParam("ServiceName", strServiceID.c_str());
			THROW_C_P1(CRMException, CHECKBEFORETRADE_ERR, "�����û���ͨ�˷���[%s]���ٰ����ҵ��!",	strServiceName.c_str());
		}
	}
	LOG_TRACE(logTradeCheckBeforeTrade, "�˳� TradeCheck_CheckTradeTypeCodeDependSVC ����");
}
void TradeCheckBeforeTrade::TradeCheck_CheckBindSaleExpDate(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
    LOG_TRACE(logTradeCheckBeforeTrade, "���� TradeCheck_CheckBindSaleExpDate");
	  
	  //Modify by wuzy@2007-07-31  begin
	  string strTradeTypeCode = inBuf.GetString("TRADE_TYPE_CODE");
	  //��Ʒ���:110,ԤԼ����:190,��������:192,����:100,�Żݱ��:150
	  if (strTradeTypeCode != "110" && strTradeTypeCode != "190" && strTradeTypeCode != "192"
	  &&  strTradeTypeCode != "100" && strTradeTypeCode != "150" )
	  {
	  	  return;	
	  }

    string strIdType = "0";
    if (inBuf.IsFldExist("ID_TYPE"))
        strIdType = inBuf.GetString("ID_TYPE"); 
        
    if (strIdType != "1")
    	  return;

 	  string strId = inBuf.GetString("ID");
	  //string strInNetMode = inBuf.GetString("IN_NET_MODE");

 	  StaticDAO& dao = StaticDAO::getDAO();
 	  CFmlBuf iBuf;
 	  int iRowCount = 0;
 	  string strSysDate,strProcessTag,strEndDate,strPEndDate;
 	  
 	  try
	  {
	  	  dao.Clear();
	  	  iBuf.ClearFmlValue();
	  	  dao.SetParam(":VUSER_ID",strId);
	  	  iRowCount = dao.jselect(iBuf,"SEL_BINDSALE_EXPDATE_INFO","TF_F_USER_PURCHASE");
	  }
	  catch (Exception &ex)
	  {
	      THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "��ѯ�û�����������Ϣ����");
	  }
	   
	  if (iRowCount > 0)
	  {
	  	  for (int i=0;i<iRowCount;i++)
	  	  {
	  	  	  strSysDate    = iBuf.GetString("X_SYSDATE"   ,i);
	  	  	  strEndDate    = iBuf.GetString("END_DATE"    ,i);
	  	  	  strProcessTag = iBuf.GetString("PROCESS_TAG" ,i);
	  	  	  strPEndDate   = iBuf.GetString("P_END_DATE"  ,i);//��ǰһ����
	  	  	  
	  	  	  LOG_TRACE_P1(logTradeCheckBeforeTrade, "strId=%s",strId.c_str());
	  	  	  LOG_TRACE_P1(logTradeCheckBeforeTrade, "strTradeTypeCode=%s",strTradeTypeCode.c_str());
	  	  	  LOG_TRACE_P1(logTradeCheckBeforeTrade, "strSysDate=%s",strSysDate.c_str());
	  	  	  LOG_TRACE_P1(logTradeCheckBeforeTrade, "strEndDate=%s",strEndDate.c_str());
	  	  	  LOG_TRACE_P1(logTradeCheckBeforeTrade, "strProcessTag=%s",strProcessTag.c_str());
	  	  	  LOG_TRACE_P1(logTradeCheckBeforeTrade, "strPEndDate=%s",strPEndDate.c_str());
        
	  	  	  if ((strTradeTypeCode != "110" && strEndDate > strSysDate)
	  	  	      ||(strTradeTypeCode == "110" && strPEndDate > strSysDate))
	  	  	  {
	  	  	  	  //����λ��1��Э�����ڲ��ܱ��Ʒ         2:ѯ���Ƿ����
	  	  	  	  //����λ��1��Э�����ڲ�����ԤԼ����     2:ѯ���Ƿ����
	  	  	  	  //����λ��1��Э�����ڲ�������������     2:ѯ���Ƿ����
	  	  	  	  //����λ��1��Э�����ڲ���������         2:ѯ���Ƿ����
	  	  	  	  //����λ��1��Э�����ڲ��ܱ��Ż�         2:ѯ���Ƿ����
	  	  	  	  //��Ʒ���:110,ԤԼ����:190,��������:192,����:100,�Żݱ��:150
	  	  	  	  char chPromptMode;
	  	  	  	  if ((strTradeTypeCode == "110" && (chPromptMode=strProcessTag[3 - 1]) != '0')
	  	  	  	       ||(strTradeTypeCode == "190" && (chPromptMode=strProcessTag[4 - 1]) != '0')
	  	  	  	       ||(strTradeTypeCode == "192" && (chPromptMode=strProcessTag[5 - 1]) != '0')
	  	  	  	       ||(strTradeTypeCode == "100" && (chPromptMode=strProcessTag[6 - 1]) != '0')
	  	  	  	       ||(strTradeTypeCode == "150" && (chPromptMode=strProcessTag[7 - 1]) != '0'))
	  	  	  	  {
	  	  	  	      CFmlBuf bufTradetype;
	  	  	  	      string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");
	  	  	  	      string strNetTypeCode = inBuf.GetString("NET_TYPE_CODE");
	  	  	  	  	  getTradeTypeInfo(strNetTypeCode,strEparchyCode,strTradeTypeCode,bufTradetype);
	  	  	  	  	  if (chPromptMode == '2')  //ѯ��
	  	  	  	  	  {
	  	  	  	  	      outBuf.SetString("X_CHECK_INFO", "�û�������Э����["+((strTradeTypeCode=="110") ? strPEndDate: strEndDate)+"]�ڣ��Ƿ��������["+string(bufTradetype.GetString("TRADE_TYPE"))+"]ҵ��");
	  	  	  	  	  }
	  	  	  	  	  else
	  	  	  	  	  {
	  	  	  	  	      THROW_C_P2(CRMException, CHECKBEFORETRADE_ERR, "�û�������Э�����ڣ����ܰ���[%s]ҵ��\r\n��������[%s]֮���ٰ���!",
	  	  	  	  	      	string(bufTradetype.GetString("TRADE_TYPE")).c_str(),(strTradeTypeCode=="110") ? strPEndDate.c_str() : strEndDate.c_str());
	  	  	  	  	  }
	  	  	  	  }
	  	  	  }
	  	  } 
	  } 
	  //Modify by wuzy@2007-07-31  end 
    LOG_TRACE(logTradeCheckBeforeTrade, "�˳� TradeCheck_CheckBindSaleExpDate ����");
}

int TradeCheckBeforeTrade::TradeCheck_CheckForegift(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "���� TradeCheck_CheckForegift ����");
	
    string strIdType = "1"; //����Ĭ��IDΪ�û���ʶ
    if (inBuf.IsFldExist("ID_TYPE"))
        strIdType = inBuf.GetString("ID_TYPE");
        
    CFmlBuf bufTradetype;
    string strTradeTypeCode = inBuf.GetString("TRADE_TYPE_CODE");
    string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");
    string strNetTypeCode = inBuf.GetString("NET_TYPE_CODE");
    	
	getTradeTypeInfo(strNetTypeCode,strEparchyCode,strTradeTypeCode,bufTradetype);
	
	string strId = inBuf.GetString("ID");
	int ierrFag=0;	
	StaticDAO& dao = StaticDAO::getDAO();
    if (strIdType == "1" && bufTradetype.GetString("RTN_FOREGIFT_TAG") != "" && bufTradetype.GetString("RTN_FOREGIFT_TAG") != "0")
    {
        try
        {
            CFmlBuf bufRecordNumber;
            dao.Clear();
            dao.SetParam(":VUSER_ID", strId);
            dao.jselect(bufRecordNumber,"SEL_SUM_OF_USER_FOREGIFT","TD_S_CPARAM");
            if (bufRecordNumber.GetInt("RECORDCOUNT") > 0)
            {
                if (bufTradetype.GetString("RTN_FOREGIFT_TAG") == "1")
                {
                    THROW_C(CRMException, CHECKBEFORETRADE_ERR, "TradeCheck_CheckForegift:���û���Ѻ�𣬲��ܰ����ҵ��");
                }
                else
                {
                	//add by chenzm@2006-11-30(������Ա��Ϊ2����ʾ̫���ˣ����Ը�Ϊһ��)
				    int iChoiceTag=1;
				    if (inBuf.IsFldExist("X_CHOICE_TAG"))
				    	iChoiceTag = inBuf.GetInt("X_CHOICE_TAG");
                	
                    if (iChoiceTag == 1)//1-ǰ̨���ȷ��ʱ���� , 0-����ҵ������س��¼��н���
                    	ierrFag = 1;
                    else
                    	ierrFag = 0;	
                }
            }
        }
        catch (CRMException &ex) {throw;}
        catch (Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "TradeCheck_CheckForegift:��ѯ�û�SP��Ϣ����");
        }
    }
	
	LOG_TRACE(logTradeCheckBeforeTrade, "�˳� TradeCheck_CheckForegift ����");
	return ierrFag;
}
int TradeCheckBeforeTrade::TradeCheck_CheckPreDeposit(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "���� TradeCheck_CheckPreDeposit ����");

    string strIdType = "1"; //����Ĭ��IDΪ�û���ʶ
    if (inBuf.IsFldExist("ID_TYPE"))
        strIdType = inBuf.GetString("ID_TYPE");
        
    CFmlBuf bufTradetype;
    string strTradeTypeCode = inBuf.GetString("TRADE_TYPE_CODE");
    string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");
    
    string strNetTypeCode = inBuf.GetString("NET_TYPE_CODE");
    
     	
	getTradeTypeInfo(strNetTypeCode,strEparchyCode,strTradeTypeCode,bufTradetype);
    	
	
    string strFee = "0";
    if (inBuf.IsFldExist("FEE"))
        strFee = inBuf.GetString("FEE");//�û�Ƿ��
    else
        strFee = inBuf.GetString("LEAVE_REAL_FEE");
	int ierrFag=0;
	//Modify by Yangf@2009-5-1:RTN_FOREGIFT_TAG->RTN_DEPOSIT_TAG
    if (strIdType == "1" && bufTradetype.GetString("RTN_DEPOSIT_TAG") != "" && bufTradetype.GetString("RTN_DEPOSIT_TAG") != "0")
    {
        if (atol(strFee.c_str()) > 0)
        {
            if (bufTradetype.GetString("RTN_FOREGIFT_TAG") == "1")
            {
                THROW_C(CRMException, CHECKBEFORETRADE_ERR, "TradeCheck_CheckPreDeposit:���û���Ԥ�棬���Ȱ���Ԥ�����ˣ�");
            }
            else
            {
            	//add by chenzm@2006-11-30(������Ա��Ϊ2����ʾ̫���ˣ����Ը�Ϊһ��)
			    int iChoiceTag=1;
			    if (inBuf.IsFldExist("X_CHOICE_TAG"))
			    	iChoiceTag = inBuf.GetInt("X_CHOICE_TAG");
            	
                if (iChoiceTag == 1)//1-ǰ̨���ȷ��ʱ���� , 0-����ҵ������س��¼��н���
                	ierrFag = 1;
                else
                	ierrFag = 0;	
            }
        }
    }
	
	LOG_TRACE(logTradeCheckBeforeTrade, "�˳� TradeCheck_CheckPreDeposit ����");
	return ierrFag;
}
void TradeCheckBeforeTrade::TradeCheck_CheckUserSP(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	//LOG_TRACE(logTradeCheckBeforeTrade, "���� TradeCheck_CheckUserSP ����");
	
	//LOG_TRACE(logTradeCheckBeforeTrade, "�˳� TradeCheck_CheckUserSP ����");
}
void TradeCheckBeforeTrade::TradeCheck_CheckPreOpen(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "�˳� TradeCheck_CheckPreOpen ����");
	
	string strIdType = "1"; //����Ĭ��IDΪ�û���ʶ
    if (inBuf.IsFldExist("ID_TYPE"))
        strIdType = inBuf.GetString("ID_TYPE");
    
    CFmlBuf bufTradetype;
    string strTradeTypeCode = inBuf.GetString("TRADE_TYPE_CODE");
    string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");
    	
     string strNetTypeCode = inBuf.GetString("NET_TYPE_CODE");
     
     string strInModeCode = "Z" ;
     if (inBuf.IsFldExist("IN_MODE_CODE"))
        strInModeCode = inBuf.GetString("IN_MODE_CODE");
    	
	getTradeTypeInfo(strNetTypeCode,strEparchyCode,strTradeTypeCode,bufTradetype);
		
	
    if(strIdType=="1" && outBuf.IsFldExist("USER_ID"))
    {
        //add by digy@20070828 ֻ��Ӫҵ�������ķ������ж��Ƿ�δ�����û� 
        if( (strTradeTypeCode != "120" && bufTradetype.GetString("PREOPEN_LIMIT_TAG")=="1" && outBuf.GetString("OPEN_MODE")=="1")||
            (strTradeTypeCode =="120" && strInModeCode =="0" && bufTradetype.GetString("PREOPEN_LIMIT_TAG")=="1" && outBuf.GetString("OPEN_MODE")=="1")
          )
        {
            THROW_C(CRMException, CS_USERSTATE_ERR, "TradeCheck_CheckPreOpen:���û���Ԥ��δ�����û������ܰ����ҵ��");
        }
    }
	
	LOG_TRACE(logTradeCheckBeforeTrade, "�˳� TradeCheck_CheckPreOpen ����");
}
void TradeCheckBeforeTrade::TradeCheck_CheckEparchyCode(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "�˳� TradeCheck_CheckEparchyCode ����");
	
	CFmlBuf bufTradetype;
    string strTradeTypeCode = inBuf.GetString("TRADE_TYPE_CODE");
    string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");
    	
	 string strNetTypeCode = inBuf.GetString("NET_TYPE_CODE");
    
    
    	
	getTradeTypeInfo(strNetTypeCode,strEparchyCode,strTradeTypeCode,bufTradetype);

    string strId = inBuf.GetString("ID");
    string strIdType = "1"; //����Ĭ��IDΪ�û���ʶ
    if (inBuf.IsFldExist("ID_TYPE"))
        strIdType = inBuf.GetString("ID_TYPE");

    if (bufTradetype.GetString("EXTEND_TAG") == "0")
    {
        //�жϿͻ������Ƿ��ڰ������ҵ��
        if (strIdType == "0" && outBuf.IsFldExist("PSPT_ID") )
        {
        	if (outBuf.GetString("EPARCHY_CODE") != strEparchyCode)
            {
                THROW_C(CRMException, CS_OTHERAREATRADE_ERR, "TradeCheck_CheckEparchyCode:�ͻ�������ذ����ҵ��");
            }
        }
        //�ж��û������Ƿ��ڰ������ҵ��
        else if (strIdType == "1" && outBuf.IsFldExist("USER_ID"))
        {
        	if (outBuf.GetString("EPARCHY_CODE") != strEparchyCode)
            {
                //abc���޸�1
                ///if((string(outBuf.GetString("USER_ID")).substr(0,2)!="99" &&outBuf.GetString("REMOVE_TAG") != "")&&
                  //  !((string(outBuf.GetString("BRAND_CODE")).substr(0,2))=="VP"&&(outBuf.GetString("RSRV_STR2")=="1"||outBuf.GetString("RSRV_STR2")=="2")))
                {
                    THROW_C(CRMException, CS_OTHERAREATRADE_ERR, "TradeCheck_CheckEparchyCode:�û�������ذ����ҵ��");
                }
            }
        }
    }
	
	LOG_TRACE(logTradeCheckBeforeTrade, "�˳� TradeCheck_CheckEparchyCode ����");	
}

void TradeCheckBeforeTrade::TradeCheck_ProdTradeLimit(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "���� TradeCheck_ProdTradeLimit ����");
    
    string strId = inBuf.GetString("ID");
    string strIdType = "1"; //����Ĭ��IDΪ�û���ʶ
    if (inBuf.IsFldExist("ID_TYPE"))
        strIdType = inBuf.GetString("ID_TYPE");

    string strTradeTypeCode = inBuf.GetString("TRADE_TYPE_CODE");
    string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");
    string strUserTagSet = inBuf.IsFldExist("USER_TAG_SET")?(inBuf.GetString("USER_TAG_SET")):(string)"";  
    string strProductId = inBuf.IsFldExist("PRODUCT_ID")?(inBuf.GetString("PRODUCT_ID")):(string)"-1";  
    
    StaticDAO& dao = StaticDAO::getDAO();	
    if (strIdType == "1")
    {
        CFmlBuf bufVProdTradeLimit;
        try
        {
            dao.Clear();
            dao.SetParam(":VUSER_ID", strId);
            dao.SetParam(":VEPARCHY_CODE",strEparchyCode);
            dao.SetParam(":VTRADE_TYPE_CODE",strTradeTypeCode);
            dao.SetParam(":VPRODUCT_ID",strProductId);
            if (dao.jselect(bufVProdTradeLimit, "SEL_PRODLIMIT_BY_USERID","TD_B_PROD_TRADE_LIMIT") > 0)
            {
                char Id[10];
                sprintf(Id,"%d",(int)bufVProdTradeLimit.GetInt("PID"));
                string strName = CParamDAO::getParam("ProductName", Id);
                THROW_C_P1(CRMException, CS_TRADELIMIT_ERR, "TradeCheck_ProdTradeLimit:�û���ǰ��ƷΪ��%s�������ܰ����ҵ��", strName.c_str());
            }
        }
        catch (CRMException &ex) {throw;}
        catch (Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "TradeCheck_ProdTradeLimit:��ѯ�û���Ʒҵ�����Ʊ����");
        }
        
        try
        {
            bufVProdTradeLimit.ClearFmlValue();
            dao.Clear();
            dao.SetParam(":VUSER_ID", strId);
            dao.SetParam(":VEPARCHY_CODE",strEparchyCode);
            dao.SetParam(":VTRADE_TYPE_CODE",strTradeTypeCode);
            if (dao.jselect(bufVProdTradeLimit, "SEL_SVCLIMIT_BY_USERID","TD_B_PROD_TRADE_LIMIT") > 0)
            {
                char Id[10];
                sprintf(Id,"%d",(int)bufVProdTradeLimit.GetInt("PID"));
                string strName = CParamDAO::getParam("ServiceName", Id);
                THROW_C_P1(CRMException, CS_TRADELIMIT_ERR, "TradeCheck_ProdTradeLimit:�û���ǰ���з���%s�������ܰ����ҵ��", strName.c_str());
            }
        }
        catch (CRMException &ex) {throw;}
        catch (Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "TradeCheck_ProdTradeLimit:��ѯ�û���Ʒҵ�����Ʊ����");
        }
        
        try
        {
            bufVProdTradeLimit.ClearFmlValue();
            dao.Clear();
            dao.SetParam(":VUSER_ID", strId);
            dao.SetParam(":VEPARCHY_CODE",strEparchyCode);
            dao.SetParam(":VTRADE_TYPE_CODE",strTradeTypeCode);
            if (dao.jselect(bufVProdTradeLimit, "SEL_DISCNTLIMIT_BY_USERID","TD_B_PROD_TRADE_LIMIT") > 0)
            {
                char Id[10];
                sprintf(Id,"%d",(int)bufVProdTradeLimit.GetInt("PID"));
                string strName = CParamDAO::getParam("DiscntName", Id);
                THROW_C_P1(CRMException, CS_TRADELIMIT_ERR, "TradeCheck_ProdTradeLimit:�û���ǰ�����Żݡ�%s�������ܰ����ҵ��", strName.c_str());
            }
        }
        catch (CRMException &ex) {throw;}
        catch (Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "TradeCheck_ProdTradeLimit:��ѯ�û���Ʒҵ�����Ʊ����");
        }
        if (strTradeTypeCode=="110"&&strUserTagSet.length()>1&&strUserTagSet.substr(1,1)=="1")
        {
            THROW_C(CRMException, CS_TRADELIMIT_ERR, "TradeCheck_ProdTradeLimit:���û���ֹ�޸Ĳ�Ʒ��");
        }
        string strInModeCode = inBuf.IsFldExist("IN_MODE_CODE")?inBuf.GetString("IN_MODE_CODE"):"";
        string strNetTypeCode = inBuf.IsFldExist("NET_TYPE_CODE")?inBuf.GetString("NET_TYPE_CODE"):"";

        if(strInModeCode !="0" || strNetTypeCode == "WV"){
            ;
        }else {
            try
	          {
	              bufVProdTradeLimit.ClearFmlValue();
	              dao.Clear();
	              dao.SetParam(":VPRODUCT_ID",strProductId);
	              if (dao.jselect(bufVProdTradeLimit, "SEL_BY_PK","TD_B_PRODUCT") > 0)
	              {
	                  if(bufVProdTradeLimit.GetString("PRODUCT_OBJ_TYPE") != "1")
	                  {
	                  	THROW_C(CRMException, CS_TRADELIMIT_ERR, "TradeCheck_ProdTradeLimit:�û���Ҫ��ESSϵͳ�����ҵ��");
	                  } 
	              }
	          }
	          catch (Exception &ex)
	          {
	              THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "TradeCheck_ProdTradeLimit:��ѯ��Ʒ�����");
	          }
	      }
    }
	
	LOG_TRACE(logTradeCheckBeforeTrade, "�˳� TradeCheck_ProdTradeLimit ����");	
}
void TradeCheckBeforeTrade::TradeCheck_CheckNotCompletedTrade(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "���� TradeCheck_CheckNotCompletedTrade");

    if (!inBuf.IsFldExist("ID"))
	{
		THROW_C(CRMException, CHECKAFTERTRADE_ERR, "TradeCheck_CheckNotCompletedTrade:������δ���� ID ��(�û���ʶ)��");
	}

    string strIdType = "1"; //����Ĭ��IDΪ�û���ʶ
    if (inBuf.IsFldExist("ID_TYPE")) strIdType = inBuf.GetString("ID_TYPE");
	
	string strId = inBuf.GetString("ID");	
	string strTradeTypeCode = inBuf.GetString("TRADE_TYPE_CODE");
    string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");
    string strStaffId = inBuf.GetString("TRADE_STAFF_ID");
    string strCityCode = inBuf.GetString("TRADE_CITY_CODE");
    string strBrandCode = outBuf.GetString("BRAND_CODE");
    string serialNumber = inBuf.IsFldExist("SERIAL_NUMBER")?inBuf.GetString("SERIAL_NUMBER"):"";

    StaticDAO& dao = StaticDAO::getDAO();
    if (strIdType == "1")
    {
	    CFmlBuf  bufVTradetypeLimit;
	    try
	    {
	        dao.Clear();
	        dao.SetParam(":VTRADE_TYPE_CODE", strTradeTypeCode);
	        dao.SetParam(":VUSER_ID", strId);
	        dao.SetParam(":VBRAND_CODE", strBrandCode);
	        dao.SetParam(":VLIMIT_ATTR", "0");
	        dao.SetParam(":VLIMIT_TAG", "0");
	        dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
	        if (dao.jselect(bufVTradetypeLimit, "SEL_EXISTS_LIMIT_TRADETYPECODE","TD_S_TRADETYPE_LIMIT") > 0)
	        {
	            char chIdA[11];
	            //add by chenzm@2006-11-24:ɽ����ͨҪ��������ԤԼ������ԤԼҵ��Ҳ��������
	            if (strTradeTypeCode != "192" && strTradeTypeCode != "190")
	            {
		            //Add by chetq 2006-7-10 begin ����ԤԼҵ����ʾ
		            CFmlBuf bufVTrade;
		            dao.Clear();
		            dao.SetParam(":VUSER_ID",strId);
		            if( dao.jselect(bufVTrade,"SEL_BOOK_BY_USERID","TF_B_TRADE")>0) {
		            	sprintf(chIdA,"%d",(int)bufVTradetypeLimit.GetInt("LIMIT_TRADE_TYPE_CODE"));
		                string strTradeType = CParamDAO::getParam("TradeType", chIdA);
		                THROW_C_P2(CRMException, CHECKBEFORETRADE_ERR, "TradeCheck_CheckNotCompletedTrade:�û���ԤԼ������ҵ��[%s][%s]��", strTradeType.c_str(),serialNumber.c_str());
		            }
	        	}
	            sprintf(chIdA,"%d",(int)bufVTradetypeLimit.GetInt("LIMIT_TRADE_TYPE_CODE"));
	            string strTradeType = CParamDAO::getParam("TradeType", chIdA);
	            THROW_C_P2(CRMException, TRADENOTCOMPLETED_ERR, "TradeCheck_CheckNotCompletedTrade:�û���δ�깤������ҵ��[%s][%s]��", strTradeType.c_str(),serialNumber.c_str());
	        }
	    }
	    catch (CRMException &ex) {throw;}
	    catch (Exception &ex)
	    {
	        THROW_AGC(CRMException,ex,Grade::WARNING,CHECKBEFORETRADE_ERR,"TradeCheck_CheckNotCompletedTrade:��ѯ�û�δ�깤������ҵ����Ϣ����");
	    }
	}
	LOG_TRACE(logTradeCheckBeforeTrade, "�˳� TradeCheck_CheckNotCompletedTrade");
}
void TradeCheckBeforeTrade::TradeCheck_CheckStaffDataRight(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "���� TradeCheck_CheckStaffDataRight");
    
    string strIdType = "1"; //����Ĭ��IDΪ�û���ʶ
    if (inBuf.IsFldExist("ID_TYPE")) strIdType = inBuf.GetString("ID_TYPE");
    if (strIdType!="1")
    	return;
    	
	if (!outBuf.IsFldExist("USER_ID"))
	{
		THROW_C(CRMException, CHECKAFTERTRADE_ERR, "TradeCheck_CheckStaffDataRight:���ȵ���TradeCheck_GetCustUserInfoIntoBuf��ȡ�û��ͻ���Ϣ��");
	}
    string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");
    string strStaffId = inBuf.GetString("TRADE_STAFF_ID");
    string strCityCode = inBuf.GetString("TRADE_CITY_CODE");

    string strEparchyCode_User = outBuf.GetString("EPARCHY_CODE");
    string strCityCode_User = outBuf.GetString("CITY_CODE");

	int iCount=0;
	StaticDAO& dao = StaticDAO::getDAO();
    CFmlBuf bufStaffdataright;
    try
    {
        dao.Clear();
        dao.SetParam(":VSTAFF_ID", strStaffId);
        dao.SetParam(":VDATA_CODE", "SYS005");
        iCount = dao.jselect(bufStaffdataright, "SEL_STAFF_EXISTS_DATACODE","TF_M_STAFFDATARIGHT");
    }
    catch (Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "TradeCheck_CheckStaffDataRight:��ȡԱ��ϵͳ����Ȩ����Ϣ����");
    }
    
    //0-��Ȩ��	1-���� 2-Ӫҵ��	3-ҵ���� 4-����	5-ȫʡ
    string strRightClass = bufStaffdataright.GetString("RIGHT_CLASS");
    if(iCount == 0 || strRightClass == "0" )
    {
        THROW_C(CRMException, CHECKBEFORETRADE_ERR, "TradeCheck_CheckStaffDataRight:��Ȩ������û���ҵ��");
    }
    else if (strRightClass == "1")
    {
        THROW_C(CRMException, CHECKBEFORETRADE_ERR, "TradeCheck_CheckStaffDataRight:��ǰԱ����Ȩ������û���ҵ��");
    }
    else if (strRightClass == "2")
    {
        THROW_C(CRMException, CHECKBEFORETRADE_ERR, "TradeCheck_CheckStaffDataRight:��ǰӪҵ����Ȩ������û���ҵ��");
    }
    else if (strRightClass == "3") //ҵ����
    {
        if (strCityCode != strCityCode_User || strEparchyCode != strEparchyCode_User)
        {
            THROW_C_P4(CRMException, CHECKBEFORETRADE_ERR, "TradeCheck_CheckStaffDataRight:��ǰҵ������Ȩ������û���ҵ��\n����Աҵ����[%s.%s],�û�����ҵ����[%s.%s]", strEparchyCode.c_str(), strCityCode.c_str(), strEparchyCode_User.c_str(), strCityCode_User.c_str());
        }
    }
    else if (strRightClass == "4") //����
    {
        if (strEparchyCode != strEparchyCode_User)
        {
        	THROW_C_P2(CRMException, CHECKBEFORETRADE_ERR, "TradeCheck_CheckStaffDataRight:��ǰ������Ȩ������û���ҵ��\n����Ա���б���[%s],�û��������б���[%s]", strEparchyCode.c_str(), strEparchyCode_User.c_str());
        }
    }
	
	LOG_TRACE(logTradeCheckBeforeTrade, "�˳� TradeCheck_CheckStaffDataRight");
}
int TradeCheckBeforeTrade::TradeCheck_NeedCheckOptrDataRight(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "����TradeCheck_NeedCheckOptrDataRight");
    int iCount=0;
    string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");

    StaticDAO& dao = StaticDAO::getDAO();
	CFmlBuf bufTag;
    try
    {
        dao.Clear();
        dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
        dao.SetParam(":VTAG_CODE", "CS_CHR_JUDGESTAFFCLASS");
        dao.SetParam(":VSUBSYS_CODE", "CSM");
        dao.SetParam(":VUSE_TAG", "0");
        iCount = dao.jselect(bufTag, "SEL_ALL_BY_TAGCODE","TD_S_TAG");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "TradeCheck_NeedCheckOptrDataRight:��ȡTAG_CODE=CSM_CHR_JUDGESTAFFCLASS��¼����");
    }
    	
	LOG_TRACE(logTradeCheckBeforeTrade, "�˳�TradeCheck_NeedCheckOptrDataRight");
	if (iCount>0 && bufTag.GetString("TAG_CHAR") == "1")
		return 1;
	return 0;
}
void TradeCheckBeforeTrade::TradeCheck_CheckInitPasswd(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "����TradeCheck_CheckInitPasswd");
   
    int iCount=0;
    string tradeTypeCode = inBuf.GetString("TRADE_TYPE_CODE");
    string eparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");
    string userId = inBuf.GetString("USER_ID");
    StaticDAO& dao = StaticDAO::getDAO();
	CFmlBuf bufTag;
    try
    {
        dao.Clear();
        bufTag.ClearFmlValue();
        dao.SetParam(":VEPARCHY_CODE", eparchyCode);
        dao.SetParam(":VTAG_CODE", "CS_CHR_CHECK_INITPASSWD");
        dao.SetParam(":VSUBSYS_CODE", "CSM");
        dao.SetParam(":VUSE_TAG", "0");
        iCount = dao.jselect(bufTag, "SEL_BY_TAGCODE_1","TD_S_TAG");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "TradeCheck_CheckInitPasswd:��ȡTAG_CODE=CS_CHR_CHECK_INITPASSWD��¼����");
    }
   
	if (iCount>0 )
	{
		string tagInfo = bufTag.GetString("TAG_INFO");
		if( tagInfo.find("|"+tradeTypeCode+"|") != string::npos )
		{
			iCount = 0 ;
			try
    		{
        		dao.Clear();
        		bufTag.ClearFmlValue();
        		dao.SetParam(":VUSER_ID", userId);
        		dao.SetParam(":VATTR_CODE", "INIT_PASSWD");
        		iCount = dao.jselect(bufTag, "SEL_BY_ID_CODE","TF_F_USER_ITEM");
    		}
    		catch(Exception &ex)
    		{
        		THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "TradeCheck_CheckInitPasswd:��ȡ�û��ݱ���Ϣ����");
    		}
    		if( iCount > 0 && bufTag.GetString("ATTR_VALUE") == "0")
    		{
    			 THROW_C(CRMException, CS_TRADELIMIT_ERR, "ҵ������ǰ�����жϣ����û�Ϊ��ʼ�����û������ܰ����ҵ��");
    		}
    	}	
    		 
	}
}
void TradeCheckBeforeTrade::TradeCheck_CheckPrdAttrLimit(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "����TradeCheck_CheckPrdAttrLimit");
   
    int iCount=0;
    string tradeTypeCode = inBuf.GetString("TRADE_TYPE_CODE");
    string eparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");
    if( tradeTypeCode != "110" ) return  ;
    string userId = inBuf.GetString("USER_ID");
    StaticDAO& dao = StaticDAO::getDAO();
	CFmlBuf bufTag;
    try
    {
        dao.Clear();
        bufTag.ClearFmlValue();
        dao.SetParam(":VEPARCHY_CODE", eparchyCode);
        dao.SetParam(":VTAG_CODE", "CS_CHR_CHECK_PRDATTRLIMIT");
        dao.SetParam(":VSUBSYS_CODE", "CSM");
        dao.SetParam(":VUSE_TAG", "0");
        iCount = dao.jselect(bufTag, "SEL_BY_TAGCODE_1","TD_S_TAG");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "TradeCheck_CheckPrdAttrLimit:��ȡTAG_CODE=CS_CHR_CHECK_PRDATTRLIMIT��¼����");
    }
	if (iCount>0 && bufTag.GetString("TAG_CHAR") == "1")
	{
		iCount = 0 ;
		try
    	{
        	dao.Clear();
        	bufTag.ClearFmlValue();
        	dao.SetParam(":VUSER_ID", userId);
        	dao.SetParam(":VATTR_CODE", "USER_PRODUCT_ATTR");
        	iCount = dao.jselect(bufTag, "SEL_BY_ID_CODE","TF_F_USER_ITEM");
    	}
    	catch(Exception &ex)
    	{
        	THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "TradeCheck_CheckPrdAttrLimit:��ȡ�û��ݱ���Ϣ����");
    	}
    	if( iCount > 0 && bufTag.GetString("ATTR_VALUE") == "1")
    	{
    		 THROW_C(CRMException, CS_TRADELIMIT_ERR, "ҵ������ǰ�����жϣ����û��в�Ʒ�����Ա�����ƣ����ܰ����Ʒ�����");
    	}	
    		 
	}
}

int TradeCheckBeforeTrade::TradeCheck_CheckBlackUser(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "���� TradeCheck_CheckBlackUser");
	if (!outBuf.IsFldExist("CUST_ID"))
	{
		THROW_C(CRMException, CHECKAFTERTRADE_ERR, "TradeCheck_CheckBlackUser:���ȵ���TradeCheck_GetCustUserInfoIntoBuf��ȡ�û��ͻ���Ϣ��");
	}

	bool hasCustInfo=outBuf.IsFldExist("PSPT_ID");
	bool hasUserInfo=outBuf.IsFldExist("USER_ID");
	
    string strCustID = outBuf.GetString("CUST_ID");
    string strSerialNumber = outBuf.GetString("SERIAL_NUMBER");

    string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");
    string strTradeTypeCode = inBuf.GetString("TRADE_TYPE_CODE");
    
    string strNetTypeCode = inBuf.GetString("NET_TYPE_CODE");
    
    LOG_TRACE_P3(logTradeCheckBeforeTrade, "net_type_code=|%s|,strTradeTypeCode=|%s|,strEparchyCode= |%s|",strNetTypeCode.c_str(),strTradeTypeCode.c_str(),strEparchyCode.c_str());
    	
    CFmlBuf bufTradetype;
    
    getTradeTypeInfo(strNetTypeCode,strEparchyCode,strTradeTypeCode,bufTradetype);
    
    
    string strIdType="1";
	if (inBuf.IsFldExist("ID_TYPE")) strIdType = inBuf.GetString("ID_TYPE");    
    
    int iMode ;
    if (inBuf.IsFldExist("X_CHOICE_TAG")) iMode  = inBuf.GetInt("X_CHOICE_TAG");
    
    StaticDAO& dao = StaticDAO::getDAO();
    int iCheckBlackUser=1;
    int ierrFag=0;
	if(string(bufTradetype.GetString("TAG_SET")).substr(3,1) != "0" )
	{
		CFmlBuf bufCust;
	    if(strIdType == "1")
	    {
            try
            {
                dao.Clear();
                dao.SetParam(":VCUST_ID", strCustID);
                hasCustInfo = dao.jselect(bufCust, "SEL_BY_PK","TF_F_CUSTOMER");
            }
            catch (Exception &ex)
            {
                THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "TradeCheck_CheckBlackUser:��ȡ�ͻ��������ϳ���");
            }
            //add by zhangzh@20060412 21:58 begin �����û����жϺ�����
			CFmlBuf bufMoffice;
			
			if (strSerialNumber.substr(0,2) == "99")
			{
				iCheckBlackUser = 0;
			}
			else
			{
				try
				{
					dao.Clear();
					dao.SetParam(":VSERIAL_NUMBER", strSerialNumber);
					iCheckBlackUser = dao.jselect(bufMoffice, "SEL_BY_NUM","TD_M_MOFFICE");
				}
				catch (Exception &ex)
				{
					THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "TradeCheck_CheckBlackUser:��ȡ�ͻ��������ϳ���");
				}
			}//add by zhangzh@20060412 21:58 end
	    }
	    else
	        bufCust = outBuf;


	    if(hasCustInfo && iCheckBlackUser != 0)//modi by zhangzh@20060412 21:58 �����û����жϺ�����
	    {
	        try
	        {                
                //modified by qiumb@2007-10-20  ��������Ϣ��td_o_blackuser��Ϊtf_o_cust_special_list
//qc:05330 begin
/*			 
			    int tag = -1;
				JTable *pTab = jdb::getConnect()->CreateJTable();
				try
				{
				    pTab->Clear();
					pTab->AddSql("SELECT COUNT(1) TAG FROM TF_O_CUST_SPECIAL_LIST \
									WHERE PSPT_TYPE_CODE = :VPSPT_TYPE_CODE AND PSPT_ID=:VPSPT_ID \
									AND LIST_TYPE='2' AND ACT_TAG ='1'  \
									AND  START_DATE<=SYSDATE AND NVL(END_DATE,SYSDATE)>=SYSDATE  \
									AND ROWNUM < 2");
					pTab->SetParam(":VPSPT_TYPE_CODE", bufCust.GetString("PSPT_TYPE_CODE"));
					pTab->SetParam(":VPSPT_ID", bufCust.GetString("PSPT_ID"));
					pTab->ExecSelect();
					while(pTab->Fetch())
					{
						tag = pTab->GetInt("TAG");
					}
					delete pTab;
				}
				catch(Exception &e)
				{
				    delete pTab;
					THROW_AGC(CRMException, e, Grade::WARNING, GETUSERSCOREINFO_ERR, "����Ƿ�������쳣��");
				}
	
				//if(daoBlackuser.jselect(objBlackuser, "SEL_BY_PK") > 0)
*/
			    //����QCS_IsBlackUser��ѯ�û��Ƿ�Ƿ�ѣ�
			    CFmlBuf lcuInBuf,lcuOutBuf;
			    lcuOutBuf.ClearFmlValue();
			    lcuInBuf.SetString("PSPT_ID",bufCust.GetString("PSPT_ID"));
			    lcuInBuf.SetString("PSPT_TYPE_CODE",bufCust.GetString("PSPT_TYPE_CODE"));
			    lcuInBuf.SetString("X_TRANS_CODE","QCS_IsBlackUser");
			    lcuInBuf.SetInt("X_TAG",0);
			    LOG_TRACE_P1(logTradeCheckBeforeTrade,"=============����QCS_IsBlackUser����buf:===========%s\n",lcuInBuf.ToString().c_str());
			    Utility::callLCU(lcuInBuf,lcuOutBuf);
			    LOG_TRACE_P1(logTradeCheckBeforeTrade,"=============����QCS_IsBlackUser���buf:===========%s\n",lcuOutBuf.ToString().c_str());
			    if (lcuOutBuf.GetInt("X_RESULTCODE")!=0)
			    {
			        THROW_C(CRMException, 8888,"�����ʹ�QCC_IsBlackUser�����쳣"+lcuOutBuf.GetString("X_RESULTINFO"));
			    } 
                //if(tag > 0)
                if(lcuOutBuf.GetString("ACT_TAG") =="1")
//qc:05330 end                	
                {
                    if(string(bufTradetype.GetString("TAG_SET")).substr(3,1) == "1") //��ʾѡ��
                    {
                        ierrFag = 1;
                    }
                    else if(string(bufTradetype.GetString("TAG_SET")).substr(3,1) == "2")    //��ֹ����
                    {
                        THROW_C(CRMException, CHECKBEFORETRADE_ERR, "TradeCheck_CheckBlackUser:�������ͻ����ܰ���ǰҵ��"); 
                    }
                }
            }
            catch(Exception &e)
            {
                THROW_AGC(CRMException, e, Grade::WARNING, CHECKBEFORETRADE_ERR, "TradeCheck_CheckBlackUser:ҵ�������:");
            }
	    }
	}
	
	LOG_TRACE(logTradeCheckBeforeTrade, "�˳� TradeCheck_CheckBlackUser");
	return ierrFag;	
}
int TradeCheckBeforeTrade::TradeCheck_CheckOweFee(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "���� TradeCheck_CheckOweFee");
	
	string isPrePay ;//"0"Ϊ�󸶷�
    isPrePay=outBuf.GetString("PREPAY_TAG");
    
    string strId = inBuf.GetString("ID");
    bool isRedUser=false;

    string strFee = "0";
    if (inBuf.IsFldExist("FEE"))
        strFee = inBuf.GetString("FEE");//�û�Ƿ��
    else
        strFee = inBuf.GetString("LEAVE_REAL_FEE");
        
    string strFee1 = "0";//����Ƿ��
    string strFee2 = "0";//ʵʱ����
    string strFee3 = "0";//ʵʱ����
    if (inBuf.IsFldExist("FEE1"))
        strFee1 = inBuf.GetString("FEE1");
    if (inBuf.IsFldExist("FEE2"))
        strFee2 = inBuf.GetString("FEE2");
    if (inBuf.IsFldExist("FEE3"))
        strFee3 = inBuf.GetString("FEE3");
        
        
    string strTradeTypeCode = inBuf.GetString("TRADE_TYPE_CODE");
    string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");
    
    string strNetTypeCode = inBuf.GetString("NET_TYPE_CODE");
    
    LOG_TRACE_P1(logTradeCheckBeforeTrade, "TradeCheck_CheckOweFee,net_type_code = |%s|",strNetTypeCode.c_str());	
    	
    CFmlBuf bufTradetype;
    StaticDAO& dao = StaticDAO::getDAO();
    getTradeTypeInfo(strNetTypeCode,strEparchyCode,strTradeTypeCode,bufTradetype);


    string strIdType = "1"; //����Ĭ��IDΪ�û���ʶ
    if (inBuf.IsFldExist("ID_TYPE"))
        strIdType = inBuf.GetString("ID_TYPE");
    
    if (strIdType == "1")
    	isRedUser=(inBuf.GetString("REDUSER_TAG") == "1"); //cclib::isRedUser(strId);

    int iCount=0;
    string strAcctId;
    
    int ierrFag=0;
    string strJudgeOweTag = bufTradetype.GetString("JUDGE_OWE_TAG");
  
     //���ֶ�Ƿ���û���ҵ���������ҪӪҵԱ�в���Ȩ�� modify xf 20091004
    int isright = CheckStaffIdRight(inBuf,outBuf);
    if(strJudgeOweTag != "0" && isright == 1){
        return ierrFag;
    }
    
    if( strJudgeOweTag == "C" ) //����Ƿ�ѣ�ǰ̨��Ҫչ���û�������Ϣ�����¼�һ��judgeOweTag��
    {
    	ierrFag=0; 	     		
    }
    else if ( strJudgeOweTag == "A" )//��Ԥ���ͺ�
    {
        if (isPrePay == "1")  //Ԥ��
        {
            if( atol(strFee3.c_str()) < 0 ) //ʵʱ����
            {
                THROW_C(CRMException, CS_OWEFEE_ERR, "TradeCheck_CheckOweFee:Ԥ�����û���ʵʱǷ�Ѳ��ܰ���ҵ��");
            }
        }
        else  //��
        {
            if( atol(strFee1.c_str()) > 0 ) //����Ƿ��
            {
                THROW_C(CRMException, CS_OWEFEE_ERR, "TradeCheck_CheckOweFee:�󸶷��û�������Ƿ�Ѳ��ܰ���ҵ��");
            }
        }
    }
    else if( strJudgeOweTag == "B" )
    {
    	if( atol(strFee1.c_str()) > 0 )//����Ƿ��
    	{
            CFmlBuf bufCparam;
        	try
        	{
            	dao.Clear();
            	dao.SetParam(":VUSER_ID", strId);
            	dao.SetParam(":VDEFAULT_TAG", "1");
            	dao.SetParam(":VACT_TAG","1");
            	dao.jselect(bufCparam,"IsOnlyAccount","TD_S_CPARAM");
            	if (bufCparam.GetInt("RECORDCOUNT") > 0)//���ʻ��û�
            	{
            		THROW_C(CRMException, CS_OWEFEE_ERR, "TradeCheck_CheckOweFee:�Բ�����������Ƿ�ѣ�����������ҵ��");	
            	}
            }
            catch(Exception &ex)
            {
                    THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "TradeCheck_CheckOweFee:�ж��û��Ƿ�����ʻ�����");
            }
       }
    }
    else if (!isRedUser && atol(strFee.c_str()) < 0)
    {
        if (strJudgeOweTag == "1")            //1-�κ��������Ƿ��
        {
            THROW_C(CRMException, CS_OWEFEE_ERR, "TradeCheck_CheckOweFee:�û��Ѿ�Ƿ�Ѳ��ܰ���ҵ��");
        }
        else if (strJudgeOweTag == "2")       //2-��Ƿ�Ѳ���ʾ�Ƿ��������
        {
            ierrFag = 1;
        }
        else if (strJudgeOweTag == "3" ||     //3-�����ղ���
                 strJudgeOweTag == "4" ||       //4-�����ղ��в���ʾ
                 strJudgeOweTag == "5" || //5-�����ղ��� ���������ʻ�����Ƿ���У�
                 strJudgeOweTag == "6" ||   //6-���ղ�������Ƿ��    
                 strJudgeOweTag == "7" || //�������ж�����Ƿ��
                 strJudgeOweTag == "8")   //�������û�������Ƿ�Ѳ����������ʵʱǷ�Ѹ�����ʾ
        {
            if (strIdType == "1")   //��������û�
            {
                //�Ȼ�ȡĬ���ʻ���ʶ
                CFmlBuf bufVPayrelation;
                try
                {
                    dao.Clear();
                    dao.SetParam(":VUSER_ID", strId);
                    iCount = dao.jselect(bufVPayrelation, "SEL_BY_USER","TF_A_PAYRELATION");
                }
                catch(Exception &ex)
                {
                    THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "TradeCheck_CheckOweFee:��ȡ�û��ĵ�ǰĬ���ʻ�����");
                }
                if(iCount == 0)
                {
                	  if(strTradeTypeCode=="311" || strTradeTypeCode=="310" || strTradeTypeCode=="191" || strTradeTypeCode=="7302" || strTradeTypeCode=="288")
                	  	//������ԤԼ��ʽ�������ɷѸ���
                	  {
                	      //δȡ����ȡ���һ��Ĭ���ʻ�
                        try
                        {
                            dao.Clear();
                            dao.SetParam(":VUSER_ID", strId);
                            dao.SetParam(":VDEFAULT_TAG", "1");
                            iCount = dao.jselect(bufVPayrelation, "SEL_BY_USER_MAX","TF_A_PAYRELATION");
                        }
                        catch(Exception &e)
                        {
                            THROW_AGC(CRMException, e, Grade::WARNING, CHECKBEFORETRADE_ERR, "TradeCheck_CheckOweFee:��ȡ�û����һ��Ĭ���ʻ��쳣��");
                        }
                    }                    
                }
                if(iCount == 0)
                {
                	  THROW_C(CRMException, CHECKBEFORETRADE_ERR, "TradeCheck_CheckOweFee:δ�ҵ��û�(" + strId + ")�ĵ�ǰĬ���ʻ�"); 
                }
                if(iCount > 1)
                {
                    THROW_GC(CRMException, Grade::WARNING, CHECKBEFORETRADE_ERR, "TradeCheck_CheckOweFee:�û�(" + strId + ")�ĸ��ѹ�ϵ���ϴ���");
                }
                strAcctId = bufVPayrelation.GetString("ACCT_ID");
            }
            else if (strIdType == "2")   //��������ʻ�
            {
                strAcctId = strId;
            }
            
            //��ȡ�ʻ���Ϣ
            CFmlBuf acctBuf;
            try
            {
                //AcctUpdateMgr objAcctUpdateMgr;
                //iCount = objAcctUpdateMgr.QueryAcctInfo(acctBuf,strAcctId);
            }
            catch(Exception &ex)
            {
                THROW_AGC(CRMException, ex, Grade::WARNING, GETACCTINFO_ERR, "TradeCheck_CheckOweFee:��ȡ�ʻ����ϳ���");
            }
            if(iCount == 0)
            {
                THROW_C(CRMException, GETACCTINFO_ERR, "TradeCheck_CheckOweFee:�ʻ�(" + strAcctId + ")������");
            }

            //Ƿ���жϣ������û��Ƿ�Ϊ�����ʻ����ж�
            string strPayModeCode = acctBuf.GetString("PAY_MODE_CODE");
            if(strPayModeCode == "1" && strJudgeOweTag == "5")
            {
                iCount = 0;
                CFmlBuf payRelaBuf;
                try
                {
                    dao.Clear();
                    dao.SetParam(":VACCT_ID", strAcctId);
                    dao.SetParam(":VDEFAULT_TAG", "1");
                    dao.SetParam(":VACT_TAG", "1");
                    iCount = dao.jselect(payRelaBuf, "SEL_BY_ACCT_DA", "TF_A_PAYRELATION");
                }
                catch(Exception &e)
                {
                    THROW_AGC(CRMException, e, Grade::WARNING, GETACCTINFO_ERR, "TradeCheck_CheckOweFee:��ȡָ���ʻ���ǰĬ�ϸ����ʻ������쳣");
                }
                
                if(iCount > 0)
                {
                    CFmlBuf userBuf;
                    for(int i = 0; i < iCount; i++)
                    {
                        try
                        {
                            userBuf.ClearFmlValue();
                            dao.Clear();
                            dao.SetParam(":VUSER_ID", payRelaBuf.GetString("USER_ID",i));
                            dao.jselect(userBuf, "SEL_BY_PK", "TF_F_USER");
                        }
                        catch(Exception &e)
                        {
                            THROW_AGC(CRMException, e, Grade::WARNING, GETUSERINFO_ERR, "TradeCheck_CheckOweFee:��ȡ�û���Ϣ�쳣!");
                        }
                        if(userBuf.GetString("REMOVE_TAG") != "0")
                        {
                            iCount--;
                        }
                    }
                    if(iCount == 1)
                    {
                        THROW_C(CRMException, CS_OWEFEE_ERR, "TradeCheck_CheckOweFee:ҵ������ǰ�����ж�:���û�Ϊ�����û������һ�����Ѿ�Ƿ�ѣ���ɷѺ����ҵ��");
                    }
                }
            }
            //Add by liufei @ 20050707 end
            //�ж��û��Ƿ����ա��Ƿ���Ƿ��
            if ( strPayModeCode != "1" && (strJudgeOweTag == "3" || strJudgeOweTag == "5" || strJudgeOweTag == "8")) //�û��������գ���ֹҵ��
            {
                THROW_C(CRMException, CS_OWEFEE_ERR, "TradeCheck_CheckOweFee:�û��Ѿ�Ƿ�Ѳ��ܰ���ҵ��");
            }
            if ( strPayModeCode != "1" && strJudgeOweTag == "4") //�û��������գ���ʾǷ��
            {
                ierrFag = 1;
            }
            if(strPayModeCode != "1" && strJudgeOweTag == "6")
            {
                THROW_C(CRMException, CS_OWEFEE_ERR, "TradeCheck_CheckOweFee:�û�������Ƿ�Ѳ��ܰ���ҵ��");
            }
            if(strJudgeOweTag == "7")     //�������ж�����Ƿ��
            {
                THROW_C(CRMException, CS_OWEFEE_ERR, "TradeCheck_CheckOweFee:�û�������Ƿ�Ѳ��ܰ���ҵ��");
            }
        }
	    LOG_TRACE(logTradeCheckBeforeTrade, "�˳� TradeCheck_CheckOweFee");
	    return ierrFag;
    }
    
    return ierrFag;
}

void TradeCheckBeforeTrade::TradeCheck_SvcstateTradeLimit(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "�˳� TradeCheck_SvcstateTradeLimit");
    bool isRedUser=false;
    string strIdType = "1"; //����Ĭ��IDΪ�û���ʶ
    
     //���ֶ�Ƿ���û���ҵ���������ҪӪҵԱ�в���Ȩ�� modify xf 20091004
    int isright = CheckStaffIdRight(inBuf,outBuf);
    if(isright == 1){
        return;
    }
    
    if (inBuf.IsFldExist("ID_TYPE"))
        strIdType = inBuf.GetString("ID_TYPE");	
    
    StaticDAO& dao = StaticDAO::getDAO();
    int iCount=0;    
	if (strIdType == "1")
	{
		string strId = inBuf.GetString("ID");
		isRedUser = (inBuf.GetString("REDUSER_TAG") == "1"); //cclib::isRedUser(strId);

	    string strTradeTypeCode = inBuf.GetString("TRADE_TYPE_CODE");
	    string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");

	    if (!isRedUser)
	    {
	        CFmlBuf bufVSvcstateTradeLimit;
	        try
	        {
	            dao.Clear();
	            dao.SetParam(":VUSER_ID",strId);
	            dao.SetParam(":VTRADE_TYPE_CODE",strTradeTypeCode);
	            dao.SetParam(":VEPARCHY_CODE",strEparchyCode);
	            iCount = dao.jselect(bufVSvcstateTradeLimit,"SEL_EXISTS_BY_USERID","TD_S_SVCSTATE_TRADE_LIMIT");
	        }
	        catch (Exception &ex)
	        {
	            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "TradeCheck_SvcstateTradeLimit:��ѯ����״̬��ҵ�����Ʊ����");
	        }
	        if (iCount > 0)
	        {
	            char chId[10];
	            sprintf(chId,"%d",(int)bufVSvcstateTradeLimit.GetInt("SERVICE_ID"));
	            string strSvcName = CParamDAO::getParam("ServiceName", chId);
	            string strSvcstateName = CParamDAO::getParam("SvcstateName", chId, bufVSvcstateTradeLimit.GetString("STATE_CODE"));
	            THROW_C_P2(CRMException, CS_USERSTATE_ERR, "TradeCheck_SvcstateTradeLimit:�û����в��ܰ����ҵ��ķ���״̬[%s.%s]��", strSvcName.c_str(), strSvcstateName.c_str());
	        }
	    }
	}
	LOG_TRACE(logTradeCheckBeforeTrade, "�˳� TradeCheck_SvcstateTradeLimit");
}
void TradeCheckBeforeTrade::TradeCheck_UserTradelimit(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "���� TradeCheck_UserTradelimit");
	
	LOG_TRACE(logTradeCheckBeforeTrade, "�˳� TradeCheck_UserTradelimit");	
}
/**
 *  ���֣��ж�ӪҵԱ�����Ƿ���Ƿ�Ѳ���Ȩ��
 *  Ƿ��ҵ�������Ҫ��ҵ�����ͺ�
*/
int TradeCheckBeforeTrade::CheckStaffIdRight(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
    LOG_TRACE(logTradeCheckBeforeTrade, "����CheckStaffIdRight����");
    StaticDAO& dao = StaticDAO::getDAO();
    CFmlBuf bufTag;

    int iTagCount = 0;
    int ierrFag=0;
    string strProvince = "";
    string strTradeTypeCode ="";
    string strTradeStaffId ="";
    try
    {
        bufTag.ClearFmlValue();
        dao.Clear();
        dao.SetParam(":VEPARCHY_CODE", inBuf.GetString("TRADE_EPARCHY_CODE"));
        dao.SetParam(":VTAG_CODE", "PUB_CUR_PROVINCE");
        dao.SetParam(":VSUBSYS_CODE", "PUB");
        dao.SetParam(":VUSE_TAG", "0");
        iTagCount = dao.jselect(bufTag, "SEL_ALL_BY_TAGCODE","TD_S_TAG");
        if(iTagCount == 1)
	    {
	        strProvince = bufTag.GetString("TAG_INFO");
	    }
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ��ǰʡ�������");
    }

   LOG_TRACE_P1(logTradeCheckBeforeTrade, "strProvince*************>>>>>=%s",strProvince.c_str());

    if(strProvince=="JLCU"){
        try  //�����м��cpp�л�ȡ�ò���
        {
            strTradeTypeCode = inBuf.GetString("TRADE_TYPE_CODE");
            strTradeStaffId = inBuf.GetString("TRADE_STAFF_ID");
    
            dao.Clear();
            bufTag.ClearFmlValue();
            dao.SetParam(":VTRADE_TYPE_CODE", strTradeTypeCode);
            dao.SetParam(":VTRADE_STAFF_ID", strTradeStaffId);
            if (dao.jselect(bufTag, "CheckStaffIdRight","TD_S_CPARAM") > 0){
                ierrFag=1;
            }
                
        }
        catch(Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "Ƿ��ҵ�����:��ȡ��ǰ����Ȩ���쳣��");
        }
	}
     LOG_TRACE_P1(logTradeCheckBeforeTrade, "ierrFagddddddddd*************>>>>>=%d",ierrFag);

    LOG_TRACE(logTradeCheckBeforeTrade, "�˳�CheckStaffIdRight����");
    return ierrFag;
}


/**
 *  ҵ������ǰ�����ж�:ҵ�����У�飬�û�����״̬��ҵ��������ü��
*/
void TradeCheckBeforeTrade::CheckSvcState(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
    LOG_TRACE(logTradeCheckBeforeTrade, "����CheckSvcState����");

    string strTradeTypeCode = inBuf.GetString("TRADE_TYPE_CODE");
    string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");
    string strBrandCode = inBuf.GetString("BRAND_CODE");
    string strProductId = inBuf.GetString("PRODUCT_ID");

    string strUserId = inBuf.GetString("USER_ID");
    string strNetTypeCode = inBuf.GetString("NET_TYPE_CODE"); 
    
    StaticDAO& dao = StaticDAO::getDAO();
    CFmlBuf BufTag;
    string strTag="";
    try  //�����м��cpp�л�ȡ�ò���
    {
        dao.Clear();
        BufTag.ClearFmlValue();
        dao.SetParam(":VEPARCHY_CODE", "ZZZZ");
        dao.SetParam(":VTAG_CODE", "CS_CHR_GENESVCSTATENEW");
        dao.SetParam(":VSUBSYS_CODE", "CSM");
        dao.SetParam(":VUSE_TAG", "0");  
        if (dao.jselect(BufTag, "SEL_ALL_BY_TAGCODE","TD_S_TAG") == 0)
        	strTag= "0";
        else
        	strTag=BufTag.GetString("TAG_CHAR");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "����״̬���:��ȡ��ǰ��������");
    }

	if (strTag == "1")
        CheckSvcStateNew(strUserId, strTradeTypeCode, strEparchyCode, strBrandCode, strProductId, strNetTypeCode);
    else
    	CheckSvcState(strUserId, strTradeTypeCode, strEparchyCode, strBrandCode, strProductId,strNetTypeCode);

    LOG_TRACE(logTradeCheckBeforeTrade, "�˳�CheckSvcState����");
}
/**
 *ҵ������ǰ�����ж�:ҵ�����У�飬�û�����״̬��ҵ��������ü��
 * @param strUserId ���� �û���ʶ
 * @param strTradeTypeCode ���� ҵ�����ʹ���
 * @param strEparchyCode ���� ������ݴ���
 * @return �ޣ�ʧ��ʱ�׳�����
*/
void TradeCheckBeforeTrade::CheckSvcState(const string &strUserId, const string &strTradeTypeCode, const string &strEparchyCode, const string &strBrandCode, const string &strProductId,const string &strNetTypeCode)
{
    StaticDAO& dao = StaticDAO::getDAO();
    CFmlBuf bufVTradesvcstatepara, bufVUserSvcstate,BufTag;
    int userSvcCount = -1;
    int userSvcStateCount=-1;
    string strTag="";
   	 try  //��ȡtag���õĹ�������
   	 {
        dao.Clear();
        BufTag.ClearFmlValue();
        dao.SetParam(":VEPARCHY_CODE", "ZZZZ");
        dao.SetParam(":VTAG_CODE", "JUDGE_OCS_NET_TYPE");
        dao.SetParam(":VSUBSYS_CODE", "CSM");
        dao.SetParam(":VUSE_TAG", "0");  
        if (dao.jselect(BufTag, "SEL_ALL_BY_TAGCODE","TD_S_TAG") == 0)
        	strTag= "0";
        else
        	strTag=BufTag.GetString("TAG_INFO");
         }
	    catch(Exception &ex)
	    {
	        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "����״̬���:��ȡ��ǰ��������");
	    }
    //��ȡ�û�����״̬����
    try
    {
        dao.Clear();
        dao.SetParam(":VUSER_ID", strUserId);
         if (strTag.find("|"+strTradeTypeCode+"|") != string::npos )
        		userSvcCount = dao.jselect(bufVUserSvcstate, "SEL_USER_MAINSATE","TF_F_USER_SVCSTATE");
        else
        		userSvcCount = dao.jselect(bufVUserSvcstate, "SEL_USER_SVCSTATE","TF_F_USER_SVCSTATE");
    }
    catch(Exception &ex)
    {
         THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "ҵ��Ǽ�ǰ���:��ȡԭ�û�����״̬���ϳ���");
    }
    if (userSvcCount==0)
    {
        THROW_C(CRMException, CHECKBEFORETRADE_ERR, strUserId+"ҵ��Ǽ�ǰ���:û�л�ȡ��ԭ�û�����״̬���ϣ�");
    }
    //��ȡ����״̬���ҵ�����
    try
    {
        dao.Clear();
        dao.SetParam(":VTRADE_TYPE_CODE",strTradeTypeCode);
        dao.SetParam(":VBRAND_CODE",strBrandCode);
        dao.SetParam(":VPRODUCT_ID",strProductId);
        dao.SetParam(":VEPARCHY_CODE",strEparchyCode);
        dao.SetParam(":VNET_TYPE_CODE",strNetTypeCode);
        userSvcStateCount=dao.jselect(bufVTradesvcstatepara, "SEL_BY_PK","TD_S_TRADE_SVCSTATE");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "ҵ��Ǽ�ǰ���:��ȡ��������������");
    }
    if (userSvcStateCount==0)
    {
        THROW_C(CRMException, CHECKBEFORETRADE_ERR, "ҵ��Ǽ�ǰ���:����������û�����ã�");
    }

    if (userSvcStateCount>0)
    {
        int iCount = userSvcStateCount; //����ѭ��
        int jCount = userSvcCount;
        //begin jianghp@2006-03-12 20:17 modify
        string strSvcName,strSvcstateName;

        for(int i=0; i<iCount; i++)
        {
            userSvcStateCount = -1;           
            char chId[10];
            sprintf(chId,"%d",(int)bufVUserSvcstate.GetInt("SERVICE_ID"));
            strSvcName = CParamDAO::getParam("ServiceName", chId);
            strSvcstateName = CParamDAO::getParam("SvcstateName", chId, bufVUserSvcstate.GetString("STATE_CODE"));
            for(int j=0; j<jCount; j++)
            {
                userSvcCount = -1;
                if (((int)bufVUserSvcstate.GetInt("SERVICE_ID",j)==(int)bufVTradesvcstatepara.GetInt("SERVICE_ID",i)) && 
                    (bufVUserSvcstate.GetString("STATE_CODE",j)==bufVTradesvcstatepara.GetString("OLD_STATE_CODE",i)))
                {
                    return;
                }
            }
        }
        if(strSvcName=="" && strSvcstateName=="")
        {
            THROW_C(CRMException, CHECKBEFORETRADE_ERR, "�û�����״̬Ϊ��,ҵ���޷�������");
        }
        else
        {
            THROW_C_P2(CRMException, CHECKBEFORETRADE_ERR, "�û����ڵķ���״̬��[%s.%s],���ܰ����ҵ��", strSvcName.c_str(), strSvcstateName.c_str());
        }
     }
}

/**
 *ҵ������ǰ�����ж�:ҵ�����У�飬�û�����״̬��ҵ��������ü��
 * @param strUserId ���� �û���ʶ
 * @param strTradeTypeCode ���� ҵ�����ʹ���
 * @param strEparchyCode ���� ������ݴ���
 * @return �ޣ�ʧ��ʱ�׳�����
*/
void TradeCheckBeforeTrade::CheckSvcStateNew(const string &strUserId, const string &strTradeTypeCode, const string &strEparchyCode,
											 const string &strBrandCode, const string &strProductId,const string strNetTypeCode)
{
    int userSvcCount = 0;
    int userSvcStateCount=0;
    StaticDAO& dao = StaticDAO::getDAO();
    
    //��ȡ�û�����״̬ 1:�Ȳ��Ƿ���δ�깤����״̬ҵ��.����,��Ӹñ�ҵ���̨���ӱ��л�ȡ,����,����û�����״̬�ӱ��ȡ
    //���� TRADE::TF_B_TRADE::SEL_LAST_SVCSTATECHG_TRADE
       
    int iCnt=0;
    CFmlBuf bufTrade;
    CFmlBuf bufUserSvcstate;
    string strSvcCode,strSvcstateCode;
    try
    {
        dao.Clear();
        bufTrade.ClearFmlValue();
        dao.SetParam(":VUSER_ID", strUserId);
        iCnt = dao.jselect(bufTrade, "SEL_LAST_TRADE","TF_B_TRADE");
    }
    catch(Exception &ex)
    {
         THROW_AGC(CRMException, ex, Grade::WARNING, GENEBACKUPTRADE_ERR, "����ҵ��̨��:��ȡԭ�û�����״̬���ϳ���");
    }
    LOG_TRACE_P1(logTradeCheckBeforeTrade, "iCnt %d.", iCnt);
    
    if (iCnt > 0)
    {
        try
        {
            dao.Clear();
            bufUserSvcstate.ClearFmlValue();
            dao.SetParam(":VTRADE_ID", bufTrade.GetString("TRADE_ID"));
            userSvcCount=dao.jselect(bufUserSvcstate, "SEL_BY_TRADETAG","TF_B_TRADE_SVCSTATE");
        }
        catch(Exception &ex)
        {
             THROW_AGC(CRMException, ex, Grade::WARNING, GENEBACKUPTRADE_ERR, "����ҵ��̨��:��ȡԭ�û�����״̬���ϳ���");
        }
    }
    //��ȡ�û�����״̬����
    if (iCnt == 0)
    {
        userSvcCount=0;
        
        try
        {
            dao.Clear();
            bufUserSvcstate.ClearFmlValue();
            dao.SetParam(":VUSER_ID", strUserId);
            userSvcCount = dao.jselect(bufUserSvcstate, "SEL_USER_SVCSTATE","TF_F_USER_SVCSTATE");
        }
        catch(Exception &ex)
        {
             THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "ҵ��Ǽ�ǰ���:��ȡԭ�û�����״̬���ϳ���");
        }
        if (userSvcCount==0)
        {
            THROW_C(CRMException, CHECKBEFORETRADE_ERR, strUserId+"ҵ��Ǽ�ǰ���:û�л�ȡ��ԭ�û�����״̬���ϣ�");
        }
    }
    
    //��ȡ����״̬���ҵ�����
    CFmlBuf bufTradeSvcPara;
    try
    {
        dao.Clear();
        dao.SetParam(":VTRADE_TYPE_CODE",strTradeTypeCode);
        dao.SetParam(":VBRAND_CODE",strBrandCode);
        dao.SetParam(":VNET_TYPE_CODE",strNetTypeCode);
        dao.SetParam(":VPRODUCT_ID",strProductId);
        dao.SetParam(":VEPARCHY_CODE",strEparchyCode);
        userSvcStateCount=dao.jselect(bufTradeSvcPara, "SEL_BY_PK","TD_S_TRADE_SVCSTATE");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "ҵ��Ǽ�ǰ���:��ȡ��������������");
    }
    if (userSvcStateCount==0)
    {
        THROW_C(CRMException, CHECKBEFORETRADE_ERR, "ҵ��Ǽ�ǰ���:����������û�����ã�");
    }
    if (userSvcStateCount>0)
    {
        for(int i=0;i<userSvcStateCount;i++)
        {
            
            
            for(int j=0;j<userSvcCount;j++)
            {
            	if (bufUserSvcstate.GetString("SERVICE_ID",j)==bufTradeSvcPara.GetString("SERVICE_ID",i)&&bufTradeSvcPara.GetString("OLD_STATE_CODE",i)=="%")
            		return;
            	if (bufUserSvcstate.GetString("SERVICE_ID",j)==bufTradeSvcPara.GetString("SERVICE_ID",i)&&
            		bufUserSvcstate.GetString("STATE_CODE",j)==bufTradeSvcPara.GetString("OLD_STATE_CODE",i))
            	{
            		return;
            	}
            	if (bufUserSvcstate.GetString("SERVICE_ID",j)==bufTradeSvcPara.GetString("SERVICE_ID",i))
            	{
            		strSvcCode=bufUserSvcstate.GetString("SERVICE_ID",j);
            		strSvcstateCode=bufUserSvcstate.GetString("STATE_CODE",j);
            	}
            }
        }
		int iCount=0;
		string strSvcName;
		string strSvcstateName;
		CFmlBuf  tempBuf;
		try
		{
			dao.Clear();
			dao.SetParam(":VNET_TYPE_CODE",strNetTypeCode);
			dao.SetParam(":VSERVICE_ID",strSvcCode);
			dao.SetParam(":VSTATE_CODE",strSvcstateCode);
			iCount=dao.jselect(tempBuf, "td_b_service1","NormalPara");
		}
		catch(Exception &ex)
		{
			THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "ҵ��Ǽ�ǰ���:��ȡ����״̬��������");
		}
		if (iCount>0)
		{
			strSvcName=tempBuf.GetString("PARACODE");
			strSvcstateName=tempBuf.GetString("PARANAME");
			THROW_C_P2(CRMException, CHECKBEFORETRADE_ERR, "�û����ڵķ���״̬��[%s.%s],���ܰ����ҵ��", strSvcName.c_str(), strSvcstateName.c_str());
		}
		else
			THROW_C(CRMException, CHECKBEFORETRADE_ERR, "�û�����״̬Ϊ��,ҵ���޷�������");
    }
}


/**
 * �ƻ�+ͣ����У�� add by zhangyangshuo
 */

void TradeCheckBeforeTrade::CheckSvcStateNewMove(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	
	 string strTradeTypeCode = inBuf.GetString("TRADE_TYPE_CODE");
    string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");
    string strBrandCode = inBuf.GetString("BRAND_CODE");
    string strProductId = inBuf.GetString("PRODUCT_ID");

    string strUserId = inBuf.GetString("USER_ID");
    string strNetTypeCode = inBuf.GetString("NET_TYPE_CODE"); 
     string strTradeId = inBuf.GetString("TRADE_ID"); 
    int userSvcCount = 0;
    int userSvcStateCount=0;
    StaticDAO& dao = StaticDAO::getDAO();
    
    //��ȡ�û�����״̬ 1:�Ȳ��Ƿ���δ�깤����״̬ҵ��.����,��Ӹñ�ҵ���̨���ӱ��л�ȡ,����,����û�����״̬�ӱ��ȡ
    //���� TRADE::TF_B_TRADE::SEL_LAST_SVCSTATECHG_TRADE
       
        CFmlBuf temptradeItemBuf;
        string commMove = "";
    
        int icount2 =0;
        try
        {
                
            dao.Clear();
            dao.SetParam(":VTRADE_ID", strTradeId);  
            dao.SetParam(":VATTR_CODE", "MOVE_SVCSTATE");   	
            icount2=dao.jselect(temptradeItemBuf,"SEL_BY_TRADEID_ATTR","TF_B_TRADE_ITEM");
        }
        catch(Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, GETTRADEINFO_ERR, "��ȡ����̨��ʧ�ܣ�");
        } 
        if(icount2>0){//ǰ̨ѡ���Ƿ���й����������ݲ���
            commMove = temptradeItemBuf.GetString("ATTR_VALUE",0);
            LOG_TRACE_P1(logTradeCheckBeforeTrade, "commMove--%s",commMove.c_str());
        }else{
        	return;
        }
       strTradeTypeCode =  commMove ;
    int iCnt=0;
    CFmlBuf bufTrade;
    CFmlBuf bufUserSvcstate;
    string strSvcCode,strSvcstateCode;
    try
    {
        dao.Clear();
        bufTrade.ClearFmlValue();
        dao.SetParam(":VUSER_ID", strUserId);
        iCnt = dao.jselect(bufTrade, "SEL_LAST_TRADE","TF_B_TRADE");
    }
    catch(Exception &ex)
    {
         THROW_AGC(CRMException, ex, Grade::WARNING, GENEBACKUPTRADE_ERR, "����ҵ��̨��:��ȡԭ�û�����״̬���ϳ���");
    }
    LOG_TRACE_P1(logTradeCheckBeforeTrade, "iCnt %d.", iCnt);
    
    if (iCnt > 0)
    {
        try
        {
            dao.Clear();
            bufUserSvcstate.ClearFmlValue();
            dao.SetParam(":VTRADE_ID", bufTrade.GetString("TRADE_ID"));
            userSvcCount=dao.jselect(bufUserSvcstate, "SEL_BY_TRADETAG","TF_B_TRADE_SVCSTATE");
        }
        catch(Exception &ex)
        {
             THROW_AGC(CRMException, ex, Grade::WARNING, GENEBACKUPTRADE_ERR, "����ҵ��̨��:��ȡԭ�û�����״̬���ϳ���");
        }
    }
    //��ȡ�û�����״̬����
    if (iCnt == 0)
    {
        userSvcCount=0;
        
        try
        {
            dao.Clear();
            bufUserSvcstate.ClearFmlValue();
            dao.SetParam(":VUSER_ID", strUserId);
            userSvcCount = dao.jselect(bufUserSvcstate, "SEL_USER_SVCSTATE","TF_F_USER_SVCSTATE");
        }
        catch(Exception &ex)
        {
             THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "ҵ��Ǽ�ǰ���:��ȡԭ�û�����״̬���ϳ���");
        }
        if (userSvcCount==0)
        {
            THROW_C(CRMException, CHECKBEFORETRADE_ERR, strUserId+"ҵ��Ǽ�ǰ���:û�л�ȡ��ԭ�û�����״̬���ϣ�");
        }
    }
    
    //��ȡ����״̬���ҵ�����
    CFmlBuf bufTradeSvcPara;
    try
    {
        dao.Clear();
        dao.SetParam(":VTRADE_TYPE_CODE",strTradeTypeCode);
        dao.SetParam(":VBRAND_CODE",strBrandCode);
        dao.SetParam(":VNET_TYPE_CODE",strNetTypeCode);
        dao.SetParam(":VPRODUCT_ID",strProductId);
        dao.SetParam(":VEPARCHY_CODE",strEparchyCode);
        userSvcStateCount=dao.jselect(bufTradeSvcPara, "SEL_BY_PK","TD_S_TRADE_SVCSTATE");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "ҵ��Ǽ�ǰ���:��ȡ��������������");
    }
    if (userSvcStateCount==0)
    {
        THROW_C(CRMException, CHECKBEFORETRADE_ERR, "ҵ��Ǽ�ǰ���:����������û�����ã�");
    }
    if (userSvcStateCount>0)
    {
        for(int i=0;i<userSvcStateCount;i++)
        {
            
            
            for(int j=0;j<userSvcCount;j++)
            {
            	if (bufUserSvcstate.GetString("SERVICE_ID",j)==bufTradeSvcPara.GetString("SERVICE_ID",i)&&bufTradeSvcPara.GetString("OLD_STATE_CODE",i)=="%")
            		return;
            	if (bufUserSvcstate.GetString("SERVICE_ID",j)==bufTradeSvcPara.GetString("SERVICE_ID",i)&&
            		bufUserSvcstate.GetString("STATE_CODE",j)==bufTradeSvcPara.GetString("OLD_STATE_CODE",i))
            	{
            		return;
            	}
            	strSvcCode=bufUserSvcstate.GetString("SERVICE_ID",j);
            	strSvcstateCode=bufUserSvcstate.GetString("STATE_CODE",j);
            }
        }
		int iCount=0;
		string strSvcName;
		string strSvcstateName;
		CFmlBuf  tempBuf;
		try
		{
			dao.Clear();
			dao.SetParam(":VNET_TYPE_CODE",strNetTypeCode);
			dao.SetParam(":VSERVICE_ID",strSvcCode);
			dao.SetParam(":VSTATE_CODE",strSvcstateCode);
			iCount=dao.jselect(tempBuf, "td_b_service1","NormalPara");
		}
		catch(Exception &ex)
		{
			THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "ҵ��Ǽ�ǰ���:��ȡ����״̬��������");
		}
		if (iCount>0)
		{
			strSvcName=tempBuf.GetString("PARACODE");
			strSvcstateName=tempBuf.GetString("PARANAME");
			THROW_C_P2(CRMException, CHECKBEFORETRADE_ERR, "�û����ڵķ���״̬��[%s.%s],���ܰ����ҵ��", strSvcName.c_str(), strSvcstateName.c_str());
		}
		else
			THROW_C(CRMException, CHECKBEFORETRADE_ERR, "�û�����״̬Ϊ��,ҵ���޷�������");
    }
}
/**
 *  �û�ѡ���µĲ�Ʒ�������ж�
*/
void TradeCheckBeforeTrade::CheckAfterChangeProduct(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
    LOG_TRACE(logTradeCheckBeforeTrade, "����CheckAfterChangeProduct����");

    string strUserId = inBuf.GetString("USER_ID");     //�û���ʶ
    int iOldProductId = inBuf.GetInt("PRODUCT_ID_A");  //�ɲ�Ʒ��ʶ
    int iNewProductId = inBuf.GetInt("PRODUCT_ID_B");  //�²�Ʒ��ʶ
    string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE"); 
    
    string strCodingStr="";

    CheckAfterChangeProduct(strUserId, iOldProductId, iNewProductId, strCodingStr, strEparchyCode);

    outBuf.SetString("X_CHECK_INFO", strCodingStr);

    LOG_TRACE(logTradeCheckBeforeTrade, "�˳�CheckAfterChangeProduct����");
}

void TradeCheckBeforeTrade::CheckAfterChangeProduct(const string &strUserId, const int &iOldProductId, const int &iNewProductId, string &strCodingStr, const string &strEparchyCode)
{
    /*StrTable stbOutPrompt;
    stbOutPrompt.AddTable("Prmp", 2);
    StaticDAO& dao = StaticDAO::getDAO();
    int iCnt(0);
    
    CFmlBuf bufTag;
    try
    {
        dao.Clear();
        dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
        dao.SetParam(":VTAG_CODE", "PUB_INF_PROVINCE");
        dao.SetParam(":VSUBSYS_CODE", "PUB");
        dao.SetParam(":VUSE_TAG", "0");
        dao.jselect(bufTag, "SEL_ALL_BY_TAGCODE","TD_S_TAG");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ��ǰʡ�������");
    }
    
    if (bufTag.GetString("TAG_INFO") == "TJIN")  //�������⴦��
    {
        //�жϺ����Ƿ�Ϊ�׿�����
        CFmlBuf bufUserRelation;
        try
        {
            dao.Clear();
            dao.SetParam(":VPARTITION_ID", strUserId.substr(strUserId.length()-4));
            dao.SetParam(":VUSER_ID_B", strUserId);
            dao.SetParam(":VRELATION_TYPE_CODE", "11");
            dao.SetParam(":VROLE_CODE_B", "1");
            iCnt = dao.jselect(bufUserRelation, "SEL_IS_RELATION_ROLEB","TF_F_RELATION_UU");
        
        }
        catch(Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "�²�Ʒ���:��ȡ�û���ϵ����");
        }
        
        if (iCnt > 0)
        {
            //�ж��²�Ʒ�Ƿ��������׿�����
            CFmlBuf bufProductLimit;
            try
            {
                dao.Clear();
                dao.SetParam(":VPRODUCT_ID_A", 5001); //�׿�
                dao.SetParam(":VPRODUCT_ID_B", iNewProductId);
                dao.SetParam(":VLIMIT_TAG", "2");
                iCnt = dao.jselect(bufProductLimit, "SEL_EXISTS_AB_LIMIT","TD_S_PRODUCTLIMIT");
            }
            catch(Exception &ex)
            {
                THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "�²�Ʒ���:��ȡ�û���ϵ����");
            }
        
            if (iCnt == 0)  //�²�Ʒ�������׿�������ȡ�������б�
            {
                CFmlBuf bufVUserRelation;
                int iUserRelationCount;
                try
                {
                    dao.Clear();
                    dao.SetParam(":VUSER_ID_A", bufUserRelation.GetString("USER_ID_A"));
                    dao.SetParam(":VRELATION_TYPE_CODE", "11");
                    dao.SetParam(":VROLE_CODE_B", "2");
                    iUserRelationCount = dao.jselect(bufVUserRelation, "SEL_USER_ROLEA","TF_F_RELATION_UU");
                }
                catch(Exception &ex)
                {
                    THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "�²�Ʒ���:��ȡ�û���ϵ����");
                }
                string strSNB;
                for (int i=0;i<iUserRelationCount;i++)
                {
                    if (i!=0) strSNB+="��";
                    strSNB+=bufVUserRelation.GetString("SERIAL_NUMBER_B",i);
                }
                stbOutPrompt.AddField("Prmp", "1");  //1����ʾ���� 2����ʾ�ж� 3��ѯ��
                string strTemp = "�ú����Ǻ��롾"+strSNB+"�����������²�Ʒ�������׿��������²�Ʒ��Ч�������뽫ȡ������";
                stbOutPrompt.AddField("Prmp", strTemp.c_str());
            }
        }
    }
    
    if (stbOutPrompt.tables[0].rowCount > 0)
    {
        if ( stbOutPrompt.ToString(strCodingStr) != 0)
        {
            THROW_C(CRMException, CHECKBEFORETRADE_ERR, "�²�Ʒ���:�����ִ������");
        }
    }*/
}

/**
 *  ҵ�����У�飬һ��˫���û��������У��
*/
void TradeCheckBeforeTrade::CheckOneCardNCodesSn(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
    LOG_TRACE(logTradeCheckBeforeTrade, "����CheckOneCardNCodesSn����");

    string strUserIdA = "";     //�û���ʶ
    string strUserIdB = "";
    if (inBuf.IsFldExist("PARA_VALUE1"))
    {
        strUserIdA = inBuf.GetString("PARA_VALUE1");
    }
    if (inBuf.IsFldExist("PARA_VALUE5"))
    {
        strUserIdB = inBuf.GetString("PARA_VALUE5");
    }
        
    string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE"); 
    CheckOneCardNCodesSn(strUserIdA, strEparchyCode, strUserIdB);
    
    LOG_TRACE(logTradeCheckBeforeTrade, "�˳�CheckOneCardNCodesSn����");
}

void TradeCheckBeforeTrade::CheckOneCardNCodesSn(const string &strUserIdA, const string &strEparchyCode, const string &strUserIdB)
{
    int iCount = -1;
    int iCountA = -1;
    int iCountB = -1;
    CFmlBuf bufUserA, bufUserB;
    StaticDAO& dao = StaticDAO::getDAO();
    try
    {
        dao.Clear();
        dao.SetParam(":VUSER_ID", strUserIdA);
        iCountA = dao.jselect(bufUserA,"SEL_BY_PK", "TF_F_USER");
    }
    catch (Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "ҵ������ǰ�����ж�:��ȡ�û��������ϳ���");
    }
    
    try
    {
        dao.Clear();
        dao.SetParam(":VUSER_ID", strUserIdB);
        iCountB = dao.jselect(bufUserB, "SEL_BY_PK","TF_F_USER");
    }
    catch (Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "ҵ������ǰ�����ж�:��ȡ�û��������ϳ���");
    }
    if (iCountA <=0 && iCountB <=0)
    {
        THROW_C(CRMException, CHECKBEFORETRADE_ERR, "ҵ������ǰ�����ж�:��ȡ�û����������ݣ�");    
    }
    iCount = -1;
    CFmlBuf bufCommparaA;
    try
    {       
       dao.Clear();
       dao.SetParam(":VSUBSYS_CODE", "CSM");
       dao.SetParam(":VPARAM_ATTR", "1014");
       dao.SetParam(":VPARAM_CODE", bufUserA.GetString("BRAND_CODE"));//Ʒ��
       dao.SetParam(":VPARA_CODE3", (int)bufUserA.GetInt("PRODUCT_ID"));//��Ʒ
       dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
       iCount = dao.jselect(bufCommparaA, "SEL2_PK_TD_S_COMMPARA","TD_S_COMMPARA");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "ҵ������ǰ�����ж�:��ѯͨ�ò��������");
    }
   
    if(iCount > 0)
    {
		if (bufCommparaA.GetString("PARA_CODE2") == "1")
		{
				THROW_C(CRMException, CHECKBEFORETRADE_ERR, "ҵ������ǰ�����ж�:�û������벻����Ϊ��Ʒ��["+bufCommparaA.GetString("PARAM_NAME")+"]��");    		
		} 
		if (bufCommparaA.GetString("PARA_CODE2") == "6")
		{
				THROW_C(CRMException, CHECKBEFORETRADE_ERR, "ҵ������ǰ�����ж�:�û������벻����Ϊ��Ʒ��["+bufCommparaA.GetString("PARAM_NAME")+"]��");    		
		} 
		if (bufCommparaA.GetString("PARA_CODE2") == "3" && atoi(string(bufCommparaA.GetString("PARA_CODE3")).c_str()) == (int)bufUserA.GetInt("PRODUCT_ID"))
		{
				THROW_C(CRMException, CHECKBEFORETRADE_ERR, "ҵ������ǰ�����ж�:�û������벻����Ϊ��Ʒ��["+bufCommparaA.GetString("PARAM_NAME")+"]�Ĵ˲�Ʒ["+bufCommparaA.GetString("PARA_CODE4")+"]��");    		
		}   
    }
    
    iCount = -1;
    CFmlBuf bufCommparaB;
    try
    {       
       dao.Clear();
       dao.SetParam(":VSUBSYS_CODE", "CSM");
       dao.SetParam(":VPARAM_ATTR", "1014");
       dao.SetParam(":VPARAM_CODE", bufUserB.GetString("BRAND_CODE"));//Ʒ��
       dao.SetParam(":VPARA_CODE3", (int)bufUserB.GetInt("PRODUCT_ID"));//��Ʒ
       dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
       iCount = dao.jselect(bufCommparaB, "SEL2_PK_TD_S_COMMPARA","TD_S_COMMPARA");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "ҵ������ǰ�����ж�:��ѯͨ�ò��������");
    }
   
    if(iCount > 0)
    {
		if (bufCommparaB.GetString("PARA_CODE2") == "2")
		{
				THROW_C(CRMException, CHECKBEFORETRADE_ERR, "ҵ������ǰ�����ж�:�û������벻����Ϊ��Ʒ��["+bufCommparaB.GetString("PARAM_NAME")+"]��");    		
		}
		if (bufCommparaB.GetString("PARA_CODE2") == "6")
		{
				THROW_C(CRMException, CHECKBEFORETRADE_ERR, "ҵ������ǰ�����ж�:�û������벻����Ϊ��Ʒ��["+bufCommparaB.GetString("PARAM_NAME")+"]��");    		
		}
		if (bufCommparaB.GetString("PARA_CODE2") == "4" && atoi(string(bufCommparaB.GetString("PARA_CODE3")).c_str()) == (int)bufUserB.GetInt("PRODUCT_ID"))
		{
				THROW_C(CRMException, CHECKBEFORETRADE_ERR, "ҵ������ǰ�����ж�:�û������벻����Ϊ��Ʒ��["+bufCommparaB.GetString("PARAM_NAME")+"]�Ĵ˲�Ʒ["+bufCommparaB.GetString("PARA_CODE4")+"]��");    		
		}
    }
    if (bufCommparaA.GetString("PARA_CODE1") != "" && bufCommparaB.GetString("PARA_CODE1") != "")
    {
		if (atoi(string(bufCommparaA.GetString("PARA_CODE1")).c_str()) < atoi(string(bufCommparaB.GetString("PARA_CODE1")).c_str()))
		{
				THROW_C(CRMException, CHECKBEFORETRADE_ERR, "ҵ������ǰ�����ж�:�û�������Ʒ��["+bufCommparaA.GetString("PARAM_NAME")+"]���𲻿��Ե��ڸ�����Ʒ��["+bufCommparaB.GetString("PARAM_NAME")+"]����");    				
		}
  	}
  	    
}

/**
 *  ��ȡ���һ����Ч���������ۼ�¼
*/
void TradeCheckBeforeTrade::GetUserLastValidBindSaleInfo(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
    LOG_TRACE(logTradeCheckBeforeTrade, "����GetUserLastValidBindSaleInfo����");
    StaticDAO& dao = StaticDAO::getDAO();    
    try
    {
	    string strUserId = inBuf.GetString("USER_ID"); 	
		int iCount;
		CFmlBuf bufUserPurchase;
	    iCount=GetUserLastValidBindSaleInfo(bufUserPurchase, strUserId);
    
    	if(iCount>0)
    	{
		   
		    //��ȡϵͳʱ��
		    DualDAO daoDual;
		    string strSysDate;
		    daoDual.Clear();
		    daoDual.jselect(strSysDate,"GET_SYSDATE");
		    //yyyy-mm-dd hh24:mi:ss
		    string strDate1,strDate2;
		    strDate1 = string(bufUserPurchase.GetString("END_DATE")).substr(0,4) + string(bufUserPurchase.GetString("END_DATE")).substr(5,2);
		    strDate2 = strSysDate.substr(0,4) + strSysDate.substr(5,2);
		    //modify by digy@20070530 ɾ�� = ���������������ɰ���ʱ���빺��Э����ֹʱ����ͬһ�µ�
		    //Э�黹û���ڵ��û����ж��ΰ� 
		    if(atol(strDate1.c_str()) <  atol(strDate2.c_str())) 
		    {
		        outBuf.SetInt("X_RECORDNUM",0);
		    }
		    else
		    {
		        outBuf.SetInt("X_RECORDNUM",iCount);
		        //add by digy@20070507 start ��ȡ��ǰ�����빺���û���ʼ���� Ϊ�����������ӿ���
		    	int iCurAcycid; // ��ȡ��ǰ����
		    	daoDual.Clear();
		    	daoDual.jselect(iCurAcycid,"GET_ACYCID");
		    	
		    	int iStartAcycid; //��ȡ�����û���ʼ���� 
		    	daoDual.Clear();
		    	daoDual.SetParam(":VDATE", bufUserPurchase.GetString("START_DATE"));
		    	daoDual.jselect(iStartAcycid,"GET_ACYCID_BY_DATE");
		    	
		    	//ֻȡ�û���ǰ���ڵ���һ���ڵ��ۻ�����
		    	outBuf.SetInt("ACYC_ID",iCurAcycid-1);
		    	outBuf.SetInt("START_ACYC_ID",iStartAcycid);
		    	
 	            CFmlBuf ibuf ;
 	            int iRowCount = 0;
	            try
	            {
	            	dao.Clear();
	            	ibuf.ClearFmlValue() ;
	            	dao.SetParam(":VBINDSALE_ATTR",bufUserPurchase.GetString("BINDSALE_ATTR"));
	            	dao.SetParam(":VTAG","0");
	            	iRowCount = dao.jselect(ibuf,"SEL_TD_B_BINDSALE","TD_S_COMMPARA");		
	            }
                catch (Exception &ex)
                {
                    THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "��ȡ������������");
                }
                if( iRowCount == 0 )
                {
                	THROW_C(CRMException, CHECKBEFORETRADE_ERR, "���ݹ�������,��ȡ�������������ݣ�");
                }
                else 
                {
                	 if( ibuf.GetString("PARA_CODE6") == "" )
                	 	outBuf.SetInt("DISN_TYPE_CODE",0) ;
                	 else 
                	 	outBuf.SetInt("DISN_TYPE_CODE",atoi(ibuf.GetString("PARA_CODE6").c_str()) );
                	 //THROW_C_P2(CRMException, CHECKBEFORETRADE_ERR, "digy[%d],[%s]",outBuf.GetInt("DISN_TYPE_CODE"),objUserPurchase.bindsaleAttr); 			
                }
		    	//add by digy@20070507 end 
			    outBuf.CoverLine(0,bufUserPurchase,0);
		    }    		
    	}
    	else
    	{
	        outBuf.SetInt("X_RECORDNUM",0);
    	}
        outBuf.SetInt("X_RESULTCODE", 0);
        outBuf.SetString("X_RESULTINFO", "��ȡ��Ч�������ۼ�¼���ϳɹ���");
        outBuf.SetString("X_REMARK1","") ; //����ǰ̨Ϊ��ֹЭ�����������
	    outBuf.SetString("X_REMARK2","") ; //����ǰ̨Ϊ��ֹЭ�����������
    }
    catch(Exception &e)
    {
        logTradeCheckBeforeTrade.alert(e);
        outBuf.SetInt("X_RECORDNUM",-1);
        outBuf.SetInt("X_RESULTCODE", e.getCode());
        outBuf.SetString("X_RESULTINFO", "��ȡ��Ч�������ۼ�¼����:" + e.toString());
    }
    
    LOG_TRACE(logTradeCheckBeforeTrade, "�˳�GetUserLastValidBindSaleInfo����");
}

int TradeCheckBeforeTrade::GetUserLastValidBindSaleInfo(CFmlBuf &bufUserPurchase, const string &strUserId)
{
    int iCount = 0;
    StaticDAO& dao = StaticDAO::getDAO();
    try
    {
        dao.Clear();
        dao.SetParam(":VUSER_ID", strUserId);
        iCount = dao.jselect(bufUserPurchase, "SEL_BY_LASTVALID","TF_F_USER_PURCHASE");
    }
    catch (Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "ҵ������ǰ�����ж�:��ȡ�û���Ч�����������ϳ���");
    }
    
    return iCount;
}

//��������������� QAM_YEARFEEQRY_OS ��ȡ���û�����Э���Ѷ�󣬴������񷵻ؽ��
void TradeCheckBeforeTrade::CheckBindSaleAcctReturn(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "����CheckBindSaleAcctReturn����");

    float fUsedFee = 0;
    //int iFoundCount = 0;
	float fTotalFee = 0;
	float fCurrFee  = 0;
    
    string strAddupValue;

	//��ȡ������ʱ�䣬�Լ����Ѷ��
    int iAcctCount = inBuf.GetInt("X_RECORDNUM");
	string strUserId = inBuf.GetString("USER_ID");
	string strConsumeValue = inBuf.GetString("CONSUME_VALUE");
	fTotalFee = atof(strConsumeValue.c_str())/(float)100;
	string strEndDate = inBuf.GetString("END_DATE");
	string strStartDate = inBuf.GetString("START_DATE");
	string strBindSaleAttr = inBuf.GetString("BINDSALE_ATTR");
    
	if (iAcctCount == 0 || !inBuf.IsFldExist("ADDUP_VALUE") )//δ��ѯ����Э�ۼ�����ֵ,����ʵЧʱ�����ж�
	{
		THROW_C_P2(CRMException, CHECKBEFORETRADE_ERR, "δ���������۽�������[%s],δ�����Ѷ��,���ܰ�����������ҵ��:���Ѷ��[%.2f],�Ѿ����� 0 Ԫ",strEndDate.c_str(),fTotalFee);
	}
	
	strAddupValue = inBuf.GetString("ADDUP_VALUE");
	fUsedFee = atof(strAddupValue.c_str())/100.00 ;
	if (fUsedFee >= fTotalFee ) 
	{
		outBuf.SetInt("X_RESULTCODE",0);
		outBuf.SetString("X_RESULTINFO","Acct Purchase check ok!");
		outBuf.SetString("X_REMARK1",strBindSaleAttr) ; //����ǰ̨Ϊ��ֹЭ�����������
	    outBuf.SetString("X_REMARK2",strStartDate) ; //����ǰ̨Ϊ��ֹЭ�����������
		//iFoundCount ++;
		return;
	}
	//δ����,���Ѷ��Ҳδ��,����
	fCurrFee  = fUsedFee ;
    outBuf.SetString("X_REMARK1","") ; //����ǰ̨Ϊ��ֹЭ�����������
    outBuf.SetString("X_REMARK2","") ; //����ǰ̨Ϊ��ֹЭ�����������
    LOG_TRACE(logTradeCheckBeforeTrade, "�˳�CheckBindSaleAcctReturn����");
    THROW_C_P3(CRMException, CHECKBEFORETRADE_ERR, "δ���������۽�������[%s],δ�����Ѷ��,���ܰ�����������ҵ��:���Ѷ��:%.2fԪ,�Ѿ�����:%.2fԪ",strEndDate.c_str(),fTotalFee,fCurrFee);
}

//������������������͵ļ�¼�Ƿ���
void TradeCheckBeforeTrade::CheckOtherBindSale(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "����CheckOtherBindSale����");

	string strBindSaleAttr = inBuf.GetString("BINDSALE_ATTR");
	int iDepositCode = inBuf.GetInt("DEPOSIT_CODE");
	string strUserId = inBuf.GetString("USER_ID");
	string strAcctId = inBuf.GetString("ACCT_ID");
    StaticDAO& dao = StaticDAO::getDAO();
    CFmlBuf bufRecordCount;
	if(strBindSaleAttr=="1" && iDepositCode!=-1)  //2��Ԥ��0Ԫ����
	{
		int iCount=0;
	    try
	    {
		    dao.Clear();
	        dao.SetParam(":VACCT_ID", strAcctId);
	        dao.SetParam(":VDEPOSIT_CODE", iDepositCode);
	        dao.jselect(bufRecordCount,"ExistsAcctIdDepositCodeMoney","TD_S_CPARAM");
	        iCount = bufRecordCount.GetInt("RECORDCOUNT");
	    }
	    catch (Exception &ex)
	    {
	        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "��ȡָ���ʻ�ָ�����۽�����");
	    }
	    if(iCount > 0)
	    {
		    THROW_C(CRMException, CHECKBEFORETRADE_ERR, "Э��Ԥ���δ������,����δ���������۽�������,���ܰ�����������ҵ��");
	    }
	}	
    else
	{
	    THROW_C(CRMException, CHECKBEFORETRADE_ERR, "δ���������۽�������,���ܰ�����������ҵ��");
	}
    outBuf.SetInt("X_RESULTCODE", 0);
    outBuf.SetString("X_RESULTINFO", "���������������ͼ�¼�Ƿ���У��ɹ�!");
    outBuf.SetString("X_REMARK1","") ; //����ǰ̨Ϊ��ֹЭ�����������
    outBuf.SetString("X_REMARK2","") ; //����ǰ̨Ϊ��ֹЭ�����������
    LOG_TRACE(logTradeCheckBeforeTrade, "�˳�CheckOtherBindSale����");
}
void TradeCheckBeforeTrade::GetParaForAddupValue(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
    LOG_TRACE(logTradeCheckBeforeTrade, "����GetParaForAddupValue����");
	string strUserId = inBuf.GetString("USER_ID"); 
	int iTradeTypeCode = inBuf.GetInt("TRADE_TYPE_CODE"); 
	int iCount = 0;
    DualDAO daoDual;
    CFmlBuf bufUserPurchase;
    StaticDAO& dao = StaticDAO::getDAO();
    try
    {
        dao.Clear();
        dao.SetParam(":VUSER_ID", strUserId);
        dao.SetParam(":VTRADE_TYPE_CODE", iTradeTypeCode);
        iCount = dao.jselect(bufUserPurchase, "SEL_BY_USERID_BINDMODE","TF_F_USER_PURCHASE");
    }
    catch (Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "ҵ������ǰ�����ж�:��ȡ�û���Ч�����������ϳ���");
    }
    
    if(iCount>0)
    {
		//add by digy@20070507 start ��ȡ��ǰ�����빺���û���ʼ���� Ϊ�����������ӿ���
		int iCurAcycid; // ��ȡ��ǰ����
		daoDual.Clear();
		daoDual.jselect(iCurAcycid,"GET_ACYCID");
		
		int iEndAcycid;
		daoDual.Clear();
		daoDual.SetParam(":VDATE", bufUserPurchase.GetString("END_DATE"));
		daoDual.jselect(iEndAcycid,"GET_ACYCID_BY_DATE");
		
		int iStartAcycid; //��ȡ�����û���ʼ���� 
		daoDual.Clear();
		daoDual.SetParam(":VDATE", bufUserPurchase.GetString("START_DATE"));
		daoDual.jselect(iStartAcycid,"GET_ACYCID_BY_DATE");
		
		//���ڴ���Э�鵽�������º���û���Ѻ���������жϵ�ǰ�������������
		if( iEndAcycid >= iCurAcycid )
			outBuf.SetInt("ACYC_ID",iCurAcycid-1);
		else 
		   outBuf.SetInt("ACYC_ID",iEndAcycid); 
		   
		outBuf.SetInt("START_ACYC_ID",iStartAcycid);

 		CFmlBuf ibuf ;
 		int iRowCount = 0;
		try
		{
			 dao.Clear();
			 ibuf.ClearFmlValue() ;
			 dao.SetParam(":VBINDSALE_ATTR",bufUserPurchase.GetString("BINDSALE_ATTR"));
			 dao.SetParam(":VTAG","0");
			 iRowCount = dao.jselect(ibuf,"SEL_TD_B_BINDSALE","TD_S_COMMPARA");		
		}
   	    catch (Exception &ex)
   	    {
   	       	THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "��ȡ������������");
   	    }
   	    if( iRowCount == 0 )
   	    {
   	    	THROW_C(CRMException, CHECKBEFORETRADE_ERR, "���ݹ�������,��ȡ�������������ݣ�");
   	    }
   	    else 
   	    {
   	    	if( ibuf.GetString("PARA_CODE6") == "" )
   	     		outBuf.SetInt("DISN_TYPE_CODE",0) ;
   	     	else 
   	     		outBuf.SetInt("DISN_TYPE_CODE",atoi(ibuf.GetString("PARA_CODE6").c_str()) );
   	    }
		//add by digy@20070507 end 
     	outBuf.CoverLine(0,bufUserPurchase,0);     		
    }
    outBuf.SetInt("RECORD_COUNT", iCount );
    outBuf.SetInt("X_RESULTCODE", 0);
    outBuf.SetString("X_RESULTINFO", "��ȡ�û�������Ϣ�ɹ���");
    LOG_TRACE(logTradeCheckBeforeTrade, "�˳�GetParaForAddupValue����");
}
//��������������� QAM_YEARFEEQRY_OS ��ȡ���û�����Э���Ѷ�󣬴������񷵻ؽ��(����Ѻ����)
void TradeCheckBeforeTrade::CheckAcctReturn(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "����CheckAcctReturn����");

    float fUsedFee = 0;
    //int iFoundCount = 0;
	float fTotalFee = 0;
	float fCurrFee  = 0;
    string strAddupValue;

	//�Լ����Ѷ��
    int iAcctCount = inBuf.GetInt("X_RECORDNUM");
	string strConsumeValue = inBuf.GetString("CONSUME_VALUE");
	fTotalFee = atof(strConsumeValue.c_str())/(float)100;

	if (iAcctCount == 0)//δ��ѯ����Э�ۼ�����ֵ
	{
		THROW_C_P1(CRMException, CHECKBEFORETRADE_ERR, "δ�����Ѷ��,���ܰ����ҵ��:���Ѷ��[%.2f],�Ѿ����� 0 Ԫ",fTotalFee);
	}
	
	if( inBuf.IsFldExist("ADDUP_VALUE"))
		strAddupValue = inBuf.GetString("ADDUP_VALUE");
	else
		strAddupValue = "0.00" ;
	fUsedFee = atof(strAddupValue.c_str())/100.00 ;
	
	if (fUsedFee >= fTotalFee ) 
	{
		outBuf.SetInt("X_RESULTCODE",0);
		outBuf.SetString("X_RESULTINFO","Acct Purchase check ok!");
		//iFoundCount ++;
		return;
	}
	//���Ѷ��Ҳδ��,����
	fCurrFee  = fUsedFee ;
    LOG_TRACE(logTradeCheckBeforeTrade, "�˳�CheckAcctReturn����");
    THROW_C_P2(CRMException, CHECKBEFORETRADE_ERR, "δ�����Ѷ��,���ܰ����ҵ��:���Ѷ��:%.2fԪ,�Ѿ�����:%.2fԪ",fTotalFee,fCurrFee);
}

//У������ջ���ҵ������ add by qiumb@2007-03-05
void TradeCheckBeforeTrade::TradeCheck_CheckScoreTradeLimit(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "���� TradeCheck_CheckScoreTradeLimit ����");
    StaticDAO& dao = StaticDAO::getDAO();
    string strTradeTypeCode = inBuf.GetString("TRADE_TYPE_CODE");
    if (strTradeTypeCode == "330" || strTradeTypeCode == "350" )
    {
        string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");
        int iCount = 0;
        CFmlBuf bufTag;
        try
        {
            dao.Clear();
            dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
            dao.SetParam(":VTAG_CODE", "CS_SCOREEXCHANGE_DATELIMIT");
            dao.SetParam(":VSUBSYS_CODE", "CSM");
            dao.SetParam(":VUSE_TAG", "0");
            iCount = dao.jselect(bufTag, "SEL_ALL_BY_TAGCODE","TD_S_TAG");
        }
        catch(Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ����ҵ��������������ݣ�");
        }
        if(iCount > 0)
        {
            string sDateLimitStr = bufTag.GetString("TAG_INFO");
            string strSysdate;
            DualMgr objDualMgr;
        	objDualMgr.GetSysDate(strSysdate);
            string sCurMM = strSysdate.substr(8,2);
            if (sDateLimitStr.find("|"+sCurMM+"|") != string::npos )
            {
                THROW_C(CRMException, CS_TRADELIMIT_ERR, "TradeCheck_CheckScoreTradeLimit:ϵͳ�����ս�ֹ���������ҵ��");
            }
        }        
    }
    if ( strTradeTypeCode == "330"  )
    {
     //   //��ȡ�ۼƻ���
    	//JTable *pTab = jdb::getConnect()->CreateJTable();
    	//string validScoreSum,allScoreSum;
    	//int iValidScoreSum,iAllScoreSum;
    	//stringstream ss;

    	//try
    	//{
    	//    pTab->Clear();
    	//	pTab->AddSql("SELECT NVL(TO_CHAR(SUM(SCORE_VALUE)),'0') SCORE_VALUE\
     //                         FROM TF_F_USER_SCORE \
     //                        WHERE PARTITION_ID = MOD(TO_NUMBER(:VUSER_ID), 10000) \
     //                          AND USER_ID = TO_NUMBER(:VUSER_ID) \
     //                          AND SYSDATE <= NVL(END_DATE, SYSDATE + 1)");
    	//	pTab->SetParam(":VUSER_ID", inBuf.GetString("USER_ID"));
    	//	pTab->ExecSelect();
    	//	while(pTab->Fetch())
    	//	{
    	//		validScoreSum = pTab->GetString("SCORE_VALUE");
    	//	}
    	//    pTab->Clear();
    	//	pTab->AddSql("SELECT NVL(TO_CHAR(SUM(SCORE_SUM)),'0') SCORE_SUM\
     //                         FROM TF_F_USER_SCORE \
     //                        WHERE PARTITION_ID = MOD(TO_NUMBER(:VUSER_ID), 10000) \
     //                          AND USER_ID = TO_NUMBER(:VUSER_ID) ");
    	//	pTab->SetParam(":VUSER_ID", inBuf.GetString("USER_ID"));
    	//	pTab->ExecSelect();
    	//	while(pTab->Fetch())
    	//	{
    	//		allScoreSum = pTab->GetString("SCORE_SUM");
    	//	}
    	//	delete pTab;
    	//}
    	//catch(Exception &e)
    	//{
    	//    delete pTab;
    	//	THROW_AGC(CRMException, e, Grade::WARNING, GETUSERSCOREINFO_ERR, "��ȡ�û��ۼƻ����쳣��");
    	//}
    	//
    	//ss.clear();
     //   ss<<validScoreSum;
     //   ss>>iValidScoreSum;
     //   ss.clear();
     //   ss<<allScoreSum;
     //   ss>>iAllScoreSum;
     //   if ( iValidScoreSum <= 0)
     //   {
     //   		//zhujl
     //      // THROW_C(CRMException, CHECKBEFORETRADE_ERR, "TradeCheck_CheckScoreTradeLimit:�û���ǰû�п��û��֣�ҵ�����ޣ�");
     //   }
        
        //int iCount = 0;
        //��ȡ��Ʒ���ܲ�Ʒ��
        //CFmlBuf bufProduct;
        //try
        //{
       //     dao.Clear();
        //    dao.SetParam(":VPRODUCT_ID",inBuf.GetString("PRODUCT_ID"));
       //     iCount = dao.jselect(bufProduct,"SEL_BY_PK","TD_B_PRODUCT");
       // }
       // catch(Exception &e)
       // {
       //   THROW_AGC(CRMException, e, Grade::WARNING, GETNORMALPARA_ERR, "��ȡ��Ʒ��Ϣʧ�ܣ�");
       // }
       // if ( iCount < 1)
        //{
        //    THROW_C(CRMException, CHECKBEFORETRADE_ERR, "TradeCheck_CheckScoreTradeLimit:��ȡ��Ʒ��Ϣ�����ݣ�");
       // }
        
        //modified by qiumb@20071009  �ж��Ƿ�ﵽ��Ʒ�Ļ��ֶһ��ż�
       // int iCriticalScoreValue = 3000;
        //ss.clear();
        //ss<<bufProduct.GetString("SCORE_VALUE");
        //ss>>iCriticalScoreValue;
		//if ( iCriticalScoreValue > iAllScoreSum)
        //{
        //    THROW_C_P4(CRMException, CHECKBEFORETRADE_ERR, "TradeCheck_CheckScoreTradeLimit:�û���ǰ���ۼƻ���[%d]δ�ﵽ��ǰ��Ʒ[%d:%s]�Ļ��ֶһ��ż�[%s]��ҵ�����ޣ�",iAllScoreSum, bufProduct.GetInt("PRODUCT_ID"), string(bufProduct.GetString("PRODUCT_NAME")).c_str() ,string(bufProduct.GetString("SCORE_VALUE")).c_str());
       // }
    	
	LOG_TRACE(logTradeCheckBeforeTrade, "�˳� TradeCheck_CheckScoreTradeLimit ����");	
    }
}
//У�� �в�Ʒ������Ƶ�ҵ����ʾ��Ʒ��������Ϣ add by digy@2007-05-28
void TradeCheckBeforeTrade::TradeCheck_CheckHintChgProduct(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "���� TradeCheck_CheckHintChgProduct ����");

    string strTradeTypeCode = inBuf.GetString("TRADE_TYPE_CODE");
    string strBrandCode = inBuf.GetString("BRAND_CODE"); 
    string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");
    string strUserId = inBuf.GetString("USER_ID");
    int iCount = 0 ;
    StaticDAO& dao = StaticDAO::getDAO();
	CFmlBuf  bufTradetypeLimit;
	try
	{
	    dao.Clear();
	    dao.SetParam(":VTRADE_TYPE_CODE", strTradeTypeCode);
	    dao.SetParam(":VLIMIT_TRADE_TYPE_CODE", "110");
	    dao.SetParam(":VBRAND_CODE", strBrandCode);
	    dao.SetParam(":VLIMIT_ATTR", "0");
	    dao.SetParam(":VLIMIT_TAG", "0");
	    dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
	    iCount = dao.jselect(bufTradetypeLimit, "SEL_BY_TRADE_TYPE","TD_S_TRADETYPE_LIMIT");
	}
	catch (Exception &ex)
	{
	    THROW_AGC(CRMException,ex,Grade::WARNING,CHECKBEFORETRADE_ERR,"TradeCheck_CheckHintChgProduct:��ѯ�û�δ�깤������ҵ����Ϣ����");
	}
	if( iCount > 0)
	{
		
		iCount = 0 ;
	    CFmlBuf bufTrade;
	    try
	    {
	    	dao.Clear();
	    	dao.SetParam(":VUSER_ID",strUserId);	
	    	dao.SetParam(":VTRADE_TYPE_CODE","110");
	    	dao.SetParam(":VEXEC_TIME","");
	    	dao.SetParam(":VCANCEL_TAG","0");
	    	iCount = dao.jselect(bufTrade, "SEL_BY_USER","TF_B_TRADE");
		}	
		catch (Exception &ex)
	    {
	        THROW_AGC(CRMException,ex,Grade::WARNING,CHECKBEFORETRADE_ERR,"TradeCheck_CheckHintChgProduct:��ѯ�û�ԤԼ��Ʒ���̨����Ϣ����");
	    }
	    
	    //abc������2
	    /*
	    if( iCount > 0 )
		{
		   //���ԭ��Ʒ��Ϣ
		   int iOldPrdId ;
		   if( bufTrade.GetString("RSRV_STR2") == "" ) iOldPrdId = 0;
		   else iOldPrdId = atoi(string(bufTrade.GetString("RSRV_STR2")).c_str()) ;
		   char chOldPrdId[10];
           sprintf(chOldPrdId,"%d",iOldPrdId );
           string strOldPrdName = CParamDAO::getParam("ProductName", chOldPrdId);	
           
           //����²�Ʒ��Ϣ
           int iNewPrdId ;
		   iNewPrdId = (int)bufTrade.GetInt("PRODUCT_ID") ; 
		   char chNewPrdId[10];
           sprintf(chNewPrdId,"%d",iNewPrdId );
           string strNewPrdName = CParamDAO::getParam("ProductName", chNewPrdId);
           
           string strAcceptDate = bufTrade.GetString("ACCEPT_DATE") ;
           string strExceTime = bufTrade.GetString("EXEC_TIME") ;	
           
           string strInfo = "ҵ������:�û�"+strAcceptDate+"ԤԼ����Ʒ:" + strOldPrdName+"["+chOldPrdId+"],�ĳɲ�Ʒ:";
           strInfo = strInfo+strNewPrdName+"["+chNewPrdId+"]," +strExceTime+"��Ч,�²�Ʒû��Чǰ,��ҵ���ܰ���" ;
           
           THROW_C(CRMException, CHECKBEFORETRADE_ERR, strInfo.c_str());
           
	    }*/
	        
	}

	LOG_TRACE(logTradeCheckBeforeTrade, "�˳� TradeCheck_CheckHintChgProduct ����");	
}
//У�� �嵥��ѯǰ���û����׼��� add by zhenjing@2008-06-19
void TradeCheckBeforeTrade::TradeCheck_CheckBillType(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "���� TradeCheck_CheckBillType ����");

    
    string strBillType = inBuf.GetString("BILL_TYPE");
    string strUserId = inBuf.GetString("USER_ID");
    int iCount = 0 ;
    StaticDAO& dao = StaticDAO::getDAO();
	CFmlBuf  bufBilltypeLimit;
	try
	{
	    dao.Clear();
	    dao.SetParam(":VUSER_ID",strUserId);
	    dao.SetParam(":VSERVICE_ID","1099");
	    iCount = dao.jselect(bufBilltypeLimit, "SEL_BY_SERVICE_ID","TF_F_USER_SVC");
	    LOG_TRACE_P1(logTradeCheckBeforeTrade,"33333333333,bufBilltypeLimit:%s",bufBilltypeLimit.ToString().c_str());
	}
	catch (Exception &ex)
	{
	    THROW_AGC(CRMException,ex,Grade::WARNING,CHECKBEFORETRADE_ERR,"TradeCheck_CheckHintChgProduct:��ѯ�û���������");
	}
	if(iCount==0)
	{
		try
	{
	    dao.Clear();
	    dao.SetParam(":VUSER_ID",strUserId);
	    dao.SetParam(":VSERVICE_ID","2099");
	    iCount = dao.jselect(bufBilltypeLimit, "SEL_BY_SERVICE_ID","TF_F_USER_SVC");
	}
	catch (Exception &ex)
	{
	    THROW_AGC(CRMException,ex,Grade::WARNING,CHECKBEFORETRADE_ERR,"TradeCheck_CheckHintChgProduct:��ѯ�û���������");
	}
	}
  if( iCount > 0 )
		{
			LOG_TRACE_P1(logTradeCheckBeforeTrade,"44444444444444,iRowCount:%s",bufBilltypeLimit.GetString("SERV_PARA1").c_str());
			if(bufBilltypeLimit.GetString("SERV_PARA1")=="1")
			{
			
				string strInfo = "���û�����������еĻ���";
			  THROW_C(CRMException, CHECKBEFORETRADE_ERR, strInfo.c_str());
			}
			if(bufBilltypeLimit.GetString("SERV_PARA1")=="2")
			{
			if(strBillType=="0")
			   {
			   
			    string strInfo = "���û��������ѯ������Ϣ";
			    THROW_C(CRMException, CHECKBEFORETRADE_ERR, strInfo.c_str());
			   }
			}
			if(bufBilltypeLimit.GetString("SERV_PARA1")=="11")
			{
				
				 string strInfo = "���û�����������еĻ���";
			   THROW_C(CRMException, CHECKBEFORETRADE_ERR, strInfo.c_str());
			}
			if(bufBilltypeLimit.GetString("SERV_PARA1")=="12")
			{
				if(strBillType!="2")
			   {
			   
			    string strInfo = "���û�ֻ�����ѯ�굥��Ϣ";
			    THROW_C(CRMException, CHECKBEFORETRADE_ERR, strInfo.c_str());
			   }
			}
			if(bufBilltypeLimit.GetString("SERV_PARA1")=="13")
			{
				if(strBillType!="0")
			   {
			   	
			    string strInfo = "���û�ֻ�����ѯ������Ϣ";
			    THROW_C(CRMException, CHECKBEFORETRADE_ERR, strInfo.c_str());
			   }
			}
		  
	        
	}

	LOG_TRACE(logTradeCheckBeforeTrade, "�˳� TradeCheck_CheckBillType ����");	
}

//У�� �ͻ����ϲ�ѯǰ���û����׼��� add by zhenjing@2008-07-16
void TradeCheckBeforeTrade::TradeCheck_CheckCustType(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "���� TradeCheck_CheckCustType ����");

    
    
    string strUserId = inBuf.GetString("USER_ID");
    int iCount = 0 ;
    StaticDAO& dao = StaticDAO::getDAO();
	CFmlBuf  bufCusttypeLimit;
	try
	{
	    dao.Clear();
	    dao.SetParam(":VUSER_ID",strUserId);
	    dao.SetParam(":VSERVICE_ID","1099");
	    iCount = dao.jselect(bufCusttypeLimit, "SEL_BY_SERVICE_ID","TF_F_USER_SVC");
	    LOG_TRACE_P1(logTradeCheckBeforeTrade,"33333333333,bufCusttypeLimit:%s",bufCusttypeLimit.ToString().c_str());
	}
	catch (Exception &ex)
	{
	    THROW_AGC(CRMException,ex,Grade::WARNING,CHECKBEFORETRADE_ERR,"TradeCheck_CheckCustType:��ѯ�û���������");
	}
	if(iCount==0)
	{
		try
	{
	    dao.Clear();
	    dao.SetParam(":VUSER_ID",strUserId);
	    dao.SetParam(":VSERVICE_ID","2099");
	    iCount = dao.jselect(bufCusttypeLimit, "SEL_BY_SERVICE_ID","TF_F_USER_SVC");
	}
	catch (Exception &ex)
	{
	    THROW_AGC(CRMException,ex,Grade::WARNING,CHECKBEFORETRADE_ERR,"TradeCheck_CheckCustType:��ѯ�û���������");
	}
	}
	
  if( iCount > 0 )
		{
			LOG_TRACE_P1(logTradeCheckBeforeTrade,"44444444444444,iRowCount:%s",bufCusttypeLimit.GetString("SERV_PARA1").c_str());
			if(bufCusttypeLimit.GetString("SERV_PARA1")=="1")
			{
			
				string strInfo = "�ͻ�,�ʵ�,�����鵥���϶��������ѯ";
			  THROW_C(CRMException, CHECKBEFORETRADE_ERR, strInfo.c_str());
			}
			if(bufCusttypeLimit.GetString("SERV_PARA1")=="2")
			{
			
			  string strInfo = "�ͻ�,�ʵ�,�����鵥���϶��������ѯ";
			  THROW_C(CRMException, CHECKBEFORETRADE_ERR, strInfo.c_str());
			}
			
	        
	}

	LOG_TRACE(logTradeCheckBeforeTrade, "�˳� TradeCheck_CheckCustType ����");	
}

//��鿪���û�������
void TradeCheckBeforeTrade::CheckOpenLimit(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "---------------�˳� CheckOpenLimit ����");
	int iCount(0);
	string strCustId = inBuf.GetString("CUST_ID");
	string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");
	StaticDAO& dao = StaticDAO::getDAO();
	//��ѯ�ͻ��²�ͬ�����û���
	CFmlBuf  bufUsrNum;
	try
	{
	    dao.Clear();
	    dao.SetParam(":VCUST_ID",strCustId);
	    dao.jselect(bufUsrNum, "SEL_USER_NUM_GROUPNET","TF_F_CUSTOMER");	    
	}
	catch (Exception &ex)
	{
	    THROW_AGC(CRMException,ex,Grade::WARNING,CHECKBEFORETRADE_ERR,"CheckOpenLimit:��ѯ�ͻ��²�ͬ�����û�������");
	}
	LOG_TRACE_P1(logTradeCheckBeforeTrade, "�����ͻ��û�:%s",bufUsrNum.ToString().c_str());
	
	string strMsg = "";
	int iCheckTag = 0;//��ʾ,�������ҵ��

	CFmlBuf  bufParam;
	try
	{
		dao.Clear();
    	dao.SetParam(":VSUBSYS_CODE","CSM");
    	dao.SetParam(":VPARAM_ATTR","4002");
    	dao.SetParam(":VPARAM_CODE","0");
    	dao.SetParam(":VEPARCHY_CODE",strEparchyCode);
    	dao.jselect(bufParam, "SEL_PK_TD_S_COMMPARA","TD_S_COMMPARA");
    	ConvertCodeToName(bufParam,"NetType","PARA_CODE1","NET_TYPE_NAME");
	}
	catch(Exception &ex)
	{
		THROW_AGC(CRMException,ex,Grade::WARNING,CHECKBEFORETRADE_ERR,"CheckOpenLimit:��ѯ�����ͻ��������������ݳ���!");
	}
	LOG_TRACE_P1(logTradeCheckBeforeTrade, "�����ͻ�������Ϣ:%s",bufParam.ToString().c_str());

	string strUserNum = "0";
	int iAllUser = 0;
	string strAllLimit = "0";
	for(int j=0;j<bufParam.GetCount("PARAM_ATTR");++j)
	{
		strUserNum = "0";
		if(bufParam.GetString("PARA_CODE1",j)=="ZZ")//����������������
		{
			strAllLimit = bufParam.GetString("PARA_CODE2",j);
			continue;
		}
		else
		{
			for(int i=0;i<bufUsrNum.GetCount("RECORDCOUNT");++i)
			{
				if(bufUsrNum.GetString("NET_TYPE_CODE",i)==bufParam.GetString("PARA_CODE1",j))
				{
					strUserNum = to_string(bufUsrNum.GetInt("RECORDCOUNT",i));
					if(iCheckTag!=1&&bufUsrNum.GetInt("RECORDCOUNT",i)>=atoi(bufParam.GetString("PARA_CODE2",j).c_str())
						&&atoi(bufParam.GetString("PARA_CODE2",j).c_str())!=0)//0��ʾ������
					{
						iCheckTag = 1;//��ֹ����ҵ��
					}
					break;
				}
			}
		}
		iAllUser += atoi(strUserNum.c_str());

		strMsg += "����Ϊ"+bufParam.GetString("NET_TYPE_NAME",j)+"�ĵ�ǰ������:"+strUserNum+",��󿪻���Ϊ:"+bufParam.GetString("PARA_CODE2",j)+"\n";		
	}

	//�ܿ��������ư���ҵ���ж�
	if(iCheckTag==0&&iAllUser>=atoi(strAllLimit.c_str()))
	{
		iCheckTag = 1;//��ֹ����ҵ��
	}
	string strAllNum = to_string(iAllUser);
	strMsg += "�ͻ���ǰ��������Ϊ:"+strAllNum+",��󿪻�����Ϊ:"+strAllLimit;

	outBuf.SetString("X_CHECK_INFO",strMsg);
	outBuf.SetInt("X_CHECK_TAG",iCheckTag);
	
	LOG_TRACE(logTradeCheckBeforeTrade, "�˳� CheckOpenLimit ����");
}

//������������add by weiliangyu@2008-07-25
void TradeCheckBeforeTrade::CheckChangeCustOwnerNum(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "���� CheckChangeCustOwnerNum ����");
   
    string strSerialNumber  = inBuf.GetString("SERIAL_NUMBER");
    string strTradeTypeCode = inBuf.GetString("TRADE_TYPE_CODE");
    string strTag = inBuf.GetString("TAG_NUMBER");
    int tag = atoi(strTag.c_str());
    int iCount = 0 ;
    StaticDAO& dao = StaticDAO::getDAO();
	CFmlBuf  bufBillInfo;
	try
	{
	    dao.Clear();
	    dao.SetParam(":VSERIAL_NUMBER",strSerialNumber);
	    dao.SetParam(":VTRADE_TYPE_CODE",strTradeTypeCode);
	    iCount = dao.jselect(bufBillInfo, "SEL_TRANSFERNUM_BY_NUM","TF_BH_TRADE");
	    LOG_TRACE_P1(logTradeCheckBeforeTrade,"��ѯ��������:%s",bufBillInfo.ToString().c_str());
	}
	catch (Exception &ex)
	{
	    THROW_AGC(CRMException,ex,Grade::WARNING,CHECKBEFORETRADE_ERR,"CheckChangeCustOwnerNum:��ѯ��ʷ̨�ʱ������");
	}	   
    outBuf.SetInt("X_RESULTCODE", 0);
    outBuf.SetString("X_RESULTINFO", "��ѯ���������ɹ���");
    outBuf.SetInt("X_RECORDNUM", iCount);
    
    if(iCount>=tag)
	{
        string strInfo = "�ú����Ѵﵽ�����������ֵ�������ٰ������ҵ�񣡣�";
		THROW_C(CRMException, CHECKBEFORETRADE_ERR, strInfo.c_str());			
    }
    
	LOG_TRACE(logTradeCheckBeforeTrade, "�˳� CheckChangeCustOwnerNum ����");	
}
//����û��Ƿ���δ�깤������ҵ��add by zhouchangxuan@2009-10-08
void TradeCheckBeforeTrade::CheckTradeState(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "���� CheckTradeState");

    if (!inBuf.IsFldExist("USER_ID"))
	{
		THROW_C(CRMException, CHECKAFTERTRADE_ERR, "CheckTradeState:���û���ʶ��");
	}

	
	string strId = inBuf.GetString("USER_ID");	
	string strTradeTypeCode = inBuf.GetString("TRADE_TYPE_CODE");
    string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");
    string strBrandCode = outBuf.GetString("BRAND_CODE");
    StaticDAO& dao = StaticDAO::getDAO();

    CFmlBuf  bufVTradetypeLimit;
    try
    {
        dao.Clear();
        dao.SetParam(":VTRADE_TYPE_CODE", strTradeTypeCode);
        dao.SetParam(":VUSER_ID", strId);
        dao.SetParam(":VBRAND_CODE", strBrandCode);
        dao.SetParam(":VLIMIT_ATTR", "0");
        dao.SetParam(":VLIMIT_TAG", "0");
        dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
        if (dao.jselect(bufVTradetypeLimit, "SEL_EXISTS_LIMIT_TRADETYPECODE","TD_S_TRADETYPE_LIMIT") > 0)
        {
            char chIdA[11];
          
            sprintf(chIdA,"%d",(int)bufVTradetypeLimit.GetInt("LIMIT_TRADE_TYPE_CODE"));
            string strTradeType = CParamDAO::getParam("TradeType", chIdA);
            THROW_C_P1(CRMException, TRADENOTCOMPLETED_ERR, "CheckTradeState:�û���δ�깤������ҵ��%s����", strTradeType.c_str());
        }
    }
    catch (CRMException &ex) {throw;}
    catch (Exception &ex)
    {
        THROW_AGC(CRMException,ex,Grade::WARNING,CHECKBEFORETRADE_ERR,"CheckTradeState:��ѯ�û�δ�깤������ҵ����Ϣ����");
    }

    LOG_TRACE(logTradeCheckBeforeTrade, "�˳� CheckTradeState");
}

//EOPN���У��
void TradeCheckBeforeTrade::CheckDestroyEPONUser(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
		LOG_TRACE(logTradeCheckBeforeTrade, "���� CheckDestroyEPONUser");
		string strUserId = inBuf.GetString("USER_ID");
		string strNetTypeCode = inBuf.GetString("NET_TYPE_CODE");
		string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");
		StaticDAO& dao = StaticDAO::getDAO();
		CFmlBuf  tempBuf;
		CFmlBuf  bufTag;
		CFmlBuf  bufUU;
		int count=0;
		int count1=0;
		int countUU=0;
		int iTagCount=0;
		try
		{
				dao.Clear();
				dao.SetParam(":VUSER_ID_A", strUserId);
				countUU=dao.jselect(bufUU,"SEL_ALL_BY_USERIDA_2","TF_F_RELATION_UU");
		}
		catch(Exception &ex)
		{
				THROW_AGC(CRMException,ex,Grade::WARNING,CHECKBEFORETRADE_ERR,"CheckDestroyEPONUser:��ѯ�û�Ⱥ����Ϣ����");
		}
		if(countUU>0)
		{
			LOG_TRACE(logTradeCheckBeforeTrade, "cchyc  1");
			LOG_TRACE_P1(logTradeCheckBeforeTrade, "cchyc  ��%s��",bufUU.GetString("ROLE_CODE_B").c_str());
			if(bufUU.GetString("ROLE_CODE_B")=="1")
			{
				LOG_TRACE(logTradeCheckBeforeTrade, "cchyc  2");
					THROW_C(CRMException, CHECKBEFORETRADE_ERR, "���û�Ϊ�����룬���ɲ������");
			}
		}
		LOG_TRACE(logTradeCheckBeforeTrade, "cchyc  5");
		try
		{
		
        dao.Clear();
        dao.SetParam(":VEPARCHY_CODE", inBuf.GetString("TRADE_EPARCHY_CODE"));
        dao.SetParam(":VTAG_CODE", "CS_INFO_EPON_DESTROY");
        dao.SetParam(":VSUBSYS_CODE", "CSM");
        dao.SetParam(":VUSE_TAG", "0");
        iTagCount = dao.jselect(bufTag, "SEL_BY_TAGCODE_1","TD_S_TAG");
        if(iTagCount == 0)
        {
            return;
        }
		}
		catch(Exception &ex)
		{
				THROW_AGC(CRMException,ex,Grade::WARNING,CHECKBEFORETRADE_ERR,"CheckDestroyEPONUser:��ѯ�û�Ⱥ����Ϣ����");
		}
		if (strNetTypeCode=="85")
		{
				try
				{
						dao.Clear();
						dao.SetParam(":VUSER_ID", strUserId);
						dao.SetParam(":VNET_TYPE_CODE", "30");
						count=dao.jselect(tempBuf,"SEL_RELATION_BY_USER_ID","TF_F_RELATION_UU");
				}
				catch(Exception &ex)
				{
						THROW_AGC(CRMException,ex,Grade::WARNING,CHECKBEFORETRADE_ERR,"CheckDestroyEPONUser:��ѯ�û�Ⱥ����Ϣ����");
				}
				if (count>0)
				{					
					try
					{
							dao.Clear();
							dao.SetParam(":VUSER_ID", strUserId);
							dao.SetParam(":VNET_TYPE_CODE", "85");
						  count1=dao.jselect(bufTag,"SEL_RELATION_BY_USER_ID","TF_F_RELATION_UU");
					}
					catch(Exception &ex)
					{
							THROW_AGC(CRMException,ex,Grade::WARNING,CHECKBEFORETRADE_ERR,"CheckDestroyEPONUser:��ѯ�û�Ⱥ����Ϣ����");
					}
					if (count1==0){
							THROW_C(CRMException, CHECKBEFORETRADE_ERR, "���û�Ⱥ���ڻ���������ҵ�񣬲��ɲ�����һ������ҵ�񣡣�");
					}
				}
		}
		LOG_TRACE(logTradeCheckBeforeTrade, "�˳� CheckDestroyEPONUser");
}

/**
 * �ܲ�CRM����� ȫ���������������ҵ�� ����
 */
void TradeCheckBeforeTrade::IsRequestFromUcrm(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
    LOG_TRACE(logTradeCheckBeforeTrade, "���� IsRequestFromUcrm����");
    string serialNumber = inBuf.IsFldExist("SERIAL_NUMBER") ? inBuf.GetString("SERIAL_NUMBER") : "";
    string inModeCode = inBuf.IsFldExist("ORIGDOMAIN") ? inBuf.GetString("ORIGDOMAIN") : "";
    string tradeTypeCode = inBuf.IsFldExist("TRADE_TYPE_CODE") ? inBuf.GetString("TRADE_TYPE_CODE") : "";
    LOG_TRACE_P1(logTradeCheckBeforeTrade, "inModeCode>>>>>>>>>%s",inModeCode.c_str());
    LOG_TRACE_P1(logTradeCheckBeforeTrade, "tradeTypeCode>>>>>>>>>%s",tradeTypeCode.c_str());
    if(inModeCode == "UCRM" && tradeTypeCode != "")
    {
		DataBuf tagInfoBj;
		UniTradeDao  &tDao = UniTradeDao::getInstance();
		int tagBj = tDao.getSysTagInfo(tagInfoBj, "CSM", "N4_1203_REQFROMUCRM", "ZZZZ");
		
        string tempTradeType = tagBj > 0 ? tagInfoBj.GetString("TAG_INFO") : "";
        string strTradeTypeCode = "|" + inBuf.GetString("TRADE_TYPE_CODE") + "|";
        
        LOG_TRACE_P1(logTradeCheckBeforeTrade, "tempTradeType>>>>>>>>>%s",tempTradeType.c_str());
        
        if(tempTradeType.find(strTradeTypeCode, 0) != string::npos)
        {
        	StaticDAO& dao = StaticDAO::getDAO();
            int count=0;
            CFmlBuf userBuf;
            try
            {
                dao.Clear();
                dao.SetParam(":VSERIAL_NUMBER", serialNumber);
                dao.SetParam(":VREMOVE_TAG", "0");
                count = dao.jselect(userBuf, "SEL_BY_SN2", "TF_F_USER");
            }
            catch(Exception &ex)
            {
                THROW_AGC(CRMException,ex,Grade::WARNING,CHECKBEFORETRADE_ERR,"��ѯ�û���Ϣ����");
            }
            if(count>0)
            {
                string productTypeCode = userBuf.GetString("PRODUCT_TYPE_CODE");
                
                DataBuf tagInfoProType;
                UniTradeDao  &tDao1 = UniTradeDao::getInstance();
                int iCount = tDao1.getSysTagInfo(tagInfoProType, "CSM", "N4_1203_PROTYPECODE", "ZZZZ");
                string tempProType = iCount > 0 ? tagInfoProType.GetString("TAG_INFO") : "";
                string strProType = "|" + productTypeCode + "|";
                
                LOG_TRACE_P1(logTradeCheckBeforeTrade, "tempProType>>>>>>>>>%s",tempProType.c_str());
                
                if(tempTradeType.find(strProType, 0) == string::npos)
                {
                    outBuf.SetInt("X_RESULTCODE", 99);
                    outBuf.SetString("X_RESULTINFO", "���û�����ͨ�������������ҵ��");
                    THROW_C(CRMException, CHECKAFTERTRADE_ERR, "���û�����ͨ�������������ҵ��");
                }
            }
        }
    }
    LOG_TRACE(logTradeCheckBeforeTrade, "�˳� IsRequestFromUcrm����");
}

