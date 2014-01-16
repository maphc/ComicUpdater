#ifndef TradeCheckAfterTrade_H_HEADER_INCLUDED_BE1432B4
#define TradeCheckAfterTrade_H_HEADER_INCLUDED_BE1432B4
//supportClone

#include "../base/CustServPubUtils.h"

/** @class TradeCheckAfterTrade
 *  @brief ҵ��ǼǺ������ж�
 */
class TradeCheckAfterTrade
{
    public:
        CLASS_FUNC(CheckAfterTrade);
        
        CLASS_FUNC(CheckProductAllElementLimit);
        
        CLASS_FUNC(CheckBindPhone);
        
        CLASS_FUNC(CheckPurchaseAcctReturn);
        
        CLASS_FUNC(CheckProductModelLimit);
        
        CLASS_FUNC(CheckSpProductModelLimit);
        
        CLASS_FUNC(CheckAgentTradeFee);                                                                                            
        
        //��Ա���Ӳ�Ʒ���ּ�ͥ��ϵ���͡��ʷѵ����Ƿ�ƥ��
        CLASS_FUNC(ChkWoDisUserProdLimit);                                                                                         
        
        //��ȡ�û���Ʒ��Ϣ��ҵ�������
        int GetUserProductInfo(const string &strTradeId, const string &strUserId,
                               CFmlBuf &bfUserProd);
        
        int CheckUserProductMaxSale(const string &strTradeId, const string &strUserId);
        
        void CheckUserElement(const string &strTradeId, const string &strUserId, const string &strEparchyCode,CFmlBuf &bfUserProd);
                             
        int GeneUserAllElements(CFmlBuf &bufTradeDiscnt_AllDiscnt, CFmlBuf &bufTradeSvc_UserAllSvc,
                                CFmlBuf &bufTradeSp_UserAllSp, CFmlBuf &bufTradeElement_UserAllElement,
                                CFmlBuf &bufUserAllElements);
                               
        //����Ʒ��ʶ��ȡ�û���ҵ��������ҵ�����Ϣ
        int GetUserPackageByProduct(const int &iProductId, CFmlBuf &bufTradeDiscnt_AllDiscnt,
                                    CFmlBuf &bufTradeSp_UserAllSp, CFmlBuf &bufTradeElement_UserAllElement,
                                    CFmlBuf &bufTradeSvc_UserAllSvc, CFmlBuf &bufUserPackageByProd, const string &strProdEndDate="2040-12-31");
                                    
        //��ȡ��Ʒ���ɰ���Ϣ
        int GetPackageByProduct(const int &iProductId, const string &strEparchyCode, const string &strTradeStaffId, CFmlBuf &bufPackageByProd, const int xTag=0);
        
        //��Ʒ��ѡ���ж�
        void CheckForcePackageByProduct(const int &iProductId, const string &strEparchyCode, const string &strTradeStaffId, CFmlBuf &bufUserPackageByProd);
        
        //��Ʒ����Ԫ�ص������Сѡ�����ж�
        void CheckPackageElementChoiceByProduct(const string &strEparchyCode, const string &strTradeStaffId, CFmlBuf &bufUserPackageByProd,
                                                CFmlBuf &bufUserAllElements, CFmlBuf &bufNoChangePackage, CFmlBuf &bufDelPackage,
                                                CFmlBuf &bufElementTimeSeries,
                                                const int xTag=0);

        //��ȡȫ�ְ����ϵ
        int ExistsPackageLimitA(const int &iPackageIdA,const string &strLimitTag,
                                const string strEparchyCode, CFmlBuf &bufPackageLimit);

        //��ȡ��Ʒ�ڰ����ϵ
        int ExistsProdPackageLimitA(const int &iProductId, const int &iPackageIdA,
                                    const string &strLimitTag,const string strEparchyCode,
                                    CFmlBuf &bufPackageLimitByProd);
        
        //��ȡЭ����ʷѡ���ϲ�Ʒ֮���ϵ(����Э���ȡ�ʷѡ���ϲ�Ʒ)                                  
		int ExistsAttrElementLimitA(const string &strAttrCode, const string &strAttrValue,const string &strIdTypeCode,
                                              const string &strLimitTag, const string strEparchyCode,
                                              CFmlBuf &bufAttrElementLimit);
        //��ȡЭ����ʷѡ���ϲ�Ʒ֮���ϵ(�����ʷѡ���ϲ�Ʒ��ȡЭ��)                                      
		int ExistsAttrElementLimitB(const string &strElementId, const string &strIdTypeCode,
                                              const string &strLimitTag, const string strEparchyCode,
                                              CFmlBuf &bufAttrElementLimit);
        //��ȡ��������                                     
		string getAttrName(const string &strAttrCode);
		
		//��ȡ����ֵ���� add by lirui@20120213
		string getAttrValueName(const string &strAttrCode,const string &strAttrValueCode);
		                                              
        //��ȡ����ҵ�����û�����������Ϣ                                                                                                                       
		int GetAllAttrAfterTrade(const string &strTradeId, const string &strUserId, CFmlBuf &bufTradeAttr_AllAttr);
		
