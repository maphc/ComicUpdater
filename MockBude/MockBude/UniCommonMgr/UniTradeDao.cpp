#include "stdafx.h"
#include "../commondao/StaticDAO.h"
#include "UniTradeDao.h"
#include "../log4cpp/Logger.h"
#include "../commondao/DualDAO.h"
#include "UniDataMgr.h"

using namespace MiscDom;
using namespace log4cpp;

static Logger& logUniTradeMgr = Logger::getLogger("gboss.crm.CustServ");

UniTradeDao * UniTradeDao::instance;



UniTradeDao::UniTradeDao(){
}

UniTradeDao & UniTradeDao::getInstance() {
    
    if (instance == 0) {
        instance = new UniTradeDao();
    }
    
    return *instance;
}

int UniTradeDao::queryCommparaInfo(DataBuf &result,DataBuf &inparam) 
{
	StaticDAO& dao = StaticDAO::getDAO();
	
	int iRowCount = 0;
	if (inparam.IsFldExist("EPARCHY_CODE"))
	{
		try
		{
			dao.Clear();
			dao.SetParam(":VSUBSYS_CODE", inparam.GetString("SUBSYS_CODE"));
			dao.SetParam(":VPARAM_ATTR", inparam.GetInt("PARAM_ATTR"));
			dao.SetParam(":VEPARCHY_CODE", inparam.GetString("EPARCHY_CODE"));
			
			if(!inparam.IsFldExist("PARAM_CODE")&&!inparam.IsFldExist("PARA_CODE1"))
			{
				iRowCount=dao.jselect(result, "SEL_BY_ATTR", "TD_S_COMMPARA");
			}
			else if (!inparam.IsFldExist("PARAM_CODE")&&inparam.IsFldExist("PARA_CODE1"))
			{
				dao.SetParam(":VPARA_CODE1", inparam.GetString("PARA_CODE1"));
				iRowCount=dao.jselect(result, "SEL_BY_PARA1", "TD_S_COMMPARA");
			}
			else if(inparam.IsFldExist("PARAM_CODE")&&!inparam.IsFldExist("PARA_CODE1"))
			{
				dao.SetParam(":VPARAM_CODE", inparam.GetString("PARAM_CODE"));
				iRowCount=dao.jselect(result, "SEL1_PK_TD_S_COMMPARA", "TD_S_COMMPARA");
			}
			else
			{
				dao.SetParam(":VPARAM_CODE", inparam.GetString("PARAM_CODE"));
				dao.SetParam(":VPARA_CODE1", inparam.GetString("PARA_CODE1"));
				iRowCount=dao.jselect(result, "SEL3_PK_TD_S_COMMPARA", "TD_S_COMMPARA");
			}
		}
		catch(Exception &ex)
		{
		    THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡͨ�ò����쳣��");
		}
	}
	else
	{
		try
		{
			dao.Clear();
			dao.SetParam(":VSUBSYS_CODE", inparam.GetString("SUBSYS_CODE"));
			dao.SetParam(":VPARAM_ATTR", inparam.GetInt("PARAM_ATTR"));
			iRowCount=dao.jselect(result, "SEL_ALL_BY_NOEPARCHY", "TD_S_COMMPARA");
		}
		catch(Exception &ex)
		{
		    THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡͨ�ò����쳣��");
		}
	}
    return iRowCount;
}

