#ifndef TradeCheckBeforeTrade_H_HEADER_INCLUDED_BE1432B4
#define TradeCheckBeforeTrade_H_HEADER_INCLUDED_BE1432B4
//supportClone

#include "../base/CustServPubUtils.h"
//qc:05330 begin
//#include "Utility.h"
//qc:05330 end

//---------------------- sdubss ---------------------
/** @class TradeCheckBeforeTrade
 *  @brief 业务受理前条件判断
 */

class TradeCheckBeforeTrade
{
    public:
        
        
        CLASS_FUNC(CheckBeforeTrade);
        
        CLASS_FUNC(CheckSvcState);
        
        CLASS_FUNC(CheckOneCardNCodesSn);
        
        CLASS_FUNC(CheckChinagoDelay);
        
        CLASS_FUNC(CheckAfterChangeProduct);
        
        CLASS_FUNC(TradeCheck_CheckStaffDataRight);
        
        CLASS_FUNC(TradeCheck_CheckNotCompletedTrade);
        
        CLASS_FUNC(TradeCheck_SvcstateTradeLimit);
        
        CLASS_FUNC(TradeCheck_UserTradelimit);
        
        CLASS_FUNC(TradeCheck_ProdTradeLimit);
        
        CLASS_FUNC(TradeCheck_CheckEparchyCode);
        
        CLASS_FUNC(TradeCheck_CheckPreOpen);
        
        CLASS_FUNC(TradeCheck_CheckUserSP);
        
        CLASS_FUNC(TradeCheck_CheckBindSaleExpDate);
        
        CLASS_FUNC(TradeCheck_CheckTradeTypeCodeDependSVC);
        
        CLASS_FUNC(GetUserLastValidBindSaleInfo);
        
        CLASS_FUNC(CheckBindSaleAcctReturn);
        
        CLASS_FUNC(TradeCheck_CheckScoreTradeLimit);
        
        CLASS_FUNC(TradeCheck_CheckHintChgProduct);
        
        CLASS_FUNC(TradeCheck_CheckBillType);
        
        CLASS_FUNC(TradeCheck_CheckCustType);
        
        CLASS_FUNC(GetParaForAddupValue);
        
        CLASS_FUNC(CheckAcctReturn);
        
        CLASS_FUNC(CheckOtherBindSale);

		CLASS_FUNC(CheckOpenLimit);
		
		CLASS_FUNC(CheckChangeCustOwnerNum);
		
		CLASS_FUNC(TradeCheck_CheckInitPasswd);
		
		CLASS_FUNC(TradeCheck_CheckPrdAttrLimit);
		
		
		CLASS_FUNC(CheckTradeState);
		CLASS_FUNC(CheckSvcStateNewMove);
		CLASS_FUNC(CheckDestroyEPONUser);
		
		//QC 1203 begin
        CLASS_FUNC(IsRequestFromUcrm);
        //QC 1203 end
        
        int TradeCheck_NeedCheckOptrDataRight(CFmlBuf &inBuf, CFmlBuf &outBuf);
        int TradeCheck_GetCustUserInfoIntoBuf(CFmlBuf &inBuf, CFmlBuf &outBuf);
        int TradeCheck_CheckBlackUser(CFmlBuf &inBuf, CFmlBuf &outBuf);
        int TradeCheck_CheckOweFee(CFmlBuf &inBuf, CFmlBuf &outBuf);
        int TradeCheck_CheckPreDeposit(CFmlBuf &inBuf, CFmlBuf &outBuf);
        int TradeCheck_CheckForegift(CFmlBuf &inBuf, CFmlBuf &outBuf);
        int CheckStaffIdRight(CFmlBuf &inBuf, CFmlBuf &outBuf);
        
        /**
         * 业务受理前条件判断:业务规则校验，包括用户状态、欠费、业务是否受限等检查
         * @param strTradeTypeCode  输入 业务类型代码
         * @param strEparchyCode 输入 受理地地州代码
         * @param strIdType 输入 标识类型 0:客户标识  1:用户标识  2:帐户标识
         * @param strId     输入 标识
         * @param strFee    输入 欠费金额
         * @return 无，失败时抛出例外
         */
        //##ModelId=41D6D8890248
        void CheckBeforeTrade(const string &strTradeTypeCode, const string &strEparchyCode, const string &strStaffId,
                                             const string &strIdType, const string &strId, const string &strFee,
                                             const string &strCodingStr, CFmlBuf &outBuf, const string &strCityCode = "",const string &strInModeCode="", const int &iMode = 0);
        /**
        *业务受理前条件判断:业务规则校验，用户服务状态与业务参数配置检查
        * @param strUserId 输入 用户标识
        * @param strTradeTypeCode 输入 业务类型代码
        * @param strEparchyCode 输入 受理地州代码
        * @return 无，失败时抛出例外
        */
        void CheckSvcState(const string &strUserId, const string &strTradeTypeCode, const string &strEparchyCode, const string &strBrandCode, const string &strProductId,const string &strNetTypeCode);
        
        void CheckOneCardNCodesSn(const string &strUserIdA, const string &strEparchyCode, const string &strUserIdB);
        
        void CheckChinagoDelay(string &strTradeId,string &strEparchyCode);
        
        void CheckAfterChangeProduct(const string &strUserId, const int &iOldProductId, const int &iNewProductId, string &strCodingStr, const string &strEparchyCode);

		int GetUserLastValidBindSaleInfo(CFmlBuf &bufUserPurchase, const string &strUserId);
		
		void CheckSvcStateNew(const string &strUserId, const string &strTradeTypeCode, const string &strEparchyCode,
							  const string &strBrandCode, const string &strProductId,const string strNetTypeCode);
        
	private:

};

#endif /* TradeCheckBeforeTrade_H_HEADER_INCLUDED_BE1432B4 */
