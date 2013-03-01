/**
 * 客户服务错误代码
 */

#ifndef __CUSTSERVERROR_H_
#define __CUSTSERVERROR_H_

//#include "stdafx.h"


const int BASECUSTSERV_ERR = 300000;
/**
 * 生成流水号异常
 */
const int GENERATEID_ERR = BASECUSTSERV_ERR + 1;

/**
 * 获取普通参数异常
 */
const int GETNORMALPARA_ERR = BASECUSTSERV_ERR + 2;

/**
 * 获取通用参数异常
 */
const int GETCOMMPARA_ERR = BASECUSTSERV_ERR + 3;

/**
 * 获取系统时间异常
 */
const int GETSYSDATE_ERR = BASECUSTSERV_ERR + 4;

/**
 * 获取业务类型参数异常
 */
const int GETTRADETYPEPARAM_ERR = BASECUSTSERV_ERR + 5;

/**
 * 资源校验：包括可用性校验、权限校验等
 */
const int CHECKRESOURCE_ERR = BASECUSTSERV_ERR + 11;

/**
 * 资源选占
 */
const int RESOURCEOCCUPY_ERR = BASECUSTSERV_ERR + 12;

/**
 * 更新资源占用状态，包括占用新使用的资源，释放不再使用的资源
 */
const int MODIFYRESSTATE_ERR = BASECUSTSERV_ERR + 13;

/**
 * 资源预占：将资源从选占状态变为预占状态，变更前需校验该资源号码是否仍为选占状态。
 */
const int RESENGROSS_ERR = BASECUSTSERV_ERR + 14;

/**
 * 释放员工选占资源
 */
const int RELESTAFFOCCUPYRES_ERR = BASECUSTSERV_ERR + 15;

/**
 * 检查该服务号码是否可用于二次开户（检查其对应的SIM卡在SIM卡渠道订购明细表中有记录且返单标志为
 */
const int CANREOPENMPCODE_ERR = BASECUSTSERV_ERR + 16;

/**
 * 登记SIM卡渠道订购明细表中的返单标志RegistSimBackTag
 */
const int REGISTSIMBACKTAG_ERR = BASECUSTSERV_ERR + 17;

/**
 * 获取客户资料
 */
const int GETCUSTINFO_ERR = BASECUSTSERV_ERR + 18;

/**
 * 判断是否为黑名单资料
 */
const int ISBLACKUSER_ERR = BASECUSTSERV_ERR + 19;

/**
 * 是否限制新开用户
 */
const int ISCREATUSERLIMIT_ERR = BASECUSTSERV_ERR + 20;

/**
 * 生成新客户异常
 */
const int CREATECUSTINFO_ERR = BASECUSTSERV_ERR + 21;

/**
 * 修改客户资料异常
 */
const int UPDATECUSTINFO_ERR = BASECUSTSERV_ERR + 22;

/**
 * 获取订单信息异常
 */
const int GETTRADEINFO_ERR = BASECUSTSERV_ERR + 23;

/**
 * 生成新帐户异常
 */
const int CREATEACCTINFO_ERR = BASECUSTSERV_ERR + 24;

/**
 * 修改帐户资料异常
 */
const int MODIFYACCTINFO_ERR = BASECUSTSERV_ERR + 25;

/**
 * 获取帐户资料异常
 */
const int GETACCTINFO_ERR = BASECUSTSERV_ERR + 26;

/**
 * 生成业务台帐异常
 */
const int GENETRADE_ERR = BASECUSTSERV_ERR + 27;

/**
 * 生成备份台帐异常
 */
const int GENEBACKUPTRADE_ERR = BASECUSTSERV_ERR + 28;

/**
 * 生成费用台帐异常
 */
const int GENETRADEFEE_ERR = BASECUSTSERV_ERR + 29;

/**
 * 生成服务子台帐异常
 */
const int GENETRADESVC_ERR = BASECUSTSERV_ERR + 30;

/**
 * 生成服务状态子台帐异常
 */
const int GENETRADESVCSTATE_ERR = BASECUSTSERV_ERR + 31;

/**
 * 生成优惠子台帐异常
 */
const int GENETRADEDISCNT_ERR = BASECUSTSERV_ERR + 32;

/**
 * 生成资源子台帐异常
 */
const int GENETRADERES_ERR = BASECUSTSERV_ERR + 33;

/**
 * 生成付费关系子台帐异常
 */
const int GENETRADEPAYRELA_ERR = BASECUSTSERV_ERR + 34;

/**
 * 生成邮寄子台帐异常
 */
const int GENETRADEPOST_ERR = BASECUSTSERV_ERR + 35;

/**
 * 台帐费用稽核异常
 */
const int AUDITTRADEFEE_ERR = BASECUSTSERV_ERR + 36;

/**
 * 生成受理单打印内容异常
 */
const int GENETRADERECEIPTINFO_ERR = BASECUSTSERV_ERR + 37;

/**
 * 判断是否需要发指令并置标志异常
 */
const int ISNEEDOLCOM_ERR = BASECUSTSERV_ERR + 38;

/**
 * 搬迁业务台帐异常
 */
const int MOVETRADE_ERR = BASECUSTSERV_ERR + 39;

/**
 * 业务登记前条件判断异常
 */
const int CHECKBEFOREREG_ERR = BASECUSTSERV_ERR + 40;

/**
 * 业务登记后条件判断异常
 */
const int CHECKAFTERTRADE_ERR = BASECUSTSERV_ERR + 41;

/**
 * 业务受理前条件判断异常
 */
const int CHECKBEFORETRADE_ERR = BASECUSTSERV_ERR + 42;

/**
 * 检查是否黑名单
 */
const int ISBLACKINFO_ERR = BASECUSTSERV_ERR + 43;

/**
 * 是否限制新开用户
 */
const int ISCREATEUSERLIMIT_ERR = BASECUSTSERV_ERR + 44;

/**
 * 生成联指工单
 */
const int GENEOLCOM_ERR = BASECUSTSERV_ERR + 45;

/**
 * 生成业务固定指令
 */
const int GENETRADEOLCOMSERV_ERR = BASECUSTSERV_ERR + 45;

/**
 * 生成服务开通指令
 */
const int GENESVCACTIVESERV_ERR = BASECUSTSERV_ERR + 46;

/**
 * 填写联指服务参数
 */
const int FILLINOLCOMPARA_ERR = BASECUSTSERV_ERR + 47;

/**
 * 搬迁联指工单
 */
const int MOVEOLCOM_ERR = BASECUSTSERV_ERR + 48;

/**
 * 获取联指工单状态
 */
const int GETOLCOMSTATE_ERR = BASECUSTSERV_ERR + 49;

//add by liufei for UserMgr
/**
 * 创建用户
 */
const int CREATEUSERINFO_ERR = BASECUSTSERV_ERR + 50;

/**
 * 修改用户资料
 */
const int MODIFYUSERINFO_ERR = BASECUSTSERV_ERR + 51;

/**
 * 获取用户和用户关系信息
 */
const int GETUSERRELATION_ERR = BASECUSTSERV_ERR + 52;

/**
 * 增加用户和用户关系信息
 */
const int ADDUSERRELATION_ERR = BASECUSTSERV_ERR + 53;

/**
 * 新增用户优惠
 */
const int ADDUSERDISCNT_ERR = BASECUSTSERV_ERR + 54;

/**
 * 修改用户优惠
 */
const int MODIFYUSERDISCNT_ERR = BASECUSTSERV_ERR + 55;

/**
 * 新增用户服务
 */
const int ADDUSERSERVICE_ERR = BASECUSTSERV_ERR + 56;

/**
 * 修改用户服务
 */
const int MODIFYUSERSERVICE_ERR = BASECUSTSERV_ERR + 57;

/**
 * 新增用户服务状态
 */
const int ADDUSERSVCSTATE_ERR = BASECUSTSERV_ERR + 58;

/**
 * 修改用户服务状态
 */
