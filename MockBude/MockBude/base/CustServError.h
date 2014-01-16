/**
 * �ͻ�����������
 */

#ifndef __CUSTSERVERROR_H_
#define __CUSTSERVERROR_H_

//#include "stdafx.h"


const int BASECUSTSERV_ERR = 300000;
/**
 * ������ˮ���쳣
 */
const int GENERATEID_ERR = BASECUSTSERV_ERR + 1;

/**
 * ��ȡ��ͨ�����쳣
 */
const int GETNORMALPARA_ERR = BASECUSTSERV_ERR + 2;

/**
 * ��ȡͨ�ò����쳣
 */
const int GETCOMMPARA_ERR = BASECUSTSERV_ERR + 3;

/**
 * ��ȡϵͳʱ���쳣
 */
const int GETSYSDATE_ERR = BASECUSTSERV_ERR + 4;

/**
 * ��ȡҵ�����Ͳ����쳣
 */
const int GETTRADETYPEPARAM_ERR = BASECUSTSERV_ERR + 5;

/**
 * ��ԴУ�飺����������У�顢Ȩ��У���
 */
const int CHECKRESOURCE_ERR = BASECUSTSERV_ERR + 11;

/**
 * ��Դѡռ
 */
const int RESOURCEOCCUPY_ERR = BASECUSTSERV_ERR + 12;

/**
 * ������Դռ��״̬������ռ����ʹ�õ���Դ���ͷŲ���ʹ�õ���Դ
 */
const int MODIFYRESSTATE_ERR = BASECUSTSERV_ERR + 13;

/**
 * ��ԴԤռ������Դ��ѡռ״̬��ΪԤռ״̬�����ǰ��У�����Դ�����Ƿ���Ϊѡռ״̬��
 */
const int RESENGROSS_ERR = BASECUSTSERV_ERR + 14;

/**
 * �ͷ�Ա��ѡռ��Դ
 */
const int RELESTAFFOCCUPYRES_ERR = BASECUSTSERV_ERR + 15;

/**
 * ���÷�������Ƿ�����ڶ��ο�����������Ӧ��SIM����SIM������������ϸ�����м�¼�ҷ�����־Ϊ
 */
const int CANREOPENMPCODE_ERR = BASECUSTSERV_ERR + 16;

/**
 * �Ǽ�SIM������������ϸ���еķ�����־RegistSimBackTag
 */
const int REGISTSIMBACKTAG_ERR = BASECUSTSERV_ERR + 17;

/**
 * ��ȡ�ͻ�����
 */
const int GETCUSTINFO_ERR = BASECUSTSERV_ERR + 18;

/**
 * �ж��Ƿ�Ϊ����������
 */
const int ISBLACKUSER_ERR = BASECUSTSERV_ERR + 19;

/**
 * �Ƿ������¿��û�
 */
const int ISCREATUSERLIMIT_ERR = BASECUSTSERV_ERR + 20;

/**
 * �����¿ͻ��쳣
 */
const int CREATECUSTINFO_ERR = BASECUSTSERV_ERR + 21;

/**
 * �޸Ŀͻ������쳣
 */
const int UPDATECUSTINFO_ERR = BASECUSTSERV_ERR + 22;

/**
 * ��ȡ������Ϣ�쳣
 */
const int GETTRADEINFO_ERR = BASECUSTSERV_ERR + 23;

/**
 * �������ʻ��쳣
 */
const int CREATEACCTINFO_ERR = BASECUSTSERV_ERR + 24;

/**
 * �޸��ʻ������쳣
 */
const int MODIFYACCTINFO_ERR = BASECUSTSERV_ERR + 25;

/**
 * ��ȡ�ʻ������쳣
 */
const int GETACCTINFO_ERR = BASECUSTSERV_ERR + 26;

/**
 * ����ҵ��̨���쳣
 */
const int GENETRADE_ERR = BASECUSTSERV_ERR + 27;

/**
 * ���ɱ���̨���쳣
 */
const int GENEBACKUPTRADE_ERR = BASECUSTSERV_ERR + 28;

/**
 * ���ɷ���̨���쳣
 */
const int GENETRADEFEE_ERR = BASECUSTSERV_ERR + 29;

/**
 * ���ɷ�����̨���쳣
 */
const int GENETRADESVC_ERR = BASECUSTSERV_ERR + 30;

/**
 * ���ɷ���״̬��̨���쳣
 */
const int GENETRADESVCSTATE_ERR = BASECUSTSERV_ERR + 31;

/**
 * �����Ż���̨���쳣
 */
const int GENETRADEDISCNT_ERR = BASECUSTSERV_ERR + 32;

/**
 * ������Դ��̨���쳣
 */
const int GENETRADERES_ERR = BASECUSTSERV_ERR + 33;

/**
 * ���ɸ��ѹ�ϵ��̨���쳣
 */
const int GENETRADEPAYRELA_ERR = BASECUSTSERV_ERR + 34;

/**
 * �����ʼ���̨���쳣
 */
const int GENETRADEPOST_ERR = BASECUSTSERV_ERR + 35;

/**
 * ̨�ʷ��û����쳣
 */
const int AUDITTRADEFEE_ERR = BASECUSTSERV_ERR + 36;

/**
 * ����������ӡ�����쳣
 */
const int GENETRADERECEIPTINFO_ERR = BASECUSTSERV_ERR + 37;

/**
 * �ж��Ƿ���Ҫ��ָ��ñ�־�쳣
 */
const int ISNEEDOLCOM_ERR = BASECUSTSERV_ERR + 38;

/**
 * ��Ǩҵ��̨���쳣
 */
const int MOVETRADE_ERR = BASECUSTSERV_ERR + 39;

/**
 * ҵ��Ǽ�ǰ�����ж��쳣
 */
const int CHECKBEFOREREG_ERR = BASECUSTSERV_ERR + 40;

/**
 * ҵ��ǼǺ������ж��쳣
 */
const int CHECKAFTERTRADE_ERR = BASECUSTSERV_ERR + 41;

/**
 * ҵ������ǰ�����ж��쳣
 */
const int CHECKBEFORETRADE_ERR = BASECUSTSERV_ERR + 42;

/**
 * ����Ƿ������
 */
const int ISBLACKINFO_ERR = BASECUSTSERV_ERR + 43;

/**
 * �Ƿ������¿��û�
 */
const int ISCREATEUSERLIMIT_ERR = BASECUSTSERV_ERR + 44;

/**
 * ������ָ����
 */
const int GENEOLCOM_ERR = BASECUSTSERV_ERR + 45;

/**
 * ����ҵ��̶�ָ��
 */
const int GENETRADEOLCOMSERV_ERR = BASECUSTSERV_ERR + 45;

/**
 * ���ɷ���ָͨ��
 */
const int GENESVCACTIVESERV_ERR = BASECUSTSERV_ERR + 46;

/**
 * ��д��ָ�������
 */
const int FILLINOLCOMPARA_ERR = BASECUSTSERV_ERR + 47;

/**
 * ��Ǩ��ָ����
 */
const int MOVEOLCOM_ERR = BASECUSTSERV_ERR + 48;

/**
 * ��ȡ��ָ����״̬
 */
const int GETOLCOMSTATE_ERR = BASECUSTSERV_ERR + 49;

//add by liufei for UserMgr
/**
 * �����û�
 */
const int CREATEUSERINFO_ERR = BASECUSTSERV_ERR + 50;

/**
 * �޸��û�����
 */
const int MODIFYUSERINFO_ERR = BASECUSTSERV_ERR + 51;

/**
 * ��ȡ�û����û���ϵ��Ϣ
 */
const int GETUSERRELATION_ERR = BASECUSTSERV_ERR + 52;

/**
 * �����û����û���ϵ��Ϣ
 */
const int ADDUSERRELATION_ERR = BASECUSTSERV_ERR + 53;

/**
 * �����û��Ż�
 */
const int ADDUSERDISCNT_ERR = BASECUSTSERV_ERR + 54;

/**
 * �޸��û��Ż�
 */
const int MODIFYUSERDISCNT_ERR = BASECUSTSERV_ERR + 55;

/**
 * �����û�����
 */
const int ADDUSERSERVICE_ERR = BASECUSTSERV_ERR + 56;

/**
 * �޸��û�����
 */
const int MODIFYUSERSERVICE_ERR = BASECUSTSERV_ERR + 57;

/**
 * �����û�����״̬
 */
