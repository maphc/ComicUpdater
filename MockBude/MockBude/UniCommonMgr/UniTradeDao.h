#ifndef UNI_TRADE_DAO_H_20110218
#define UNI_TRADE_DAO_H_20110218


#include <iostream>
#include <string>
#include "../DataBuf.h"
#include "../base/config-all.h"

using namespace std;

class UniTradeDao {

public:

	static UniTradeDao & getInstance();

	int getUserInfoById(DataBuf &userInfo, const string &userId) ;

	int getDestroyUserInfoBySn(DataBuf &userInfo, const string &serialNumber, const string &netTypeCode,const string mode);

	int getUserInfoBySn(DataBuf &userInfo, const string &serialNumber, const string &netTypeCode,const string strRemoveTag="0") ;

	int getAcctIdByPayRelation(DataBuf &acctInfo, const string &strUserId);

	int getTradeTypeInfo(DataBuf &tradeTypeInfo, int TradeType,const string &strEparchyCode);

	int getCustInfoById(DataBuf &custInfo, const string &custId) ;

	int getProductInfo(DataBuf &prodInfo, const int &productId) ;

	bool GetStaffRight(const string &staffId, const string &rightCode, const string &rightType);

	int getUserInfoBySim(DataBuf &userResInfo, const string &strResCode,const string &strResTypeCode);

	int getPostInfo(DataBuf &postInfo, const string &id,const string &idType) ;

	string getSequenceId(const string &sequenceName, const string &eparchyCode="") ;

	int getDefaultPayRelationByUId(DataBuf &payRelation, const string &userId) const{return 0;};

	int getUserResByResType(DataBuf &userResInfo, const string &userId,const string &strResTypeCode) ;

	int getSimIdleByCardType(DataBuf &simIdle, const string &simcardType,const string &eparchyCode) ;

	int getUserOrderInfo(DataBuf &orderInfo, const string &orgOrderId,const string &operType) const{return 0;};

	int getSysTagInfo(DataBuf &tagInfo,const string &sysCode, const string &tagCode,const string &strEparchyCode) ;

	int getCustomerById(DataBuf &customer, const string &custId);

	int getPersonById(DataBuf &person, const string &custId);

	int getElementInfoByActionCode(DataBuf &elementinfo, const string &actioncode);

	int GetPartyProductInfoByActionCode(DataBuf &partyproductinfo, const string &actioncode);

	int getGiftExchangeInfoByActionCode(DataBuf &exchangeinfo, const string &actioncode);
	int ExistUserElementInfo(const string &strUserId,const string &strElementTypeCode,const int &iElementId);

	int getUserMainSvcStateById(DataBuf &usermainsvcSate, const string &userid); 

	int getUserMainSvcById(DataBuf &usermainsvc, const string &userid);    

	int getUserSvcBySvcId(DataBuf &usersvc, const string &userid,const string &serviceid);     

	int getNewSvcStateByTradeTypeCode(DataBuf &tradesvcstate,const int &tradetypecode,const string &nettypecode,const string &brandcode,const string &productid,const string &eparchycode);

	int getUserInfoBySn(DataBuf &userInfo, const string &serialNumber);  

	//string getParam(const string &codename,const string &code) const{ return "";};

	int queryMphoneIdle(DataBuf &PhoneInfo,const string &serialNumber,const string &netTypeCode,const string &strEparchyCode) ;

	int queryCommparaInfo(DataBuf &result,DataBuf &inparam);

	int getStaffRight(DataBuf &staffRightBuf,const string &strStaffId);

	int queryFeeItemInfo(DataBuf &feeItem,const string &strEparchyCode) ;

	int getNodeParser(DataBuf &NodeParser) ;

	int getAllRelationUUByUserId(DataBuf &relationUU, const string &userId, const string &relationTypeCode);        

	int getOldOrderInfo(DataBuf &oldOrderInfo, const string &strTradeId);

	int getOrderInfo(DataBuf &orderInfo, const string &strSubscribeId);

	int getOrderInfoComp(DataBuf &orderInfo, const string &strSubscribeId);

	int getCompInfoAfterContinue(DataBuf &orderInfo, const string &strSubscribeId);        

	int getOrderInfo1(DataBuf &orderInfo, const string &strTradeId);

	int getIdInfo(DataBuf &idInfo, const string &strPsptId,const string &strPsptTypeCode);

	int getHisOrderInfo(DataBuf &orderInfo, const string &strSubscribeId);

	void InsertInvoice(const string &tabName,const int mode=0);

	void InsertPayMoney(const string &tabName,const int mode=0);

	void InsertCheck(const string &tabName,const int mode=0);

	void InsertTradefeeDefer(const string &tabName,const int mode=0);

	int getUserForegift(DataBuf &userForegiftBuf, const string &strUserId,const string &strForegiftCode);

	int getUserPurchaseForegift(DataBuf &userForegiftBuf, const string &strUserId,const string &eparchyCode,const string &strForegiftCode);

	string getUserLimitForegift( const string &strUserId,const string &strForegiftCode);  

	//int chkSvcForegift(const string &strUserId,const string &strForegiftCode,const string &eparchyCode);      

	string chkSvcForegift(const string &strUserId,const string &strForegiftCode,const string &eparchyCode);

	string checkPurchaseForegift(const string &strUserId,const string &strForegiftCode,const string &strMoney,const string &strName,const long &fee);

	int CheckFeeBeforeSubmit(const string strTradeTypecode,const string &strNetTypeCode,const string &strFeeMode,const string strFeeitemCode,const string &strCityCode,const string &strEparchyCode);

	//QC:31132 Begin
	int getADSLUserInfoByAttr(DataBuf &userInfo, const string &acctNumber,const string &acctPassword);//根据宽带账号+密码获取宽带用户信息  
	int getADSLUserInfoByPTDH(DataBuf &relInfo, const string &userId);//根据固话+服务密码获取宽带用户信息
	string geneEncryptPassword(const string& userId,const string& inPassword);//将服务密码加密
	//QC:31132 End

private :
	UniTradeDao();

	static UniTradeDao * instance;
};


#endif //UNI_TRADE_DAO_H_20110218