const int MODIFYUSERSVCSTATE_ERR = BASECUSTSERV_ERR + 59;

/**
 * 新增用户占用资源
 */
const int ADDUSERRES_ERR = BASECUSTSERV_ERR + 60;

/**
 * 修改用户占用资源
 */
const int MODIFYUSERRES_ERR = BASECUSTSERV_ERR + 61;

/**
 * 新增用户付费关系
 */
const int ADDUSERPAYRELA_ERR = BASECUSTSERV_ERR + 62;

/**
 * 修改用户付费关系
 */
const int MODIFYUSERPAYRELA_ERR = BASECUSTSERV_ERR + 63;

/**
 * 修改用户押金
 */
const int MODIFYUSERFOREGIFT_ERR = MODIFYUSERFOREGIFT_ERR + 64;

/**
 * 新增用户押金
 */
const int ADDUSERFOREGIFT_ERR = BASECUSTSERV_ERR + 65;

/**
 * 新增用户资料变更
 */
const int ADDUSERINFOCHG_ERR = BASECUSTSERV_ERR + 66;

/**
 * 修改用户资料变更
 */
const int MODIFYUSERINFOCHG_ERR = BASECUSTSERV_ERR + 67;

/**
 * 新增用户业务受限
 */
const int ADDUSERTRADELIMIT_ERR = BASECUSTSERV_ERR + 68;

/**
 * 修改用户业务受限
 */
const int MODIFYUSERTRADELIMIT_ERR = BASECUSTSERV_ERR + 69;

/**
 * 获取用户信息
 */
const int GETUSERINFO_ERR = BASECUSTSERV_ERR + 70;

/**
 * 根据服务号码获取帐户资料
 */
const int GETACCTBYSN_ERR = BASECUSTSERV_ERR + 71;

/**
 * 修改用户邮寄资料
 */
const int MODIFYUSERPOSTINFO_ERR = BASECUSTSERV_ERR + 72;

/**
 * 增加用户邮寄资料
 */
const int ADDUSERPOSTINFO_ERR = BASECUSTSERV_ERR + 73;

/**
 * 获取用户服务资料
 */
const int GETUSERSERVICE_ERR = BASECUSTSERV_ERR + 74;

/**
 * 获取用户优惠资料
 */
const int GETUSERDISCNT_ERR = BASECUSTSERV_ERR + 75;

/**
 * 获取用户占用资源
 */
const int GETUSERRESOURCE_ERR = BASECUSTSERV_ERR + 76;

/**
 * 费用减免校验异常
 */
const int CheckDerateFee_ERR = BASECUSTSERV_ERR + 77;

/**
 * 获取付款子类型列表异常
 */
const int GetSubPayMoney_ERR = BASECUSTSERV_ERR + 78;

/**
 * 生成台帐积分主表
 */
const int GENETRADESCOREMAIN_ERR = BASECUSTSERV_ERR + 79;

/**
 * 生成台帐积分子表
 */
const int GENETRADESCORECHILD_ERR = BASECUSTSERV_ERR + 80;

/**
 * 删除客户资料
 */
const int DESTROYCUST_ERR = BASECUSTSERV_ERR + 81;

/**
 * 获取亲情资料
 */
const int GETFAMILYINFO_ERR = BASECUSTSERV_ERR + 82;


/**
 * 增加副卡用户资料
 */
const int ADDDECKFAMILYUSER_ERR = BASECUSTSERV_ERR + 83;

/**
 * 生成指令服务编码资料
 */
const int GENECHGRESSERV_ERR = BASECUSTSERV_ERR + 84;

/**
 * 生成指令服务状态编码资料
 */
const int GENESVCSTATECHGSERV_ERR = BASECUSTSERV_ERR + 85;

/**
 * 恢复用户服务
 */
const int RESTOREUSERSVC_ERR = BASECUSTSERV_ERR + 86;

/**
 * 将用户恢复为正常用户
 */
const int RESTOREUSER_ERR = BASECUSTSERV_ERR + 87;

/**
 * 恢复用户优惠
 */
const int RESTOREUSERDISCNT_ERR = BASECUSTSERV_ERR + 88;

/**
 * 恢复用户关联优惠
 */
const int RESTOREUSERRELADISCNT_ERR = BASECUSTSERV_ERR + 89;

/**
 * 恢复用户与用户关系
 */
const int RESTOREUSERRELATION_ERR = BASECUSTSERV_ERR + 90;


/**
 * 根据用户标识获取用户服务状态
 */
const int GETUSERSVCSTATE_ERR = BASECUSTSERV_ERR + 91;

/**
 * 查询用户邮寄资料
 */
const int GETUSERPOSTINFO_ERR = BASECUSTSERV_ERR + 92;

/**
 * 获取用户关联资料
 */
const int GETUSERRELATIONINFO_ERR = BASECUSTSERV_ERR + 93;

/**
 * 获取用户订购信息
 */
const int GETUSERSUBSCRIBEINFO_ERR = BASECUSTSERV_ERR + 94;

/**
 * 增加用户订购信息
 */
const int MODIFYUSERSUBSCRIBEINFO_ERR = BASECUSTSERV_ERR + 95;

/**
 * 查询用户积分资料表记录
 */
const int GETUSERSCOREINFO_ERR = BASECUSTSERV_ERR + 96;

/**
 * 获取用户当前及产品下所有服务资料
 */
const int GETUSERALLSERVICE_ERR = BASECUSTSERV_ERR + 97;

/**
 * 更新用户归属客户标识
 */
const int REPLACEUSERCUSTID_ERR = BASECUSTSERV_ERR + 98;

/**
 * 拷贝用户服务
 */
const int COPYUSERSERVICE_ERR = BASECUSTSERV_ERR + 99;

/**
 * 拷贝用户服务状态
 */
const int COPYUSERSVCSTATE_ERR = BASECUSTSERV_ERR + 100;

/**
 * 拷贝用户资源
 */
const int COPYUSERRES_ERR = BASECUSTSERV_ERR + 101;

/**
 * 拷贝用户优惠资料
 */
const int COPYUSERDISCNT_ERR = BASECUSTSERV_ERR + 102;

/**
 * 拷贝用户押金资料
 */
const int COPYUSERFOREGIFT_ERR = BASECUSTSERV_ERR + 103;

/**
 * 拷贝用户
 */
const int COPYUSER_ERR = BASECUSTSERV_ERR + 104;

/**
 * 生成用户购机资料
 */
const int GENEUSERPARCHASE_ERR = BASECUSTSERV_ERR + 105;

/**
 * 返单开户检查
 */
const int CHECKRETURNOPEN_ERR = BASECUSTSERV_ERR + 106;

/**
 * 获取用户当前及产品下所有优惠资料
 */
const int GETALLUSERDISCNT_ERR = BASECUSTSERV_ERR + 107;

/**
 * 查找用户付费关系
 */
const int GETUSERPAYRELAINFO_ERR = BASECUSTSERV_ERR + 108;

/**
 * 获取以此帐户为默认帐户的用户标识及服务号码
 */
const int GETUSERBYACCTID_ERR = BASECUSTSERV_ERR + 109;

/**
 * 修改副卡用户资料
 */
const int CHGDECKFAMILYUSER_ERR = BASECUSTSERV_ERR + 110;

/**
 * 删除副卡用户资料
 */
const int DELDECKFAMILYUSER_ERR = BASECUSTSERV_ERR + 111;


/**
 * 得到购机业务参数
 */
const int GETPURCHASEPARA_ERR = BASECUSTSERV_ERR + 112;

/**
 * 得到积分类型参数
 */
const int GETSCOREEXCHANGETYPE_ERR = BASECUSTSERV_ERR + 113;

/**
 *得到积分动作参数
 */
const int GETSCOREACTIONPARA_ERR = BASECUSTSERV_ERR + 114;


/*更新用户积分信息*/
const int MODIFYUSERSCOREINFO_ERR = BASECUSTSERV_ERR + 115;

