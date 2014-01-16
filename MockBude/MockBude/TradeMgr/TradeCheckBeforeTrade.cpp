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

//该函数放在最前面,供内部调用，不发布
void getTradeTypeInfo(string strNetTypeCode,string strEparchyCode,string strTradeTypeCode,CFmlBuf &bufTradetype)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "进入 getTradeTypeInfo");
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
            count = dao.jselect(bufTradetype,"SEL_BY_PK_NETTYPE","TD_S_TRADETYPE");  //已经设置找不到记录，当作判断
        }
        catch (Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "getTradeTypeInfo:查询业务类型参数表出错！");
        }
        if (count == 0)
        {
        	THROW_C_P3(CRMException, CHECKBEFORETRADE_ERR, "业务受理前条件判断:未获取到业务受理类型信息.NetTypeCode=%s,TradeTypeCode=%s,EparchyCode=%s",strNetTypeCode.c_str(),strTradeTypeCode.c_str(),strEparchyCode.c_str());
        }
    }

	LOG_TRACE(logTradeCheckBeforeTrade, "退出 getTradeTypeInfo");
}

/**
 *  业务受理前条件判断:业务规则校验，包括用户状态、欠费、业务是否受限等检查
 */
void TradeCheckBeforeTrade::CheckChinagoDelay(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
    LOG_TRACE(logTradeCheckBeforeTrade, "进入CheckChinagoDelay函数");

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

    LOG_TRACE(logTradeCheckBeforeTrade, "退出CheckChinagoDelay函数");
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
        THROW_AGC(CRMException, e, Grade::WARNING, GENEOLCOM_ERR, "生成业务流水号:获取业务流水号失败2:");
    }
	
}
/**
 *  业务受理前条件判断:业务规则校验，包括用户状态、欠费、业务是否受限等检查
 */
void TradeCheckBeforeTrade::CheckBeforeTrade(CFmlBuf &inBuf, CFmlBuf &outBuf)//ubss
{
	LOG_TRACE(logTradeCheckBeforeTrade, "进入 CheckBeforeTrade 函数");

    //0.设置Check返回的初始值 默认检查通过
    int ierrFag = 0;
    outBuf.SetInt("X_CHECK_TAG", 0);
    outBuf.SetString("X_CHECK_INFO","");
    StrTable stbOutPrompt;
    stbOutPrompt.AddTable("Prmp", 1);
            
    string strIdType = "1"; //设置默认ID为用户标识
    if (inBuf.IsFldExist("ID_TYPE")) strIdType = inBuf.GetString("ID_TYPE");

	string strId=inBuf.GetString("ID");
	string strNetTypeCode;
	if (inBuf.IsFldExist("NET_TYPE_CODE"))
	    strNetTypeCode = inBuf.GetString("NET_TYPE_CODE");
	
    bool hasCustInfo = false;
    bool hasUserInfo = false;
    //bool hasUserPurchaseInfo = false;
        
    //获取用户 or 客户信息
    int iCount=0;
    iCount=TradeCheck_GetCustUserInfoIntoBuf(inBuf,outBuf);
    if (strIdType=="1")	hasUserInfo = iCount >0;
    if (strIdType=="0")	hasCustInfo = iCount >0;
    
    if (strIdType=="1" && !hasUserInfo)//add by tz@2008-1-23 16:12
        outBuf.SetString("BRAND_CODE","");
    
    if (strNetTypeCode !="")
        outBuf.SetString("NET_TYPE_CODE",strNetTypeCode);
    
    //根据配置决定是否需要判断员工的数据权限
    int iNeedCheckOptrDataRight=0;//1:check data right,0:not check data right
    iNeedCheckOptrDataRight=TradeCheck_NeedCheckOptrDataRight(inBuf,outBuf);
    
    //受理员工业务办理系统数据权限(可以办理什么地域范围的业务)判断。是否可以办理？
    if (iNeedCheckOptrDataRight == 1)
    	TradeCheck_CheckStaffDataRight(inBuf,outBuf);	
    
    //用户如果有了预约产品变更，
    //提醒用户有预约的限制业务,增加详细显示(操作时间：****年**月**日，用户预约将**产品改成**产品，*月生效。)
	TradeCheck_CheckHintChgProduct(inBuf,outBuf);

	//用户当前受理的业务类型是否存在未完工业务的限制
	TradeCheck_CheckNotCompletedTrade(inBuf,outBuf);
	
	
	//黑名单客户办理业务判断。是否继续？
	if (TradeCheck_CheckBlackUser(inBuf,outBuf)==1)
	{
		ierrFag=1;
        stbOutPrompt.AddField("Prmp", "TradeCheck_CheckBlackUser:黑名单客户，建议终止业务的办理！\n是否要继续业务的办理？选择【确定】继续办理业务，选择【取消】终止办理业务。");
	}
	//判断用户是否红名单
	bool isRedUser=false;
	if (strIdType == "1")   //传入的是用户
        isRedUser = (inBuf.GetString("REDUSER_TAG") == "1"); //cclib::isRedUser(strId);

	//欠费判断
	if (TradeCheck_CheckOweFee(inBuf,outBuf)==1)
	{
		ierrFag=1;
		stbOutPrompt.AddField("Prmp", "TradeCheck_CheckOweFee:用户已经欠费，建议终止业务的办理！\n是否要继续业务的办理？选择【确定】继续办理业务，选择【取消】终止办理业务。");
	}
	
	//判断用户是否是捆绑销售，如果是，有没有到达协议期，没有到协议结束时间不能办理相关业务
	outBuf.SetString("X_CHECK_INFO", "");
	TradeCheck_CheckBindSaleExpDate(inBuf,outBuf);
	string strPrompt = outBuf.GetString("X_CHECK_INFO");
	if ( strPrompt != "")
	{
	    ierrFag=1;
		stbOutPrompt.AddField("Prmp", strPrompt);
	}
	
	//判断用户是否有不能办理业务的服务状态。是否继续？
	TradeCheck_SvcstateTradeLimit(inBuf,outBuf);
	
	//判断用户是否存在业务受限
	TradeCheck_UserTradelimit(inBuf,outBuf);
	
	//判断业务受理类型依赖的服务是否开通了
	TradeCheck_CheckTradeTypeCodeDependSVC(inBuf,outBuf);
	
	//判断用户是否存在产品业务限制。是否继续？
	TradeCheck_ProdTradeLimit(inBuf,outBuf);
	
	//检查是否可以异地办理业务
	TradeCheck_CheckEparchyCode(inBuf,outBuf);
	
	//判断批开用户是否可以办理
	TradeCheck_CheckPreOpen(inBuf,outBuf);
	
	//校验是否有sp信息，以及是否可以办理
	TradeCheck_CheckUserSP(inBuf,outBuf);//目前没有sp，预留接口在此
	
	//用户有预存款，是否可以办理业务
	if (TradeCheck_CheckPreDeposit(inBuf,outBuf)==1)
	{
		ierrFag=1;
        stbOutPrompt.AddField("Prmp", "TradeCheck_CheckPreDeposit:该用户有结余，是否要继续业务的办理？\n选择【是】继续办理业务，选择【否】终止办理业务。");
	}
	
	//当前用户有押金是否可以办理。是否继续？
    if (TradeCheck_CheckForegift(inBuf,outBuf)==1)
    {
    	ierrFag=1;
    	stbOutPrompt.AddField("Prmp", "TradeCheck_CheckForegift:该用户有押金，是否要继续业务的办理？\n选择【是】继续办理业务，选择【否】终止办理业务。");
	}
	//初始密码用户不能进行业务受理
	TradeCheck_CheckInitPasswd(inBuf,outBuf);
	TradeCheck_CheckPrdAttrLimit(inBuf,outBuf);
	
	//校验积分业务的出帐日限制(限制从外围接口传入积分调用)
    TradeCheck_CheckScoreTradeLimit(inBuf,outBuf);
	
    //考虑到接口等非营业前台调用时,buff中没有FEE1/FEE2/FEE3字段，再此为buff中添加这些字段，防止后面调用报错
    if (!inBuf.IsFldExist("FEE1")) inBuf.SetString("FEE1", "0");
    if (!inBuf.IsFldExist("FEE2")) inBuf.SetString("FEE2", "0");
    if (!inBuf.IsFldExist("FEE3")) inBuf.SetString("FEE3", "0");
    
    //营业前台调用时，buff中没有IN_MODE_CODE字段，在此为buff中添加这个字段
    if (!inBuf.IsFldExist("IN_MODE_CODE")) inBuf.SetString("IN_MODE_CODE", "0");
    
    //总结
    if ( ierrFag != 0)
    {
        outBuf.SetInt("X_CHECK_TAG", ierrFag);
        string strResult;
        if ( stbOutPrompt.ToString(strResult) != 0)
            THROW_C(CRMException, CHECKBEFORETRADE_ERR, "业务受理前条件判断:解析字串表出错！");
        outBuf.SetString("X_CHECK_INFO", strResult);
    }
	LOG_TRACE(logTradeCheckBeforeTrade, "退出 CheckBeforeTrade 函数");
} 
int TradeCheckBeforeTrade::TradeCheck_GetCustUserInfoIntoBuf(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "进入 TradeCheck_GetCustUserInfoIntoBuf");

	if (!inBuf.IsFldExist("ID"))
	{
		THROW_C(CRMException, CHECKAFTERTRADE_ERR, "TradeCheck_GetCustUserInfo:缓冲区未输入 ID 域(用户或客户标识)！");
	}

    string strId = inBuf.GetString("ID");
    string strIdType = "1"; //设置默认ID为用户标识
    if (inBuf.IsFldExist("ID_TYPE"))
        strIdType = inBuf.GetString("ID_TYPE");	
    int iRowCount=0;
    
    bool bPutIntoBuf=true;//是否将查询到的数据填写到缓冲区
    
    string strNetType = inBuf.GetString("NET_TYPE_CODE");
    
    StaticDAO& dao = StaticDAO::getDAO();
    if (strIdType == "0")   //获取客户资料
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
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "TradeCheck_GetCustUserInfo:获取客户核心资料出错！");
        }
    }
    else if (strIdType == "1")    //获取用户资料
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
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "TradeCheck_GetCustUserInfo:获取用户主表资料出错！");
        }
    }
    LOG_TRACE(logTradeCheckBeforeTrade, "退出 TradeCheck_GetCustUserInfoIntoBuf");
    return iRowCount;
}
void TradeCheckBeforeTrade::TradeCheck_CheckTradeTypeCodeDependSVC(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "进入 TradeCheck_CheckTradeTypeCodeDependSVC 函数");
	
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
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "查询业务依赖的服务出错！");
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
	        THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "查询用户是否开通了服务[%s]出错！",strServiceID.c_str());
	    }
		if (atoi(iBuf.GetString("ROW_COUNT").c_str()) == 0)
		{
			string strServiceName = CParamDAO::getParam("ServiceName", strServiceID.c_str());
			THROW_C_P1(CRMException, CHECKBEFORETRADE_ERR, "请在用户开通了服务[%s]后再办理该业务!",	strServiceName.c_str());
		}
	}
	LOG_TRACE(logTradeCheckBeforeTrade, "退出 TradeCheck_CheckTradeTypeCodeDependSVC 函数");
}
void TradeCheckBeforeTrade::TradeCheck_CheckBindSaleExpDate(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
    LOG_TRACE(logTradeCheckBeforeTrade, "进入 TradeCheck_CheckBindSaleExpDate");
	  
	  //Modify by wuzy@2007-07-31  begin
	  string strTradeTypeCode = inBuf.GetString("TRADE_TYPE_CODE");
	  //产品变更:110,预约销户:190,立即销户:192,过户:100,优惠变更:150
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
	      THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "查询用户捆绑销售信息出错！");
	  }
	   
	  if (iRowCount > 0)
	  {
	  	  for (int i=0;i<iRowCount;i++)
	  	  {
	  	  	  strSysDate    = iBuf.GetString("X_SYSDATE"   ,i);
	  	  	  strEndDate    = iBuf.GetString("END_DATE"    ,i);
	  	  	  strProcessTag = iBuf.GetString("PROCESS_TAG" ,i);
	  	  	  strPEndDate   = iBuf.GetString("P_END_DATE"  ,i);//提前一个月
	  	  	  
	  	  	  LOG_TRACE_P1(logTradeCheckBeforeTrade, "strId=%s",strId.c_str());
	  	  	  LOG_TRACE_P1(logTradeCheckBeforeTrade, "strTradeTypeCode=%s",strTradeTypeCode.c_str());
	  	  	  LOG_TRACE_P1(logTradeCheckBeforeTrade, "strSysDate=%s",strSysDate.c_str());
	  	  	  LOG_TRACE_P1(logTradeCheckBeforeTrade, "strEndDate=%s",strEndDate.c_str());
	  	  	  LOG_TRACE_P1(logTradeCheckBeforeTrade, "strProcessTag=%s",strProcessTag.c_str());
	  	  	  LOG_TRACE_P1(logTradeCheckBeforeTrade, "strPEndDate=%s",strPEndDate.c_str());
        
	  	  	  if ((strTradeTypeCode != "110" && strEndDate > strSysDate)
	  	  	      ||(strTradeTypeCode == "110" && strPEndDate > strSysDate))
	  	  	  {
	  	  	  	  //第三位：1：协议期内不能变产品         2:询问是否继续
	  	  	  	  //第四位：1：协议期内不能做预约销号     2:询问是否继续
	  	  	  	  //第五位：1：协议期内不能做立即销号     2:询问是否继续
	  	  	  	  //第六位：1：协议期内不能做过户         2:询问是否继续
	  	  	  	  //第七位：1：协议期内不能变优惠         2:询问是否继续
	  	  	  	  //产品变更:110,预约销户:190,立即销户:192,过户:100,优惠变更:150
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
	  	  	  	  	  if (chPromptMode == '2')  //询问
	  	  	  	  	  {
	  	  	  	  	      outBuf.SetString("X_CHECK_INFO", "用户还在在协议期["+((strTradeTypeCode=="110") ? strPEndDate: strEndDate)+"]内，是否继续办理["+string(bufTradetype.GetString("TRADE_TYPE"))+"]业务？");
	  	  	  	  	  }
	  	  	  	  	  else
	  	  	  	  	  {
	  	  	  	  	      THROW_C_P2(CRMException, CHECKBEFORETRADE_ERR, "用户还在在协议期内，不能办理[%s]业务\r\n请在日期[%s]之后再办理!",
	  	  	  	  	      	string(bufTradetype.GetString("TRADE_TYPE")).c_str(),(strTradeTypeCode=="110") ? strPEndDate.c_str() : strEndDate.c_str());
	  	  	  	  	  }
	  	  	  	  }
	  	  	  }
	  	  } 
	  } 
	  //Modify by wuzy@2007-07-31  end 
    LOG_TRACE(logTradeCheckBeforeTrade, "退出 TradeCheck_CheckBindSaleExpDate 函数");
}