int UniTradeDao::getStaffRight(DataBuf &staffRightBuf,const string &strStaffId)
{
	StaticDAO& dao = StaticDAO::getDAO();
	
	int iRowCount = 0;
	
	try
    {
        dao.Clear();
        dao.SetParam(":VSTAFF_ID", strStaffId);
        iRowCount = dao.jselect(staffRightBuf, "SEL_BY_STAFFID","TF_M_STAFFDATARIGHT");
    }
    catch(Exception &ex)
    {
        THROW_AGC_P1(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡԱ��[%s]����Ȩ���쳣��", strStaffId.c_str());
    }
    
    return iRowCount;
}

int UniTradeDao::getNodeParser(DataBuf &NodeParser)
{
	StaticDAO& dao = StaticDAO::getDAO();
	
	int iRowCount = 0;
	try
    {
        dao.Clear();
        iRowCount = dao.jselect(NodeParser, "SEL_ALL","TD_S_NODEPARSER");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡ�ڵ��������Ϣ�쳣��");
    }
    
    return iRowCount;
}

int UniTradeDao::queryFeeItemInfo(DataBuf &feeItem,const string &strEparchyCode)
{
	StaticDAO& dao = StaticDAO::getDAO();
	
	int iRowCount = 0;
	
	try
    {
        dao.Clear();
        dao.SetParam(":VTRADE_EPARCHY_CODE", strEparchyCode);
        iRowCount = dao.jselect(feeItem, "SEL_ALL","TD_B_FEEITEM");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡ���ñ�����Ϣ�쳣��");
    }
    
    return iRowCount;
}

int  UniTradeDao::queryMphoneIdle(DataBuf &PhoneInfo,const string &serialNumber,const string &netTypeCode,const string &strEparchyCode)
{
	StaticDAO& dao = StaticDAO::getDAO();
	

	int iRowCount = 0;
	
	try
    {
        dao.Clear();
        dao.SetParam(":VSERIAL_NUMBER",serialNumber);
        dao.SetParam(":VNET_TYPE_CODE",netTypeCode);
        dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
        iRowCount = dao.jselect(PhoneInfo, "SEL_BY_PK","TF_R_MPHONECODE_IDLE");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡ������б���Ϣ�쳣��");
    }
    

    return iRowCount;
	   
}

int UniTradeDao::getSimIdleByCardType(DataBuf &simIdle, const string &simcardType,const string &eparchyCode)
{
	int iRowCount = 0;
	return iRowCount;
}

int UniTradeDao::getSysTagInfo(DataBuf &tagInfo,const string &sysCode ,const string &tagCode,const string &strEparchyCode)
{
	StaticDAO& dao = StaticDAO::getDAO();
	
	int iRowCount = 0;
	
	try
    {
        dao.Clear();
        dao.SetParam(":VSUBSYS_CODE", sysCode);
        dao.SetParam(":VTAG_CODE",tagCode);
        dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
        dao.SetParam(":VUSE_TAG", "0");
        iRowCount = dao.jselect(tagInfo, "SEL_BY_TAGCODE_1","TD_S_TAG");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡϵͳ���Բ�������Ϣ�쳣��");
    }
    
    return iRowCount;
	   
}

int UniTradeDao::getUserResByResType(DataBuf &userResInfo, const string &userId,const string &strResTypeCode) 
{
	StaticDAO& dao = StaticDAO::getDAO();
	

	int iRowCount = 0;
	
	try
	{
	    dao.Clear();
	    dao.SetParam(":VUSER_ID", userId);
	    dao.SetParam(":VRES_TYPE_CODE",strResTypeCode);
	    iRowCount = dao.jselect(userResInfo, "SEL_BY_USERID_ONE","TF_F_USER_RES");
	}
	catch(Exception &e)
	{
	    THROW_AGC(CRMException, e, Grade::WARNING, CALLCENTERENCODESTR_ERR, "��ȡ�û���Դ�쳣!");
	}
	

	 return iRowCount;
}

int UniTradeDao::getUserInfoBySim(DataBuf &userResInfo, const string &strResCode,const string &strResTypeCode) 
{
	StaticDAO& dao = StaticDAO::getDAO();
	

	int iRowCount = 0;
	
	try
	{
	    dao.Clear();
	    dao.SetParam(":VRES_CODE", strResCode);
	    dao.SetParam(":VRES_TYPE_CODE",strResTypeCode);
	    iRowCount = dao.jselect(userResInfo, "SEL_BY_RESCODE","TF_F_USER_RES");
	}
	catch(Exception &e)
	{
	    THROW_AGC(CRMException, e, Grade::WARNING, CALLCENTERENCODESTR_ERR, "��ȡ�û���Դ�쳣!");
	}
	

	 return iRowCount;
}

int UniTradeDao::getUserInfoById(DataBuf &userInfo, const string &userId)
{
	StaticDAO& dao = StaticDAO::getDAO();
		

	int iRowCount = 0;
	try
    {
        dao.Clear();
		dao.SetParam(":VUSER_ID", userId);
        iRowCount = dao.jselect(userInfo, "SEL_BY_PK", "TF_F_USER");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡ�û�������Ϣ�쳣��");
    }

	return iRowCount;
}

        
int UniTradeDao::getDestroyUserInfoBySn(DataBuf &userInfo, const string &serialNumber, const string &netTypeCode,const string mode)
{
	StaticDAO& dao = StaticDAO::getDAO();
		
	int iRowCount = 0;
	
	if (mode=="0")
	{
		try
		{
		    dao.Clear();
			dao.SetParam(":VSERIAL_NUMBER", serialNumber);
			dao.SetParam(":VNET_TYPE_CODE", netTypeCode);
		    iRowCount = dao.jselect(userInfo, "SEL_BY_SN_DESTROY_3", "TF_F_USER");
		}
		catch(Exception &ex)
		{
		    THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡ�û�������Ϣ�쳣��");
		}
	}
	else if(mode=="1")
	{
		try
		{
		    dao.Clear();
			dao.SetParam(":VSERIAL_NUMBER", serialNumber);
			dao.SetParam(":VNET_TYPE_CODE", netTypeCode);
			dao.SetParam(":VREMOVE_TAG", "1");
		    iRowCount = dao.jselect(userInfo, "SEL_BY_SN", "TF_F_USER");
		}
		catch(Exception &ex)
		{
		    THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡ�û�������Ϣ�쳣��");
		}
	}
	else if(mode=="2")
	{
		try
		{
		    dao.Clear();
			dao.SetParam(":VSERIAL_NUMBER", serialNumber);
			dao.SetParam(":VNET_TYPE_CODE", netTypeCode);
		    iRowCount = dao.jselect(userInfo, "SEL_BY_SN_MAXOPENDATE", "TF_F_USER");
		}
		catch(Exception &ex)
		{
		    THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡ�û�������Ϣ�쳣��");
		}
	}
    
	return iRowCount;
}


int UniTradeDao::getUserInfoBySn(DataBuf &userInfo, const string &serialNumber, const string &netTypeCode,const string strRemoveTag)
{
	StaticDAO& dao = StaticDAO::getDAO();
		

	int iRowCount = 0;
	
	if (netTypeCode!="")
	{
		try
		{
		    dao.Clear();
			dao.SetParam(":VSERIAL_NUMBER", serialNumber);
			dao.SetParam(":VNET_TYPE_CODE", netTypeCode);
			dao.SetParam(":VREMOVE_TAG",strRemoveTag);
		    iRowCount = dao.jselect(userInfo, "SEL_BY_SN", "TF_F_USER");
		}
		catch(Exception &ex)
		{
		    THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡ�û�������Ϣ�쳣��");
		}
	}
	else
	{
		try
		{
		    dao.Clear();
			dao.SetParam(":VSERIAL_NUMBER", serialNumber);
			dao.SetParam(":VREMOVE_TAG",strRemoveTag);
		    iRowCount = dao.jselect(userInfo, "SEL_BY_SERIALNUMBER", "TF_F_USER");
		}
		catch(Exception &ex)
		{
		    THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡ�û�������Ϣ�쳣��");
		}
	}

	return iRowCount;
}

int UniTradeDao::getTradeTypeInfo(DataBuf &tradeTypeInfo, int TradeType,const string &strEparchyCode)
{
	StaticDAO& dao = StaticDAO::getDAO();
		

	int iRowCount = 0;
	
	try
    {
        dao.Clear();
		dao.SetParam(":VTRADE_TYPE_CODE", TradeType);
		dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
        iRowCount = dao.jselect(tradeTypeInfo, "SEL_BY_PK", "TD_S_TRADETYPE");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡҵ�����Ͳ�������Ϣ�쳣��");
    }

	return iRowCount;
}

int UniTradeDao::getCustInfoById(DataBuf &custInfo, const string &custId)
{
	StaticDAO& dao = StaticDAO::getDAO();
		

	int iRowCount = 0;
	
	try
    {
        dao.Clear();
		dao.SetParam(":VCUST_ID", custId);
        iRowCount = dao.jselect(custInfo, "SEL_BY_PK", "TF_F_CUSTOMER");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡ�ͻ�������Ϣ�쳣��");
    }
    

	return iRowCount;
}
     
string UniTradeDao::getSequenceId(const string &sequenceName, const string &eparchyCode)
{
	StaticDAO& dao = StaticDAO::getDAO();
	if(sequenceName.length() == 0)
    {
        THROW_C(CRMException, GENERATEID_ERR, "������ˮ��:û��������ˮ�����ͣ�");;
    }
   

	string sequenceId="";
	
	DualDAO daoDual;
	
	try
    {
        daoDual.Clear();
		daoDual.SetParam(":VEPARCHY_CODE", eparchyCode);
        daoDual.SetParam(":VSEQUENCENAME", sequenceName);
        daoDual.jselect(sequenceId, "SEQUENCE");
      
    }
    catch(Exception &ex)
    {
    	THROW_AGC(CRMException, ex, Grade::WARNING, GENERATEID_ERR, "������ˮ�ų���");
    }

	return sequenceId;
}

int UniTradeDao::getAcctIdByPayRelation(DataBuf &acctInfo, const string &strUserId)
{
	StaticDAO& dao = StaticDAO::getDAO();
		

	int iRowCount = 0;
	
	try
    {
        dao.Clear();
		dao.SetParam(":VUSER_ID", strUserId);
        iRowCount = dao.jselect(acctInfo, "SEL_BY_USER", "TF_A_PAYRELATION");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡ���ѹ�ϵ����Ϣ�쳣��");
    }
    
    if (iRowCount==0)
    {
    	//������ȡ��������˻�ID
    	try
		{
		    dao.Clear();
			dao.SetParam(":VUSER_ID", strUserId);
			dao.SetParam(":VDEFAULT_TAG", "1");
		    iRowCount = dao.jselect(acctInfo, "SEL_BY_USER_MAX", "TF_A_PAYRELATION");
		}
		catch(Exception &ex)
		{
		    THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡ���ѹ�ϵ����Ϣ�쳣��");
		}
    }
    

	return iRowCount;
}


int UniTradeDao::getProductInfo(DataBuf &prodInfo, const int &productId)
{
	StaticDAO& dao = StaticDAO::getDAO();
		

	int iRowCount = 0;
	
	try
    {
        dao.Clear();
		dao.SetParam(":VPRODUCT_ID", productId);
        iRowCount = dao.jselect(prodInfo, "SEL_BY_PK", "TD_B_PRODUCT");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡ��Ʒ��������Ϣ�쳣��");
    }
    

	return iRowCount;
}

int UniTradeDao::getUserMainSvcById(DataBuf &usermainsvc, const string &userid)
{
	StaticDAO& dao = StaticDAO::getDAO();

	int iRowCount = 0;
	
	try
    {
        dao.Clear();
		dao.SetParam(":VUSER_ID", userid);
        iRowCount = dao.jselect(usermainsvc, "SEL_USER_MAINSVC", "TF_F_USER_SVC");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡ�û�������������");
    }

	return iRowCount;
}

int UniTradeDao::getUserSvcBySvcId(DataBuf &usersvc, const string &userid,const string &serviceid)
{
	StaticDAO& dao = StaticDAO::getDAO();

	int iRowCount = 0;
	
	try
    {
        dao.Clear();
		dao.SetParam(":VUSER_ID", userid);
		dao.SetParam(":VSERVICE_ID", serviceid);
        iRowCount = dao.jselect(usersvc, "SEL_BY_SERVICE_ID", "TF_F_USER_SVC");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡ�û��ķ������");
    }

	return iRowCount;
}

int UniTradeDao::getElementInfoByActionCode(DataBuf &elementinfo, const string &actioncode)
{
	StaticDAO& dao = StaticDAO::getDAO();

	int iRowCount = 0;
	
	try
    {
        dao.Clear();
		dao.SetParam(":VACTION_CODE", actioncode);
        iRowCount = dao.jselect(elementinfo, "SEL_FOR_SCOREEXCHANGE", "TD_B_PRODUCT");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡ������Ϣ����");
    }

	return iRowCount;
}

int UniTradeDao::getGiftExchangeInfoByActionCode(DataBuf &exchangeinfo, const string &actioncode)
{
	StaticDAO& dao = StaticDAO::getDAO();

	int iRowCount = 0;
	
	try
    {
        dao.Clear();
		dao.SetParam(":VACTION_CODE", actioncode);
        iRowCount = dao.jselect(exchangeinfo, "SEL_BY_ACTIONCODE_NEW", "TD_B_GIFT_EXCHANGE");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡ������Ϣ����");
    }

	return iRowCount;
}
int UniTradeDao::GetPartyProductInfoByActionCode(DataBuf &partyproductinfo, const string &actioncode)
{
	StaticDAO& dao = StaticDAO::getDAO();

	int iRowCount = 0;
	
	try
    {
        dao.Clear();
		dao.SetParam(":VSP_SERVICE_ID", actioncode);
        iRowCount = dao.jselect(partyproductinfo, "SEL_BY_SERVICE_ID", "TD_B_PARTY_PRODUCT");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡ������Ϣ����");
    }

	return iRowCount;
}

int UniTradeDao::ExistUserElementInfo(const string &strUserId,const string &strElementTypeCode,const int &iElementId)
{
    StaticDAO& dao = StaticDAO::getDAO();
    int icount=0;
    DataBuf outBuf;
    if(strElementTypeCode=="D")  //��ѯ�Ż���Ϣ
    {
        try
        {
            dao.SetParam(":VUSER_ID",strUserId);
            dao.SetParam(":VDISCNT_CODE",iElementId);
            icount = dao.jselect(outBuf, "SEL_USER_BY_PK","TF_F_USER_DISCNT");
        }
        catch(Exception &e)
        {
            THROW_C(CRMException, 8888,"У���û��Ƿ��Ѿ�����Ԫ��ʧ��"+e.getMessage()); 
        }
    }
    else if(strElementTypeCode=="S")
    {
        try
        {
            dao.SetParam(":VUSER_ID",strUserId);
            dao.SetParam(":VSERVICE_ID",iElementId);
            icount = dao.jselect(outBuf, "SEL_BY_SERVICE_ID","TF_F_USER_SVC");
        }
        catch(Exception &e)
        {
            THROW_C(CRMException, 8888,"У���û��Ƿ��Ѿ�����Ԫ��ʧ��"+e.getMessage()); 
        }
    }
    else if(strElementTypeCode=="X")
    {
        try
        {
            dao.SetParam(":VUSER_ID",strUserId);
            dao.SetParam(":VSP_SERVICE_ID",iElementId);
            icount = dao.jselect(outBuf, "SEL_BY_SP_SERVICE_ID","TF_F_USER_SP");
        }
        catch(Exception &e)
        {
            THROW_C(CRMException, 8888,"У���û��Ƿ��Ѿ�����Ԫ��ʧ��"+e.getMessage()); 
        }
    }
    return   icount;
}

int UniTradeDao::getCustomerById(DataBuf &customer, const string &custId)
{
	StaticDAO& dao = StaticDAO::getDAO();

	int iRowCount = 0;
	
	try
    {
        dao.Clear();
		dao.SetParam(":VCUST_ID", custId);
        iRowCount = dao.jselect(customer, "SEL_BY_PK", "TF_F_CUSTOMER");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡTF_F_CUSTOMER����");
    }

	return iRowCount;
}

int UniTradeDao::getPersonById(DataBuf &person, const string &custId)
{
	StaticDAO& dao = StaticDAO::getDAO();

	int iRowCount = 0;
	
	try
    {
        dao.Clear();
		dao.SetParam(":VCUST_ID", custId);
        iRowCount = dao.jselect(person, "SEL_BY_PK", "TF_F_CUST_PERSON");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡTF_F_CUSTOMER����");
    }

	return iRowCount;    
}

int UniTradeDao::getUserMainSvcStateById(DataBuf &usermainsvcSate, const string &userid)
{
	StaticDAO& dao = StaticDAO::getDAO();

	int iRowCount = 0;
	
	try
    {
        dao.Clear();
		dao.SetParam(":VUSER_ID", userid);
        iRowCount = dao.jselect(usermainsvcSate, "SEL_USER_MAINSATE", "TF_F_USER_SVCSTATE");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡTF_F_USER_SVCSTATE����");
    }

	return iRowCount;    
}

int UniTradeDao::getNewSvcStateByTradeTypeCode(DataBuf &tradesvcstate,const int &tradetypecode,const string &nettypecode,const string &brandcode,const string &productid,const string &eparchycode)
{
	StaticDAO& dao = StaticDAO::getDAO();

	int iRowCount = 0;
	
	try
    {
        dao.Clear();
        dao.SetParam(":VTRADE_TYPE_CODE", tradetypecode);
        dao.SetParam(":VNET_TYPE_CODE", nettypecode);
        dao.SetParam(":VBRAND_CODE", brandcode);
        dao.SetParam(":VPRODUCT_ID", productid);
		dao.SetParam(":VEPARCHY_CODE", eparchycode);
        iRowCount = dao.jselect(tradesvcstate, "SEL_BY_PK", "TD_S_TRADE_SVCSTATE");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡTF_F_USER_SVCSTATE����");
    }

	return iRowCount;    
}

int UniTradeDao::getUserInfoBySn(DataBuf &userInfo, const string &serialNumber)
{
	StaticDAO& dao = StaticDAO::getDAO();

	int iRowCount = 0;
	
	try
    {
        dao.Clear();
		dao.SetParam(":VSERIAL_NUMBER", serialNumber);
		dao.SetParam(":VREMOVE_TAG", "0");
        iRowCount = dao.jselect(userInfo, "SEL_BY_SERIALNUMBER", "TF_F_USER");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡTF_F_USER_SVCSTATE����");
    }

	return iRowCount;    
}

int UniTradeDao::getPostInfo(DataBuf &postInfo, const string &id,const string &idType)
{
	StaticDAO& dao = StaticDAO::getDAO();
		

	int iRowCount = 0;
	
	try
    {
        dao.Clear();
		dao.SetParam(":VID", id);
		dao.SetParam(":VID_TYPE", idType);
        iRowCount = dao.jselect(postInfo, "SEL_BY_PK", "TF_F_POSTINFO");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡ�ʼ���Ϣ���쳣��");
    }

	return iRowCount;
}

int UniTradeDao::getAllRelationUUByUserId(DataBuf &relationUU, const string &userId, const string &relationTypeCode)
{
	StaticDAO& dao = StaticDAO::getDAO();

	int iRowCount = 0;
	
	try
    {
        dao.Clear();
		dao.SetParam(":VUSER_ID_B", userId);
		dao.SetParam(":VRELATION_TYPE_CODE", relationTypeCode);
        iRowCount = dao.jselect(relationUU, "SEL_USER_UUROL", "TF_F_RELATION_UU");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡTF_F_RELATION_UU����");
    }

	return iRowCount;    
}

int UniTradeDao::getOldOrderInfo(DataBuf &oldOrderInfo, const string &strTradeId)
{
	StaticDAO& dao = StaticDAO::getDAO();
		

	int iRowCount = 0;
	
	try
    {
        dao.Clear();
		dao.SetParam(":VTRADE_ID", strTradeId);
        iRowCount = dao.jselect(oldOrderInfo, "SEL_BY_TRADE_ID", "TF_BH_TRADE");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡԭ������Ϣ�쳣��");
    }
    

	return iRowCount;
}
int UniTradeDao::getOrderInfo(DataBuf &orderInfo, const string &strSubscribeId)
{
	StaticDAO& dao = StaticDAO::getDAO();
		

	int iRowCount = 0;
	
	try
    {
        dao.Clear();
		dao.SetParam(":VSUBSCRIBE_ID", strSubscribeId);
        iRowCount = dao.jselect(orderInfo, "SEL_BY_SUBSCRIBEID", "TF_B_TRADE");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡԤ������Ϣ�쳣��");
    }
    

	return iRowCount;
}
/**
	*����SCRID ��ѯ	���Ԥ����
*/
int UniTradeDao::getOrderInfoComp(DataBuf &orderInfo, const string &strSubscribeId)
{
	StaticDAO& dao = StaticDAO::getDAO();
	int iRowCount = 0;
	
	try
    {
        dao.Clear();
		dao.SetParam(":VSUBSCRIBE_ID", strSubscribeId);
        iRowCount = dao.jselect(orderInfo, "SEL_TRADE_COMP_SCRIB", "TF_B_TRADE");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡ���Ԥ������Ϣ�쳣��");
    }
	return iRowCount;
}


/**
	*����SCRID ��ѯ	���Ԥ���� --���ж������ύ֮��ſ��Բ�ѯԤ������
*/
int UniTradeDao::getCompInfoAfterContinue(DataBuf &orderInfo, const string &strSubscribeId)
{
	StaticDAO& dao = StaticDAO::getDAO();
	int iRowCount = 0;
	
	try
    {
        dao.Clear();
		dao.SetParam(":VSUBSCRIBE_ID", strSubscribeId);
        iRowCount = dao.jselect(orderInfo, "SEL_TRADE_INFO_BY_ALL_CONTINUE", "TF_B_TRADE");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡ���Ԥ������Ϣ�쳣��");
    }
	return iRowCount;
}

int UniTradeDao::getOrderInfo1(DataBuf &orderInfo, const string &strTradeId)
{
	StaticDAO& dao = StaticDAO::getDAO();
		

	int iRowCount = 0;
	
	try
    {
        dao.Clear();
		dao.SetParam(":VTRADE_ID", strTradeId);
        iRowCount = dao.jselect(orderInfo, "SEL_BY_TRADE_ID", "TF_B_TRADE");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡԤ������Ϣ�쳣��");
    }
    

	return iRowCount;
}

int UniTradeDao::getHisOrderInfo(DataBuf &orderInfo, const string &strSubscribeId)
{
	StaticDAO& dao = StaticDAO::getDAO();
		

	int iRowCount = 0;
	
	try
    {
        dao.Clear();
		dao.SetParam(":VSUBSCRIBE_ID", strSubscribeId);
		dao.SetParam(":VCANCEL_TAG", "0");
        iRowCount = dao.jselect(orderInfo, "SEL_BY_SUBSCRIBEID", "TF_BH_TRADE");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡԤ������Ϣ�쳣��");
    }
    

	return iRowCount;
}

int UniTradeDao::getIdInfo(DataBuf &idInfo, const string &strPsptId,const string &strPsptTypeCode)
{
	StaticDAO& dao = StaticDAO::getDAO();
		

	int iRowCount = 0;
	DataBuf tempBuf;
	try
    {
        dao.Clear();
		dao.SetParam(":VPSPT_ID", strPsptId);
		dao.SetParam(":VPSPT_TYPE_CODE", strPsptTypeCode);
        iRowCount = dao.jselect(tempBuf, "SEL_PSPTINFO", "TF_F_USER_OTHER");
        if (iRowCount>0)
        {
            for (int i=0;i<iRowCount;i++)
            {
                idInfo.SetString("PARA_ID",tempBuf.GetString("PARA_CODE",i),i);
                idInfo.SetString("PARA_VALUE",tempBuf.GetString("PARA_VALUE",i),i);
            }
        }
        
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡ�������֤��Ϣ�쳣��");
    }
    

	return iRowCount;
}

void UniTradeDao::InsertInvoice(const string &tabName,const int mode)
{
	UniDataMgr &uniDataMgr = UniDataMgr::getDataMgrImpl();
	StaticDAO& dao = StaticDAO::getDAO();
	DataBuf paraBuf;
	paraBuf.Cover(uniDataMgr.getExtBuf(tabName));
	//try
    {
        for (int i=0;i<paraBuf.size();i++)
        {
            dao.Clear();  
            
            dao.SetParam(":VTRADE_ID",paraBuf.GetString("TRADE_ID",i));
            dao.SetParam(":VTICKET_ID",paraBuf.GetString("TICKET_ID",i));
            dao.SetParam(":VTAX_NO",paraBuf.GetString("TAX_NO",i));
            dao.SetParam(":VTICKET_TYPE_CODE",paraBuf.GetString("TICKET_TYPE_CODE",i));
            dao.SetParam(":VTICKET_STATE_CODE",paraBuf.GetString("TICKET_STATE_CODE",i));
            dao.jinsert("INS_TRADEINFO", tabName);        
        }
    }
    //catch(Exception &ex)
    {
  //      THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "����Ʊ��̨�˳���");
    }

}

void UniTradeDao::InsertPayMoney(const string &tabName,const int mode)
{
	UniDataMgr &uniDataMgr = UniDataMgr::getDataMgrImpl();
	StaticDAO& dao = StaticDAO::getDAO();
	DataBuf paraBuf;
	paraBuf.Cover(uniDataMgr.getExtBuf(tabName));
//	try
    {
        for (int i=0;i<paraBuf.size();i++)
        {
            dao.Clear();  
            
            dao.SetParam(":VTRADE_ID",paraBuf.GetString("TRADE_ID",i));
            dao.SetParam(":VACCEPT_MONTH",atoi(paraBuf.GetString("TRADE_ID",i).substr(4,2).c_str()));
            dao.SetParam(":VPAY_MONEY_CODE",paraBuf.GetString("PAY_MONEY_CODE",i));
            dao.SetParam(":VMONEY",paraBuf.GetString("MONEY",i));
            dao.SetParam(":VPAY_ID",paraBuf.GetString("PAY_ID",i));
            dao.SetParam(":VPAY_DATE",paraBuf.GetString("PAY_DATE",i));
            dao.SetParam(":VFEE_STAFF_ID",paraBuf.GetString("FEE_STAFF_ID",i));
            dao.SetParam(":VFEE_DEPART_ID",paraBuf.GetString("FEE_DEPART_ID",i));
            dao.jinsert("INS_TRADEINFO", tabName);        
        }
    }
   // catch(Exception &ex)
    {
 //       THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "����Ʊ��̨�˳���");
    }

}


void UniTradeDao::InsertTradefeeDefer(const string &tabName,const int mode)
{
	UniDataMgr &uniDataMgr = UniDataMgr::getDataMgrImpl();
	StaticDAO& dao = StaticDAO::getDAO();
	DataBuf paraBuf;
	paraBuf.Cover(uniDataMgr.getExtBuf(tabName));
//	try
    {
        for (int i=0;i<paraBuf.size();i++)
        {
            dao.Clear();  
            
            dao.SetParam(":VTRADE_ID",paraBuf.GetString("TRADE_ID",i));
            dao.SetParam(":VUSER_ID",paraBuf.GetString("USER_ID",i));
            dao.SetParam(":VFEE_MODE",paraBuf.GetString("FEE_MODE",i));
            dao.SetParam(":VFEE_TYPE_CODE",paraBuf.GetInt("FEE_TYPE_CODE",i));
            dao.SetParam(":VMONEY",paraBuf.GetString("MONEY",i));
            dao.SetParam(":VDEFER_ITEM_CODE",paraBuf.GetInt("DEFER_ITEM_CODE",i));
            dao.SetParam(":VCANCEL_TAG",paraBuf.GetString("CANCEL_TAG",i));
            dao.SetParam(":VPAY_ID",paraBuf.GetString("PAY_ID",i));
            dao.jinsert("INS_TRADEINFO1", tabName);        
        }
    }
//    catch(Exception &ex)
    {
//        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "����Ʊ��̨�˳���");
    }

}

void UniTradeDao::InsertCheck(const string &tabName,const int mode)
{
	UniDataMgr &uniDataMgr = UniDataMgr::getDataMgrImpl();
	StaticDAO& dao = StaticDAO::getDAO();
	DataBuf paraBuf;
	paraBuf.Cover(uniDataMgr.getExtBuf(tabName));
//	try
    {
        for (int i=0;i<paraBuf.size();i++)
        {
            dao.Clear();  
            
            dao.SetParam(":VTRADE_ID",paraBuf.GetString("TRADE_ID",i));
            dao.SetParam(":VCHECK_CARD_NO",paraBuf.GetString("CHECK_CARD_NO",i));
            dao.SetParam(":VCHECK_CARD_NAME",paraBuf.GetString("CHECK_CARD_NAME",i));
            dao.SetParam(":VCHECK_BANK_CODE",paraBuf.GetString("CHECK_BANK_CODE",i));
            dao.SetParam(":VCHECK_MONEY",paraBuf.GetString("CHECK_MONEY",i));
            
            dao.SetParam(":VCHECK_MONEY",paraBuf.GetString("CHECK_MONEY",i));
            dao.SetParam(":VCHECK_LIMIT",paraBuf.GetString("CHECK_LIMIT",i));
            dao.SetParam(":VCHECK_TAG",paraBuf.GetString("CHECK_TAG",i));
            dao.SetParam(":VRECEIVE_DATE",paraBuf.GetString("RECEIVE_DATE",i));
            dao.SetParam(":VCUST_NAME",paraBuf.GetString("CUST_NAME",i));
            dao.SetParam(":VPSPT_TYPE_CODE",paraBuf.GetString("PSPT_TYPE_CODE",i));
            
            dao.SetParam(":VPSPT_ID",paraBuf.GetString("PSPT_ID",i));
            dao.SetParam(":VPSPT_ADDR",paraBuf.GetString("PSPT_ADDR",i));
            dao.SetParam(":VPHONE",paraBuf.GetString("PHONE",i));
            dao.SetParam(":VREMARK",paraBuf.GetString("REMARK",i));
            dao.SetParam(":VPAY_ID",paraBuf.GetString("PAY_ID",i));
            dao.jinsert("INS_TRADEINFO", tabName);        
        }
    }
//    catch(Exception &ex)
    {
//        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "����Ʊ��̨�˳���");
    }

}

int UniTradeDao::getUserForegift(DataBuf &userForegiftBuf, const string &strUserId,const string &strForegiftCode)
{
	StaticDAO& dao = StaticDAO::getDAO();
		

	int iRowCount = 0;
	
	try
    {
        dao.Clear();
		dao.SetParam(":VUSER_ID", strUserId);
		dao.SetParam(":VFOREGIFT_CODE", strForegiftCode);
        iRowCount = dao.jselect(userForegiftBuf, "SEL_BY_USER_ID_0", "TF_F_USER_FOREGIFT");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡѺ����Ϣ��Ϣ�쳣��");
    }
    

	return iRowCount;
}

int UniTradeDao::getUserPurchaseForegift(DataBuf &userForegiftBuf, const string &strUserId,const string &eparchyCode,const string &strForegiftCode)
{
    LOG_TRACE(logUniTradeMgr, "����getUserPurchaseForegift����");
	StaticDAO& dao = StaticDAO::getDAO();
	int iRowCount = 0;
	
	try
    {
        dao.Clear();
		dao.SetParam(":VUSER_ID", strUserId);
		dao.SetParam(":VEPARCHY_CODE", eparchyCode);
		dao.SetParam(":VFEE_TYPE_CODE", strForegiftCode);
        iRowCount = dao.jselect(userForegiftBuf, "SEL_INTERFOGFT1", "TD_S_COMMPARA");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ѯ�û���������Ѻ����Ϣ�쳣��");
    }
    
    LOG_TRACE(logUniTradeMgr, "�˳�getUserPurchaseForegift����");
	return iRowCount;
}

string UniTradeDao::getUserLimitForegift( const string &strUserId,const string &strForegiftCode)
{
	StaticDAO& dao = StaticDAO::getDAO();
    string foregift="0";
	DataBuf temp;

	try
    {
        dao.Clear();
		dao.SetParam(":VUSER_ID", strUserId);
		dao.SetParam(":VFOREGIFT_CODE", strForegiftCode);
        dao.jselect(temp, "SUM_FOREGIFT_BY_ID", "TF_F_USER_PURCHASE");   
        foregift=temp.GetString("FOREGIFT");         
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡ�������˵Ĺ���Ѻ����Ϣ�쳣��");
    }

	return foregift;
}
/*
int UniTradeDao::chkSvcForegift(const string &strUserId,const string &strForegiftCode,const string &eparchyCode)
{
	StaticDAO& dao = StaticDAO::getDAO();
	DataBuf temp;	

	int iRowCount = 0;
	
	try
    {
        dao.Clear();
		dao.SetParam(":VUSER_ID", strUserId);
		dao.SetParam(":VFOREGIFT_CODE", strForegiftCode);
		dao.SetParam(":VEPARCHY_CODE", eparchyCode);
        dao.jselect(temp, "CHK_FOREGIFT_BY_SVC", "TF_F_USER_PURCHASE");
        iRowCount=temp.GetLong("NUM");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡѺ����Ϣ��Ϣ�쳣��");
    }
    

	return iRowCount;
}
*/
string UniTradeDao::chkSvcForegift(const string &strUserId,const string &strForegiftCode,const string &eparchyCode)
{
	StaticDAO& dao = StaticDAO::getDAO();
	DataBuf temp;	

	int iRowCount = 0;
	string info="";
	
	try
    {
        dao.Clear();
		dao.SetParam(":VUSER_ID", strUserId);
		dao.SetParam(":VFOREGIFT_CODE", strForegiftCode);
		dao.SetParam(":VEPARCHY_CODE", eparchyCode);
        iRowCount=dao.jselect(temp, "CHK_FOREGIFT_BY_SVC", "TF_F_USER_SVC");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡѺ����Ϣ��Ϣ�쳣��");
    }
    if (iRowCount>0)
    info=temp.GetString("REMARK");

	return info;
}

string UniTradeDao::checkPurchaseForegift(const string &strUserId,const string &strForegiftCode,const string &strMoney,const string &strName,const long &fee)
{  
    LOG_TRACE(logUniTradeMgr, "����checkPurchaseForegift����");    
	StaticDAO& dao = StaticDAO::getDAO();
	DataBuf temp;	

	int iRowCount = 0;
	string info="",timestr="";
	long canRefundFee=0;
	try
    {
        dao.Clear();
		dao.SetParam(":VPARA_CODE1", strUserId);
		dao.SetParam(":VPARA_CODE2", strForegiftCode);
		dao.SetParam(":VPARA_CODE3", strMoney);
        iRowCount=dao.jselect(temp, "SEL_REFNDFOGFT1", "TD_S_COMMPARA");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "���������ι���Ѻ���Ƿ�����˴����쳣��");
    }
    if (iRowCount>0){ 
        LOG_TRACE_P1(logUniTradeMgr, "fee=%d",fee);
        long fe=fee/100;
         canRefundFee=atol(temp.GetString("PARA_CODE1").c_str()); 
         LOG_TRACE_P1(logUniTradeMgr, "canRefundFee=%d",canRefundFee);
         timestr=temp.GetString("END_DATE");
         if (canRefundFee <= 0) {
            info=strName+"���ڲ��������ˣ��뵽����ʱ��["+timestr +"]�������������ˣ�";
        }
        if (canRefundFee - fe< 0){
            info=strName+"���ڿ������˽��Ϊ��"+temp.GetString("PARA_CODE1")+" Ԫ��";
        }
    }
    LOG_TRACE(logUniTradeMgr, "�˳�checkPurchaseForegift����");  
	return info;
}