/*更新预销户用户信息*/
const int PREDESTROYUSER_ERR = BASECUSTSERV_ERR + 116;

/*更新正式销户用户信息*/
const int DESTROYUSER_ERR = BASECUSTSERV_ERR + 117;

/*销户用户的资料*/
const int GETREMOVEUSERINFO_ERR = BASECUSTSERV_ERR + 118;

/**
 * 处理关联优惠
 */
const int DEALUSERRELADISCNT_ERR = BASECUSTSERV_ERR + 119;

/**
 * 根据手机用户标识获取绑定的IP固定电话用户列表
 */
const int GETBINDIPPHONE_ERR = BASECUSTSERV_ERR + 120;

/**
 * 短号码校验（同一集团下不能有同样的短号码）
 */
const int CHECKSHORTCODE_ERR = BASECUSTSERV_ERR + 121;

/**
 * 固定号码校验:检查该固定号码是否已经开通了IP业务
 */
const int CHECKIPPHONE_ERR = BASECUSTSERV_ERR + 122;

/**
 * 获取VPMN集团下闭合群列表
 */
const int GETVPMNCLOSEGROUP_ERR = BASECUSTSERV_ERR + 123;

/**
 * 增加VPMN闭合群组信息
 */
const int ADDUSERCLOSEGROUP_ERR = BASECUSTSERV_ERR + 124;

/**
 * 维护VPMN闭合群组信息
 */
const int MODIFYUSERCLOSEGROUP_ERR = BASECUSTSERV_ERR + 125;

/**
 * 增加VPMN闭合群用户信息
 */
const int ADDUSERCLOSEUSER_ERR = BASECUSTSERV_ERR + 126;

/**
 * 维护VPMN闭合群用户信息
 */
const int MODIFYUSERCLOSEUSER_ERR = BASECUSTSERV_ERR + 127;

/**
 * 增加IP固定电话用户
 */
const int ADDIPPHONEUSER_ERR = BASECUSTSERV_ERR + 128;

/**
 * 取消IP固定电话用户
 */
const int DELIPPHONEUSER_ERR = BASECUSTSERV_ERR + 129;
/**
 * 修改IP固定电话用户
 */
const int CHGIPPHONEUSER_ERR = BASECUSTSERV_ERR + 130;

/**
 * 获取银行资料信息
 */
const int GETBANKINFO_ERR = BASECUSTSERV_ERR + 131;

/**
 * 获取营业费用信息
 */
const int GETOPERFEEINFO_ERR = BASECUSTSERV_ERR + 132;

/**
 * 处理IP固定电话用户服务及服务状态
 */
const int DEALUSERIPSERVICE_ERR = BASECUSTSERV_ERR + 133;

/**
 * 获取用户重要信息异动资料
 */
const int GETUSERINFOCHANGE_ERR = BASECUSTSERV_ERR + 134;

/**
 * 获取当前帐期
 */
const int GETACYCID_ERR = BASECUSTSERV_ERR + 135;

/*查询销户用户信息*/
//const int GETREMOVEUSERINFO_ERR = BASECUSTSERV_ERR + 136;

/*设置销户标志*/
//const int DESTROYUSER_ERR = BASECUSTSERV_ERR + 137;

/*变更用户积分*/
const int MODIFYUSERSCORE_ERR = BASECUSTSERV_ERR + 136;

/*查询台帐信息*/
const int SELECTTRADEINFO_ERR = BASECUSTSERV_ERR + 137;

/*生成普通付费关系台帐子表*/
const int GENETRADENORPAYRELA_ERR = BASECUSTSERV_ERR + 138;

/**
 * 办理过户业务时，获取客户下要过户的用户列表
 */
const int GETALLUSERID_ERR = BASECUSTSERV_ERR + 139;

/**
 * 办理过户业务时，从用户列表中取出要处理的一条用户信息
 */
const int FETCHUSERID_ERR = BASECUSTSERV_ERR + 140;

/**
 * 新增用户档案信息
 */
const int ADDUSERFILE_ERR = BASECUSTSERV_ERR + 141;

/**
 * 获取付费帐户明细帐目
 */
const int GETPAYITEMDETAIL_ERR = BASECUSTSERV_ERR + 142;

/**
 * 获取综合帐目明细
 */
const int INTEITEMDETAIL_ERR = BASECUSTSERV_ERR + 143;

/**
 * 获取业务类型编码错误
 */
 const int TRADETYPECODE_ERR = BASECUSTSERV_ERR + 144;

/**
 * 获取邮寄资料错误
 */
 const int GETPOSTINFO_ERR = BASECUSTSERV_ERR + 145;

/**
 * 查询账户下用户的客户资料错误
 */
 const int GETCUSTOFACCTUSER_ERR = BASECUSTSERV_ERR + 146;

/**
 * 查询用户押金错误
 */
 const int GETUSERFOREGIFT_ERR = BASECUSTSERV_ERR + 147;

/**
 * 查询付费关系错误
 */
 const int GETPAYRELATION_ERR = BASECUSTSERV_ERR + 148;

/**
 * 缴费更新预付费用户有效期错误
 */
 const int UPDATEUSERVALIDDATE_ERR = BASECUSTSERV_ERR + 149;

/**
 * 查询是否为预付费用户错误
 */
 const int ISPREPAYUSER_ERR = BASECUSTSERV_ERR + 150;

/**
 * 查询帐户是否实时销账错误
 */
 const int ISREALPAYUSER_ERR = BASECUSTSERV_ERR + 151;

/**
 * 解析服务状态错误
 */
 const int GETSVCSTATE_ERR = BASECUSTSERV_ERR + 152;

/**
 * 获取付费编码
 */
 const int GENEPAYITEMCODE_ERR = BASECUSTSERV_ERR + 153;

/**
 * 获取付费编码
 */
const int MODIFYLESTUSERPAYRELA_ERR = BASECUSTSERV_ERR + 154;

/**
 * 获取资源设备价格信息
 */
const int GETDEVICEPRICE_ERR = BASECUSTSERV_ERR + 155;

/**
 * 获取业务固定费用错误
 */
const int GETTRADEOPERFEE_ERR = BASECUSTSERV_ERR + 156;

/**
 * 获取界面必输项错误
 */
const int GETMUSTFILLITEMS_ERR = BASECUSTSERV_ERR + 157;

/**
 * 产品变更业务中修改用户产品信息
 */
const int MODIFYUSERPRODUCT_ERR = BASECUSTSERV_ERR + 158;

/*
 *生成返销台帐
 */
const int GENETRADECANCEL_ERR = BASECUSTSERV_ERR + 159;

/*
 *查询返销业务
 */
const int GetUSERCANCELTRADE_ERR = BASECUSTSERV_ERR + 160;

/*
 *业务返销前检查
 */
const int CHECKBEFORECANCEL_ERR = BASECUSTSERV_ERR + 161;

/*
 *  资源占用
 */
const int RESPOSSESS_ERR = BASECUSTSERV_ERR + 162;

/*
 *  资源释放
 */
const int RESRELA_ERR = BASECUSTSERV_ERR + 163;

/*
 *  资源查询
 */
const int GETRESOURCEINFO_ERR = BASECUSTSERV_ERR + 164;

/*
 *  获取台帐明细表历史资料
 */
const int GETTRADEINFOBYUSERTRADE_ERR = BASECUSTSERV_ERR + 165;

/**
 * 生成返销业务固定指令
 */
const int UNDOGENETRADEOLCOMSERV_ERR = BASECUSTSERV_ERR + 166;

/**
 * 修改客户资料返销
 */
const int UNDOUPDATECUSTINFO_ERR = BASECUSTSERV_ERR + 167;

/**
 * 修改客户资料返销
 */
const int UNDODESTROYCUST_ERR = BASECUSTSERV_ERR + 168;

/**
 * 生成帐户资料返销
 */
const int UNDOCREATEACCTINFO_ERR = BASECUSTSERV_ERR + 169;

/**
 * 修改帐户资料返销
 */