int TradeCheckBeforeTrade::TradeCheck_CheckForegift(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "进入 TradeCheck_CheckForegift 函数");
	
    string strIdType = "1"; //设置默认ID为用户标识
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
                    THROW_C(CRMException, CHECKBEFORETRADE_ERR, "TradeCheck_CheckForegift:该用户有押金，不能办理此业务！");
                }
                else
                {
                	//add by chenzm@2006-11-30(测试人员认为2次提示太多了，所以改为一次)
				    int iChoiceTag=1;
				    if (inBuf.IsFldExist("X_CHOICE_TAG"))
				    	iChoiceTag = inBuf.GetInt("X_CHOICE_TAG");
                	
                    if (iChoiceTag == 1)//1-前台点击确定时交验 , 0-输入业务号码后回车事件中交验
                    	ierrFag = 1;
                    else
                    	ierrFag = 0;	
                }
            }
        }
        catch (CRMException &ex) {throw;}
        catch (Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "TradeCheck_CheckForegift:查询用户SP信息出错！");
        }
    }
	
	LOG_TRACE(logTradeCheckBeforeTrade, "退出 TradeCheck_CheckForegift 函数");
	return ierrFag;
}
int TradeCheckBeforeTrade::TradeCheck_CheckPreDeposit(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "进入 TradeCheck_CheckPreDeposit 函数");

    string strIdType = "1"; //设置默认ID为用户标识
    if (inBuf.IsFldExist("ID_TYPE"))
        strIdType = inBuf.GetString("ID_TYPE");
        
    CFmlBuf bufTradetype;
    string strTradeTypeCode = inBuf.GetString("TRADE_TYPE_CODE");
    string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");
    
    string strNetTypeCode = inBuf.GetString("NET_TYPE_CODE");
    
     	
	getTradeTypeInfo(strNetTypeCode,strEparchyCode,strTradeTypeCode,bufTradetype);
    	
	
    string strFee = "0";
    if (inBuf.IsFldExist("FEE"))
        strFee = inBuf.GetString("FEE");//用户欠费
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
                THROW_C(CRMException, CHECKBEFORETRADE_ERR, "TradeCheck_CheckPreDeposit:该用户有预存，请先办理预存清退！");
            }
            else
            {
            	//add by chenzm@2006-11-30(测试人员认为2次提示太多了，所以改为一次)
			    int iChoiceTag=1;
			    if (inBuf.IsFldExist("X_CHOICE_TAG"))
			    	iChoiceTag = inBuf.GetInt("X_CHOICE_TAG");
            	
                if (iChoiceTag == 1)//1-前台点击确定时交验 , 0-输入业务号码后回车事件中交验
                	ierrFag = 1;
                else
                	ierrFag = 0;	
            }
        }
    }
	
	LOG_TRACE(logTradeCheckBeforeTrade, "退出 TradeCheck_CheckPreDeposit 函数");
	return ierrFag;
}
void TradeCheckBeforeTrade::TradeCheck_CheckUserSP(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	//LOG_TRACE(logTradeCheckBeforeTrade, "进入 TradeCheck_CheckUserSP 函数");
	
	//LOG_TRACE(logTradeCheckBeforeTrade, "退出 TradeCheck_CheckUserSP 函数");
}
void TradeCheckBeforeTrade::TradeCheck_CheckPreOpen(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "退出 TradeCheck_CheckPreOpen 函数");
	
	string strIdType = "1"; //设置默认ID为用户标识
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
        //add by digy@20070828 只对营业厅过来的服务变更判断是否未返单用户 
        if( (strTradeTypeCode != "120" && bufTradetype.GetString("PREOPEN_LIMIT_TAG")=="1" && outBuf.GetString("OPEN_MODE")=="1")||
            (strTradeTypeCode =="120" && strInModeCode =="0" && bufTradetype.GetString("PREOPEN_LIMIT_TAG")=="1" && outBuf.GetString("OPEN_MODE")=="1")
          )
        {
            THROW_C(CRMException, CS_USERSTATE_ERR, "TradeCheck_CheckPreOpen:该用户是预开未返单用户，不能办理此业务！");
        }
    }
	
	LOG_TRACE(logTradeCheckBeforeTrade, "退出 TradeCheck_CheckPreOpen 函数");
}
void TradeCheckBeforeTrade::TradeCheck_CheckEparchyCode(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "退出 TradeCheck_CheckEparchyCode 函数");
	
	CFmlBuf bufTradetype;
    string strTradeTypeCode = inBuf.GetString("TRADE_TYPE_CODE");
    string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");
    	
	 string strNetTypeCode = inBuf.GetString("NET_TYPE_CODE");
    
    
    	
	getTradeTypeInfo(strNetTypeCode,strEparchyCode,strTradeTypeCode,bufTradetype);

    string strId = inBuf.GetString("ID");
    string strIdType = "1"; //设置默认ID为用户标识
    if (inBuf.IsFldExist("ID_TYPE"))
        strIdType = inBuf.GetString("ID_TYPE");

    if (bufTradetype.GetString("EXTEND_TAG") == "0")
    {
        //判断客户现在是否在办理异地业务
        if (strIdType == "0" && outBuf.IsFldExist("PSPT_ID") )
        {
        	if (outBuf.GetString("EPARCHY_CODE") != strEparchyCode)
            {
                THROW_C(CRMException, CS_OTHERAREATRADE_ERR, "TradeCheck_CheckEparchyCode:客户不能异地办理该业务！");
            }
        }
        //判断用户现在是否在办理异地业务
        else if (strIdType == "1" && outBuf.IsFldExist("USER_ID"))
        {
        	if (outBuf.GetString("EPARCHY_CODE") != strEparchyCode)
            {
                //abc待修改1
                ///if((string(outBuf.GetString("USER_ID")).substr(0,2)!="99" &&outBuf.GetString("REMOVE_TAG") != "")&&
                  //  !((string(outBuf.GetString("BRAND_CODE")).substr(0,2))=="VP"&&(outBuf.GetString("RSRV_STR2")=="1"||outBuf.GetString("RSRV_STR2")=="2")))
                {
                    THROW_C(CRMException, CS_OTHERAREATRADE_ERR, "TradeCheck_CheckEparchyCode:用户不能异地办理该业务！");
                }
            }
        }
    }
	
	LOG_TRACE(logTradeCheckBeforeTrade, "退出 TradeCheck_CheckEparchyCode 函数");	
}