const int ADDUSERSVCSTATE_ERR = BASECUSTSERV_ERR + 58;

/**
 * �޸��û�����״̬
 */
const int MODIFYUSERSVCSTATE_ERR = BASECUSTSERV_ERR + 59;

/**
 * �����û�ռ����Դ
 */
const int ADDUSERRES_ERR = BASECUSTSERV_ERR + 60;

/**
 * �޸��û�ռ����Դ
 */
const int MODIFYUSERRES_ERR = BASECUSTSERV_ERR + 61;

/**
 * �����û����ѹ�ϵ
 */
const int ADDUSERPAYRELA_ERR = BASECUSTSERV_ERR + 62;

/**
 * �޸��û����ѹ�ϵ
 */
const int MODIFYUSERPAYRELA_ERR = BASECUSTSERV_ERR + 63;

/**
 * �޸��û�Ѻ��
 */
const int MODIFYUSERFOREGIFT_ERR = MODIFYUSERFOREGIFT_ERR + 64;

/**
 * �����û�Ѻ��
 */
const int ADDUSERFOREGIFT_ERR = BASECUSTSERV_ERR + 65;

/**
 * �����û����ϱ��
 */
const int ADDUSERINFOCHG_ERR = BASECUSTSERV_ERR + 66;

/**
 * �޸��û����ϱ��
 */
const int MODIFYUSERINFOCHG_ERR = BASECUSTSERV_ERR + 67;

/**
 * �����û�ҵ������
 */
const int ADDUSERTRADELIMIT_ERR = BASECUSTSERV_ERR + 68;

/**
 * �޸��û�ҵ������
 */
const int MODIFYUSERTRADELIMIT_ERR = BASECUSTSERV_ERR + 69;

/**
 * ��ȡ�û���Ϣ
 */
const int GETUSERINFO_ERR = BASECUSTSERV_ERR + 70;

/**
 * ���ݷ�������ȡ�ʻ�����
 */
const int GETACCTBYSN_ERR = BASECUSTSERV_ERR + 71;

/**
 * �޸��û��ʼ�����
 */
const int MODIFYUSERPOSTINFO_ERR = BASECUSTSERV_ERR + 72;

/**
 * �����û��ʼ�����
 */
const int ADDUSERPOSTINFO_ERR = BASECUSTSERV_ERR + 73;

/**
 * ��ȡ�û���������
 */
const int GETUSERSERVICE_ERR = BASECUSTSERV_ERR + 74;

/**
 * ��ȡ�û��Ż�����
 */
const int GETUSERDISCNT_ERR = BASECUSTSERV_ERR + 75;

/**
 * ��ȡ�û�ռ����Դ
 */
const int GETUSERRESOURCE_ERR = BASECUSTSERV_ERR + 76;

/**
 * ���ü���У���쳣
 */
const int CheckDerateFee_ERR = BASECUSTSERV_ERR + 77;

/**
 * ��ȡ�����������б��쳣
 */
const int GetSubPayMoney_ERR = BASECUSTSERV_ERR + 78;

/**
 * ����̨�ʻ�������
 */
const int GENETRADESCOREMAIN_ERR = BASECUSTSERV_ERR + 79;

/**
 * ����̨�ʻ����ӱ�
 */
const int GENETRADESCORECHILD_ERR = BASECUSTSERV_ERR + 80;

/**
 * ɾ���ͻ�����
 */
const int DESTROYCUST_ERR = BASECUSTSERV_ERR + 81;

/**
 * ��ȡ��������
 */
const int GETFAMILYINFO_ERR = BASECUSTSERV_ERR + 82;


/**
 * ���Ӹ����û�����
 */
const int ADDDECKFAMILYUSER_ERR = BASECUSTSERV_ERR + 83;

/**
 * ����ָ������������
 */
const int GENECHGRESSERV_ERR = BASECUSTSERV_ERR + 84;

/**
 * ����ָ�����״̬��������
 */
const int GENESVCSTATECHGSERV_ERR = BASECUSTSERV_ERR + 85;

/**
 * �ָ��û�����
 */
const int RESTOREUSERSVC_ERR = BASECUSTSERV_ERR + 86;

/**
 * ���û��ָ�Ϊ�����û�
 */
const int RESTOREUSER_ERR = BASECUSTSERV_ERR + 87;

/**
 * �ָ��û��Ż�
 */
const int RESTOREUSERDISCNT_ERR = BASECUSTSERV_ERR + 88;

/**
 * �ָ��û������Ż�
 */
const int RESTOREUSERRELADISCNT_ERR = BASECUSTSERV_ERR + 89;

/**
 * �ָ��û����û���ϵ
 */
const int RESTOREUSERRELATION_ERR = BASECUSTSERV_ERR + 90;


/**
 * �����û���ʶ��ȡ�û�����״̬
 */
const int GETUSERSVCSTATE_ERR = BASECUSTSERV_ERR + 91;

/**
 * ��ѯ�û��ʼ�����
 */
const int GETUSERPOSTINFO_ERR = BASECUSTSERV_ERR + 92;

/**
 * ��ȡ�û���������
 */
const int GETUSERRELATIONINFO_ERR = BASECUSTSERV_ERR + 93;

/**
 * ��ȡ�û�������Ϣ
 */
const int GETUSERSUBSCRIBEINFO_ERR = BASECUSTSERV_ERR + 94;

/**
 * �����û�������Ϣ
 */
const int MODIFYUSERSUBSCRIBEINFO_ERR = BASECUSTSERV_ERR + 95;

/**
 * ��ѯ�û��������ϱ��¼
 */
const int GETUSERSCOREINFO_ERR = BASECUSTSERV_ERR + 96;

/**
 * ��ȡ�û���ǰ����Ʒ�����з�������
 */
const int GETUSERALLSERVICE_ERR = BASECUSTSERV_ERR + 97;

/**
 * �����û������ͻ���ʶ
 */
const int REPLACEUSERCUSTID_ERR = BASECUSTSERV_ERR + 98;

/**
 * �����û�����
 */
const int COPYUSERSERVICE_ERR = BASECUSTSERV_ERR + 99;

/**
 * �����û�����״̬
 */
const int COPYUSERSVCSTATE_ERR = BASECUSTSERV_ERR + 100;

/**
 * �����û���Դ
 */
const int COPYUSERRES_ERR = BASECUSTSERV_ERR + 101;

/**
 * �����û��Ż�����
 */
const int COPYUSERDISCNT_ERR = BASECUSTSERV_ERR + 102;

/**
 * �����û�Ѻ������
 */
const int COPYUSERFOREGIFT_ERR = BASECUSTSERV_ERR + 103;

/**
 * �����û�
 */
const int COPYUSER_ERR = BASECUSTSERV_ERR + 104;

/**
 * �����û���������
 */
const int GENEUSERPARCHASE_ERR = BASECUSTSERV_ERR + 105;

/**
 * �����������
 */
const int CHECKRETURNOPEN_ERR = BASECUSTSERV_ERR + 106;

/**
 * ��ȡ�û���ǰ����Ʒ�������Ż�����
 */
const int GETALLUSERDISCNT_ERR = BASECUSTSERV_ERR + 107;

/**
 * �����û����ѹ�ϵ
 */
const int GETUSERPAYRELAINFO_ERR = BASECUSTSERV_ERR + 108;

/**
 * ��ȡ�Դ��ʻ�ΪĬ���ʻ����û���ʶ���������
 */
const int GETUSERBYACCTID_ERR = BASECUSTSERV_ERR + 109;

/**
 * �޸ĸ����û�����
 */
const int CHGDECKFAMILYUSER_ERR = BASECUSTSERV_ERR + 110;

/**
 * ɾ�������û�����
 */
const int DELDECKFAMILYUSER_ERR = BASECUSTSERV_ERR + 111;


/**
 * �õ�����ҵ�����
 */
const int GETPURCHASEPARA_ERR = BASECUSTSERV_ERR + 112;

/**
 * �õ��������Ͳ���
 */
const int GETSCOREEXCHANGETYPE_ERR = BASECUSTSERV_ERR + 113;

/**
 *�õ����ֶ�������
 */
const int GETSCOREACTIONPARA_ERR = BASECUSTSERV_ERR + 114;


/*�����û�������Ϣ*/
const int MODIFYUSERSCOREINFO_ERR = BASECUSTSERV_ERR + 115;

/*����Ԥ�����û���Ϣ*/
const int PREDESTROYUSER_ERR = BASECUSTSERV_ERR + 116;