const int UNDOMODIFYACCTINFO_ERR = BASECUSTSERV_ERR + 170;

/**
 * 修改客户资料返销
 */
const int UNDOCREATECUSTINFO_ERR = BASECUSTSERV_ERR + 171;

/**
 * 删除帐户资料
 */
const int DESTROYACCTINFO_ERR = BASECUSTSERV_ERR + 172;

/**
 * 删除帐户资料返销
 */
const int UNDODESTROYACCTINFO_ERR = BASECUSTSERV_ERR + 173;

/**
 * 获取批量处理功能编码参数
 */
const int GETBATOPERTYPE_ERR = BASECUSTSERV_ERR + 174;

/**
 * 生成批量业务处理台帐
 */
const int GENEBATCHDEALTRADE_ERR = BASECUSTSERV_ERR + 175;

/**
 * 收取预存款异常
 */
const int RECVDEPOSIT_ERR = BASECUSTSERV_ERR + 176;

/**
 * 帐户转帐异常
 */
const int TRANSACCTDEPOSIT_ERR = BASECUSTSERV_ERR + 177;

/**
 * 获取批量业务受理处理结果信息
 */
const int GETBATDEALRESULTINFO_ERR = BASECUSTSERV_ERR + 178;

/**
 * 获取标志集参数信息
 */
const int GETTAGSETINFO_ERR = BASECUSTSERV_ERR + 179;

/**
 * 用户优惠资料返销
 */
const int UNDOUSERDISCNT_ERR = BASECUSTSERV_ERR + 180;

/**
 * 用户存档资料返销
 */
const int UNDOADDUSERFILE_ERR = BASECUSTSERV_ERR + 181;

/**
 * 新增用户押金资料返销
 */
const int UNDOADDUSERFOREGIFT_ERR = BASECUSTSERV_ERR + 182;

/**
 * 修改用户押金资料返销
 */
const int UNDOMODIFYUSERFOREGIFT_ERR = BASECUSTSERV_ERR + 183;

/**
 * 拷贝用户押金资料返销
 */
const int UNDOCOPYUSERFOREGIFT_ERR = BASECUSTSERV_ERR + 184;

/**
 * 用户易动资料返销
 */
const int UNDOUSERINFOCHG_ERR = BASECUSTSERV_ERR + 185;

/**
 * 用户付费关系资料返销
 */
const int UNDOUSERPAYRELA_ERR = BASECUSTSERV_ERR + 186;

/**
 * 用户邮寄资料返销
 */
const int UNDOUSERPOSTINFO_ERR = BASECUSTSERV_ERR + 187;

/**
 * 用户资源资料返销
 */
const int UNDOUSERRES_ERR = BASECUSTSERV_ERR + 188;

/**
 * 用户积分资料返销
 */
const int UNDOUSERSCOREINFO_ERR = BASECUSTSERV_ERR + 189;

/**
 * 用户服务资料返销
 */
const int UNDOUSERSVC_ERR = BASECUSTSERV_ERR + 190;

/**
 * 用户服务状态资料返销
 */
const int UNDOUSERSVCSTATE_ERR = BASECUSTSERV_ERR + 191;

/**
 * 用户生成资料返销
 */
const int UNDOCREATEUSERINFO_ERR = BASECUSTSERV_ERR + 192;

/**
 * 用户关系资料返销
 */
const int UNDOUSERRELATION_ERR = BASECUSTSERV_ERR + 193;

/**
 * 用户业务限制资料返销
 */
const int UNDOUSERTRADELIMIT_ERR = BASECUSTSERV_ERR + 194;

/**
 * 拷贝用户资料返销
 */
const int UNDOCOPYUSER_ERR = BASECUSTSERV_ERR + 195;

/**
 * 用户预销户返销
 */
const int UNDODESTROYUSER_ERR = BASECUSTSERV_ERR + 196;

/**
 * 用户资料变更返销
 */
const int UNDOMODIFYUSERINFO_ERR = BASECUSTSERV_ERR + 197;

/**
 * 修改用户使用关系返销
 */
const int UNDOREPLACEUSERCUSTID_ERR = BASECUSTSERV_ERR + 198;

/**
 * IP直通车登记台帐
 */
const int GENEIPEXPRESSTRADE_ERR = BASECUSTSERV_ERR + 199;

/**
 * sim卡返单标记返销
 */
const int UNDOREGISTSIMBACKTAG_ERR = BASECUSTSERV_ERR + 200;

/**
 * 资源预占返销：删除预占表记录。
 */
const int UNDORESENGROSS_ERR = BASECUSTSERV_ERR + 201;

/**
 * 资源占用返销：删除占用表，插入空闲表
 */
const int UNDORESPOSSESS_ERR = BASECUSTSERV_ERR + 202;

/**
 * 资源释放返销
 */
const int UNDORESRELA_ERR = BASECUSTSERV_ERR + 203;

/**
 * IP直通车
 */
const int IPUSERINFODEAL_ERR = BASECUSTSERV_ERR + 204;

/**
 * 查询用户VPMN信息
 */
const int GETUSERVPMNINFO_ERR = BASECUSTSERV_ERR + 205;

/**
 * 新增用户VPMN信息
 */
const int ADDUSERVPMN_ERR = BASECUSTSERV_ERR + 206;

/**
 * 修改用户VPMN信息
 */
const int MODIFYUSERVPMN_ERR = BASECUSTSERV_ERR + 207;

/**
 * 生成计费增量接口
 */
const int GENEBILLINTF_ERR = BASECUSTSERV_ERR + 208;

/**
 * 获取当前产品下的用户服务
 */
const int GETUSERSVCOFPID_ERR = BASECUSTSERV_ERR + 209;

/**
 * 获取当前产品下的用户优惠
 */
const int GETUSERDISCNTOFPID_ERR = BASECUSTSERV_ERR + 210;

/**
 * 通过用户A的用户标识和用户B的服务号码及关系类型获取用户与用户的关系
 */
const int GETMEMBERUSERRELATION_ERR = BASECUSTSERV_ERR + 211;

/**
 * 获取成员用户所有的组合产品下的当前服务
 */
const int GETMEMBERSVCOFPID_ERR = BASECUSTSERV_ERR + 212;

/**
 * 获取成员用户的组合产品下的当前优惠
 */
const int GETMEMBERDISCNTOFPID_ERR = BASECUSTSERV_ERR + 213;

/**
 * 获取当前产品下的用户优惠
 */
const int CALLCENTERENCODESTR_ERR = BASECUSTSERV_ERR + 214;

/**
 * 获取当前产品下的用户服务的资源
 */
const int GETUSERRESOFPID_ERR = BASECUSTSERV_ERR + 215;

/**
 * 获取当前组合产品下的成员用户服务的资源
 */
const int GETMEMBERRESOFPID_ERR = BASECUSTSERV_ERR + 216;

/**
 * 根据关系类型获取角色代码列表
 */
const int GETRELATIONROLE_ERR = BASECUSTSERV_ERR + 217;

/**
 * 获取大客户备卡资料
 */
const int CUSTGETVIPSIMBAK_ERR = BASECUSTSERV_ERR + 218;

/**
 * 校验大客户备卡资料
 */
const int CUSTCHECKVIPSIMBAKINFO_ERR = BASECUSTSERV_ERR + 219;

/**
 * 新增大客户备卡资料
 */
const int CUSTCREATEVIPSIMBAK_ERR = BASECUSTSERV_ERR + 220;

/**
 * 修改大客户备卡资料
 */
const int CUSTMODIFYVIPSIMBAK_ERR = BASECUSTSERV_ERR + 221;

/**
 * 根据员工获取部门编码
 */
const int GETDEPARTIDBYSTAFFID_ERR = BASECUSTSERV_ERR + 222;

/**
 * 根据部门获取归属业务区
 */
const int GETCITYCODEBYDEPARTID_ERR = BASECUSTSERV_ERR + 223;


/**
 * 业务鉴权
 */
const int TRADEAUTH_ERR = BASECUSTSERV_ERR + 224;