		//Э�����ʷѡ���ϲ�Ʒ֮�����ƹ�ϵ�ж�
		void CheckAttrElementLimit(CFmlBuf &bufTradeDiscnt_AllDiscnt, const string &strEparchyCode,const string strTradeId, const string &strUserId,const string &strCustId, const string &strAcctId);
		
		
        //ȫ�ְ����ϵ�ж�
        void CheckPackageLimit(CFmlBuf &bufUserPackage, const string strEparchyCode);
                                        
        //��Ʒ�ڰ����ϵ�ж�
        void CheckPackageLimitByProduct(const int &iProductId,CFmlBuf &bufUserPackageByProd,
                                        const string strEparchyCode);

        //���ڱ�ѡԪ���ж�
        void CheckPackageForceElements(const int &iProductId, const string &strProductName,
                                       CFmlBuf &bufUserPackageByProd, CFmlBuf &bufTradeDiscnt_AllDiscnt,
                                       CFmlBuf &bufTradeSvc_UserAllSvc,
                                       CFmlBuf &bufTradeSp_UserAllSp, CFmlBuf &bufTradeElement_UserAllElement,
                                       CFmlBuf &bufNoChangePackage, CFmlBuf &bufDelPackage);

        //��ȡ���ڱ�ѡԪ��
        int GetPackageForceElements(const int &iPackageId,CFmlBuf &bufPackageForceElements);
        
        //ȫ��Ԫ��֮�����ƹ�ϵ�ж�
        void CheckElementLimit(CFmlBuf &bufUserAllElements, const string &strEparchyCode,const string strTradeId);
        
        //����Ԫ��֮�����ƹ�ϵ�ж�
        void CheckPackageElementLimit(CFmlBuf &bufUserAllElements, const string &strEparchyCode,
                                      CFmlBuf &bufNoChangePackage, CFmlBuf &bufDelPackage);

        //��ȡȫ��Ԫ��֮���ϵ
        int ExistsElementLimitA(const string &strElementTypeA, const int &iElementIdA,
                                const string &strLimitTag, const string strEparchyCode,
                                CFmlBuf &bufElementLimit);
                                        
        //��ȡ����Ԫ��֮���ϵ
        int ExistsPackageElementLimitA(const int &iPackageId, const string &strElementTypeA,
                                       const int &iElementIdA, const string &strLimitTag,
                                       const string strEparchyCode, CFmlBuf &bufElementLimit);
                                
        //ͬһԪ����һ��ҵ����ֻ��ѡ��һ��
        void CheckSameElement(CFmlBuf &bufUserAllElements);
        
        //SP�����жϣ�ͬһ��SP_IDֻ��ѡ��һ��
        void CheckSameSPID(CFmlBuf &bufTradeSp_UserAllSp);
        
        //��ȡ����Ԫ��֮���ϵ
        int ExistsPackageElementsLimitA(const int &iPackageId, const string &strLimitTag,
                                        CFmlBuf &bufPackageElementsLimit);
                                         
        //����Ԫ�ع�ϵ�ж�
        void CheckPackageAndElementsLimit(CFmlBuf &bufUserPackageByProd, CFmlBuf &bufUserAllElements);                                         

        //��ȡ��Ʒ֮�����ƹ�ϵ
        int ExistsProductLimitA(const int &iProduct, const string &strLimitTag,
                                CFmlBuf &bfProductLimit);
                                
        //��Ʒ������
        void CheckProductLimit(CFmlBuf &bfUserProd);

        //��Ʒ��ԴԼ��
        void CheckProductRes(CFmlBuf &bfUserProd, const string &strTradeId, const string &strUserId,
                             const string &strEparchyCode);
                             
        //��ȡ�û�����
        int GetUserSerialNumber(CFmlBuf &bfUserSn, const string &strTradeId, const string &strUserId);
        
        //��ȡ�û�SIM��
        int GetUserSimCard(CFmlBuf &bfUserSimCard, const string &strTradeId, const string &strUserId);        

        //��ȡ�û��ֻ�����
        int GetUserDeviceType(CFmlBuf &bfUserDeviceType, const string &strTradeId, const string &strUserId);

        //��ȡ��Ʒ��ԴԼ��
        int GetProdResLimit(CFmlBuf &bfProdResLimit, const int &iProduct, const string &strForceTag,
                            const string &strCondTypeCode, const string &strResTypeCode,
                            const string &strEparchyCode);

        //��ȡɾ��������Ϣ
        int GetDelPackage(const string &strTradeId,CFmlBuf &bufDelPackage);

        //��ȡ��ǰҵ��δ�����仯�İ���Ϣ
        int GetNoChangePackage(const string &strTradeId,const string &strUserId, CFmlBuf &bufNoChangePackage);
        
        //�ж�Ԫ�غ�������֮������ƹ�ϵ
        void CheckElmentsAndItsAttr(CFmlBuf &inBuf,CFmlBuf &bufUserAllElements);
        
         //�ж�Ԫ�غ�������֮������ƹ�ϵ
        void CheckElmentsAndItsDiscnt(CFmlBuf &inBuf);
        
