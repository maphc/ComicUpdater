#ifndef TradeCheckAfterTrade_H_HEADER_INCLUDED_BE1432B4
#define TradeCheckAfterTrade_H_HEADER_INCLUDED_BE1432B4
//supportClone

#include "../base/CustServPubUtils.h"

/** @class TradeCheckAfterTrade
 *  @brief 业务登记后条件判断
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
        
        //成员附加产品和沃家庭关系类型、资费档次是否匹配
        CLASS_FUNC(ChkWoDisUserProdLimit);                                                                                         
        
        //获取用户产品信息（业务受理后）
        int GetUserProductInfo(const string &strTradeId, const string &strUserId,
                               CFmlBuf &bfUserProd);
        
        int CheckUserProductMaxSale(const string &strTradeId, const string &strUserId);
        
        void CheckUserElement(const string &strTradeId, const string &strUserId, const string &strEparchyCode,CFmlBuf &bfUserProd);
                             
        int GeneUserAllElements(CFmlBuf &bufTradeDiscnt_AllDiscnt, CFmlBuf &bufTradeSvc_UserAllSvc,
                                CFmlBuf &bufTradeSp_UserAllSp, CFmlBuf &bufTradeElement_UserAllElement,
                                CFmlBuf &bufUserAllElements);
                               
        //按产品标识获取用户在业务受理后的业务包信息
        int GetUserPackageByProduct(const int &iProductId, CFmlBuf &bufTradeDiscnt_AllDiscnt,
                                    CFmlBuf &bufTradeSp_UserAllSp, CFmlBuf &bufTradeElement_UserAllElement,
                                    CFmlBuf &bufTradeSvc_UserAllSvc, CFmlBuf &bufUserPackageByProd, const string &strProdEndDate="2040-12-31");
                                    
        //获取产品构成包信息
        int GetPackageByProduct(const int &iProductId, const string &strEparchyCode, const string &strTradeStaffId, CFmlBuf &bufPackageByProd, const int xTag=0);
        
        //产品必选包判断
        void CheckForcePackageByProduct(const int &iProductId, const string &strEparchyCode, const string &strTradeStaffId, CFmlBuf &bufUserPackageByProd);
        
        //产品包内元素的最大最小选择数判断
        void CheckPackageElementChoiceByProduct(const string &strEparchyCode, const string &strTradeStaffId, CFmlBuf &bufUserPackageByProd,
                                                CFmlBuf &bufUserAllElements, CFmlBuf &bufNoChangePackage, CFmlBuf &bufDelPackage,
                                                CFmlBuf &bufElementTimeSeries,
                                                const int xTag=0);

        //获取全局包间关系
        int ExistsPackageLimitA(const int &iPackageIdA,const string &strLimitTag,
                                const string strEparchyCode, CFmlBuf &bufPackageLimit);

        //获取产品内包间关系
        int ExistsProdPackageLimitA(const int &iProductId, const int &iPackageIdA,
                                    const string &strLimitTag,const string strEparchyCode,
                                    CFmlBuf &bufPackageLimitByProd);
        
        //获取协议和资费、组合产品之间关系(根据协议获取资费、组合产品)                                  
		int ExistsAttrElementLimitA(const string &strAttrCode, const string &strAttrValue,const string &strIdTypeCode,
                                              const string &strLimitTag, const string strEparchyCode,
                                              CFmlBuf &bufAttrElementLimit);
        //获取协议和资费、组合产品之间关系(根据资费、组合产品获取协议)                                      
		int ExistsAttrElementLimitB(const string &strElementId, const string &strIdTypeCode,
                                              const string &strLimitTag, const string strEparchyCode,
                                              CFmlBuf &bufAttrElementLimit);
        //获取属性名称                                     
		string getAttrName(const string &strAttrCode);
		
		//获取属性值名称 add by lirui@20120213
		string getAttrValueName(const string &strAttrCode,const string &strAttrValueCode);
		                                              
        //获取办理业务后的用户所有属性信息                                                                                                                       
		int GetAllAttrAfterTrade(const string &strTradeId, const string &strUserId, CFmlBuf &bufTradeAttr_AllAttr);
		
		//协议与资费、组合产品之间限制关系判断
		void CheckAttrElementLimit(CFmlBuf &bufTradeDiscnt_AllDiscnt, const string &strEparchyCode,const string strTradeId, const string &strUserId,const string &strCustId, const string &strAcctId);
		
		
        //全局包间关系判断
        void CheckPackageLimit(CFmlBuf &bufUserPackage, const string strEparchyCode);
                                        
        //产品内包间关系判断
        void CheckPackageLimitByProduct(const int &iProductId,CFmlBuf &bufUserPackageByProd,
                                        const string strEparchyCode);

        //包内必选元素判断
        void CheckPackageForceElements(const int &iProductId, const string &strProductName,
                                       CFmlBuf &bufUserPackageByProd, CFmlBuf &bufTradeDiscnt_AllDiscnt,
                                       CFmlBuf &bufTradeSvc_UserAllSvc,
                                       CFmlBuf &bufTradeSp_UserAllSp, CFmlBuf &bufTradeElement_UserAllElement,
                                       CFmlBuf &bufNoChangePackage, CFmlBuf &bufDelPackage);

        //获取包内必选元素
        int GetPackageForceElements(const int &iPackageId,CFmlBuf &bufPackageForceElements);
        
        //全局元素之间限制关系判断
        void CheckElementLimit(CFmlBuf &bufUserAllElements, const string &strEparchyCode,const string strTradeId);
        
        //包内元素之间限制关系判断
        void CheckPackageElementLimit(CFmlBuf &bufUserAllElements, const string &strEparchyCode,
                                      CFmlBuf &bufNoChangePackage, CFmlBuf &bufDelPackage);

        //获取全局元素之间关系
        int ExistsElementLimitA(const string &strElementTypeA, const int &iElementIdA,
                                const string &strLimitTag, const string strEparchyCode,
                                CFmlBuf &bufElementLimit);
                                        
        //获取包内元素之间关系
        int ExistsPackageElementLimitA(const int &iPackageId, const string &strElementTypeA,
                                       const int &iElementIdA, const string &strLimitTag,
                                       const string strEparchyCode, CFmlBuf &bufElementLimit);
                                
        //同一元素在一笔业务中只能选择一次
        void CheckSameElement(CFmlBuf &bufUserAllElements);
        
        //SP服务判断：同一个SP_ID只能选择一个
        void CheckSameSPID(CFmlBuf &bufTradeSp_UserAllSp);
        
        //获取包与元素之间关系
        int ExistsPackageElementsLimitA(const int &iPackageId, const string &strLimitTag,
                                        CFmlBuf &bufPackageElementsLimit);
                                         
        //包与元素关系判断
        void CheckPackageAndElementsLimit(CFmlBuf &bufUserPackageByProd, CFmlBuf &bufUserAllElements);                                         

        //获取产品之间限制关系
        int ExistsProductLimitA(const int &iProduct, const string &strLimitTag,
                                CFmlBuf &bfProductLimit);
                                
        //产品间限制
        void CheckProductLimit(CFmlBuf &bfUserProd);

        //产品资源约束
        void CheckProductRes(CFmlBuf &bfUserProd, const string &strTradeId, const string &strUserId,
                             const string &strEparchyCode);
                             
        //获取用户号码
        int GetUserSerialNumber(CFmlBuf &bfUserSn, const string &strTradeId, const string &strUserId);
        
        //获取用户SIM卡
        int GetUserSimCard(CFmlBuf &bfUserSimCard, const string &strTradeId, const string &strUserId);        

        //获取用户手机类型
        int GetUserDeviceType(CFmlBuf &bfUserDeviceType, const string &strTradeId, const string &strUserId);

        //获取产品资源约束
        int GetProdResLimit(CFmlBuf &bfProdResLimit, const int &iProduct, const string &strForceTag,
                            const string &strCondTypeCode, const string &strResTypeCode,
                            const string &strEparchyCode);

        //获取删除包的信息
        int GetDelPackage(const string &strTradeId,CFmlBuf &bufDelPackage);

        //获取当前业务未发生变化的包信息
        int GetNoChangePackage(const string &strTradeId,const string &strUserId, CFmlBuf &bufNoChangePackage);
        
        //判断元素和其属性之间的限制关系
        void CheckElmentsAndItsAttr(CFmlBuf &inBuf,CFmlBuf &bufUserAllElements);
        
         //判断元素和其属性之间的限制关系
        void CheckElmentsAndItsDiscnt(CFmlBuf &inBuf);
        
        //获取包内元素时间连续的元素个数，用于计算包内元素最大最小数时，时间连续的两个元素计算为一个
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
         *检查服务iServiceId的状态strStateCode与其他状态是否存在互斥、部分依赖、完全依赖的关系
         *
         */
        bool ExistsSvcstateLimitA(const int & iServiceId,const string &strStateCode, const string &strEparchyCode,
                                  const string &strLimitTag, CFmlBuf &bufVSvcstateLimit);

        /**
         *检查服务iServiceId的状态strStateCode与其他状态是否存在被部分依赖、被完全依赖的关系
         *
         */
        bool ExistsSvcstateLimitB(const int & iServiceId,const string &strStateCode, const string &strEparchyCode,
                                  const string &strLimitTag, CFmlBuf &bufVSvcstateLimit);


        /**
         *获取办理业务后用户的所有服务
         */
        int GetUserAllServiceAfterTrade(const string &strTradeId, const string &strUserId,
                                         CFmlBuf &bufTradeSvc_UserAllSvc);

        /**
         *获取办理业务后用户的所有SP
         */
        int GetUserAllSpAfterTrade(const string &strTradeId, const string &strUserId,
                                   CFmlBuf &bufTradeSp_UserAllSp);
        /**
         *获取办理业务后用户的所有物品等其他元素
         */
        int GetUserAllElementAfterTrade(const string &strTradeId, const string &strUserId,
                                        CFmlBuf &bufTradeElement_UserAllElement);
        /**
         *获取办理业务后的用户所有优惠和优惠的开始时间、结束时间、和修改状态（三户原有优惠的修改状态默认为'A','B','C'）,并判断是否包括产品的所有必选优惠
         *iJudge =1 时判断产品下的优惠是否存在
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
         *获取办理业务后用户的所有服务状态
         *
         */
        void GetUserAllSvcstateAfterTrade(const string &strTradeId, const string &strUserId,
                                          CFmlBuf &bufVTradeSvcState_UserAllSvcState);

        /**
         *  组合产品与个人产品限制校验
         */
        void ChkRelaUserProductLimit(const string &tradeId, const string &userId, const string &eparchyCode);
		
		/**
         * 获取本次业务取消的元素
         */
		int GetUserTradeDelElements(const string &strTradeId,CFmlBuf &bufTradeDelElement);
        
        void CheckCustProductLimit(CFmlBuf &bufTrade);
};

#endif /* TradeCheckAfterTrade_H_HEADER_INCLUDED_BE1432B4 */