/*������ʽ�����û���Ϣ*/
const int DESTROYUSER_ERR = BASECUSTSERV_ERR + 117;

/*�����û�������*/
const int GETREMOVEUSERINFO_ERR = BASECUSTSERV_ERR + 118;

/**
 * ��������Ż�
 */
const int DEALUSERRELADISCNT_ERR = BASECUSTSERV_ERR + 119;

/**
 * �����ֻ��û���ʶ��ȡ�󶨵�IP�̶��绰�û��б�
 */
const int GETBINDIPPHONE_ERR = BASECUSTSERV_ERR + 120;

/**
 * �̺���У�飨ͬһ�����²�����ͬ���Ķ̺��룩
 */
const int CHECKSHORTCODE_ERR = BASECUSTSERV_ERR + 121;

/**
 * �̶�����У��:���ù̶������Ƿ��Ѿ���ͨ��IPҵ��
 */
const int CHECKIPPHONE_ERR = BASECUSTSERV_ERR + 122;

/**
 * ��ȡVPMN�����±պ�Ⱥ�б�
 */
const int GETVPMNCLOSEGROUP_ERR = BASECUSTSERV_ERR + 123;

/**
 * ����VPMN�պ�Ⱥ����Ϣ
 */
const int ADDUSERCLOSEGROUP_ERR = BASECUSTSERV_ERR + 124;

/**
 * ά��VPMN�պ�Ⱥ����Ϣ
 */
const int MODIFYUSERCLOSEGROUP_ERR = BASECUSTSERV_ERR + 125;

/**
 * ����VPMN�պ�Ⱥ�û���Ϣ
 */
const int ADDUSERCLOSEUSER_ERR = BASECUSTSERV_ERR + 126;

/**
 * ά��VPMN�պ�Ⱥ�û���Ϣ
 */
const int MODIFYUSERCLOSEUSER_ERR = BASECUSTSERV_ERR + 127;

/**
 * ����IP�̶��绰�û�
 */
const int ADDIPPHONEUSER_ERR = BASECUSTSERV_ERR + 128;

/**
 * ȡ��IP�̶��绰�û�
 */
const int DELIPPHONEUSER_ERR = BASECUSTSERV_ERR + 129;
/**
 * �޸�IP�̶��绰�û�
 */
const int CHGIPPHONEUSER_ERR = BASECUSTSERV_ERR + 130;

/**
 * ��ȡ����������Ϣ
 */
const int GETBANKINFO_ERR = BASECUSTSERV_ERR + 131;

/**
 * ��ȡӪҵ������Ϣ
 */
const int GETOPERFEEINFO_ERR = BASECUSTSERV_ERR + 132;

/**
 * ����IP�̶��绰�û����񼰷���״̬
 */
const int DEALUSERIPSERVICE_ERR = BASECUSTSERV_ERR + 133;

/**
 * ��ȡ�û���Ҫ��Ϣ�춯����
 */
const int GETUSERINFOCHANGE_ERR = BASECUSTSERV_ERR + 134;

/**
 * ��ȡ��ǰ����
 */
const int GETACYCID_ERR = BASECUSTSERV_ERR + 135;

/*��ѯ�����û���Ϣ*/
//const int GETREMOVEUSERINFO_ERR = BASECUSTSERV_ERR + 136;

/*����������־*/
//const int DESTROYUSER_ERR = BASECUSTSERV_ERR + 137;

/*����û�����*/
const int MODIFYUSERSCORE_ERR = BASECUSTSERV_ERR + 136;

/*��ѯ̨����Ϣ*/
const int SELECTTRADEINFO_ERR = BASECUSTSERV_ERR + 137;

/*������ͨ���ѹ�ϵ̨���ӱ�*/
const int GENETRADENORPAYRELA_ERR = BASECUSTSERV_ERR + 138;

/**
 * �������ҵ��ʱ����ȡ�ͻ���Ҫ�������û��б�
 */
const int GETALLUSERID_ERR = BASECUSTSERV_ERR + 139;

/**
 * �������ҵ��ʱ�����û��б���ȡ��Ҫ�����һ���û���Ϣ
 */
const int FETCHUSERID_ERR = BASECUSTSERV_ERR + 140;

/**
 * �����û�������Ϣ
 */
const int ADDUSERFILE_ERR = BASECUSTSERV_ERR + 141;

/**
 * ��ȡ�����ʻ���ϸ��Ŀ
 */
const int GETPAYITEMDETAIL_ERR = BASECUSTSERV_ERR + 142;

/**
 * ��ȡ�ۺ���Ŀ��ϸ
 */
const int INTEITEMDETAIL_ERR = BASECUSTSERV_ERR + 143;

/**
 * ��ȡҵ�����ͱ������
 */
 const int TRADETYPECODE_ERR = BASECUSTSERV_ERR + 144;

/**
 * ��ȡ�ʼ����ϴ���
 */
 const int GETPOSTINFO_ERR = BASECUSTSERV_ERR + 145;

/**
 * ��ѯ�˻����û��Ŀͻ����ϴ���
 */
 const int GETCUSTOFACCTUSER_ERR = BASECUSTSERV_ERR + 146;

/**
 * ��ѯ�û�Ѻ�����
 */
 const int GETUSERFOREGIFT_ERR = BASECUSTSERV_ERR + 147;

/**
 * ��ѯ���ѹ�ϵ����
 */
 const int GETPAYRELATION_ERR = BASECUSTSERV_ERR + 148;

/**
 * �ɷѸ���Ԥ�����û���Ч�ڴ���
 */
 const int UPDATEUSERVALIDDATE_ERR = BASECUSTSERV_ERR + 149;

/**
 * ��ѯ�Ƿ�ΪԤ�����û�����
 */
 const int ISPREPAYUSER_ERR = BASECUSTSERV_ERR + 150;

/**
 * ��ѯ�ʻ��Ƿ�ʵʱ���˴���
 */
 const int ISREALPAYUSER_ERR = BASECUSTSERV_ERR + 151;

/**
 * ��������״̬����
 */
 const int GETSVCSTATE_ERR = BASECUSTSERV_ERR + 152;

/**
 * ��ȡ���ѱ���
 */
 const int GENEPAYITEMCODE_ERR = BASECUSTSERV_ERR + 153;

/**
 * ��ȡ���ѱ���
 */
const int MODIFYLESTUSERPAYRELA_ERR = BASECUSTSERV_ERR + 154;

/**
 * ��ȡ��Դ�豸�۸���Ϣ
 */
const int GETDEVICEPRICE_ERR = BASECUSTSERV_ERR + 155;

/**
 * ��ȡҵ��̶����ô���
 */
const int GETTRADEOPERFEE_ERR = BASECUSTSERV_ERR + 156;

/**
 * ��ȡ������������
 */
const int GETMUSTFILLITEMS_ERR = BASECUSTSERV_ERR + 157;

/**
 * ��Ʒ���ҵ�����޸��û���Ʒ��Ϣ
 */
const int MODIFYUSERPRODUCT_ERR = BASECUSTSERV_ERR + 158;

/*
 *���ɷ���̨��
 */
const int GENETRADECANCEL_ERR = BASECUSTSERV_ERR + 159;

/*
 *��ѯ����ҵ��
 */
const int GetUSERCANCELTRADE_ERR = BASECUSTSERV_ERR + 160;

/*
 *ҵ����ǰ���
 */
const int CHECKBEFORECANCEL_ERR = BASECUSTSERV_ERR + 161;

/*
 *  ��Դռ��
 */
const int RESPOSSESS_ERR = BASECUSTSERV_ERR + 162;

/*
 *  ��Դ�ͷ�
 */
const int RESRELA_ERR = BASECUSTSERV_ERR + 163;

/*
 *  ��Դ��ѯ
 */
const int GETRESOURCEINFO_ERR = BASECUSTSERV_ERR + 164;

/*
 *  ��ȡ̨����ϸ����ʷ����
 */
const int GETTRADEINFOBYUSERTRADE_ERR = BASECUSTSERV_ERR + 165;

/**
 * ���ɷ���ҵ��̶�ָ��
 */
const int UNDOGENETRADEOLCOMSERV_ERR = BASECUSTSERV_ERR + 166;

/**
 * �޸Ŀͻ����Ϸ���
 */
const int UNDOUPDATECUSTINFO_ERR = BASECUSTSERV_ERR + 167;