/**
 * E动互联
 */
const int CREDENCEREG_ERR = BASECUSTSERV_ERR + 225;

/**
 *  生成过户时的新旧用户信息台帐
 */
const int GENENEWOLDUSERTRADE_ERR = BASECUSTSERV_ERR + 226;

/**
 *  修改用户关系
 */
const int MODIFYUSERRELATION_ERR = BASECUSTSERV_ERR + 227;

/**
 *  增加成员服务
 */
const int ADDMEMBERSERVICE_ERR = BASECUSTSERV_ERR + 228;

/**
 * 修改成员服务
 */
const int MODIFYMEMBERSERVICE_ERR = BASECUSTSERV_ERR + 229;

/**
 * 修改成员服务
 */
const int CHECKFAMILYPHONE_ERR = BASECUSTSERV_ERR + 230;

/**
 * 修改客户状态
 */
const int MODIFYCUSTSTATE_ERR = BASECUSTSERV_ERR + 231;

/**
 * 查询联指工单信息异常
 */
const int GETOLCOMINFOBYSN_ERR = BASECUSTSERV_ERR + 232;

/**
 * 检查用户资源位置异常
 */
const int CHECKRESLOCAT_ERR = BASECUSTSERV_ERR + 233;

/**
 * 检查客户限制开户数
 */
const int CHECKOPENLIMIT_ERR = BASECUSTSERV_ERR + 234;

/**
 * 生成复机台帐服务子表错误
 */
const int GENERESTORETRADESVC_ERR = BASECUSTSERV_ERR + 235;

/**
 * 终止指定帐户下的高级付费关系
 */
const int STOPACCTADVPAYRELA_ERR = BASECUSTSERV_ERR + 236;

/**
 * 备份指定帐户下的高级付费关系
 */
const int BAKACCTADVPAYRELA_ERR = BASECUSTSERV_ERR + 237;

/**
 * 终止指定帐户下的高级付费关系返销
 */
const int UNDOSTOPACCTADVPAYRELA_ERR = BASECUSTSERV_ERR + 238;

/**
 * 将原帐户下地所有用户付费关系搬迁到新帐户返销
 */
const int UNDOMODIFYUSERPAYRELA_ERR = BASECUSTSERV_ERR + 239;

/**
 * 判断是否为独立帐户
 */
const int CHECKACCTONLY_ERR = BASECUSTSERV_ERR + 240;

/**
 * 信用控制业务登记错误
 */
const int CREDITTRADEREG_ERR = BASECUSTSERV_ERR + 241;

/**
 * 个人代扣业务错误
 */
const int GETSERVDEDUCT_ERR = BASECUSTSERV_ERR + 242;

/**
 *查询语音清单错误
 */
const int GETBILLVOICE_ERR = BASECUSTSERV_ERR + 243;

/**
 *查询短信清单错误
 */
const int GETBILLSMS_ERR = BASECUSTSERV_ERR + 244;

/**
 *查询梦网清单错误
 */
const int GETBILLMONTERNET_ERR = BASECUSTSERV_ERR + 245;

/**
 *查询GPRS清单错误
 */
const int GETBILLGPRS_ERR = BASECUSTSERV_ERR + 246;

/**
 *查询神州行语音清单错误
 */
const int GETBILLSZXVOICE_ERR = BASECUSTSERV_ERR + 247;

/**
 *查询神州行短信清单错误
 */
const int GETBILLSZXSMS_ERR = BASECUSTSERV_ERR + 248;

/**
 *查询神州行梦网清单错误
 */
const int GETBILLSZXMONTERNET_ERR = BASECUSTSERV_ERR + 249;

/**
 *查询IP直通车清单错误
 */
const int GETBILLIPBIND_ERR = BASECUSTSERV_ERR + 250;

/**
 *查询会议电话清单错误
 */
const int GETBILLMEETPHONE_ERR = BASECUSTSERV_ERR + 251;

/**
 *查询百宝箱清单错误
 */
const int GETBILLKJAVA_ERR = BASECUSTSERV_ERR + 252;

/**
 *查询语音杂志清单错误
 */
const int GETBILLVOICEJ_ERR = BASECUSTSERV_ERR + 253;

/**
 *查询WAP清单错误
 */
const int GETBILLWAP_ERR = BASECUSTSERV_ERR + 254;

/**
 *查询WLAN清单错误
 */
const int GETBILLWLAN_ERR = BASECUSTSERV_ERR + 255;

/**
 *查询彩铃清单错误
 */
const int GETBILLCRBT_ERR = BASECUSTSERV_ERR + 256;

/**
 *查询彩信清单错误
 */
const int GETBILLMMS_ERR = BASECUSTSERV_ERR + 257;

/**
 * 根据业务流水号查询联指工单信息错误
 */
const int GETOLCOMINFOBYTRADE_ERR = BASECUSTSERV_ERR + 258;

/**
 * 获取产品下可以预约开通的服务信息
 */
const int GETPRECHANGESVCINFO_ERR = BASECUSTSERV_ERR + 259;

/**
 * 获取用户购机资料
 */
const int GETUSERPRUCHASE_ERR = BASECUSTSERV_ERR + 260;

/**
 * 获取用户有效期
 */
const int GETUSERVALIDDATE_ERR = BASECUSTSERV_ERR + 261;

/**
 * 取消用户购机
 */
const int REMOVEUSERPARCHASE_ERR = BASECUSTSERV_ERR + 262;

/**
 * 业务特殊限制总控
 */
const int CHECKTRADEEPECLIMIT_ERR = BASECUSTSERV_ERR + 263;

/**
 * 销户终止用户所有资料
 */
const int ENDUSERALLINFO_ERR = BASECUSTSERV_ERR + 264;

/**
 * 用户购机资料返销
 */
const int UNDOUSERPURCHASE_ERR = BASECUSTSERV_ERR + 265;

/**
 * 系统组织信息查询
 */
const int GETSYSORGINFO_ERR = BASECUSTSERV_ERR + 266;

/**
 * 获取用户其它服务资料
 */
const int GETUSEROTHERSERV_ERR = BASECUSTSERV_ERR + 267;

/**
 * 增加用户其它服务资料
 */
const int ADDUSEROTHERSERV_ERR = BASECUSTSERV_ERR + 268;

/**
 * 修改用户其它服务资料
 */
const int MODIFYUSEROTHERSERV_ERR = BASECUSTSERV_ERR + 269;

/**
 * 新增特殊费用信息记录
 */
const int CREATEFIXEDFEESPECINFO_ERR = BASECUSTSERV_ERR + 270;

/**
 * 修改特殊费用信息记录
 */
const int MODIFYFIXEDFEESPECINFO_ERR = BASECUSTSERV_ERR + 271;

/**
 * 删除特殊费用信息记录
 */
const int DELETEFIXEDFEESPECINFO_ERR = BASECUSTSERV_ERR + 272;

/**
 * 修改用户其他资料表信息
 */
const int MODIFYUSEROTHER_ERR = BASECUSTSERV_ERR + 273;

/**
 * 指令绑定服务处理
 */
const int DEALOLCOMBINDSVC_ERR = BASECUSTSERV_ERR + 274;

/**
 * 获取用户其他资料表资料
 */
const int GETUSEROTHER_ERR = BASECUSTSERV_ERR + 275;

/**
 * 新增用户特殊优惠资料表资料
 */
const int ADDUSERSPECIFICDISCNT_ERR = BASECUSTSERV_ERR + 276;

/**
 * 修改用户特殊优惠资料表资料
 */
const int MODIFYUSERSPECIFICDISCNT_ERR = BASECUSTSERV_ERR + 277;

/**
 * 修改用户资料表冗余字段
 */
const int MODIFYUSERRSRVSTR_ERR = BASECUSTSERV_ERR + 278;

/**
 * 修改用户资料表冗余字段(返销)
 */
const int UNDOMODIFYUSERRSRVSTR_ERR = BASECUSTSERV_ERR + 279;