void TradeCheckBeforeTrade::TradeCheck_ProdTradeLimit(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "进入 TradeCheck_ProdTradeLimit 函数");
    
    string strId = inBuf.GetString("ID");
    string strIdType = "1"; //设置默认ID为用户标识
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
                THROW_C_P1(CRMException, CS_TRADELIMIT_ERR, "TradeCheck_ProdTradeLimit:用户当前产品为【%s】，不能办理该业务！", strName.c_str());
            }
        }
        catch (CRMException &ex) {throw;}
        catch (Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "TradeCheck_ProdTradeLimit:查询用户产品业务限制表出错！");
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
                THROW_C_P1(CRMException, CS_TRADELIMIT_ERR, "TradeCheck_ProdTradeLimit:用户当前具有服务【%s】，不能办理该业务！", strName.c_str());
            }
        }
        catch (CRMException &ex) {throw;}
        catch (Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "TradeCheck_ProdTradeLimit:查询用户产品业务限制表出错！");
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
                THROW_C_P1(CRMException, CS_TRADELIMIT_ERR, "TradeCheck_ProdTradeLimit:用户当前具有优惠【%s】，不能办理该业务！", strName.c_str());
            }
        }
        catch (CRMException &ex) {throw;}
        catch (Exception &ex)
        {
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "TradeCheck_ProdTradeLimit:查询用户产品业务限制表出错！");
        }
        if (strTradeTypeCode=="110"&&strUserTagSet.length()>1&&strUserTagSet.substr(1,1)=="1")
        {
            THROW_C(CRMException, CS_TRADELIMIT_ERR, "TradeCheck_ProdTradeLimit:该用户禁止修改产品！");
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
	                  	THROW_C(CRMException, CS_TRADELIMIT_ERR, "TradeCheck_ProdTradeLimit:用户需要到ESS系统受理该业务！");
	                  } 
	              }
	          }
	          catch (Exception &ex)
	          {
	              THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "TradeCheck_ProdTradeLimit:查询产品表出错！");
	          }
	      }
    }
	
	LOG_TRACE(logTradeCheckBeforeTrade, "退出 TradeCheck_ProdTradeLimit 函数");	
}
void TradeCheckBeforeTrade::TradeCheck_CheckNotCompletedTrade(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "进入 TradeCheck_CheckNotCompletedTrade");

    if (!inBuf.IsFldExist("ID"))
	{
		THROW_C(CRMException, CHECKAFTERTRADE_ERR, "TradeCheck_CheckNotCompletedTrade:缓冲区未输入 ID 域(用户标识)！");
	}

    string strIdType = "1"; //设置默认ID为用户标识
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
	            //add by chenzm@2006-11-24:山东联通要求销户或预约销户有预约业务也可以受理
	            if (strTradeTypeCode != "192" && strTradeTypeCode != "190")
	            {
		            //Add by chetq 2006-7-10 begin 增加预约业务提示
		            CFmlBuf bufVTrade;
		            dao.Clear();
		            dao.SetParam(":VUSER_ID",strId);
		            if( dao.jselect(bufVTrade,"SEL_BOOK_BY_USERID","TF_B_TRADE")>0) {
		            	sprintf(chIdA,"%d",(int)bufVTradetypeLimit.GetInt("LIMIT_TRADE_TYPE_CODE"));
		                string strTradeType = CParamDAO::getParam("TradeType", chIdA);
		                THROW_C_P2(CRMException, CHECKBEFORETRADE_ERR, "TradeCheck_CheckNotCompletedTrade:用户有预约的限制业务[%s][%s]！", strTradeType.c_str(),serialNumber.c_str());
		            }
	        	}
	            sprintf(chIdA,"%d",(int)bufVTradetypeLimit.GetInt("LIMIT_TRADE_TYPE_CODE"));
	            string strTradeType = CParamDAO::getParam("TradeType", chIdA);
	            THROW_C_P2(CRMException, TRADENOTCOMPLETED_ERR, "TradeCheck_CheckNotCompletedTrade:用户有未完工的限制业务[%s][%s]！", strTradeType.c_str(),serialNumber.c_str());
	        }
	    }
	    catch (CRMException &ex) {throw;}
	    catch (Exception &ex)
	    {
	        THROW_AGC(CRMException,ex,Grade::WARNING,CHECKBEFORETRADE_ERR,"TradeCheck_CheckNotCompletedTrade:查询用户未完工的限制业务信息出错！");
	    }
	}
	LOG_TRACE(logTradeCheckBeforeTrade, "退出 TradeCheck_CheckNotCompletedTrade");
}
void TradeCheckBeforeTrade::TradeCheck_CheckStaffDataRight(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "进入 TradeCheck_CheckStaffDataRight");
    
    string strIdType = "1"; //设置默认ID为用户标识
    if (inBuf.IsFldExist("ID_TYPE")) strIdType = inBuf.GetString("ID_TYPE");
    if (strIdType!="1")
    	return;
    	
	if (!outBuf.IsFldExist("USER_ID"))
	{
		THROW_C(CRMException, CHECKAFTERTRADE_ERR, "TradeCheck_CheckStaffDataRight:请先调用TradeCheck_GetCustUserInfoIntoBuf获取用户客户信息！");
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
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "TradeCheck_CheckStaffDataRight:获取员工系统数据权限信息出错！");
    }
    
    //0-无权限	1-个人 2-营业厅	3-业务区 4-地州	5-全省
    string strRightClass = bufStaffdataright.GetString("RIGHT_CLASS");
    if(iCount == 0 || strRightClass == "0" )
    {
        THROW_C(CRMException, CHECKBEFORETRADE_ERR, "TradeCheck_CheckStaffDataRight:无权办理该用户的业务！");
    }
    else if (strRightClass == "1")
    {
        THROW_C(CRMException, CHECKBEFORETRADE_ERR, "TradeCheck_CheckStaffDataRight:当前员工无权办理该用户的业务！");
    }
    else if (strRightClass == "2")
    {
        THROW_C(CRMException, CHECKBEFORETRADE_ERR, "TradeCheck_CheckStaffDataRight:当前营业厅无权办理该用户的业务！");
    }
    else if (strRightClass == "3") //业务区
    {
        if (strCityCode != strCityCode_User || strEparchyCode != strEparchyCode_User)
        {
            THROW_C_P4(CRMException, CHECKBEFORETRADE_ERR, "TradeCheck_CheckStaffDataRight:当前业务区无权办理该用户的业务！\n操作员业务区[%s.%s],用户归属业务区[%s.%s]", strEparchyCode.c_str(), strCityCode.c_str(), strEparchyCode_User.c_str(), strCityCode_User.c_str());
        }
    }
    else if (strRightClass == "4") //地州
    {
        if (strEparchyCode != strEparchyCode_User)
        {
        	THROW_C_P2(CRMException, CHECKBEFORETRADE_ERR, "TradeCheck_CheckStaffDataRight:当前地州无权办理该用户的业务！\n操作员地市编码[%s],用户归属地市编码[%s]", strEparchyCode.c_str(), strEparchyCode_User.c_str());
        }
    }
	
	LOG_TRACE(logTradeCheckBeforeTrade, "退出 TradeCheck_CheckStaffDataRight");
}
int TradeCheckBeforeTrade::TradeCheck_NeedCheckOptrDataRight(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "进入TradeCheck_NeedCheckOptrDataRight");
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
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "TradeCheck_NeedCheckOptrDataRight:获取TAG_CODE=CSM_CHR_JUDGESTAFFCLASS记录出错！");
    }
    	
	LOG_TRACE(logTradeCheckBeforeTrade, "退出TradeCheck_NeedCheckOptrDataRight");
	if (iCount>0 && bufTag.GetString("TAG_CHAR") == "1")
		return 1;
	return 0;
}
void TradeCheckBeforeTrade::TradeCheck_CheckInitPasswd(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "进入TradeCheck_CheckInitPasswd");
   
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
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "TradeCheck_CheckInitPasswd:获取TAG_CODE=CS_CHR_CHECK_INITPASSWD记录出错！");
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
        		THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "TradeCheck_CheckInitPasswd:获取用户纵表信息出错！");
    		}
    		if( iCount > 0 && bufTag.GetString("ATTR_VALUE") == "0")
    		{
    			 THROW_C(CRMException, CS_TRADELIMIT_ERR, "业务受理前条件判断：该用户为初始密码用户，不能办理此业务！");
    		}
    	}	
    		 
	}
}
void TradeCheckBeforeTrade::TradeCheck_CheckPrdAttrLimit(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "进入TradeCheck_CheckPrdAttrLimit");
   
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
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "TradeCheck_CheckPrdAttrLimit:获取TAG_CODE=CS_CHR_CHECK_PRDATTRLIMIT记录出错！");
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
        	THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "TradeCheck_CheckPrdAttrLimit:获取用户纵表信息出错！");
    	}
    	if( iCount > 0 && bufTag.GetString("ATTR_VALUE") == "1")
    	{
    		 THROW_C(CRMException, CS_TRADELIMIT_ERR, "业务受理前条件判断：该用户有产品不可以变更限制，不能办理产品变更！");
    	}	
    		 
	}
}

