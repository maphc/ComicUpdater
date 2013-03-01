#ifndef TradeCheckBeforeTrade_H_HEADER_INCLUDED_BE1432B4
#define TradeCheckBeforeTrade_H_HEADER_INCLUDED_BE1432B4
//supportClone

#include "../base/CustServPubUtils.h"
//qc:05330 begin
//#include "Utility.h"
//qc:05330 end

//---------------------- sdubss ---------------------
/** @class TradeCheckBeforeTrade
 *  @brief ҵ������ǰ�����ж�
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
         * ҵ������ǰ�����ж�:ҵ�����У�飬�����û�״̬��Ƿ�ѡ�ҵ���Ƿ����޵ȼ��
         * @param strTradeTypeCode  ���� ҵ�����ʹ���
         * @param strEparchyCode ���� ����ص��ݴ���
         * @param strIdType ���� ��ʶ���� 0:�ͻ���ʶ  1:�û���ʶ  2:�ʻ���ʶ
         * @param strId     ���� ��ʶ
         * @param strFee    ���� Ƿ�ѽ��
         * @return �ޣ�ʧ��ʱ�׳�����
         */
        //##ModelId=41D6D8890248
        void CheckBeforeTrade(const string &strTradeTypeCode, const string &strEparchyCode, const string &strStaffId,
                                             const string &strIdType, const string &strId, const string &strFee,
                                             const string &strCodingStr, CFmlBuf &outBuf, const string &strCityCode = "",const string &strInModeCode="", const int &iMode = 0);
        /**
        *ҵ������ǰ�����ж�:ҵ�����У�飬�û�����״̬��ҵ��������ü��
        * @param strUserId ���� �û���ʶ
        * @param strTradeTypeCode ���� ҵ�����ʹ���
        * @param strEparchyCode ���� ������ݴ���
        * @return �ޣ�ʧ��ʱ�׳�����
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