/**
 * �޸Ŀͻ����Ϸ���
 */
const int UNDODESTROYCUST_ERR = BASECUSTSERV_ERR + 168;

/**
 * �����ʻ����Ϸ���
 */
const int UNDOCREATEACCTINFO_ERR = BASECUSTSERV_ERR + 169;

/**
 * �޸��ʻ����Ϸ���
 */
const int UNDOMODIFYACCTINFO_ERR = BASECUSTSERV_ERR + 170;

/**
 * �޸Ŀͻ����Ϸ���
 */
const int UNDOCREATECUSTINFO_ERR = BASECUSTSERV_ERR + 171;

/**
 * ɾ���ʻ�����
 */
const int DESTROYACCTINFO_ERR = BASECUSTSERV_ERR + 172;

/**
 * ɾ���ʻ����Ϸ���
 */
const int UNDODESTROYACCTINFO_ERR = BASECUSTSERV_ERR + 173;

/**
 * ��ȡ���������ܱ������
 */
const int GETBATOPERTYPE_ERR = BASECUSTSERV_ERR + 174;

/**
 * ��������ҵ����̨��
 */
const int GENEBATCHDEALTRADE_ERR = BASECUSTSERV_ERR + 175;

/**
 * ��ȡԤ����쳣
 */
const int RECVDEPOSIT_ERR = BASECUSTSERV_ERR + 176;

/**
 * �ʻ�ת���쳣
 */
const int TRANSACCTDEPOSIT_ERR = BASECUSTSERV_ERR + 177;

/**
 * ��ȡ����ҵ������������Ϣ
 */
const int GETBATDEALRESULTINFO_ERR = BASECUSTSERV_ERR + 178;

/**
 * ��ȡ��־��������Ϣ
 */
const int GETTAGSETINFO_ERR = BASECUSTSERV_ERR + 179;

/**
 * �û��Ż����Ϸ���
 */
const int UNDOUSERDISCNT_ERR = BASECUSTSERV_ERR + 180;

/**
 * �û��浵���Ϸ���
 */
const int UNDOADDUSERFILE_ERR = BASECUSTSERV_ERR + 181;

/**
 * �����û�Ѻ�����Ϸ���
 */
const int UNDOADDUSERFOREGIFT_ERR = BASECUSTSERV_ERR + 182;

/**
 * �޸��û�Ѻ�����Ϸ���
 */
const int UNDOMODIFYUSERFOREGIFT_ERR = BASECUSTSERV_ERR + 183;

/**
 * �����û�Ѻ�����Ϸ���
 */
const int UNDOCOPYUSERFOREGIFT_ERR = BASECUSTSERV_ERR + 184;

/**
 * �û��׶����Ϸ���
 */
const int UNDOUSERINFOCHG_ERR = BASECUSTSERV_ERR + 185;

/**
 * �û����ѹ�ϵ���Ϸ���
 */
const int UNDOUSERPAYRELA_ERR = BASECUSTSERV_ERR + 186;

/**
 * �û��ʼ����Ϸ���
 */
const int UNDOUSERPOSTINFO_ERR = BASECUSTSERV_ERR + 187;

/**
 * �û���Դ���Ϸ���
 */
const int UNDOUSERRES_ERR = BASECUSTSERV_ERR + 188;

/**
 * �û��������Ϸ���
 */
const int UNDOUSERSCOREINFO_ERR = BASECUSTSERV_ERR + 189;

/**
 * �û��������Ϸ���
 */
const int UNDOUSERSVC_ERR = BASECUSTSERV_ERR + 190;

/**
 * �û�����״̬���Ϸ���
 */
const int UNDOUSERSVCSTATE_ERR = BASECUSTSERV_ERR + 191;

/**
 * �û��������Ϸ���
 */
const int UNDOCREATEUSERINFO_ERR = BASECUSTSERV_ERR + 192;

/**
 * �û���ϵ���Ϸ���
 */
const int UNDOUSERRELATION_ERR = BASECUSTSERV_ERR + 193;

/**
 * �û�ҵ���������Ϸ���
 */
const int UNDOUSERTRADELIMIT_ERR = BASECUSTSERV_ERR + 194;

/**
 * �����û����Ϸ���
 */
const int UNDOCOPYUSER_ERR = BASECUSTSERV_ERR + 195;

/**
 * �û�Ԥ��������
 */
const int UNDODESTROYUSER_ERR = BASECUSTSERV_ERR + 196;

/**
 * �û����ϱ������
 */
const int UNDOMODIFYUSERINFO_ERR = BASECUSTSERV_ERR + 197;

/**
 * �޸��û�ʹ�ù�ϵ����
 */
const int UNDOREPLACEUSERCUSTID_ERR = BASECUSTSERV_ERR + 198;

/**
 * IPֱͨ���Ǽ�̨��
 */
const int GENEIPEXPRESSTRADE_ERR = BASECUSTSERV_ERR + 199;

/**
 * sim��������Ƿ���
 */
const int UNDOREGISTSIMBACKTAG_ERR = BASECUSTSERV_ERR + 200;

/**
 * ��ԴԤռ������ɾ��Ԥռ���¼��
 */
const int UNDORESENGROSS_ERR = BASECUSTSERV_ERR + 201;

/**
 * ��Դռ�÷�����ɾ��ռ�ñ�������б�
 */
const int UNDORESPOSSESS_ERR = BASECUSTSERV_ERR + 202;

/**
 * ��Դ�ͷŷ���
 */
const int UNDORESRELA_ERR = BASECUSTSERV_ERR + 203;

/**
 * IPֱͨ��
 */
const int IPUSERINFODEAL_ERR = BASECUSTSERV_ERR + 204;

/**
 * ��ѯ�û�VPMN��Ϣ
 */
const int GETUSERVPMNINFO_ERR = BASECUSTSERV_ERR + 205;

/**
 * �����û�VPMN��Ϣ
 */
const int ADDUSERVPMN_ERR = BASECUSTSERV_ERR + 206;

/**
 * �޸��û�VPMN��Ϣ
 */
const int MODIFYUSERVPMN_ERR = BASECUSTSERV_ERR + 207;

/**
 * ���ɼƷ������ӿ�
 */
const int GENEBILLINTF_ERR = BASECUSTSERV_ERR + 208;

/**
 * ��ȡ��ǰ��Ʒ�µ��û�����
 */
const int GETUSERSVCOFPID_ERR = BASECUSTSERV_ERR + 209;

/**
 * ��ȡ��ǰ��Ʒ�µ��û��Ż�
 */
const int GETUSERDISCNTOFPID_ERR = BASECUSTSERV_ERR + 210;

/**
 * ͨ���û�A���û���ʶ���û�B�ķ�����뼰��ϵ���ͻ�ȡ�û����û��Ĺ�ϵ
 */
const int GETMEMBERUSERRELATION_ERR = BASECUSTSERV_ERR + 211;

/**
 * ��ȡ��Ա�û����е���ϲ�Ʒ�µĵ�ǰ����
 */
const int GETMEMBERSVCOFPID_ERR = BASECUSTSERV_ERR + 212;

/**
 * ��ȡ��Ա�û�����ϲ�Ʒ�µĵ�ǰ�Ż�
 */
const int GETMEMBERDISCNTOFPID_ERR = BASECUSTSERV_ERR + 213;

/**
 * ��ȡ��ǰ��Ʒ�µ��û��Ż�
 */
const int CALLCENTERENCODESTR_ERR = BASECUSTSERV_ERR + 214;

/**
 * ��ȡ��ǰ��Ʒ�µ��û��������Դ
 */
const int GETUSERRESOFPID_ERR = BASECUSTSERV_ERR + 215;

/**
 * ��ȡ��ǰ��ϲ�Ʒ�µĳ�Ա�û��������Դ
 */
const int GETMEMBERRESOFPID_ERR = BASECUSTSERV_ERR + 216;

/**
 * ���ݹ�ϵ���ͻ�ȡ��ɫ�����б�
 */
const int GETRELATIONROLE_ERR = BASECUSTSERV_ERR + 217;

/**
 * ��ȡ��ͻ���������
 */
const int CUSTGETVIPSIMBAK_ERR = BASECUSTSERV_ERR + 218;

/**
 * У���ͻ���������
 */
const int CUSTCHECKVIPSIMBAKINFO_ERR = BASECUSTSERV_ERR + 219;

/**
 * ������ͻ���������
 */