int TradeCheckBeforeTrade::TradeCheck_CheckBlackUser(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "进入 TradeCheck_CheckBlackUser");
	if (!outBuf.IsFldExist("CUST_ID"))
	{
		THROW_C(CRMException, CHECKAFTERTRADE_ERR, "TradeCheck_CheckBlackUser:请先调用TradeCheck_GetCustUserInfoIntoBuf获取用户客户信息！");
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
                THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "TradeCheck_CheckBlackUser:获取客户核心资料出错！");
            }
            //add by zhangzh@20060412 21:58 begin 集团用户不判断黑名单
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
					THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "TradeCheck_CheckBlackUser:获取客户核心资料出错！");
				}
			}//add by zhangzh@20060412 21:58 end
	    }
	    else
	        bufCust = outBuf;


	    if(hasCustInfo && iCheckBlackUser != 0)//modi by zhangzh@20060412 21:58 集团用户不判断黑名单
	    {
	        try
	        {                
                //modified by qiumb@2007-10-20  黑名单信息由td_o_blackuser改为tf_o_cust_special_list
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
					THROW_AGC(CRMException, e, Grade::WARNING, GETUSERSCOREINFO_ERR, "检查是否黑名单异常！");
				}
	
				//if(daoBlackuser.jselect(objBlackuser, "SEL_BY_PK") > 0)
*/
			    //调用QCS_IsBlackUser查询用户是否欠费？
			    CFmlBuf lcuInBuf,lcuOutBuf;
			    lcuOutBuf.ClearFmlValue();
			    lcuInBuf.SetString("PSPT_ID",bufCust.GetString("PSPT_ID"));
			    lcuInBuf.SetString("PSPT_TYPE_CODE",bufCust.GetString("PSPT_TYPE_CODE"));
			    lcuInBuf.SetString("X_TRANS_CODE","QCS_IsBlackUser");
			    lcuInBuf.SetInt("X_TAG",0);
			    LOG_TRACE_P1(logTradeCheckBeforeTrade,"=============调用QCS_IsBlackUser输入buf:===========%s\n",lcuInBuf.ToString().c_str());
			    Utility::callLCU(lcuInBuf,lcuOutBuf);
			    LOG_TRACE_P1(logTradeCheckBeforeTrade,"=============调用QCS_IsBlackUser输出buf:===========%s\n",lcuOutBuf.ToString().c_str());
			    if (lcuOutBuf.GetInt("X_RESULTCODE")!=0)
			    {
			        THROW_C(CRMException, 8888,"调用帐管QCC_IsBlackUser流程异常"+lcuOutBuf.GetString("X_RESULTINFO"));
			    } 
                //if(tag > 0)
                if(lcuOutBuf.GetString("ACT_TAG") =="1")
//qc:05330 end                	
                {
                    if(string(bufTradetype.GetString("TAG_SET")).substr(3,1) == "1") //提示选择
                    {
                        ierrFag = 1;
                    }
                    else if(string(bufTradetype.GetString("TAG_SET")).substr(3,1) == "2")    //禁止办理
                    {
                        THROW_C(CRMException, CHECKBEFORETRADE_ERR, "TradeCheck_CheckBlackUser:黑名单客户不能办理当前业务！"); 
                    }
                }
            }
            catch(Exception &e)
            {
                THROW_AGC(CRMException, e, Grade::WARNING, CHECKBEFORETRADE_ERR, "TradeCheck_CheckBlackUser:业务处理出错:");
            }
	    }
	}
	
	LOG_TRACE(logTradeCheckBeforeTrade, "退出 TradeCheck_CheckBlackUser");
	return ierrFag;	
}
int TradeCheckBeforeTrade::TradeCheck_CheckOweFee(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "进入 TradeCheck_CheckOweFee");
	
	string isPrePay ;//"0"为后付费
    isPrePay=outBuf.GetString("PREPAY_TAG");
    
    string strId = inBuf.GetString("ID");
    bool isRedUser=false;

    string strFee = "0";
    if (inBuf.IsFldExist("FEE"))
        strFee = inBuf.GetString("FEE");//用户欠费
    else
        strFee = inBuf.GetString("LEAVE_REAL_FEE");
        
    string strFee1 = "0";//往月欠费
    string strFee2 = "0";//实时话费
    string strFee3 = "0";//实时结余
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


    string strIdType = "1"; //设置默认ID为用户标识
    if (inBuf.IsFldExist("ID_TYPE"))
        strIdType = inBuf.GetString("ID_TYPE");
    
    if (strIdType == "1")
    	isRedUser=(inBuf.GetString("REDUSER_TAG") == "1"); //cclib::isRedUser(strId);

    int iCount=0;
    string strAcctId;
    
    int ierrFag=0;
    string strJudgeOweTag = bufTradetype.GetString("JUDGE_OWE_TAG");
  
     //吉林对欠费用户的业务操作，需要营业员有操作权限 modify xf 20091004
    int isright = CheckStaffIdRight(inBuf,outBuf);
    if(strJudgeOweTag != "0" && isright == 1){
        return ierrFag;
    }
    
    if( strJudgeOweTag == "C" ) //不判欠费，前台需要展现用户费用信息，故新加一种judgeOweTag。
    {
    	ierrFag=0; 	     		
    }
    else if ( strJudgeOweTag == "A" )//分预付和后付
    {
        if (isPrePay == "1")  //预付
        {
            if( atol(strFee3.c_str()) < 0 ) //实时结余
            {
                THROW_C(CRMException, CS_OWEFEE_ERR, "TradeCheck_CheckOweFee:预付费用户有实时欠费不能办理业务！");
            }
        }
        else  //后付
        {
            if( atol(strFee1.c_str()) > 0 ) //往月欠费
            {
                THROW_C(CRMException, CS_OWEFEE_ERR, "TradeCheck_CheckOweFee:后付费用户有往月欠费不能办理业务！");
            }
        }
    }
    else if( strJudgeOweTag == "B" )
    {
    	if( atol(strFee1.c_str()) > 0 )//往月欠费
    	{
            CFmlBuf bufCparam;
        	try
        	{
            	dao.Clear();
            	dao.SetParam(":VUSER_ID", strId);
            	dao.SetParam(":VDEFAULT_TAG", "1");
            	dao.SetParam(":VACT_TAG","1");
            	dao.jselect(bufCparam,"IsOnlyAccount","TD_S_CPARAM");
            	if (bufCparam.GetInt("RECORDCOUNT") > 0)//独帐户用户
            	{
            		THROW_C(CRMException, CS_OWEFEE_ERR, "TradeCheck_CheckOweFee:对不起您有往月欠费，不允许办理此业务！");	
            	}
            }
            catch(Exception &ex)
            {
                    THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "TradeCheck_CheckOweFee:判断用户是否独立帐户出错");
            }
       }
    }
    else if (!isRedUser && atol(strFee.c_str()) < 0)
    {
        if (strJudgeOweTag == "1")            //1-任何情况都判欠费
        {
            THROW_C(CRMException, CS_OWEFEE_ERR, "TradeCheck_CheckOweFee:用户已经欠费不能办理业务！");
        }
        else if (strJudgeOweTag == "2")       //2-判欠费并提示是否继续办理
        {
            ierrFag = 1;
        }
        else if (strJudgeOweTag == "3" ||     //3-仅托收不判
                 strJudgeOweTag == "4" ||       //4-仅托收不判并提示
                 strJudgeOweTag == "5" || //5-仅托收不判 （但独立帐户托收欠费判）
                 strJudgeOweTag == "6" ||   //6-托收不判往月欠费    
                 strJudgeOweTag == "7" || //无条件判断往月欠费
                 strJudgeOweTag == "8")   //非托收用户有往月欠费不允许办理，有实时欠费给予提示
        {
            if (strIdType == "1")   //传入的是用户
            {
                //先获取默认帐户标识
                CFmlBuf bufVPayrelation;
                try
                {
                    dao.Clear();
                    dao.SetParam(":VUSER_ID", strId);
                    iCount = dao.jselect(bufVPayrelation, "SEL_BY_USER","TF_A_PAYRELATION");
                }
                catch(Exception &ex)
                {
                    THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "TradeCheck_CheckOweFee:获取用户的当前默认帐户出错");
                }
                if(iCount == 0)
                {
                	  if(strTradeTypeCode=="311" || strTradeTypeCode=="310" || strTradeTypeCode=="191" || strTradeTypeCode=="7302" || strTradeTypeCode=="288")
                	  	//复机、预约正式销户、缴费复机
                	  {
                	      //未取到再取最后一个默认帐户
                        try
                        {
                            dao.Clear();
                            dao.SetParam(":VUSER_ID", strId);
                            dao.SetParam(":VDEFAULT_TAG", "1");
                            iCount = dao.jselect(bufVPayrelation, "SEL_BY_USER_MAX","TF_A_PAYRELATION");
                        }
                        catch(Exception &e)
                        {
                            THROW_AGC(CRMException, e, Grade::WARNING, CHECKBEFORETRADE_ERR, "TradeCheck_CheckOweFee:获取用户最后一个默认帐户异常！");
                        }
                    }                    
                }
                if(iCount == 0)
                {
                	  THROW_C(CRMException, CHECKBEFORETRADE_ERR, "TradeCheck_CheckOweFee:未找到用户(" + strId + ")的当前默认帐户"); 
                }
                if(iCount > 1)
                {
                    THROW_GC(CRMException, Grade::WARNING, CHECKBEFORETRADE_ERR, "TradeCheck_CheckOweFee:用户(" + strId + ")的付费关系资料错误");
                }
                strAcctId = bufVPayrelation.GetString("ACCT_ID");
            }
            else if (strIdType == "2")   //传入的是帐户
            {
                strAcctId = strId;
            }
            
            //获取帐户信息
            CFmlBuf acctBuf;
            try
            {
                //AcctUpdateMgr objAcctUpdateMgr;
                //iCount = objAcctUpdateMgr.QueryAcctInfo(acctBuf,strAcctId);
            }
            catch(Exception &ex)
            {
                THROW_AGC(CRMException, ex, Grade::WARNING, GETACCTINFO_ERR, "TradeCheck_CheckOweFee:获取帐户资料出错");
            }
            if(iCount == 0)
            {
                THROW_C(CRMException, GETACCTINFO_ERR, "TradeCheck_CheckOweFee:帐户(" + strAcctId + ")不存在");
            }

            //欠费判断：托收用户是否为独立帐户的判断
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
                    THROW_AGC(CRMException, e, Grade::WARNING, GETACCTINFO_ERR, "TradeCheck_CheckOweFee:获取指定帐户当前默认付费帐户资料异常");
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
                            THROW_AGC(CRMException, e, Grade::WARNING, GETUSERINFO_ERR, "TradeCheck_CheckOweFee:获取用户信息异常!");
                        }
                        if(userBuf.GetString("REMOVE_TAG") != "0")
                        {
                            iCount--;
                        }
                    }
                    if(iCount == 1)
                    {
                        THROW_C(CRMException, CS_OWEFEE_ERR, "TradeCheck_CheckOweFee:业务受理前条件判断:该用户为托收用户的最后一个，已经欠费，请缴费后办理业务！");
                    }
                }
            }
            //Add by liufei @ 20050707 end
            //判断用户是否托收、是否判欠费
            if ( strPayModeCode != "1" && (strJudgeOweTag == "3" || strJudgeOweTag == "5" || strJudgeOweTag == "8")) //用户不是托收，终止业务
            {
                THROW_C(CRMException, CS_OWEFEE_ERR, "TradeCheck_CheckOweFee:用户已经欠费不能办理业务！");
            }
            if ( strPayModeCode != "1" && strJudgeOweTag == "4") //用户不是托收，提示欠费
            {
                ierrFag = 1;
            }
            if(strPayModeCode != "1" && strJudgeOweTag == "6")
            {
                THROW_C(CRMException, CS_OWEFEE_ERR, "TradeCheck_CheckOweFee:用户有往月欠费不能办理业务！");
            }
            if(strJudgeOweTag == "7")     //无条件判断往月欠费
            {
                THROW_C(CRMException, CS_OWEFEE_ERR, "TradeCheck_CheckOweFee:用户有往月欠费不能办理业务！");
            }
        }
	    LOG_TRACE(logTradeCheckBeforeTrade, "退出 TradeCheck_CheckOweFee");
	    return ierrFag;
    }
    
    return ierrFag;
}

void TradeCheckBeforeTrade::TradeCheck_SvcstateTradeLimit(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "退出 TradeCheck_SvcstateTradeLimit");
    bool isRedUser=false;
    string strIdType = "1"; //设置默认ID为用户标识
    
     //吉林对欠费用户的业务操作，需要营业员有操作权限 modify xf 20091004
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
	            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "TradeCheck_SvcstateTradeLimit:查询服务状态与业务限制表出错！");
	        }
	        if (iCount > 0)
	        {
	            char chId[10];
	            sprintf(chId,"%d",(int)bufVSvcstateTradeLimit.GetInt("SERVICE_ID"));
	            string strSvcName = CParamDAO::getParam("ServiceName", chId);
	            string strSvcstateName = CParamDAO::getParam("SvcstateName", chId, bufVSvcstateTradeLimit.GetString("STATE_CODE"));
	            THROW_C_P2(CRMException, CS_USERSTATE_ERR, "TradeCheck_SvcstateTradeLimit:用户具有不能办理该业务的服务状态[%s.%s]！", strSvcName.c_str(), strSvcstateName.c_str());
	        }
	    }
	}
	LOG_TRACE(logTradeCheckBeforeTrade, "退出 TradeCheck_SvcstateTradeLimit");
}
void TradeCheckBeforeTrade::TradeCheck_UserTradelimit(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "进入 TradeCheck_UserTradelimit");
	
	LOG_TRACE(logTradeCheckBeforeTrade, "退出 TradeCheck_UserTradelimit");	
}
/**
 *  吉林：判断营业员操作是否有欠费操作权限
 *  欠费业务操作需要将业务类型和
*/
int TradeCheckBeforeTrade::CheckStaffIdRight(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
    LOG_TRACE(logTradeCheckBeforeTrade, "进入CheckStaffIdRight函数");
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
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取当前省代码出错！");
    }

   LOG_TRACE_P1(logTradeCheckBeforeTrade, "strProvince*************>>>>>=%s",strProvince.c_str());

    if(strProvince=="JLCU"){
        try  //仅在中间层cpp中获取该参数
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
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "欠费业务操作:获取当前操作权限异常！");
        }
	}
     LOG_TRACE_P1(logTradeCheckBeforeTrade, "ierrFagddddddddd*************>>>>>=%d",ierrFag);

    LOG_TRACE(logTradeCheckBeforeTrade, "退出CheckStaffIdRight函数");
    return ierrFag;
}