/**
 * 租机用户资料处理
 */
const int DEALRENTMOBILE_ERR = BASECUSTSERV_ERR + 280;

/**
 * 手机支付插入台帐资料
 */
const int GENENOTECASETRADEINFO_ERR = BASECUSTSERV_ERR + 281;

/**
 * 手机支付插入台帐资料
 */
const int MODIFYBANKBAND_ERR = BASECUSTSERV_ERR + 282;

/**
 * 复制用户与用户关系资料
 */
const int COPYUSERRELATION_ERR = BASECUSTSERV_ERR + 283;

/**
 * 停止用户高级付费关系
 */
const int STOPUSERADVPAYRELA_ERR = BASECUSTSERV_ERR + 284;

/**
 * 修改用户高级付费关系
 */
const int MODIFYUSERADVPAYRELA_ERR = BASECUSTSERV_ERR + 285;

/**
 * 根据业务类型生成业务服务开通指令
 */
const int GENETRADESVCOLOCM_ERR = BASECUSTSERV_ERR + 286;

/**
 * 通过成员查询集团信息
 */
const int GETGRPINFOBYMEMBER_ERR = BASECUSTSERV_ERR + 287;

/**
 * 激活大客户备卡(返销)
 */
const int UNDOCUSTMODIFYVIPSIMBAK_ERR = BASECUSTSERV_ERR + 288;

/**
 * 更新用户关联优惠
 */
const int DESTROYRELAINFO_ERR = BASECUSTSERV_ERR + 289;

/**
 * 获取优惠参数
 */
const int GETDISCNT_ERR = BASECUSTSERV_ERR + 290;

/**
 * 租机号码处理
 */
const int DEALRENTNO_ERR = BASECUSTSERV_ERR + 291;

/**
 * 信用度计算接口调用
 */
const int CALCCREDIT_ERR = BASECUSTSERV_ERR + 292;

/**
 * 用户资料不存在
 */
const int USERNOTEXISTS_ERR = BASECUSTSERV_ERR + 293;

/**
 * 客户资料不存在
 */
const int CUSTNOTEXISTS_ERR = BASECUSTSERV_ERR + 294;

/**
 * 黑名单限制
 */
const int CS_BLACKUSER_ERR = BASECUSTSERV_ERR + 295;

/**
 * 欠费限制
 */
const int CS_OWEFEE_ERR = BASECUSTSERV_ERR + 296;

/**
 * 用户状态限制
 */
const int CS_USERSTATE_ERR = BASECUSTSERV_ERR + 297;

/**
 * 用户业务限制
 */
const int CS_TRADELIMIT_ERR = BASECUSTSERV_ERR + 298;

/**
 * 异地业务限制
 */
const int CS_OTHERAREATRADE_ERR = BASECUSTSERV_ERR + 299;

/**
 * 非大客户
 */
const int CS_NOTVIP_ERR = BASECUSTSERV_ERR + 300;

/**
 * 无备卡
 */
const int CS_NOSIMBAK_ERR = BASECUSTSERV_ERR + 301;

/**
 * 根据集团用户的ID查询所有成员用户ID
 */
const int GETMEMBERUSERBYGROUPUSER_ERR = BASECUSTSERV_ERR + 302;

/**
 * 营业发送短信通知失败
 */
const int CS_SENDSMS_ERR = BASECUSTSERV_ERR + 303;

/**
 * 获取用户业务平台业务资料失败
 */
const int CS_GETUSERMBMPINFO_ERR = BASECUSTSERV_ERR + 304;

/**
 * 业务平台业务申请,用户已经申请过
 */
const int CS_USERMBMPINFO_BIZNOW_ERR = BASECUSTSERV_ERR + 305;

/**
 * 业务平台业务申请,用户相应服务判断
 */
const int CS_USERSEPCSVCINFO_ERR = BASECUSTSERV_ERR + 306;

/**
 * 神州行用户
 */
const int CS_ISGS01USER_ERR = BASECUSTSERV_ERR + 307;

/**
 * 业务平台久密码错误
 */
const int CS_CHECKOLDPASSWD_ERR = BASECUSTSERV_ERR + 308;

/**
 * 业务平台业务删除,用户无相应申请资料
 */
const int CS_USERMBMPINFO_BIZNOWNO_ERR = BASECUSTSERV_ERR + 309;

/**
 * 获取用户业务平台业务参数资料失败
 */
const int CS_GETUSERMBMPPLUSINFO_ERR = BASECUSTSERV_ERR + 310;

/**
 * 国际判断
 */
const int CS_CHECKWORDTRADE_02_ERR = BASECUSTSERV_ERR + 311;

/**
 * SP厂商校验
 */
const int CS_CHECKSPFACTORY_ERR = BASECUSTSERV_ERR + 312;

/**
 * SP服务校验
 */
const int CS_CHECKSPSERVICE_ERR = BASECUSTSERV_ERR + 313;

/**
 * 业务平台业务编码判断
 */
const int CS_BIZTYPECODECHECK_ERR = BASECUSTSERV_ERR + 314;

/**
 * 业务平台操作编码判断
 */
const int CS_OPERTYPECODECHECK_ERR = BASECUSTSERV_ERR + 315;

/**
 * VIP机场服务,服务级别判断
 */
const int CS_SERVERLEVER_ERR = BASECUSTSERV_ERR + 316;

/**
 * 多个备卡
 */
const int CS_MANYSIMBAK_ERR = BASECUSTSERV_ERR + 317;

/**
 * 无对应备卡
 */
const int CS_SIMBAKEXISTS_ERR = BASECUSTSERV_ERR + 318;

/**
 * 国内判断
 */
const int CS_CHECKWORDTRADE_01_ERR = BASECUSTSERV_ERR + 319;

/**
 * 品牌编码转换
 */
const int BRANDCODECONVER_ERR = BASECUSTSERV_ERR + 320;

/**
 * 获取通用参数
 */
const int GETPARAMINFO_ERR = BASECUSTSERV_ERR + 321;

/**
 * 异动互联初始化
 */
const int CREDENCEINITTRADE_ERR = BASECUSTSERV_ERR + 322;

/**
 * 异动互联业务前判断
 */
const int CHECKBEFORECREDENCE_ERR = BASECUSTSERV_ERR + 323;

/**
 * 异动互联信息获取判断
 */
const int GETCREDENCE_ERR = BASECUSTSERV_ERR + 324;

/**
 * 获取BOSS信息参数
 */
const int GETBOSSCODE_ERR = BASECUSTSERV_ERR + 325;

/**
 * 获取优惠包对应优惠信息
 */
const int GETOPERSETTODISCNT_ERR = BASECUSTSERV_ERR + 326;

/**
 * 拼串错误
 */
const int GENECODESTR_ERR = BASECUSTSERV_ERR + 327;

/**
 * 服务拼串错误
 */
const int GENESERVICESTR_ERR = BASECUSTSERV_ERR + 328;

/**
 * 优惠拼串错误
 */
const int GENEDISCNTSTR_ERR = BASECUSTSERV_ERR + 329;

/**
 * 增加月份计算时间错误
 */
const int GETADDMONTHSDATE_ERR = BASECUSTSERV_ERR + 330;

/**
 * 新增异动互联信息
 */
const int ADDCREDENCE_ERR = BASECUSTSERV_ERR + 331;

/**
 * 修改异动互联状态
 */
const int MODICREDENCESTATE_ERR = BASECUSTSERV_ERR + 332;

/**
 * 新增异动信息返销
 */
const int UNDOADDCREDENCE_ERR = BASECUSTSERV_ERR + 333;

/**
 *检查是否免身份证判断
 */
const int JUDGEDERATEIDENTITYCHECK_ERR = BASECUSTSERV_ERR + 334;

/**
 *记录成功的身份验证
 */
const int RECORDIDENTITYCHECK_ERR = BASECUSTSERV_ERR + 335;

/**
 *通用指令填写
 */
const int CREATECOMNOLCOM_ERR = BASECUSTSERV_ERR + 336;