const int CUSTCREATEVIPSIMBAK_ERR = BASECUSTSERV_ERR + 220;

/**
 * �޸Ĵ�ͻ���������
 */
const int CUSTMODIFYVIPSIMBAK_ERR = BASECUSTSERV_ERR + 221;

/**
 * ����Ա����ȡ���ű���
 */
const int GETDEPARTIDBYSTAFFID_ERR = BASECUSTSERV_ERR + 222;

/**
 * ���ݲ��Ż�ȡ����ҵ����
 */
const int GETCITYCODEBYDEPARTID_ERR = BASECUSTSERV_ERR + 223;


/**
 * ҵ���Ȩ
 */
const int TRADEAUTH_ERR = BASECUSTSERV_ERR + 224;

/**
 * E������
 */
const int CREDENCEREG_ERR = BASECUSTSERV_ERR + 225;

/**
 *  ���ɹ���ʱ���¾��û���Ϣ̨��
 */
const int GENENEWOLDUSERTRADE_ERR = BASECUSTSERV_ERR + 226;

/**
 *  �޸��û���ϵ
 */
const int MODIFYUSERRELATION_ERR = BASECUSTSERV_ERR + 227;

/**
 *  ���ӳ�Ա����
 */
const int ADDMEMBERSERVICE_ERR = BASECUSTSERV_ERR + 228;

/**
 * �޸ĳ�Ա����
 */
const int MODIFYMEMBERSERVICE_ERR = BASECUSTSERV_ERR + 229;

/**
 * �޸ĳ�Ա����
 */
const int CHECKFAMILYPHONE_ERR = BASECUSTSERV_ERR + 230;

/**
 * �޸Ŀͻ�״̬
 */
const int MODIFYCUSTSTATE_ERR = BASECUSTSERV_ERR + 231;

/**
 * ��ѯ��ָ������Ϣ�쳣
 */
const int GETOLCOMINFOBYSN_ERR = BASECUSTSERV_ERR + 232;

/**
 * ����û���Դλ���쳣
 */
const int CHECKRESLOCAT_ERR = BASECUSTSERV_ERR + 233;

/**
 * ���ͻ����ƿ�����
 */
const int CHECKOPENLIMIT_ERR = BASECUSTSERV_ERR + 234;

/**
 * ���ɸ���̨�ʷ����ӱ����
 */
const int GENERESTORETRADESVC_ERR = BASECUSTSERV_ERR + 235;

/**
 * ��ָֹ���ʻ��µĸ߼����ѹ�ϵ
 */
const int STOPACCTADVPAYRELA_ERR = BASECUSTSERV_ERR + 236;

/**
 * ����ָ���ʻ��µĸ߼����ѹ�ϵ
 */
const int BAKACCTADVPAYRELA_ERR = BASECUSTSERV_ERR + 237;

/**
 * ��ָֹ���ʻ��µĸ߼����ѹ�ϵ����
 */
const int UNDOSTOPACCTADVPAYRELA_ERR = BASECUSTSERV_ERR + 238;

/**
 * ��ԭ�ʻ��µ������û����ѹ�ϵ��Ǩ�����ʻ�����
 */
const int UNDOMODIFYUSERPAYRELA_ERR = BASECUSTSERV_ERR + 239;

/**
 * �ж��Ƿ�Ϊ�����ʻ�
 */
const int CHECKACCTONLY_ERR = BASECUSTSERV_ERR + 240;

/**
 * ���ÿ���ҵ��ǼǴ���
 */
const int CREDITTRADEREG_ERR = BASECUSTSERV_ERR + 241;

/**
 * ���˴���ҵ�����
 */
const int GETSERVDEDUCT_ERR = BASECUSTSERV_ERR + 242;

/**
 *��ѯ�����嵥����
 */
const int GETBILLVOICE_ERR = BASECUSTSERV_ERR + 243;

/**
 *��ѯ�����嵥����
 */
const int GETBILLSMS_ERR = BASECUSTSERV_ERR + 244;

/**
 *��ѯ�����嵥����
 */
const int GETBILLMONTERNET_ERR = BASECUSTSERV_ERR + 245;

/**
 *��ѯGPRS�嵥����
 */
const int GETBILLGPRS_ERR = BASECUSTSERV_ERR + 246;

/**
 *��ѯ�����������嵥����
 */
const int GETBILLSZXVOICE_ERR = BASECUSTSERV_ERR + 247;

/**
 *��ѯ�����ж����嵥����
 */
const int GETBILLSZXSMS_ERR = BASECUSTSERV_ERR + 248;

/**
 *��ѯ�����������嵥����
 */
const int GETBILLSZXMONTERNET_ERR = BASECUSTSERV_ERR + 249;

/**
 *��ѯIPֱͨ���嵥����
 */
const int GETBILLIPBIND_ERR = BASECUSTSERV_ERR + 250;

/**
 *��ѯ����绰�嵥����
 */
const int GETBILLMEETPHONE_ERR = BASECUSTSERV_ERR + 251;

/**
 *��ѯ�ٱ����嵥����
 */
const int GETBILLKJAVA_ERR = BASECUSTSERV_ERR + 252;

/**
 *��ѯ������־�嵥����
 */
const int GETBILLVOICEJ_ERR = BASECUSTSERV_ERR + 253;

/**
 *��ѯWAP�嵥����
 */
const int GETBILLWAP_ERR = BASECUSTSERV_ERR + 254;

/**
 *��ѯWLAN�嵥����
 */
const int GETBILLWLAN_ERR = BASECUSTSERV_ERR + 255;

/**
 *��ѯ�����嵥����
 */
const int GETBILLCRBT_ERR = BASECUSTSERV_ERR + 256;

/**
 *��ѯ�����嵥����
 */
const int GETBILLMMS_ERR = BASECUSTSERV_ERR + 257;

/**
 * ����ҵ����ˮ�Ų�ѯ��ָ������Ϣ����
 */
const int GETOLCOMINFOBYTRADE_ERR = BASECUSTSERV_ERR + 258;

/**
 * ��ȡ��Ʒ�¿���ԤԼ��ͨ�ķ�����Ϣ
 */
const int GETPRECHANGESVCINFO_ERR = BASECUSTSERV_ERR + 259;

/**
 * ��ȡ�û���������
 */
const int GETUSERPRUCHASE_ERR = BASECUSTSERV_ERR + 260;

/**
 * ��ȡ�û���Ч��
 */
const int GETUSERVALIDDATE_ERR = BASECUSTSERV_ERR + 261;

/**
 * ȡ���û�����
 */
const int REMOVEUSERPARCHASE_ERR = BASECUSTSERV_ERR + 262;

/**
 * ҵ�����������ܿ�
 */
const int CHECKTRADEEPECLIMIT_ERR = BASECUSTSERV_ERR + 263;

/**
 * ������ֹ�û���������
 */
const int ENDUSERALLINFO_ERR = BASECUSTSERV_ERR + 264;

/**
 * �û��������Ϸ���
 */
const int UNDOUSERPURCHASE_ERR = BASECUSTSERV_ERR + 265;

/**
 * ϵͳ��֯��Ϣ��ѯ
 */
const int GETSYSORGINFO_ERR = BASECUSTSERV_ERR + 266;

/**
 * ��ȡ�û�������������
 */
const int GETUSEROTHERSERV_ERR = BASECUSTSERV_ERR + 267;

/**
 * �����û�������������
 */
const int ADDUSEROTHERSERV_ERR = BASECUSTSERV_ERR + 268;

/**
 * �޸��û�������������
 */
const int MODIFYUSEROTHERSERV_ERR = BASECUSTSERV_ERR + 269;

/**
 * �������������Ϣ��¼
 */
const int CREATEFIXEDFEESPECINFO_ERR = BASECUSTSERV_ERR + 270;

/**
 * �޸����������Ϣ��¼
 */
const int MODIFYFIXEDFEESPECINFO_ERR = BASECUSTSERV_ERR + 271;

/**
 * ɾ�����������Ϣ��¼
 */
const int DELETEFIXEDFEESPECINFO_ERR = BASECUSTSERV_ERR + 272;

/**
 * �޸��û��������ϱ���Ϣ
 */
const int MODIFYUSEROTHER_ERR = BASECUSTSERV_ERR + 273;

/**
 * ָ��󶨷�����
 */
const int DEALOLCOMBINDSVC_ERR = BASECUSTSERV_ERR + 274;

/**
 * ��ȡ�û��������ϱ�����
 */