        //��ȡ����Ԫ��ʱ��������Ԫ�ظ��������ڼ������Ԫ�������С��ʱ��ʱ������������Ԫ�ؼ���Ϊһ��
        int GetElementTimeSeries(CFmlBuf &bufUserAllElements, CFmlBuf &bufElementTimeSeries);
        
        int GetAllDiscntAfterTradeComp(const string &strSubscribeId, 
                                   CFmlBuf &bufTradeDiscnt_AllDiscnt);  

        int GetUserAllServiceAfterTradeComp(const string &strSubscribeId,
                                         CFmlBuf &bufTradeSvc_UserAllSvc);                                         
        
        int GetUserAllElementAfterTradeComp(const string &strSubscribeId,
                                         CFmlBuf &bufTradeSvc_UserAllSvc);  
        
        int GetUserAllSpAfterTradeComp(const string &strSubscribeId,
                                         CFmlBuf &bufTradeSvc_UserAllSvc);
                                         
        void CheckElementLimitComp(CFmlBuf &bufUserAllElements, const string &strEparchyCode,const string &strSubscribeId);
    
        void AddDays(const string &strStartDateTime,const int &iDays, string &strResultDateTime);
        void AddDays(const string &strStartDateTime,const float &fDays, string &strResultDateTime);
        
        string toTableCol(const string &strCentent);
        string toTableRow(const string &strCentent1);
        string toTableRow(const string &strCentent1, const string &strCentent2);
        string toTableRow(const string &strCentent1, const string &strCentent2,
                          const string &strCentent3);
        string toTableRow(const string &strCentent1, const string &strCentent2,
                          const string &strCentent3, const string &strCentent4);
        string toTableRow(const string &strCentent1, const string &strCentent2,
                          const string &strCentent3, const string &strCentent4,
                          const string &strCentent5);
                          
    private:
        /**
         *������iServiceId��״̬strStateCode������״̬�Ƿ���ڻ��⡢������������ȫ�����Ĺ�ϵ
         *
         */
        bool ExistsSvcstateLimitA(const int & iServiceId,const string &strStateCode, const string &strEparchyCode,
                                  const string &strLimitTag, CFmlBuf &bufVSvcstateLimit);

        /**
         *������iServiceId��״̬strStateCode������״̬�Ƿ���ڱ���������������ȫ�����Ĺ�ϵ
         *
         */
        bool ExistsSvcstateLimitB(const int & iServiceId,const string &strStateCode, const string &strEparchyCode,
                                  const string &strLimitTag, CFmlBuf &bufVSvcstateLimit);


        /**
         *��ȡ����ҵ����û������з���
         */
        int GetUserAllServiceAfterTrade(const string &strTradeId, const string &strUserId,
                                         CFmlBuf &bufTradeSvc_UserAllSvc);

        /**
         *��ȡ����ҵ����û�������SP
         */
        int GetUserAllSpAfterTrade(const string &strTradeId, const string &strUserId,
                                   CFmlBuf &bufTradeSp_UserAllSp);
        /**
         *��ȡ����ҵ����û���������Ʒ������Ԫ��
         */
        int GetUserAllElementAfterTrade(const string &strTradeId, const string &strUserId,
                                        CFmlBuf &bufTradeElement_UserAllElement);
        /**
         *��ȡ����ҵ�����û������Żݺ��ŻݵĿ�ʼʱ�䡢����ʱ�䡢���޸�״̬������ԭ���Żݵ��޸�״̬Ĭ��Ϊ'A','B','C'��,���ж��Ƿ������Ʒ�����б�ѡ�Ż�
         *iJudge =1 ʱ�жϲ�Ʒ�µ��Ż��Ƿ����
         */
        int GetAllDiscntAfterTrade(const string &strTradeId, const string &strUserId,
                                   const string &strCustId, const string &strAcctId,
                                   CFmlBuf &bufTradeDiscnt_AllDiscnt);
        
        int GetAllDiscntAfterTradehb(const string &strTradeId, const string &strUserId,
                                   const string &strCustId, const string &strAcctId,
                                   CFmlBuf &bufTradeDiscnt_AllDiscnt);
        int GetUserAllServiceAfterTradehb(const string &strTradeId, const string &strUserId,
                                         CFmlBuf &bufTradeSvc_UserAllSvc);
        /**
         *��ȡ����ҵ����û������з���״̬
         *
         */
        void GetUserAllSvcstateAfterTrade(const string &strTradeId, const string &strUserId,
                                          CFmlBuf &bufVTradeSvcState_UserAllSvcState);

        /**
         *  ��ϲ�Ʒ����˲�Ʒ����У��
         */
        void ChkRelaUserProductLimit(const string &tradeId, const string &userId, const string &eparchyCode);
		
		/**
         * ��ȡ����ҵ��ȡ����Ԫ��
         */
		int GetUserTradeDelElements(const string &strTradeId,CFmlBuf &bufTradeDelElement);
        
        void CheckCustProductLimit(CFmlBuf &bufTrade);
};

#endif /* TradeCheckAfterTrade_H_HEADER_INCLUDED_BE1432B4 */