/**
 *  业务受理前条件判断:业务规则校验，用户服务状态与业务参数配置检查
*/
void TradeCheckBeforeTrade::CheckSvcState(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
    LOG_TRACE(logTradeCheckBeforeTrade, "进入CheckSvcState函数");

    string strTradeTypeCode = inBuf.GetString("TRADE_TYPE_CODE");
    string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");
    string strBrandCode = inBuf.GetString("BRAND_CODE");
    string strProductId = inBuf.GetString("PRODUCT_ID");

    string strUserId = inBuf.GetString("USER_ID");
    string strNetTypeCode = inBuf.GetString("NET_TYPE_CODE"); 
    
    StaticDAO& dao = StaticDAO::getDAO();
    CFmlBuf BufTag;
    string strTag="";
    try  //仅在中间层cpp中获取该参数
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
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "服务状态变更:获取当前参数出错！");
    }

	if (strTag == "1")
        CheckSvcStateNew(strUserId, strTradeTypeCode, strEparchyCode, strBrandCode, strProductId, strNetTypeCode);
    else
    	CheckSvcState(strUserId, strTradeTypeCode, strEparchyCode, strBrandCode, strProductId,strNetTypeCode);

    LOG_TRACE(logTradeCheckBeforeTrade, "退出CheckSvcState函数");
}
/**
 *业务受理前条件判断:业务规则校验，用户服务状态与业务参数配置检查
 * @param strUserId 输入 用户标识
 * @param strTradeTypeCode 输入 业务类型代码
 * @param strEparchyCode 输入 受理地州代码
 * @return 无，失败时抛出例外
*/
void TradeCheckBeforeTrade::CheckSvcState(const string &strUserId, const string &strTradeTypeCode, const string &strEparchyCode, const string &strBrandCode, const string &strProductId,const string &strNetTypeCode)
{
    StaticDAO& dao = StaticDAO::getDAO();
    CFmlBuf bufVTradesvcstatepara, bufVUserSvcstate,BufTag;
    int userSvcCount = -1;
    int userSvcStateCount=-1;
    string strTag="";
   	 try  //获取tag配置的过滤网别
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
	        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "服务状态变更:获取当前参数出错！");
	    }
    //获取用户服务状态资料
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
         THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "业务登记前检查:获取原用户服务状态资料出错");
    }
    if (userSvcCount==0)
    {
        THROW_C(CRMException, CHECKBEFORETRADE_ERR, strUserId+"业务登记前检查:没有获取到原用户服务状态资料！");
    }
    //获取服务状态变更业务参数
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
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "业务登记前检查:获取服务变更参数出错");
    }
    if (userSvcStateCount==0)
    {
        THROW_C(CRMException, CHECKBEFORETRADE_ERR, "业务登记前检查:服务变更参数没有配置！");
    }

    if (userSvcStateCount>0)
    {
        int iCount = userSvcStateCount; //用来循环
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
            THROW_C(CRMException, CHECKBEFORETRADE_ERR, "用户服务状态为空,业务无法继续！");
        }
        else
        {
            THROW_C_P2(CRMException, CHECKBEFORETRADE_ERR, "用户现在的服务状态是[%s.%s],不能办理该业务！", strSvcName.c_str(), strSvcstateName.c_str());
        }
     }
}

/**
 *业务受理前条件判断:业务规则校验，用户服务状态与业务参数配置检查
 * @param strUserId 输入 用户标识
 * @param strTradeTypeCode 输入 业务类型代码
 * @param strEparchyCode 输入 受理地州代码
 * @return 无，失败时抛出例外
*/
void TradeCheckBeforeTrade::CheckSvcStateNew(const string &strUserId, const string &strTradeTypeCode, const string &strEparchyCode,
											 const string &strBrandCode, const string &strProductId,const string strNetTypeCode)
{
    int userSvcCount = 0;
    int userSvcStateCount=0;
    StaticDAO& dao = StaticDAO::getDAO();
    
    //获取用户服务状态 1:先查是否有未完工服务状态业务.若有,则从该笔业务的台帐子表中获取,若无,则从用户服务状态子表获取
    //新增 TRADE::TF_B_TRADE::SEL_LAST_SVCSTATECHG_TRADE
       
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
         THROW_AGC(CRMException, ex, Grade::WARNING, GENEBACKUPTRADE_ERR, "生成业务台帐:获取原用户服务状态资料出错");
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
             THROW_AGC(CRMException, ex, Grade::WARNING, GENEBACKUPTRADE_ERR, "生成业务台帐:获取原用户服务状态资料出错");
        }
    }
    //获取用户服务状态资料
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
             THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "业务登记前检查:获取原用户服务状态资料出错");
        }
        if (userSvcCount==0)
        {
            THROW_C(CRMException, CHECKBEFORETRADE_ERR, strUserId+"业务登记前检查:没有获取到原用户服务状态资料！");
        }
    }
    
    //获取服务状态变更业务参数
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
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "业务登记前检查:获取服务变更参数出错");
    }
    if (userSvcStateCount==0)
    {
        THROW_C(CRMException, CHECKBEFORETRADE_ERR, "业务登记前检查:服务变更参数没有配置！");
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
			THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "业务登记前检查:获取服务状态参数出错");
		}
		if (iCount>0)
		{
			strSvcName=tempBuf.GetString("PARACODE");
			strSvcstateName=tempBuf.GetString("PARANAME");
			THROW_C_P2(CRMException, CHECKBEFORETRADE_ERR, "用户现在的服务状态是[%s.%s],不能办理该业务！", strSvcName.c_str(), strSvcstateName.c_str());
		}
		else
			THROW_C(CRMException, CHECKBEFORETRADE_ERR, "用户服务状态为空,业务无法继续！");
    }
}


/**
 * 移机+停开机校验 add by zhangyangshuo
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
    
    //获取用户服务状态 1:先查是否有未完工服务状态业务.若有,则从该笔业务的台帐子表中获取,若无,则从用户服务状态子表获取
    //新增 TRADE::TF_B_TRADE::SEL_LAST_SVCSTATECHG_TRADE
       
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
            THROW_AGC(CRMException, ex, Grade::WARNING, GETTRADEINFO_ERR, "获取关联台帐失败！");
        } 
        if(icount2>0){//前台选择是否进行关联操作传递参数
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
         THROW_AGC(CRMException, ex, Grade::WARNING, GENEBACKUPTRADE_ERR, "生成业务台帐:获取原用户服务状态资料出错");
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
             THROW_AGC(CRMException, ex, Grade::WARNING, GENEBACKUPTRADE_ERR, "生成业务台帐:获取原用户服务状态资料出错");
        }
    }
    //获取用户服务状态资料
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
             THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "业务登记前检查:获取原用户服务状态资料出错");
        }
        if (userSvcCount==0)
        {
            THROW_C(CRMException, CHECKBEFORETRADE_ERR, strUserId+"业务登记前检查:没有获取到原用户服务状态资料！");
        }
    }
    
    //获取服务状态变更业务参数
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
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "业务登记前检查:获取服务变更参数出错");
    }
    if (userSvcStateCount==0)
    {
        THROW_C(CRMException, CHECKBEFORETRADE_ERR, "业务登记前检查:服务变更参数没有配置！");
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
			THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "业务登记前检查:获取服务状态参数出错");
		}
		if (iCount>0)
		{
			strSvcName=tempBuf.GetString("PARACODE");
			strSvcstateName=tempBuf.GetString("PARANAME");
			THROW_C_P2(CRMException, CHECKBEFORETRADE_ERR, "用户现在的服务状态是[%s.%s],不能办理该业务！", strSvcName.c_str(), strSvcstateName.c_str());
		}
		else
			THROW_C(CRMException, CHECKBEFORETRADE_ERR, "用户服务状态为空,业务无法继续！");
    }
}
/**
 *  用户选择新的产品后作的判断
*/
void TradeCheckBeforeTrade::CheckAfterChangeProduct(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
    LOG_TRACE(logTradeCheckBeforeTrade, "进入CheckAfterChangeProduct函数");

    string strUserId = inBuf.GetString("USER_ID");     //用户标识
    int iOldProductId = inBuf.GetInt("PRODUCT_ID_A");  //旧产品标识
    int iNewProductId = inBuf.GetInt("PRODUCT_ID_B");  //新产品标识
    string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE"); 
    
    string strCodingStr="";

    CheckAfterChangeProduct(strUserId, iOldProductId, iNewProductId, strCodingStr, strEparchyCode);

    outBuf.SetString("X_CHECK_INFO", strCodingStr);

    LOG_TRACE(logTradeCheckBeforeTrade, "退出CheckAfterChangeProduct函数");
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
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取当前省代码出错！");
    }
    
    if (bufTag.GetString("TAG_INFO") == "TJIN")  //天津的特殊处理
    {
        //判断号码是否为套卡主卡
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
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "新产品检查:获取用户关系出错");
        }
        
        if (iCnt > 0)
        {
            //判断新产品是否允许作套卡主卡
            CFmlBuf bufProductLimit;
            try
            {
                dao.Clear();
                dao.SetParam(":VPRODUCT_ID_A", 5001); //套卡
                dao.SetParam(":VPRODUCT_ID_B", iNewProductId);
                dao.SetParam(":VLIMIT_TAG", "2");
                iCnt = dao.jselect(bufProductLimit, "SEL_EXISTS_AB_LIMIT","TD_S_PRODUCTLIMIT");
            }
            catch(Exception &ex)
            {
                THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "新产品检查:获取用户关系出错");
            }
        
            if (iCnt == 0)  //新产品不能做套卡主卡，取出副卡列表
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
                    THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "新产品检查:获取用户关系出错");
                }
                string strSNB;
                for (int i=0;i<iUserRelationCount;i++)
                {
                    if (i!=0) strSNB+="和";
                    strSNB+=bufVUserRelation.GetString("SERIAL_NUMBER_B",i);
                }
                stbOutPrompt.AddField("Prmp", "1");  //1：提示继续 2：提示中断 3：询问
                string strTemp = "该号码是号码【"+strSNB+"】的主卡，新产品不能做套卡主卡，新产品生效后两号码将取消捆绑！";
                stbOutPrompt.AddField("Prmp", strTemp.c_str());
            }
        }
    }
    
    if (stbOutPrompt.tables[0].rowCount > 0)
    {
        if ( stbOutPrompt.ToString(strCodingStr) != 0)
        {
            THROW_C(CRMException, CHECKBEFORETRADE_ERR, "新产品检查:解析字串表出错！");
        }
    }*/
}