const int GETUSEROTHER_ERR = BASECUSTSERV_ERR + 275;

/**
 * �����û������Ż����ϱ�����
 */
const int ADDUSERSPECIFICDISCNT_ERR = BASECUSTSERV_ERR + 276;

/**
 * �޸��û������Ż����ϱ�����
 */
const int MODIFYUSERSPECIFICDISCNT_ERR = BASECUSTSERV_ERR + 277;

/**
 * �޸��û����ϱ������ֶ�
 */
const int MODIFYUSERRSRVSTR_ERR = BASECUSTSERV_ERR + 278;

/**
 * �޸��û����ϱ������ֶ�(����)
 */
const int UNDOMODIFYUSERRSRVSTR_ERR = BASECUSTSERV_ERR + 279;

/**
 * ����û����ϴ���
 */
const int DEALRENTMOBILE_ERR = BASECUSTSERV_ERR + 280;

/**
 * �ֻ�֧������̨������
 */
const int GENENOTECASETRADEINFO_ERR = BASECUSTSERV_ERR + 281;

/**
 * �ֻ�֧������̨������
 */
const int MODIFYBANKBAND_ERR = BASECUSTSERV_ERR + 282;

/**
 * �����û����û���ϵ����
 */
const int COPYUSERRELATION_ERR = BASECUSTSERV_ERR + 283;

/**
 * ֹͣ�û��߼����ѹ�ϵ
 */
const int STOPUSERADVPAYRELA_ERR = BASECUSTSERV_ERR + 284;

/**
 * �޸��û��߼����ѹ�ϵ
 */
const int MODIFYUSERADVPAYRELA_ERR = BASECUSTSERV_ERR + 285;

/**
 * ����ҵ����������ҵ�����ָͨ��
 */
const int GENETRADESVCOLOCM_ERR = BASECUSTSERV_ERR + 286;

/**
 * ͨ����Ա��ѯ������Ϣ
 */
const int GETGRPINFOBYMEMBER_ERR = BASECUSTSERV_ERR + 287;

/**
 * �����ͻ�����(����)
 */
const int UNDOCUSTMODIFYVIPSIMBAK_ERR = BASECUSTSERV_ERR + 288;

/**
 * �����û������Ż�
 */
const int DESTROYRELAINFO_ERR = BASECUSTSERV_ERR + 289;

/**
 * ��ȡ�Żݲ���
 */
const int GETDISCNT_ERR = BASECUSTSERV_ERR + 290;

/**
 * ������봦��
 */
const int DEALRENTNO_ERR = BASECUSTSERV_ERR + 291;

/**
 * ���öȼ���ӿڵ���
 */
const int CALCCREDIT_ERR = BASECUSTSERV_ERR + 292;

/**
 * �û����ϲ�����
 */
const int USERNOTEXISTS_ERR = BASECUSTSERV_ERR + 293;

/**
 * �ͻ����ϲ�����
 */
const int CUSTNOTEXISTS_ERR = BASECUSTSERV_ERR + 294;

/**
 * ����������
 */
const int CS_BLACKUSER_ERR = BASECUSTSERV_ERR + 295;

/**
 * Ƿ������
 */
const int CS_OWEFEE_ERR = BASECUSTSERV_ERR + 296;

/**
 * �û�״̬����
 */
const int CS_USERSTATE_ERR = BASECUSTSERV_ERR + 297;

/**
 * �û�ҵ������
 */
const int CS_TRADELIMIT_ERR = BASECUSTSERV_ERR + 298;

/**
 * ���ҵ������
 */
const int CS_OTHERAREATRADE_ERR = BASECUSTSERV_ERR + 299;

/**
 * �Ǵ�ͻ�
 */
const int CS_NOTVIP_ERR = BASECUSTSERV_ERR + 300;

/**
 * �ޱ���
 */
const int CS_NOSIMBAK_ERR = BASECUSTSERV_ERR + 301;

/**
 * ���ݼ����û���ID��ѯ���г�Ա�û�ID
 */
const int GETMEMBERUSERBYGROUPUSER_ERR = BASECUSTSERV_ERR + 302;

/**
 * Ӫҵ���Ͷ���֪ͨʧ��
 */
const int CS_SENDSMS_ERR = BASECUSTSERV_ERR + 303;

/**
 * ��ȡ�û�ҵ��ƽ̨ҵ������ʧ��
 */
const int CS_GETUSERMBMPINFO_ERR = BASECUSTSERV_ERR + 304;

/**
 * ҵ��ƽ̨ҵ������,�û��Ѿ������
 */
const int CS_USERMBMPINFO_BIZNOW_ERR = BASECUSTSERV_ERR + 305;

/**
 * ҵ��ƽ̨ҵ������,�û���Ӧ�����ж�
 */
const int CS_USERSEPCSVCINFO_ERR = BASECUSTSERV_ERR + 306;

/**
 * �������û�
 */
const int CS_ISGS01USER_ERR = BASECUSTSERV_ERR + 307;

/**
 * ҵ��ƽ̨���������
 */
const int CS_CHECKOLDPASSWD_ERR = BASECUSTSERV_ERR + 308;

/**
 * ҵ��ƽ̨ҵ��ɾ��,�û�����Ӧ��������
 */
const int CS_USERMBMPINFO_BIZNOWNO_ERR = BASECUSTSERV_ERR + 309;

/**
 * ��ȡ�û�ҵ��ƽ̨ҵ���������ʧ��
 */
const int CS_GETUSERMBMPPLUSINFO_ERR = BASECUSTSERV_ERR + 310;

/**
 * �����ж�
 */
const int CS_CHECKWORDTRADE_02_ERR = BASECUSTSERV_ERR + 311;

/**
 * SP����У��
 */
const int CS_CHECKSPFACTORY_ERR = BASECUSTSERV_ERR + 312;

/**
 * SP����У��
 */
const int CS_CHECKSPSERVICE_ERR = BASECUSTSERV_ERR + 313;

/**
 * ҵ��ƽ̨ҵ������ж�
 */
const int CS_BIZTYPECODECHECK_ERR = BASECUSTSERV_ERR + 314;

/**
 * ҵ��ƽ̨���������ж�
 */
const int CS_OPERTYPECODECHECK_ERR = BASECUSTSERV_ERR + 315;

/**
 * VIP��������,���񼶱��ж�
 */
const int CS_SERVERLEVER_ERR = BASECUSTSERV_ERR + 316;

/**
 * �������
 */
const int CS_MANYSIMBAK_ERR = BASECUSTSERV_ERR + 317;

/**
 * �޶�Ӧ����
 */
const int CS_SIMBAKEXISTS_ERR = BASECUSTSERV_ERR + 318;

/**
 * �����ж�
 */
const int CS_CHECKWORDTRADE_01_ERR = BASECUSTSERV_ERR + 319;

/**
 * Ʒ�Ʊ���ת��
 */
const int BRANDCODECONVER_ERR = BASECUSTSERV_ERR + 320;

/**
 * ��ȡͨ�ò���
 */
const int GETPARAMINFO_ERR = BASECUSTSERV_ERR + 321;

/**
 * �춯������ʼ��
 */
const int CREDENCEINITTRADE_ERR = BASECUSTSERV_ERR + 322;

/**
 * �춯����ҵ��ǰ�ж�
 */
const int CHECKBEFORECREDENCE_ERR = BASECUSTSERV_ERR + 323;

/**
 * �춯������Ϣ��ȡ�ж�
 */
const int GETCREDENCE_ERR = BASECUSTSERV_ERR + 324;

/**
 * ��ȡBOSS��Ϣ����
 */
const int GETBOSSCODE_ERR = BASECUSTSERV_ERR + 325;

/**
 * ��ȡ�Żݰ���Ӧ�Ż���Ϣ
 */
const int GETOPERSETTODISCNT_ERR = BASECUSTSERV_ERR + 326;

/**
 * ƴ������
 */
const int GENECODESTR_ERR = BASECUSTSERV_ERR + 327;

/**
 * ����ƴ������
 */
const int GENESERVICESTR_ERR = BASECUSTSERV_ERR + 328;

/**
 * �Ż�ƴ������
 */
const int GENEDISCNTSTR_ERR = BASECUSTSERV_ERR + 329;

/**
 * �����·ݼ���ʱ�����
 */
const int GETADDMONTHSDATE_ERR = BASECUSTSERV_ERR + 330;

/**
 * �����춯������Ϣ
 */