bool UniTradeDao::GetStaffRight(const string &staffId, const string &rightCode, const string &rightType)
{
    StaticDAO& dao = StaticDAO::getDAO();
    CFmlBuf tempBuf;
    int count(0);
    try
    {
        dao.Clear();
        dao.SetParam(":VSTAFF_ID", staffId);
        dao.SetParam(":VDATA_CODE", rightCode);
        dao.SetParam(":VDATA_TYPE", rightType);
        count = dao.jselect(tempBuf, "SEL_BY_STAFFID_DATACODE","TF_M_STAFFDATARIGHT");
    }
    catch(Exception &e)
    {
        THROW_AGC(CRMException, e, Grade::WARNING, GENETRADE_ERR, "��ȡӪҵ������Ϣ:��ȡԱ��Ȩ�޴���");
    }
    return count>0?true:false;
}


int UniTradeDao::CheckFeeBeforeSubmit(const string strTradeTypecode,const string &strNetTypeCode,const string &strFeeMode,const string strFeeitemCode,const string &strCityCode,const string &strEparchyCode)
{
	StaticDAO& dao = StaticDAO::getDAO();
		

	int iRowCount = 0;
	DataBuf temp;
	try
    {
        dao.Clear();
        dao.SetParam(":VTRADE_TYPE_CODE", strTradeTypecode);
        dao.SetParam(":VNET_TYPE_CODE", strNetTypeCode);
        dao.SetParam(":VFEE_MODE", strFeeMode);
        dao.SetParam(":VFEEITEM_CODE", strFeeitemCode);
        dao.SetParam(":VAREA_CODE", strCityCode);
        dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
        iRowCount = dao.jselect(temp,"SEL_CHECK_FEE_PARA","TD_S_COMMPARA");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "У��δ�շ���Ŀ��ѯ����");
    }
    

	return iRowCount;
}