/**
 *IP信息
 */
const int GETIPINFO_ERR = BASECUSTSERV_ERR + 337;

/**
 *神州行短信发送
 */
const int SMSCHINAGO_ERR = BASECUSTSERV_ERR + 338;

/**
 *  发送业务短信通知
 */
const int SENDTRADESMS_ERR = BASECUSTSERV_ERR + 339;

/**
 *  智能网关通知
 */
const int USERINGWINFO_ERR = BASECUSTSERV_ERR + 340;

/**
 *  智能网关通知
 */
const int INTFMUSICRING_ERR = BASECUSTSERV_ERR + 341;

/**
 *  处理用户关联关系
 */
const int DEALRELATIONINFO_ERR = BASECUSTSERV_ERR + 342;

/**
 *  守护天使其他资料处理
 */
const int DEALOTHER4ANGEL_ERR = BASECUSTSERV_ERR + 343;

/**
 *  获取传入时间与系统时间差值
 */
const int GETODDSDAYS_ERR = BASECUSTSERV_ERR + 344;

/**
 *  合并集团
 */
const int UNIONGROUPUSER_ERR = BASECUSTSERV_ERR + 345;

/**
 *  查找用户随e行捆绑资料
 */
const int GETUSERECARDBINDINFO_ERR = BASECUSTSERV_ERR + 346;

/**
 *  回退业务台帐资料
 */
const int ROLLBACKTRADEINFO_ERR = BASECUSTSERV_ERR + 347;

/**
 *  台帐关系资料表特殊处理
 */
const int TRADERELASPECDEAL_ERR = BASECUSTSERV_ERR + 348;

/**
 *  登记绑定优惠台帐
 */
const int BINDDISCNTTRADE_ERR = BASECUSTSERV_ERR + 349;

/**
 * 获取SP厂商参数
 */
const int CS_GETSPFACTORY_ERR = BASECUSTSERV_ERR + 350;

/**
 * 获取SP服务参数
 */
const int CS_GETSPSERVICE_ERR = BASECUSTSERV_ERR + 351;

/**
 * 获取SP服务参数
 */
const int CS_GETGRPHISTRADEQUERY_ERR = BASECUSTSERV_ERR + 352;

/**
 * 获取VPMN集团网外号码信息
 */
const int GETVPMNGRPOUTBYVPMN_ERR = BASECUSTSERV_ERR + 353;

/**
 * 获取VPMN集团成员网外号码信息
 */
const int GETVPMNMENBEROUTBYVPMN_ERR = BASECUSTSERV_ERR + 354;

/**
 * 修改VPMN网外号码信息
 */
const int MODIFYVPMNGRPOUT_ERR = BASECUSTSERV_ERR + 355;

/**
 *
 */
const int REDEALBATTRADE_ERR = BASECUSTSERV_ERR + 356;

/**
 * 获取VPMN客户欠费信息
 */
const int GETVPMNOWE_ERR = BASECUSTSERV_ERR + 357;

/**
 * 获取VPMN客户欠费信息
 */
const int CS_JUDGEACCOUNTTRADELIMIT_ERR = BASECUSTSERV_ERR + 358;

/**
 * 获取VPN集团业务信息 add lif 20060301
 */
const int CS_GETVPNGROUPTRADEQUERY_ERR = BASECUSTSERV_ERR + 359;

/**
 * 获取批量免填单信息
 */
const int GetTradeinfoTrade_ERR = BASECUSTSERV_ERR + 360;

/**
 * 密码卡手机号码无效
 */
const int CS_InvalidSN_ERR = BASECUSTSERV_ERR + 361;

/**
 * 密码卡手机号码已开户
 */
const int CS_SNHasOpen_ERR = BASECUSTSERV_ERR + 362;

/**
 * 密码卡密码无效
 */
const int CS_WrongPasswd_ERR = BASECUSTSERV_ERR + 363;

/**
 * 密码卡开户失败
 */
const int CS_PCardOpen_ERR = BASECUSTSERV_ERR + 364;

/**
 * 购机参数获取
 */
const int GETUSERPURCHASEMODE_ERR = BASECUSTSERV_ERR + 364;

/**
 * 获取用户新积分
 */
const int GETUSERNEWSCORE = BASECUSTSERV_ERR + 365;

/**
 * 用户购机业务优惠处理
 */
const int DEALPURCHASEDISCNT_ERR = BASECUSTSERV_ERR + 366;

/**
 * 获取跨省集团业务台帐主表
 */
const int GETSPANTRADEINFO_ERR = BASECUSTSERV_ERR + 367;

/**
 * 获取跨省集团业务台帐明细表
 */
const int GETSPANTRADEPLUS_ERR = BASECUSTSERV_ERR + 368;

/**
 * 获取跨省集团业务产品子表
 */
const int GETSPANTRADESUBINFO_ERR = BASECUSTSERV_ERR + 369;

/**
 * 获取跨省集团业务子表明细
 */
const int GETSPANTRADEDETAILINFO_ERR = BASECUSTSERV_ERR + 370;

/**
 * 终止跨省集团记录
 */
const int STOPSPANTRADE_ERR = BASECUSTSERV_ERR + 371;

/**
 * 修改跨省集团产品记录
 */
const int MODIFYSPANTRADESUB_ERR = BASECUSTSERV_ERR + 372;

/**
 * 终止跨省集团产品明细
 */
const int DELSPANTRADEDETAIL_ERR = BASECUSTSERV_ERR + 373;

/**
 * 终止跨省集团产品子表
 */
const int DELSPANTRADESUB_ERR = BASECUSTSERV_ERR + 374;

/**
 * 终止跨省集团产品明细表
 */
const int DELSPANTRADEPLUS_ERR = BASECUSTSERV_ERR + 375;

/**
 * 终止跨省集团主表
 */
const int DELSPANTRADE_ERR = BASECUSTSERV_ERR + 376;

/**
 * 业务未完工
 */
const int TRADENOTCOMPLETED_ERR = BASECUSTSERV_ERR + 377;

/**
 * 根据标识查询抽奖参数出错
 */
const int GETTAFFLEPARA_PROJECT_ERR = BASECUSTSERV_ERR + 378;

/**
 * 根据时间查询抽奖参数出错
 */
const int GETTAFFLEPARA_JOINTIME_ERR = BASECUSTSERV_ERR + 379;

/**
 * 根据用户抽奖信息规则
 */
const int GETTAFFLEINFO_USER_ERR = BASECUSTSERV_ERR + 380;

/**
 * 根据流水查询抽奖信息出错
 */
const int GETTAFFLEINFO_ID_ERR = BASECUSTSERV_ERR + 381;

/**
 * 生成个人客户信息失败
 */
const int CREATECUSTPERSONINFO_ERR = BASECUSTSERV_ERR + 382;

/**
 * 生成账户托收信息失败
 */
const int CREATEACCTCONSIGNINFO_ERR = BASECUSTSERV_ERR + 383;

/**
 * 生成担保用户资料失败
 */
const int CREATEUSERASSUREINFO_ERR = BASECUSTSERV_ERR + 384;

/**
 * 参数分派报错
 */
const int TASKITEMINFO_ERR = BASECUSTSERV_ERR + 385;

/**
 * 工单稽核异常 
 */
const int TRADEAUDIT_ERR = BASECUSTSERV_ERR + 386;

/**
 * 根据手机号码查询产品信息（接口）
 */
const int QUERYPRODUCT_INFO_ERR = BASECUSTSERV_ERR + 387;

/**
 * 根据手机号码查询可用产品信息（接口）
 */
const int QUERYPRODUCT_DO_ERR = BASECUSTSERV_ERR + 388;

/**
 * 根据手机号码查询简单可用产品信息（接口）
 */
const int QUERYPRODUCT_SINPLEDO_ERR = BASECUSTSERV_ERR + 389;

/**
 * 根据手机号码查询简单产品信息（接口）
 */