const int ADDCREDENCE_ERR = BASECUSTSERV_ERR + 331;

/**
 * �޸��춯����״̬
 */
const int MODICREDENCESTATE_ERR = BASECUSTSERV_ERR + 332;

/**
 * �����춯��Ϣ����
 */
const int UNDOADDCREDENCE_ERR = BASECUSTSERV_ERR + 333;

/**
 *����Ƿ������֤�ж�
 */
const int JUDGEDERATEIDENTITYCHECK_ERR = BASECUSTSERV_ERR + 334;

/**
 *��¼�ɹ��������֤
 */
const int RECORDIDENTITYCHECK_ERR = BASECUSTSERV_ERR + 335;

/**
 *ͨ��ָ����д
 */
const int CREATECOMNOLCOM_ERR = BASECUSTSERV_ERR + 336;

/**
 *IP��Ϣ
 */
const int GETIPINFO_ERR = BASECUSTSERV_ERR + 337;

/**
 *�����ж��ŷ���
 */
const int SMSCHINAGO_ERR = BASECUSTSERV_ERR + 338;

/**
 *  ����ҵ�����֪ͨ
 */
const int SENDTRADESMS_ERR = BASECUSTSERV_ERR + 339;

/**
 *  ��������֪ͨ
 */
const int USERINGWINFO_ERR = BASECUSTSERV_ERR + 340;

/**
 *  ��������֪ͨ
 */
const int INTFMUSICRING_ERR = BASECUSTSERV_ERR + 341;

/**
 *  �����û�������ϵ
 */
const int DEALRELATIONINFO_ERR = BASECUSTSERV_ERR + 342;

/**
 *  �ػ���ʹ�������ϴ���
 */
const int DEALOTHER4ANGEL_ERR = BASECUSTSERV_ERR + 343;

/**
 *  ��ȡ����ʱ����ϵͳʱ���ֵ
 */
const int GETODDSDAYS_ERR = BASECUSTSERV_ERR + 344;

/**
 *  �ϲ�����
 */
const int UNIONGROUPUSER_ERR = BASECUSTSERV_ERR + 345;

/**
 *  �����û���e����������
 */
const int GETUSERECARDBINDINFO_ERR = BASECUSTSERV_ERR + 346;

/**
 *  ����ҵ��̨������
 */
const int ROLLBACKTRADEINFO_ERR = BASECUSTSERV_ERR + 347;

/**
 *  ̨�ʹ�ϵ���ϱ����⴦��
 */
const int TRADERELASPECDEAL_ERR = BASECUSTSERV_ERR + 348;

/**
 *  �Ǽǰ��Ż�̨��
 */
const int BINDDISCNTTRADE_ERR = BASECUSTSERV_ERR + 349;

/**
 * ��ȡSP���̲���
 */
const int CS_GETSPFACTORY_ERR = BASECUSTSERV_ERR + 350;

/**
 * ��ȡSP�������
 */
const int CS_GETSPSERVICE_ERR = BASECUSTSERV_ERR + 351;

/**
 * ��ȡSP�������
 */
const int CS_GETGRPHISTRADEQUERY_ERR = BASECUSTSERV_ERR + 352;

/**
 * ��ȡVPMN�������������Ϣ
 */
const int GETVPMNGRPOUTBYVPMN_ERR = BASECUSTSERV_ERR + 353;

/**
 * ��ȡVPMN���ų�Ա���������Ϣ
 */
const int GETVPMNMENBEROUTBYVPMN_ERR = BASECUSTSERV_ERR + 354;

/**
 * �޸�VPMN���������Ϣ
 */
const int MODIFYVPMNGRPOUT_ERR = BASECUSTSERV_ERR + 355;

/**
 *
 */
const int REDEALBATTRADE_ERR = BASECUSTSERV_ERR + 356;

/**
 * ��ȡVPMN�ͻ�Ƿ����Ϣ
 */
const int GETVPMNOWE_ERR = BASECUSTSERV_ERR + 357;

/**
 * ��ȡVPMN�ͻ�Ƿ����Ϣ
 */
const int CS_JUDGEACCOUNTTRADELIMIT_ERR = BASECUSTSERV_ERR + 358;

/**
 * ��ȡVPN����ҵ����Ϣ add lif 20060301
 */
const int CS_GETVPNGROUPTRADEQUERY_ERR = BASECUSTSERV_ERR + 359;

/**
 * ��ȡ���������Ϣ
 */
const int GetTradeinfoTrade_ERR = BASECUSTSERV_ERR + 360;

/**
 * ���뿨�ֻ�������Ч
 */
const int CS_InvalidSN_ERR = BASECUSTSERV_ERR + 361;

/**
 * ���뿨�ֻ������ѿ���
 */
const int CS_SNHasOpen_ERR = BASECUSTSERV_ERR + 362;

/**
 * ���뿨������Ч
 */
const int CS_WrongPasswd_ERR = BASECUSTSERV_ERR + 363;

/**
 * ���뿨����ʧ��
 */
const int CS_PCardOpen_ERR = BASECUSTSERV_ERR + 364;

/**
 * ����������ȡ
 */
const int GETUSERPURCHASEMODE_ERR = BASECUSTSERV_ERR + 364;

/**
 * ��ȡ�û��»���
 */
const int GETUSERNEWSCORE = BASECUSTSERV_ERR + 365;

/**
 * �û�����ҵ���Żݴ���
 */
const int DEALPURCHASEDISCNT_ERR = BASECUSTSERV_ERR + 366;

/**
 * ��ȡ��ʡ����ҵ��̨������
 */
const int GETSPANTRADEINFO_ERR = BASECUSTSERV_ERR + 367;

/**
 * ��ȡ��ʡ����ҵ��̨����ϸ��
 */
const int GETSPANTRADEPLUS_ERR = BASECUSTSERV_ERR + 368;

/**
 * ��ȡ��ʡ����ҵ���Ʒ�ӱ�
 */
const int GETSPANTRADESUBINFO_ERR = BASECUSTSERV_ERR + 369;

/**
 * ��ȡ��ʡ����ҵ���ӱ���ϸ
 */
const int GETSPANTRADEDETAILINFO_ERR = BASECUSTSERV_ERR + 370;

/**
 * ��ֹ��ʡ���ż�¼
 */
const int STOPSPANTRADE_ERR = BASECUSTSERV_ERR + 371;

/**
 * �޸Ŀ�ʡ���Ų�Ʒ��¼
 */
const int MODIFYSPANTRADESUB_ERR = BASECUSTSERV_ERR + 372;

/**
 * ��ֹ��ʡ���Ų�Ʒ��ϸ
 */
const int DELSPANTRADEDETAIL_ERR = BASECUSTSERV_ERR + 373;

/**
 * ��ֹ��ʡ���Ų�Ʒ�ӱ�
 */
const int DELSPANTRADESUB_ERR = BASECUSTSERV_ERR + 374;

/**
 * ��ֹ��ʡ���Ų�Ʒ��ϸ��
 */
const int DELSPANTRADEPLUS_ERR = BASECUSTSERV_ERR + 375;

/**
 * ��ֹ��ʡ��������
 */
const int DELSPANTRADE_ERR = BASECUSTSERV_ERR + 376;

/**
 * ҵ��δ�깤
 */
const int TRADENOTCOMPLETED_ERR = BASECUSTSERV_ERR + 377;

/**
 * ���ݱ�ʶ��ѯ�齱��������
 */
const int GETTAFFLEPARA_PROJECT_ERR = BASECUSTSERV_ERR + 378;

/**
 * ����ʱ���ѯ�齱��������
 */
const int GETTAFFLEPARA_JOINTIME_ERR = BASECUSTSERV_ERR + 379;

/**
 * �����û��齱��Ϣ����
 */
const int GETTAFFLEINFO_USER_ERR = BASECUSTSERV_ERR + 380;

/**
 * ������ˮ��ѯ�齱��Ϣ����
 */
const int GETTAFFLEINFO_ID_ERR = BASECUSTSERV_ERR + 381;

/**
 * ���ɸ��˿ͻ���Ϣʧ��
 */
const int CREATECUSTPERSONINFO_ERR = BASECUSTSERV_ERR + 382;

/**
 * �����˻�������Ϣʧ��
 */
const int CREATEACCTCONSIGNINFO_ERR = BASECUSTSERV_ERR + 383;

/**
 * ���ɵ����û�����ʧ��
 */