/**
 *  业务规则校验，一卡双号用户服务号码校验
*/
void TradeCheckBeforeTrade::CheckOneCardNCodesSn(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
    LOG_TRACE(logTradeCheckBeforeTrade, "进入CheckOneCardNCodesSn函数");

    string strUserIdA = "";     //用户标识
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
    
    LOG_TRACE(logTradeCheckBeforeTrade, "退出CheckOneCardNCodesSn函数");
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
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "业务受理前条件判断:获取用户主表资料出错！");
    }
    
    try
    {
        dao.Clear();
        dao.SetParam(":VUSER_ID", strUserIdB);
        iCountB = dao.jselect(bufUserB, "SEL_BY_PK","TF_F_USER");
    }
    catch (Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "业务受理前条件判断:获取用户主表资料出错！");
    }
    if (iCountA <=0 && iCountB <=0)
    {
        THROW_C(CRMException, CHECKBEFORETRADE_ERR, "业务受理前条件判断:获取用户资料无数据！");    
    }
    iCount = -1;
    CFmlBuf bufCommparaA;
    try
    {       
       dao.Clear();
       dao.SetParam(":VSUBSYS_CODE", "CSM");
       dao.SetParam(":VPARAM_ATTR", "1014");
       dao.SetParam(":VPARAM_CODE", bufUserA.GetString("BRAND_CODE"));//品牌
       dao.SetParam(":VPARA_CODE3", (int)bufUserA.GetInt("PRODUCT_ID"));//产品
       dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
       iCount = dao.jselect(bufCommparaA, "SEL2_PK_TD_S_COMMPARA","TD_S_COMMPARA");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "业务受理前条件判断:查询通用参数表出错！");
    }
   
    if(iCount > 0)
    {
		if (bufCommparaA.GetString("PARA_CODE2") == "1")
		{
				THROW_C(CRMException, CHECKBEFORETRADE_ERR, "业务受理前条件判断:用户主号码不可以为此品牌["+bufCommparaA.GetString("PARAM_NAME")+"]！");    		
		} 
		if (bufCommparaA.GetString("PARA_CODE2") == "6")
		{
				THROW_C(CRMException, CHECKBEFORETRADE_ERR, "业务受理前条件判断:用户主号码不可以为此品牌["+bufCommparaA.GetString("PARAM_NAME")+"]！");    		
		} 
		if (bufCommparaA.GetString("PARA_CODE2") == "3" && atoi(string(bufCommparaA.GetString("PARA_CODE3")).c_str()) == (int)bufUserA.GetInt("PRODUCT_ID"))
		{
				THROW_C(CRMException, CHECKBEFORETRADE_ERR, "业务受理前条件判断:用户主号码不可以为此品牌["+bufCommparaA.GetString("PARAM_NAME")+"]的此产品["+bufCommparaA.GetString("PARA_CODE4")+"]！");    		
		}   
    }
    
    iCount = -1;
    CFmlBuf bufCommparaB;
    try
    {       
       dao.Clear();
       dao.SetParam(":VSUBSYS_CODE", "CSM");
       dao.SetParam(":VPARAM_ATTR", "1014");
       dao.SetParam(":VPARAM_CODE", bufUserB.GetString("BRAND_CODE"));//品牌
       dao.SetParam(":VPARA_CODE3", (int)bufUserB.GetInt("PRODUCT_ID"));//产品
       dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
       iCount = dao.jselect(bufCommparaB, "SEL2_PK_TD_S_COMMPARA","TD_S_COMMPARA");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "业务受理前条件判断:查询通用参数表出错！");
    }
   
    if(iCount > 0)
    {
		if (bufCommparaB.GetString("PARA_CODE2") == "2")
		{
				THROW_C(CRMException, CHECKBEFORETRADE_ERR, "业务受理前条件判断:用户副号码不可以为此品牌["+bufCommparaB.GetString("PARAM_NAME")+"]！");    		
		}
		if (bufCommparaB.GetString("PARA_CODE2") == "6")
		{
				THROW_C(CRMException, CHECKBEFORETRADE_ERR, "业务受理前条件判断:用户副号码不可以为此品牌["+bufCommparaB.GetString("PARAM_NAME")+"]！");    		
		}
		if (bufCommparaB.GetString("PARA_CODE2") == "4" && atoi(string(bufCommparaB.GetString("PARA_CODE3")).c_str()) == (int)bufUserB.GetInt("PRODUCT_ID"))
		{
				THROW_C(CRMException, CHECKBEFORETRADE_ERR, "业务受理前条件判断:用户副号码不可以为此品牌["+bufCommparaB.GetString("PARAM_NAME")+"]的此产品["+bufCommparaB.GetString("PARA_CODE4")+"]！");    		
		}
    }
    if (bufCommparaA.GetString("PARA_CODE1") != "" && bufCommparaB.GetString("PARA_CODE1") != "")
    {
		if (atoi(string(bufCommparaA.GetString("PARA_CODE1")).c_str()) < atoi(string(bufCommparaB.GetString("PARA_CODE1")).c_str()))
		{
				THROW_C(CRMException, CHECKBEFORETRADE_ERR, "业务受理前条件判断:用户主号码品牌["+bufCommparaA.GetString("PARAM_NAME")+"]级别不可以低于副号码品牌["+bufCommparaB.GetString("PARAM_NAME")+"]级别！");    				
		}
  	}
  	    
}

/**
 *  获取最大一条有效的捆绑销售记录
*/
void TradeCheckBeforeTrade::GetUserLastValidBindSaleInfo(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
    LOG_TRACE(logTradeCheckBeforeTrade, "进入GetUserLastValidBindSaleInfo函数");
    StaticDAO& dao = StaticDAO::getDAO();    
    try
    {
	    string strUserId = inBuf.GetString("USER_ID"); 	
		int iCount;
		CFmlBuf bufUserPurchase;
	    iCount=GetUserLastValidBindSaleInfo(bufUserPurchase, strUserId);
    
    	if(iCount>0)
    	{
		   
		    //获取系统时间
		    DualDAO daoDual;
		    string strSysDate;
		    daoDual.Clear();
		    daoDual.jselect(strSysDate,"GET_SYSDATE");
		    //yyyy-mm-dd hh24:mi:ss
		    string strDate1,strDate2;
		    strDate1 = string(bufUserPurchase.GetString("END_DATE")).substr(0,4) + string(bufUserPurchase.GetString("END_DATE")).substr(5,2);
		    strDate2 = strSysDate.substr(0,4) + strSysDate.substr(5,2);
		    //modify by digy@20070530 删除 = 的条件，否则会造成办理时间与购机协议终止时间在同一月但
		    //协议还没到期的用户进行二次绑定 
		    if(atol(strDate1.c_str()) <  atol(strDate2.c_str())) 
		    {
		        outBuf.SetInt("X_RECORDNUM",0);
		    }
		    else
		    {
		        outBuf.SetInt("X_RECORDNUM",iCount);
		        //add by digy@20070507 start 获取当前帐期与购机用户开始帐期 为下面调用帐务接口用
		    	int iCurAcycid; // 获取当前帐期
		    	daoDual.Clear();
		    	daoDual.jselect(iCurAcycid,"GET_ACYCID");
		    	
		    	int iStartAcycid; //获取购机用户开始帐期 
		    	daoDual.Clear();
		    	daoDual.SetParam(":VDATE", bufUserPurchase.GetString("START_DATE"));
		    	daoDual.jselect(iStartAcycid,"GET_ACYCID_BY_DATE");
		    	
		    	//只取用户当前帐期的上一帐期的累积消费
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
                    THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "获取购机参数出错！");
                }
                if( iRowCount == 0 )
                {
                	THROW_C(CRMException, CHECKBEFORETRADE_ERR, "根据购机方案,获取购机参数无数据！");
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
        outBuf.SetString("X_RESULTINFO", "获取有效捆绑销售记录资料成功！");
        outBuf.SetString("X_REMARK1","") ; //返回前台为终止协议结束日期用
	    outBuf.SetString("X_REMARK2","") ; //返回前台为终止协议结束日期用
    }
    catch(Exception &e)
    {
        logTradeCheckBeforeTrade.alert(e);
        outBuf.SetInt("X_RECORDNUM",-1);
        outBuf.SetInt("X_RESULTCODE", e.getCode());
        outBuf.SetString("X_RESULTINFO", "获取有效捆绑销售记录资料:" + e.toString());
    }
    
    LOG_TRACE(logTradeCheckBeforeTrade, "退出GetUserLastValidBindSaleInfo函数");
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
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "业务受理前条件判断:获取用户有效捆绑销售资料出错！");
    }
    
    return iCount;
}

//调用完毕帐务流程 QAM_YEARFEEQRY_OS 获取了用户的年协消费额后，处理帐务返回结果
void TradeCheckBeforeTrade::CheckBindSaleAcctReturn(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "进入CheckBindSaleAcctReturn函数");

    float fUsedFee = 0;
    //int iFoundCount = 0;
	float fTotalFee = 0;
	float fCurrFee  = 0;
    
    string strAddupValue;

	//获取可清退时间，以及消费额度
    int iAcctCount = inBuf.GetInt("X_RECORDNUM");
	string strUserId = inBuf.GetString("USER_ID");
	string strConsumeValue = inBuf.GetString("CONSUME_VALUE");
	fTotalFee = atof(strConsumeValue.c_str())/(float)100;
	string strEndDate = inBuf.GetString("END_DATE");
	string strStartDate = inBuf.GetString("START_DATE");
	string strBindSaleAttr = inBuf.GetString("BINDSALE_ATTR");
    
	if (iAcctCount == 0 || !inBuf.IsFldExist("ADDUP_VALUE") )//未查询到年协累计消费值,按照实效时间来判断
	{
		THROW_C_P2(CRMException, CHECKBEFORETRADE_ERR, "未到捆绑销售结束期限[%s],未满消费额度,不能办理捆绑销售业务:消费额度[%.2f],已经消费 0 元",strEndDate.c_str(),fTotalFee);
	}
	
	strAddupValue = inBuf.GetString("ADDUP_VALUE");
	fUsedFee = atof(strAddupValue.c_str())/100.00 ;
	if (fUsedFee >= fTotalFee ) 
	{
		outBuf.SetInt("X_RESULTCODE",0);
		outBuf.SetString("X_RESULTINFO","Acct Purchase check ok!");
		outBuf.SetString("X_REMARK1",strBindSaleAttr) ; //返回前台为终止协议结束日期用
	    outBuf.SetString("X_REMARK2",strStartDate) ; //返回前台为终止协议结束日期用
		//iFoundCount ++;
		return;
	}
	//未到期,消费额度也未满,报错
	fCurrFee  = fUsedFee ;
    outBuf.SetString("X_REMARK1","") ; //返回前台为终止协议结束日期用
    outBuf.SetString("X_REMARK2","") ; //返回前台为终止协议结束日期用
    LOG_TRACE(logTradeCheckBeforeTrade, "退出CheckBindSaleAcctReturn函数");
    THROW_C_P3(CRMException, CHECKBEFORETRADE_ERR, "未到捆绑销售结束期限[%s],未满消费额度,不能办理捆绑销售业务:消费额度:%.2f元,已经消费:%.2f元",strEndDate.c_str(),fTotalFee,fCurrFee);
}