//QC:31132 Begin ���������Ż���վ��������ʡ���ʱ������У��֧��(INFO_TAG:��1λ����ȡ�û�������Ϣ(����ʺţ�7,�̶��绰��8))
/**
 * �����û���ĳ�����Ի�ȡ��ǰ�û�������������Ϣ
 */
int UniTradeDao::getADSLUserInfoByAttr(DataBuf &userItemInfo, const string &attrCode,const string &attrValue)
{
	StaticDAO& dao = StaticDAO::getDAO();

	int iRowCount = 0;
	
	try
    {
        dao.Clear();
		dao.SetParam(":VATTR_CODE", attrCode);
		dao.SetParam(":VATTR_VALUE", attrValue);
        iRowCount = dao.jselect(userItemInfo, "SEL_ALLUSERITEM_BY_ATTR", "TF_F_USER_ITEM");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡTF_F_USER_ITEM��Ϣ����");
    }

	return iRowCount;    
}
int UniTradeDao::getADSLUserInfoByPTDH(DataBuf &relInfo, const string &userId)
{
	StaticDAO& dao = StaticDAO::getDAO();

	int iRowCount = 0;
	
	try
    {
        dao.Clear();
		dao.SetParam(":VUSER_ID_B", userId);
        iRowCount = dao.jselect(relInfo, "SEL_X3USER_BY_USERIDB", "TF_F_RELATION_UU");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡTF_F_RELATION_UU����");
    }

	return iRowCount;    
}
/**
 * ������ķ����������(���ú���F_CSB_ENCRYPT���м���)
 *
 */
string UniTradeDao::geneEncryptPassword(const string& userId,const string& inPassword)
{
	StaticDAO& dao = StaticDAO::getDAO();
	string tempStr = inPassword;
	int iRowCount = 0;
	DataBuf newPasswdBuf;
	try
    {
        dao.Clear();
		dao.SetParam(":VUSER_PASSWD", inPassword);
		dao.SetParam(":VUSER_ID", userId);
        iRowCount = dao.jselect(newPasswdBuf, "F_CSB_ENCRYPT", "TF_F_USER_OTHER");
    }
    catch(Exception &ex)
    {
        THROW_AGC(CRMException, ex, Grade::WARNING, GENETRADE_ERR, "��ȡTF_F_USER_SVCSTATE����");
    }
    if(iRowCount > 0){
    	tempStr = newPasswdBuf.GetString("USER_PASSWD");
    }
    return tempStr;
}
//QC:31132 End