const int QUERYPRODUCT_SINPLE_ERR = BASECUSTSERV_ERR + 390;

/**
 * 匹配属性参数
 */
const int MATCHINGITEMPARA_ERR = BASECUSTSERV_ERR + 391;

/**
 * 根据主卡服务号码获取关系成员主服务及服务状态
 */
const int GETMEMBERMAINSVCBYSN_ERR = BASECUSTSERV_ERR + 392;

/**
 * 获取元素明细
 */
const int GETELEMENTDETAIL_ERR = BASECUSTSERV_ERR + 393;

/**
 * 生成指令定制接口信息
 */
const int GENEOLCOMORDER_ERR = BASECUSTSERV_ERR + 394;

/**
 * 生成SP指令
 */
const int GENESPOLCOM_ERR = BASECUSTSERV_ERR + 395;

/**
 * 修改交换机资料
 */
const int UPDOLCOMSWITCH = BASECUSTSERV_ERR + 396;

/**
 * 生成同步指令交换机编码
 */
const int GENEOLCOMORDERTB_ERR = BASECUSTSERV_ERR + 397;

/**
 * 获取ITEM编码
 */
const int FETCHITEMVALUE_ERR = BASECUSTSERV_ERR + 398;

/**
 * 生成优惠指令
 */
const int GENEDISCNTOLCOM_ERR = BASECUSTSERV_ERR + 399;

/**
 * 生成优惠指令
 */
const int GETSCPCODE_ERR = BASECUSTSERV_ERR + 400;

/**
 * 获取SP绑定参数
 */
const int GETSPBINDPARA_ERR = BASECUSTSERV_ERR + 401;

/**
 * 插入相应参数
 */
const int INSOLCOMVAR_ERR = BASECUSTSERV_ERR + 402;

/**
 * 查询台帐所有关系
 */
const int GETTRADEALLRELA_ERR = BASECUSTSERV_ERR + 403;

/*
 * 查询方式异常
 */
const int GETGETMODE_ERR = BASECUSTSERV_ERR + 404;

/*
 * 查询Scp特殊资费编码
 */
const int GETSCPSEPDISCNT_ERR = BASECUSTSERV_ERR + 405;

/*
 * 信誉度修改服务
 */
const int CREDITCHANGESVC_ERR = BASECUSTSERV_ERR + 406;

/*
 * Q币鉴权失败
 */
const int QQPERIMITION_ERR = BASECUSTSERV_ERR + 407;

/*
 * 用户积分值为零
 */
const int GETUSERSCOREZERO_ERR = BASECUSTSERV_ERR + 408;

/*
 * 用户积分值不足
 */
const int GETUSERSCORELIMIT_ERR = BASECUSTSERV_ERR + 409;

/*
 * 非积分用户
 */
const int GETNONSCOREUSER_ERR = BASECUSTSERV_ERR + 410;

/*
 * 积分未到首次兑换的门限
 */
const int GETSCORENONLIMIT_ERR = BASECUSTSERV_ERR + 411;

/*
 * SP重复订购错误
 */
const int SP_REPEATE_ORDER_ERR = BASECUSTSERV_ERR + 412;

/*
 * SP退订不存在的订购关系错误
 */
const int SP_CANCEL_NOTEXIST_ERR = BASECUSTSERV_ERR + 413;


/*
 * 获取基本产品列表无记录   VIP平台错误码规定5位
 */
const int AGENT_GET_PRODUCT_ERR = 30416;

/*
 * 获取必选包无记录 VIP平台错误码规定5位
 */
const int AGENT_GET_MPACKAGE_ERR = 30417;

/*
 * 获取基本产品列表无权限  VIP平台错误码规定5位
 */
const int AGENT_GET_PRODUCT_RIGHT_ERR = 30418;

/*
 * SP订购存在互斥依赖关系错误
 */
const int SP_MUTEX_ERR = BASECUSTSERV_ERR + 414;

/*
 * SP退订存在互斥依赖限制错误
 */
const int SP_DEPEND_ON_ERR = BASECUSTSERV_ERR + 415;

/*
 * SP退订存在包限制错误
 */
const int SP_PACKAGE_LIMIT_ERR = BASECUSTSERV_ERR + 416;

const int INTFGENETRADE_ERR = BASECUSTSERV_ERR + 417;

const int INTFGENETRADEUSR_ERR = BASECUSTSERV_ERR + 418;

const int INTFGENETRADESVC_ERR = BASECUSTSERV_ERR + 419;

const int INTFGENETRADESVCSTATE_ERR = BASECUSTSERV_ERR + 420;

const int INTFGENETRADEDISCNT_ERR = BASECUSTSERV_ERR + 421;

const int INTFMOFIDFYTRADEDISCNT_ERR = BASECUSTSERV_ERR + 422;

const int INTFGENETRADEPAYRELA_ERR = BASECUSTSERV_ERR + 423;

const int INTFGENETRADEACCT_ERR = BASECUSTSERV_ERR + 424;

const int INTFGENETRADEPROD_ERR = BASECUSTSERV_ERR + 425;

const int INTFGENETRADERELA_ERR = BASECUSTSERV_ERR + 426;

/*
 * 主产品已存在，不允许再订购错误
 */
const int MAIN_PRODUCT_EXSITS_ERR = BASECUSTSERV_ERR + 427;

/*
 * 主产品不存在，取消订购错误
 */
const int MAIN_PRODUCT_NOTEXSITS_ERR = BASECUSTSERV_ERR + 428;

/*
 * 用户已变更主产品，需取消变更再订购错误
 */
const int HAD_CHANGE_MAIN_PRODUCT_ERR = BASECUSTSERV_ERR + 429;

/*
 * 用户仅有该主产品，不能取消
 */
const int IS_THE_ONLY_MAIN_PRODUCT_ERR = BASECUSTSERV_ERR + 430;

/*
 * 不允许通过接口对产品元素做特定操作错误
 */
const int INTF_OPRE_ELEMENT_LIMIT = BASECUSTSERV_ERR + 431;

/*
 * 客户积分查询
 */
const int GETCUSTSCORE_ERR = BASECUSTSERV_ERR + 432;

/*
 * 插入客户积分
 */
const int INSCUSTSCORE_ERR = BASECUSTSERV_ERR + 433;

/*
 * 修改客户积分
 */
const int UPDCUSTSCORE_ERR = BASECUSTSERV_ERR + 434; 

/*
 * 客户合并
 */
const int GENEUNIONPSPTID_ERR = BASECUSTSERV_ERR + 435; 

/*
 * 靓号未绑定规定产品
 */
const int NUMBERRULEBINDPRODUCT_ERR = BASECUSTSERV_ERR + 436;

/*
 * 检查普通号码3G基本产品转兑包配置出错
 */
const int NORMALNUMBERBINDPRODUCT_ERR = BASECUSTSERV_ERR + 437;
/*
 * ESS效验错误
 */
const int ESSCHECKMODE_ERR = BASECUSTSERV_ERR + 438;

/*
 * ESS效验错误(帐务)
 */
const int ESSACCTCHECK_ERR = BASECUSTSERV_ERR + 439;

/*
 * ESS效验错误
 */
const int ESSCHECKMODE_1_ERR = BASECUSTSERV_ERR + 440;
const int INSERTREMOTECARD_ERR = BASECUSTSERV_ERR + 441;
const int CHANGEREMOTECARD_ERR = BASECUSTSERV_ERR + 442;

/*
 * 预付费开户不允许匿名 
 */
const int ANONYMOUS_OPEN_ACCOUNT_ERR = BASECUSTSERV_ERR + 444;

/*
 * 客户资料已返档 
 */
const int HAD_RETURN_CUSTINFO = BASECUSTSERV_ERR + 445;

/*
 * 查询代理商信息异常
 */
const int QY_AGENT_ERR = BASECUSTSERV_ERR + 446;

/*
 * 代理商帐户费用操作异常
 */
const int ACTION_AGENTACCT_ERR = BASECUSTSERV_ERR + 447;

#endif