//检查其他捆绑销售类型的记录是否到期
void TradeCheckBeforeTrade::CheckOtherBindSale(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "进入CheckOtherBindSale函数");

	string strBindSaleAttr = inBuf.GetString("BINDSALE_ATTR");
	int iDepositCode = inBuf.GetInt("DEPOSIT_CODE");
	string strUserId = inBuf.GetString("USER_ID");
	string strAcctId = inBuf.GetString("ACCT_ID");
    StaticDAO& dao = StaticDAO::getDAO();
    CFmlBuf bufRecordCount;
	if(strBindSaleAttr=="1" && iDepositCode!=-1)  //2倍预存0元购机
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
	        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "获取指定帐户指定存折金额出错！");
	    }
	    if(iCount > 0)
	    {
		    THROW_C(CRMException, CHECKBEFORETRADE_ERR, "协议预存款未消费完,或者未到捆绑销售结束期限,不能办理捆绑销售业务！");
	    }
	}	
    else
	{
	    THROW_C(CRMException, CHECKBEFORETRADE_ERR, "未到捆绑销售结束期限,不能办理捆绑销售业务！");
	}
    outBuf.SetInt("X_RESULTCODE", 0);
    outBuf.SetString("X_RESULTINFO", "其他捆绑销售类型记录是否到期校验成功!");
    outBuf.SetString("X_REMARK1","") ; //返回前台为终止协议结束日期用
    outBuf.SetString("X_REMARK2","") ; //返回前台为终止协议结束日期用
    LOG_TRACE(logTradeCheckBeforeTrade, "退出CheckOtherBindSale函数");
}
void TradeCheckBeforeTrade::GetParaForAddupValue(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
    LOG_TRACE(logTradeCheckBeforeTrade, "进入GetParaForAddupValue函数");
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
        THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "业务受理前条件判断:获取用户有效捆绑销售资料出错！");
    }
    
    if(iCount>0)
    {
		//add by digy@20070507 start 获取当前帐期与购机用户开始帐期 为下面调用帐务接口用
		int iCurAcycid; // 获取当前帐期
		daoDual.Clear();
		daoDual.jselect(iCurAcycid,"GET_ACYCID");
		
		int iEndAcycid;
		daoDual.Clear();
		daoDual.SetParam(":VDATE", bufUserPurchase.GetString("END_DATE"));
		daoDual.jselect(iEndAcycid,"GET_ACYCID_BY_DATE");
		
		int iStartAcycid; //获取购机用户开始帐期 
		daoDual.Clear();
		daoDual.SetParam(":VDATE", bufUserPurchase.GetString("START_DATE"));
		daoDual.jselect(iStartAcycid,"GET_ACYCID_BY_DATE");
		
		//由于存在协议到期若干月后的用户退押金，所以先判断当前帐期与结束帐期
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
   	       	THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "获取购机参数出错！");
   	    }
   	    if( iRowCount == 0 )
   	    {
   	    	THROW_C(CRMException, CHECKBEFORETRADE_ERR, "根据购机方案,获取购机参数无数据！");
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
    outBuf.SetString("X_RESULTINFO", "获取用户购机信息成功！");
    LOG_TRACE(logTradeCheckBeforeTrade, "退出GetParaForAddupValue函数");
}
//调用完毕帐务流程 QAM_YEARFEEQRY_OS 获取了用户的年协消费额后，处理帐务返回结果(清退押金用)
void TradeCheckBeforeTrade::CheckAcctReturn(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "进入CheckAcctReturn函数");

    float fUsedFee = 0;
    //int iFoundCount = 0;
	float fTotalFee = 0;
	float fCurrFee  = 0;
    string strAddupValue;

	//以及消费额度
    int iAcctCount = inBuf.GetInt("X_RECORDNUM");
	string strConsumeValue = inBuf.GetString("CONSUME_VALUE");
	fTotalFee = atof(strConsumeValue.c_str())/(float)100;

	if (iAcctCount == 0)//未查询到年协累计消费值
	{
		THROW_C_P1(CRMException, CHECKBEFORETRADE_ERR, "未满消费额度,不能办理此业务:消费额度[%.2f],已经消费 0 元",fTotalFee);
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
	//消费额度也未满,报错
	fCurrFee  = fUsedFee ;
    LOG_TRACE(logTradeCheckBeforeTrade, "退出CheckAcctReturn函数");
    THROW_C_P2(CRMException, CHECKBEFORETRADE_ERR, "未满消费额度,不能办理此业务:消费额度:%.2f元,已经消费:%.2f元",fTotalFee,fCurrFee);
}

//校验出帐日积分业务限制 add by qiumb@2007-03-05
void TradeCheckBeforeTrade::TradeCheck_CheckScoreTradeLimit(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "进入 TradeCheck_CheckScoreTradeLimit 函数");
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
            THROW_AGC(CRMException, ex, Grade::WARNING, CHECKBEFORETRADE_ERR, "业务登记后条件判断:获取积分业务出帐日限制数据！");
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
                THROW_C(CRMException, CS_TRADELIMIT_ERR, "TradeCheck_CheckScoreTradeLimit:系统出帐日禁止办理积分类业务！");
            }
        }        
    }
    if ( strTradeTypeCode == "330"  )
    {
     //   //获取累计积分
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
    	//	THROW_AGC(CRMException, e, Grade::WARNING, GETUSERSCOREINFO_ERR, "获取用户累计积分异常！");
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
     //      // THROW_C(CRMException, CHECKBEFORETRADE_ERR, "TradeCheck_CheckScoreTradeLimit:用户当前没有可用积分，业务受限！");
     //   }
        
        //int iCount = 0;
        //获取产品的总部品牌
        //CFmlBuf bufProduct;
        //try
        //{
       //     dao.Clear();
        //    dao.SetParam(":VPRODUCT_ID",inBuf.GetString("PRODUCT_ID"));
       //     iCount = dao.jselect(bufProduct,"SEL_BY_PK","TD_B_PRODUCT");
       // }
       // catch(Exception &e)
       // {
       //   THROW_AGC(CRMException, e, Grade::WARNING, GETNORMALPARA_ERR, "获取产品信息失败！");
       // }
       // if ( iCount < 1)
        //{
        //    THROW_C(CRMException, CHECKBEFORETRADE_ERR, "TradeCheck_CheckScoreTradeLimit:获取产品信息无数据！");
       // }
        
        //modified by qiumb@20071009  判断是否达到产品的积分兑换门槛
       // int iCriticalScoreValue = 3000;
        //ss.clear();
        //ss<<bufProduct.GetString("SCORE_VALUE");
        //ss>>iCriticalScoreValue;
		//if ( iCriticalScoreValue > iAllScoreSum)
        //{
        //    THROW_C_P4(CRMException, CHECKBEFORETRADE_ERR, "TradeCheck_CheckScoreTradeLimit:用户当前的累计积分[%d]未达到当前产品[%d:%s]的积分兑换门槛[%s]，业务受限！",iAllScoreSum, bufProduct.GetInt("PRODUCT_ID"), string(bufProduct.GetString("PRODUCT_NAME")).c_str() ,string(bufProduct.GetString("SCORE_VALUE")).c_str());
       // }
    	
	LOG_TRACE(logTradeCheckBeforeTrade, "退出 TradeCheck_CheckScoreTradeLimit 函数");	
    }
}
//校验 有产品变更限制的业务，提示产品变更相关信息 add by digy@2007-05-28
void TradeCheckBeforeTrade::TradeCheck_CheckHintChgProduct(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "进入 TradeCheck_CheckHintChgProduct 函数");

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
	    THROW_AGC(CRMException,ex,Grade::WARNING,CHECKBEFORETRADE_ERR,"TradeCheck_CheckHintChgProduct:查询用户未完工的限制业务信息出错！");
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
	        THROW_AGC(CRMException,ex,Grade::WARNING,CHECKBEFORETRADE_ERR,"TradeCheck_CheckHintChgProduct:查询用户预约产品变更台帐信息出错！");
	    }
	    
	    //abc待处理2
	    /*
	    if( iCount > 0 )
		{
		   //获得原产品信息
		   int iOldPrdId ;
		   if( bufTrade.GetString("RSRV_STR2") == "" ) iOldPrdId = 0;
		   else iOldPrdId = atoi(string(bufTrade.GetString("RSRV_STR2")).c_str()) ;
		   char chOldPrdId[10];
           sprintf(chOldPrdId,"%d",iOldPrdId );
           string strOldPrdName = CParamDAO::getParam("ProductName", chOldPrdId);	
           
           //获得新产品信息
           int iNewPrdId ;
		   iNewPrdId = (int)bufTrade.GetInt("PRODUCT_ID") ; 
		   char chNewPrdId[10];
           sprintf(chNewPrdId,"%d",iNewPrdId );
           string strNewPrdName = CParamDAO::getParam("ProductName", chNewPrdId);
           
           string strAcceptDate = bufTrade.GetString("ACCEPT_DATE") ;
           string strExceTime = bufTrade.GetString("EXEC_TIME") ;	
           
           string strInfo = "业务限制:用户"+strAcceptDate+"预约将产品:" + strOldPrdName+"["+chOldPrdId+"],改成产品:";
           strInfo = strInfo+strNewPrdName+"["+chNewPrdId+"]," +strExceTime+"生效,新产品没生效前,此业务不能办理！" ;
           
           THROW_C(CRMException, CHECKBEFORETRADE_ERR, strInfo.c_str());
           
	    }*/
	        
	}

	LOG_TRACE(logTradeCheckBeforeTrade, "退出 TradeCheck_CheckHintChgProduct 函数");	
}
//校验 清单查询前的用户屏弊级别 add by zhenjing@2008-06-19
void TradeCheckBeforeTrade::TradeCheck_CheckBillType(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "进入 TradeCheck_CheckBillType 函数");

    
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
	    THROW_AGC(CRMException,ex,Grade::WARNING,CHECKBEFORETRADE_ERR,"TradeCheck_CheckHintChgProduct:查询用户服务表出错！");
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
	    THROW_AGC(CRMException,ex,Grade::WARNING,CHECKBEFORETRADE_ERR,"TradeCheck_CheckHintChgProduct:查询用户服务表出错！");
	}
	}
  if( iCount > 0 )
		{
			LOG_TRACE_P1(logTradeCheckBeforeTrade,"44444444444444,iRowCount:%s",bufBilltypeLimit.GetString("SERV_PARA1").c_str());
			if(bufBilltypeLimit.GetString("SERV_PARA1")=="1")
			{
			
				string strInfo = "该用户不允许查所有的话单";
			  THROW_C(CRMException, CHECKBEFORETRADE_ERR, strInfo.c_str());
			}
			if(bufBilltypeLimit.GetString("SERV_PARA1")=="2")
			{
			if(strBillType=="0")
			   {
			   
			    string strInfo = "该用户不允许查询话费信息";
			    THROW_C(CRMException, CHECKBEFORETRADE_ERR, strInfo.c_str());
			   }
			}
			if(bufBilltypeLimit.GetString("SERV_PARA1")=="11")
			{
				
				 string strInfo = "该用户不允许查所有的话单";
			   THROW_C(CRMException, CHECKBEFORETRADE_ERR, strInfo.c_str());
			}
			if(bufBilltypeLimit.GetString("SERV_PARA1")=="12")
			{
				if(strBillType!="2")
			   {
			   
			    string strInfo = "该用户只允许查询详单信息";
			    THROW_C(CRMException, CHECKBEFORETRADE_ERR, strInfo.c_str());
			   }
			}
			if(bufBilltypeLimit.GetString("SERV_PARA1")=="13")
			{
				if(strBillType!="0")
			   {
			   	
			    string strInfo = "该用户只允许查询话费信息";
			    THROW_C(CRMException, CHECKBEFORETRADE_ERR, strInfo.c_str());
			   }
			}
		  
	        
	}

	LOG_TRACE(logTradeCheckBeforeTrade, "退出 TradeCheck_CheckBillType 函数");	
}