const int CREATEUSERASSUREINFO_ERR = BASECUSTSERV_ERR + 384;

/**
 * �������ɱ���
 */
const int TASKITEMINFO_ERR = BASECUSTSERV_ERR + 385;

/**
 * ���������쳣 
 */
const int TRADEAUDIT_ERR = BASECUSTSERV_ERR + 386;

/**
 * �����ֻ������ѯ��Ʒ��Ϣ���ӿڣ�
 */
const int QUERYPRODUCT_INFO_ERR = BASECUSTSERV_ERR + 387;

/**
 * �����ֻ������ѯ���ò�Ʒ��Ϣ���ӿڣ�
 */
const int QUERYPRODUCT_DO_ERR = BASECUSTSERV_ERR + 388;

/**
 * �����ֻ������ѯ�򵥿��ò�Ʒ��Ϣ���ӿڣ�
 */
const int QUERYPRODUCT_SINPLEDO_ERR = BASECUSTSERV_ERR + 389;

/**
 * �����ֻ������ѯ�򵥲�Ʒ��Ϣ���ӿڣ�
 */
const int QUERYPRODUCT_SINPLE_ERR = BASECUSTSERV_ERR + 390;

/**
 * ƥ�����Բ���
 */
const int MATCHINGITEMPARA_ERR = BASECUSTSERV_ERR + 391;

/**
 * ����������������ȡ��ϵ��Ա�����񼰷���״̬
 */
const int GETMEMBERMAINSVCBYSN_ERR = BASECUSTSERV_ERR + 392;

/**
 * ��ȡԪ����ϸ
 */
const int GETELEMENTDETAIL_ERR = BASECUSTSERV_ERR + 393;

/**
 * ����ָ��ƽӿ���Ϣ
 */
const int GENEOLCOMORDER_ERR = BASECUSTSERV_ERR + 394;

/**
 * ����SPָ��
 */
const int GENESPOLCOM_ERR = BASECUSTSERV_ERR + 395;

/**
 * �޸Ľ���������
 */
const int UPDOLCOMSWITCH = BASECUSTSERV_ERR + 396;

/**
 * ����ͬ��ָ���������
 */
const int GENEOLCOMORDERTB_ERR = BASECUSTSERV_ERR + 397;

/**
 * ��ȡITEM����
 */
const int FETCHITEMVALUE_ERR = BASECUSTSERV_ERR + 398;

/**
 * �����Ż�ָ��
 */
const int GENEDISCNTOLCOM_ERR = BASECUSTSERV_ERR + 399;

/**
 * �����Ż�ָ��
 */
const int GETSCPCODE_ERR = BASECUSTSERV_ERR + 400;

/**
 * ��ȡSP�󶨲���
 */
const int GETSPBINDPARA_ERR = BASECUSTSERV_ERR + 401;

/**
 * ������Ӧ����
 */
const int INSOLCOMVAR_ERR = BASECUSTSERV_ERR + 402;

/**
 * ��ѯ̨�����й�ϵ
 */
const int GETTRADEALLRELA_ERR = BASECUSTSERV_ERR + 403;

/*
 * ��ѯ��ʽ�쳣
 */
const int GETGETMODE_ERR = BASECUSTSERV_ERR + 404;

/*
 * ��ѯScp�����ʷѱ���
 */
const int GETSCPSEPDISCNT_ERR = BASECUSTSERV_ERR + 405;

/*
 * �������޸ķ���
 */
const int CREDITCHANGESVC_ERR = BASECUSTSERV_ERR + 406;

/*
 * Q�Ҽ�Ȩʧ��
 */
const int QQPERIMITION_ERR = BASECUSTSERV_ERR + 407;

/*
 * �û�����ֵΪ��
 */
const int GETUSERSCOREZERO_ERR = BASECUSTSERV_ERR + 408;

/*
 * �û�����ֵ����
 */
const int GETUSERSCORELIMIT_ERR = BASECUSTSERV_ERR + 409;

/*
 * �ǻ����û�
 */
const int GETNONSCOREUSER_ERR = BASECUSTSERV_ERR + 410;

/*
 * ����δ���״ζһ�������
 */
const int GETSCORENONLIMIT_ERR = BASECUSTSERV_ERR + 411;

/*
 * SP�ظ���������
 */
const int SP_REPEATE_ORDER_ERR = BASECUSTSERV_ERR + 412;

/*
 * SP�˶������ڵĶ�����ϵ����
 */
const int SP_CANCEL_NOTEXIST_ERR = BASECUSTSERV_ERR + 413;


/*
 * ��ȡ������Ʒ�б��޼�¼   VIPƽ̨������涨5λ
 */
const int AGENT_GET_PRODUCT_ERR = 30416;

/*
 * ��ȡ��ѡ���޼�¼ VIPƽ̨������涨5λ
 */
const int AGENT_GET_MPACKAGE_ERR = 30417;

/*
 * ��ȡ������Ʒ�б���Ȩ��  VIPƽ̨������涨5λ
 */
const int AGENT_GET_PRODUCT_RIGHT_ERR = 30418;

/*
 * SP�������ڻ���������ϵ����
 */
const int SP_MUTEX_ERR = BASECUSTSERV_ERR + 414;

/*
 * SP�˶����ڻ����������ƴ���
 */
const int SP_DEPEND_ON_ERR = BASECUSTSERV_ERR + 415;

/*
 * SP�˶����ڰ����ƴ���
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
 * ����Ʒ�Ѵ��ڣ��������ٶ�������
 */
const int MAIN_PRODUCT_EXSITS_ERR = BASECUSTSERV_ERR + 427;

/*
 * ����Ʒ�����ڣ�ȡ����������
 */
const int MAIN_PRODUCT_NOTEXSITS_ERR = BASECUSTSERV_ERR + 428;

/*
 * �û��ѱ������Ʒ����ȡ������ٶ�������
 */
const int HAD_CHANGE_MAIN_PRODUCT_ERR = BASECUSTSERV_ERR + 429;

/*
 * �û����и�����Ʒ������ȡ��
 */
const int IS_THE_ONLY_MAIN_PRODUCT_ERR = BASECUSTSERV_ERR + 430;

/*
 * ������ͨ���ӿڶԲ�ƷԪ�����ض���������
 */
const int INTF_OPRE_ELEMENT_LIMIT = BASECUSTSERV_ERR + 431;

/*
 * �ͻ����ֲ�ѯ
 */
const int GETCUSTSCORE_ERR = BASECUSTSERV_ERR + 432;

/*
 * ����ͻ�����
 */
const int INSCUSTSCORE_ERR = BASECUSTSERV_ERR + 433;

/*
 * �޸Ŀͻ�����
 */
const int UPDCUSTSCORE_ERR = BASECUSTSERV_ERR + 434; 

/*
 * �ͻ��ϲ�
 */
const int GENEUNIONPSPTID_ERR = BASECUSTSERV_ERR + 435; 

/*
 * ����δ�󶨹涨��Ʒ
 */
const int NUMBERRULEBINDPRODUCT_ERR = BASECUSTSERV_ERR + 436;

/*
 * �����ͨ����3G������Ʒת�Ұ����ó���
 */
const int NORMALNUMBERBINDPRODUCT_ERR = BASECUSTSERV_ERR + 437;
/*
 * ESSЧ�����
 */
const int ESSCHECKMODE_ERR = BASECUSTSERV_ERR + 438;

/*
 * ESSЧ�����(����)
 */
const int ESSACCTCHECK_ERR = BASECUSTSERV_ERR + 439;

/*
 * ESSЧ�����
 */
const int ESSCHECKMODE_1_ERR = BASECUSTSERV_ERR + 440;
const int INSERTREMOTECARD_ERR = BASECUSTSERV_ERR + 441;
const int CHANGEREMOTECARD_ERR = BASECUSTSERV_ERR + 442;

/*
 * Ԥ���ѿ������������� 
 */
const int ANONYMOUS_OPEN_ACCOUNT_ERR = BASECUSTSERV_ERR + 444;

/*
 * �ͻ������ѷ��� 
 */
const int HAD_RETURN_CUSTINFO = BASECUSTSERV_ERR + 445;

/*
 * ��ѯ��������Ϣ�쳣
 */
const int QY_AGENT_ERR = BASECUSTSERV_ERR + 446;

/*
 * �������ʻ����ò����쳣
 */
const int ACTION_AGENTACCT_ERR = BASECUSTSERV_ERR + 447;

#endif