//校验 客户资料查询前的用户屏弊级别 add by zhenjing@2008-07-16
void TradeCheckBeforeTrade::TradeCheck_CheckCustType(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "进入 TradeCheck_CheckCustType 函数");

    
    
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
	    THROW_AGC(CRMException,ex,Grade::WARNING,CHECKBEFORETRADE_ERR,"TradeCheck_CheckCustType:查询用户服务表出错！");
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
	    THROW_AGC(CRMException,ex,Grade::WARNING,CHECKBEFORETRADE_ERR,"TradeCheck_CheckCustType:查询用户服务表出错！");
	}
	}
	
  if( iCount > 0 )
		{
			LOG_TRACE_P1(logTradeCheckBeforeTrade,"44444444444444,iRowCount:%s",bufCusttypeLimit.GetString("SERV_PARA1").c_str());
			if(bufCusttypeLimit.GetString("SERV_PARA1")=="1")
			{
			
				string strInfo = "客户,帐单,所有祥单资料都不允许查询";
			  THROW_C(CRMException, CHECKBEFORETRADE_ERR, strInfo.c_str());
			}
			if(bufCusttypeLimit.GetString("SERV_PARA1")=="2")
			{
			
			  string strInfo = "客户,帐单,话费祥单资料都不允许查询";
			  THROW_C(CRMException, CHECKBEFORETRADE_ERR, strInfo.c_str());
			}
			
	        
	}

	LOG_TRACE(logTradeCheckBeforeTrade, "退出 TradeCheck_CheckCustType 函数");	
}

//检查开户用户限制数
void TradeCheckBeforeTrade::CheckOpenLimit(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "---------------退出 CheckOpenLimit 函数");
	int iCount(0);
	string strCustId = inBuf.GetString("CUST_ID");
	string strEparchyCode = inBuf.GetString("TRADE_EPARCHY_CODE");
	StaticDAO& dao = StaticDAO::getDAO();
	//查询客户下不同网别用户数
	CFmlBuf  bufUsrNum;
	try
	{
	    dao.Clear();
	    dao.SetParam(":VCUST_ID",strCustId);
	    dao.jselect(bufUsrNum, "SEL_USER_NUM_GROUPNET","TF_F_CUSTOMER");	    
	}
	catch (Exception &ex)
	{
	    THROW_AGC(CRMException,ex,Grade::WARNING,CHECKBEFORETRADE_ERR,"CheckOpenLimit:查询客户下不同网别用户数出错！");
	}
	LOG_TRACE_P1(logTradeCheckBeforeTrade, "归属客户用户:%s",bufUsrNum.ToString().c_str());
	
	string strMsg = "";
	int iCheckTag = 0;//提示,允许办理业务

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
		THROW_AGC(CRMException,ex,Grade::WARNING,CHECKBEFORETRADE_ERR,"CheckOpenLimit:查询开户客户限制数配置数据出错!");
	}
	LOG_TRACE_P1(logTradeCheckBeforeTrade, "开户客户限制信息:%s",bufParam.ToString().c_str());

	string strUserNum = "0";
	int iAllUser = 0;
	string strAllLimit = "0";
	for(int j=0;j<bufParam.GetCount("PARAM_ATTR");++j)
	{
		strUserNum = "0";
		if(bufParam.GetString("PARA_CODE1",j)=="ZZ")//所有网别总数限制
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
						&&atoi(bufParam.GetString("PARA_CODE2",j).c_str())!=0)//0表示不限制
					{
						iCheckTag = 1;//禁止办理业务
					}
					break;
				}
			}
		}
		iAllUser += atoi(strUserNum.c_str());

		strMsg += "网别为"+bufParam.GetString("NET_TYPE_NAME",j)+"的当前开户数:"+strUserNum+",最大开户数为:"+bufParam.GetString("PARA_CODE2",j)+"\n";		
	}

	//总开户数限制办理业务判断
	if(iCheckTag==0&&iAllUser>=atoi(strAllLimit.c_str()))
	{
		iCheckTag = 1;//禁止办理业务
	}
	string strAllNum = to_string(iAllUser);
	strMsg += "客户当前开户总数为:"+strAllNum+",最大开户总数为:"+strAllLimit;

	outBuf.SetString("X_CHECK_INFO",strMsg);
	outBuf.SetInt("X_CHECK_TAG",iCheckTag);
	
	LOG_TRACE(logTradeCheckBeforeTrade, "退出 CheckOpenLimit 函数");
}

//检查过户限制数add by weiliangyu@2008-07-25
void TradeCheckBeforeTrade::CheckChangeCustOwnerNum(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "进入 CheckChangeCustOwnerNum 函数");
   
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
	    LOG_TRACE_P1(logTradeCheckBeforeTrade,"查询过户次数:%s",bufBillInfo.ToString().c_str());
	}
	catch (Exception &ex)
	{
	    THROW_AGC(CRMException,ex,Grade::WARNING,CHECKBEFORETRADE_ERR,"CheckChangeCustOwnerNum:查询历史台帐表出错！！");
	}	   
    outBuf.SetInt("X_RESULTCODE", 0);
    outBuf.SetString("X_RESULTINFO", "查询过户次数成功！");
    outBuf.SetInt("X_RECORDNUM", iCount);
    
    if(iCount>=tag)
	{
        string strInfo = "该号码已达到过户次数最大值，不能再办理过户业务！！";
		THROW_C(CRMException, CHECKBEFORETRADE_ERR, strInfo.c_str());			
    }
    
	LOG_TRACE(logTradeCheckBeforeTrade, "退出 CheckChangeCustOwnerNum 函数");	
}
//检查用户是否有未完工的限制业务add by zhouchangxuan@2009-10-08
void TradeCheckBeforeTrade::CheckTradeState(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
	LOG_TRACE(logTradeCheckBeforeTrade, "进入 CheckTradeState");

    if (!inBuf.IsFldExist("USER_ID"))
	{
		THROW_C(CRMException, CHECKAFTERTRADE_ERR, "CheckTradeState:无用户标识！");
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
            THROW_C_P1(CRMException, TRADENOTCOMPLETED_ERR, "CheckTradeState:用户有未完工的限制业务【%s】！", strTradeType.c_str());
        }
    }
    catch (CRMException &ex) {throw;}
    catch (Exception &ex)
    {
        THROW_AGC(CRMException,ex,Grade::WARNING,CHECKBEFORETRADE_ERR,"CheckTradeState:查询用户未完工的限制业务信息出错！");
    }

    LOG_TRACE(logTradeCheckBeforeTrade, "退出 CheckTradeState");
}

//EOPN拆机校验
void TradeCheckBeforeTrade::CheckDestroyEPONUser(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
		LOG_TRACE(logTradeCheckBeforeTrade, "进入 CheckDestroyEPONUser");
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
				THROW_AGC(CRMException,ex,Grade::WARNING,CHECKBEFORETRADE_ERR,"CheckDestroyEPONUser:查询用户群组信息出错！");
		}
		if(countUU>0)
		{
			LOG_TRACE(logTradeCheckBeforeTrade, "cchyc  1");
			LOG_TRACE_P1(logTradeCheckBeforeTrade, "cchyc  【%s】",bufUU.GetString("ROLE_CODE_B").c_str());
			if(bufUU.GetString("ROLE_CODE_B")=="1")
			{
				LOG_TRACE(logTradeCheckBeforeTrade, "cchyc  2");
					THROW_C(CRMException, CHECKBEFORETRADE_ERR, "该用户为主号码，不可拆除！！");
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
				THROW_AGC(CRMException,ex,Grade::WARNING,CHECKBEFORETRADE_ERR,"CheckDestroyEPONUser:查询用户群组信息出错！");
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
						THROW_AGC(CRMException,ex,Grade::WARNING,CHECKBEFORETRADE_ERR,"CheckDestroyEPONUser:查询用户群组信息出错！");
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
							THROW_AGC(CRMException,ex,Grade::WARNING,CHECKBEFORETRADE_ERR,"CheckDestroyEPONUser:查询用户群组信息出错！");
					}
					if (count1==0){
							THROW_C(CRMException, CHECKBEFORETRADE_ERR, "该用户群组内还存在语音业务，不可拆除最后一个数据业务！！");
					}
				}
		}
		LOG_TRACE(logTradeCheckBeforeTrade, "退出 CheckDestroyEPONUser");
}

/**
 * 总部CRM发起的 全国无线上网卡异地业务 限制
 */
void TradeCheckBeforeTrade::IsRequestFromUcrm(CFmlBuf &inBuf, CFmlBuf &outBuf)
{
    LOG_TRACE(logTradeCheckBeforeTrade, "进入 IsRequestFromUcrm函数");
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
                THROW_AGC(CRMException,ex,Grade::WARNING,CHECKBEFORETRADE_ERR,"查询用户信息出错！");
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
                    outBuf.SetString("X_RESULTINFO", "该用户不能通过该渠道受理此业务！");
                    THROW_C(CRMException, CHECKAFTERTRADE_ERR, "该用户不能通过该渠道受理此业务！");
                }
            }
        }
    }
    LOG_TRACE(logTradeCheckBeforeTrade, "退出 IsRequestFromUcrm函数");
}

