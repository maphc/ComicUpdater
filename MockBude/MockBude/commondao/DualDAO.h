#ifndef __DualDAO_H_
#define __DualDAO_H_


#include "../commondao/StaticDAO.h"
#include "../base/config-all.h"


/**=!===============SEGMENG2==========================*/
namespace MiscDom {
 
using namespace std;

/**=~===============SEGMENG4==========================*/

/** @class DualDAO
 *  @brief ���ݿ�Dual�����ݿ���ʶ���
 *  ����������ݿ�Dual�����ݿ���ʽ��з�װ��
 *  �ṩ���ж�Dual��SQL����������������ҵ�������ֱ�����á�
 */
class DualDAO {
public:

	DualDAO():dao(StaticDAO::getDAO())
	{
		;
	};

	virtual ~DualDAO() { };

	inline void Clear()
	{
		dao.Clear();
	}
	inline void SetParam(const string& par, int val)
	{
		dao.SetParam(par, val);
	}
	inline void SetParam(const string& par, const string& val)
	{
		dao.SetParam(par, val);
	}

    //��ѯ����
    void jselect(string &outStr, const string &selBy)
    {
		DataBuf out;
		dao.Clear();
    	if ( selBy == "GET_SYSDATE" )   //��ȡ���ݿ�ϵͳʱ��
    	{
			dao.execQueryInternal(out,"SELECT TO_CHAR(SYSDATE,'YYYY-MM-DD HH24:MI:SS') OUTSTR FROM DUAL");
		}
		else if ( selBy == "GET_SYSDATE_ADDMONTHS" )   //
    	{
			dao.execQueryInternal(out,"SELECT TO_CHAR(ADD_MONTHS(SYSDATE,:VMONTHS),'YYYY-MM-DD HH24:MI:SS') OutStr FROM DUAL");
		}
		else if ( selBy == "GET_DAY_ADD" )   //
    	{
			dao.execQueryInternal(out,"SELECT TO_CHAR(NVL(TO_DATE(:VDATE,'YYYY-MM-DD HH24:MI:SS'),sysdate)+:VNUM,'YYYY-MM-DD HH24:MI:SS') OutStr FROM DUAL");
		}
		else if ( selBy == "GET_MONTH_ADD")
		{
		    dao.execQueryInternal(out,"SELECT to_char(add_months(trunc(NVL(to_date(:VDATE, 'YYYY-MM-DD HH24:MI:SS'),SYSDATE),'MM'),:VNUM),'YYYY-MM-DD HH24:MI:SS') OutStr FROM DUAL");
		}
		else if ( selBy == "GET_ODDSDATES" )   //��ȡ����ʱ����ϵͳʱ���ֵ
    	{
			dao.execQueryInternal(out,"SELECT TO_CHAR(TRUNC(SYSDATE) - TRUNC(TO_DATE(:VDATE,'YYYY-MM-DD HH24:MI:SS'))) OutStr FROM DUAL");
		}

		else if ( selBy == "GET_FIRSTDAYOFNEXTMONTH" )   //��ȡ���ݿ����µ�һ��
    	{
			dao.execQueryInternal(out,"SELECT TO_CHAR(TRUNC(ADD_MONTHS(SYSDATE,1), 'mm'),'YYYY-MM-DD HH24:MI:SS') OutStr FROM DUAL");
		}
		else if ( selBy == "GET_FIRSTDAYOFNEXTMONTHS" )   //��ȡN+1�µ�һ��
    {
			dao.execQueryInternal(out,"SELECT TO_CHAR(TRUNC(ADD_MONTHS(TO_DATE(:VDATE,'YYYY-MM-DD HH24:MI:SS'),1), 'mm'),'YYYY-MM-DD HH24:MI:SS') OutStr FROM DUAL");
		}
		else if ( selBy == "GET_LASTDAYOFCURMONTH" )   //��ȡ����ĩʱ��
    	{
			dao.execQueryInternal(out,"SELECT TO_CHAR(TRUNC(ADD_MONTHS(SYSDATE,1), 'mm') - 1/24/3600,'YYYY-MM-DD HH24:MI:SS') OutStr FROM DUAL");
		}
		else if ( selBy == "GET_LASTDAYOFNMONTHS" )   //��ȡN��ĩʱ�� add by zhoush @ 2005-11-19
    	{
			dao.execQueryInternal(out,"SELECT TO_CHAR(TRUNC(ADD_MONTHS(TO_DATE(:VDATE,'YYYY-MM-DD HH24:MI:SS'),:VNUM), 'mm') - 1/24/3600,'YYYY-MM-DD HH24:MI:SS') OutStr FROM DUAL");
		}
		else if ( selBy == "GET_NUM_SYSDATE" )  //��ȡ��ǰʱ���ȥ1900-1-1������
    	{
			dao.execQueryInternal(out,"SELECT TO_CHAR(TRUNC(SYSDATE) - TO_DATE('19000101','YYYYMMDD')) OUTSTR FROM DUAL");
		}
		else if ( selBy == "GET_NUM_DATE" )  //��ȡ����ʱ���ȥ1900-1-1������
    	{
			dao.execQueryInternal(out,"SELECT TO_CHAR(TRUNC(TO_DATE(:VINPUT_DATE,'YYYY-MM-DD HH24:MI:SS')) - TO_DATE('19000101','YYYYMMDD')) OUTSTR FROM DUAL");
		}
		else if ( selBy == "SEQUENCE" ) //��ȡ���к�
		{
			dao.execQueryInternal(out,"SELECT f_sys_getseqid(:VEPARCHY_CODE,:VSEQUENCENAME) OUTSTR FROM dual");
		}
		else if ( selBy == "GET_DEPARTID_BY_STAFFID" ) //����Ա����ȡ��������   jianghp@20050601 add
		{
			dao.execQueryInternal(out,"select depart_id OUTSTR from td_m_staff where staff_id=:VSTAFF_ID");
		}
		else if ( selBy == "GET_CITYCODE_BY_DEPARTID" ) //���ݲ��Ż�ȡ����ҵ����   jianghp@20050601 add
		{
			dao.execQueryInternal(out,"select area_code OUTSTR from td_m_depart where depart_id=:VDEPART_ID and validflag='0'");
		}
		else if ( selBy == "GET_CODESETNAME" ) //��ȡ�������뼯��Ӧ�����Ƽ�   jianghp@20050601 add
		{
			dao.execQueryInternal(out,"select f_sys_getcodesetname(:CODE_TYPE,:TABLE_NAME,:CODE_FIELD,:NAME_FIELD,:CODE_SET) OUTSTR FROM dual");
		}
		else if ( selBy == "GET_START_DATE_BY_MONTH" ) //���߽���ʱ������ò���ȡ�ÿ�ʼʱ��  songzj@20050625 add
		{
			dao.execQueryInternal(out,"select TO_CHAR(add_months(TO_DATE(:VEND_DATE,'YYYY-MM-DD HH24:MI:SS'), \
                           -NVL((SELECT TAG_NUMBER FROM TD_S_TAG WHERE TAG_CODE=:VTAG_CODE AND USE_TAG='1'),3) \
                           ) \
                 ,'YYYY-MM-DD HH24:MI:SS') OUTSTR FROM DUAL ");
		}
		else if ( selBy == "GET_USERPWD_RANDOM" ) //ȡ�û�����������   zhangzh@20050808 add
		{
			dao.execQueryInternal(out,"SELECT rpad(floor(abs(DBMS_RANDOM.RANDOM)/10000),6,0) OUTSTR FROM dual");
		}
		else if ( selBy == "GET_DBUSER" ) //ȡ�û�����������   zhangzh@20050808 add
		{
			dao.execQueryInternal(out,"select user OutStr from dual");
		}

		if(out.IsFldExist("OUTSTR")){
			outStr = out.GetString("OUTSTR");
		}
		
	}

	//��������ѯ
	void jselect(int &outi, const string &selBy)
	{
		DataBuf out;
		dao.Clear();
		if ( selBy == "GET_ACYCID" ) //��ȡ��ǰ����
		{
			dao.execQueryInternal(out,"select a.cycle_id OUTINT FROM td_b_cycle a WHERE SYSDATE BETWEEN a.cyc_start_time AND a.cyc_end_time");
		}
		else if ( selBy == "GET_ACYCID_MAXNOT" ) //��ȡ��Сδ��������
		{
			dao.execQueryInternal(out,"select min(cycle_id) OUTINT from td_b_cycle WHERE use_tag='0'");
		}
		else if ( selBy == "GET_ACYCID_BY_DATE" ) //����ʱ���ȡ����
		{
			dao.execQueryInternal(out,"select cycle_id OUTINT from td_b_cycle where TO_DATE(:VDATE,'YYYY-MM-DD HH24:MI:SS') >= cyc_start_time and TO_DATE(:VDATE,'YYYY-MM-DD HH24:MI:SS')< cyc_end_time");
		}
		else if ( selBy == "GET_NEXT_CYCLE" )
		{
		    dao.execQueryInternal(out,"select CYCLE_ID OUTINT from td_b_cycle where (SELECT cyc_end_time + 1/24/3600 FROM TD_B_CYCLE a WHERE SYSDATE BETWEEN a.cyc_start_time and a.cyc_end_time) between cyc_start_time and cyc_end_time");
		}
		else if ( selBy == "GET_LAST_CYCLE" )
		{
		    dao.execQueryInternal(out,"select CYCLE_ID OUTINT from td_b_cycle where (SELECT cyc_start_time - 1/24/3600 FROM TD_B_CYCLE a WHERE SYSDATE BETWEEN a.cyc_start_time and a.cyc_end_time) between cyc_start_time and cyc_end_time");
		}
		
		if(out.IsFldExist("OUTINT")){
			outi = out.GetInt("OUTINT");
		}
	}
	
	
	void getSeqId(const string & eparchyCode,
							const string & seqName,int size, vector<string> & vSeqId)
	{
		DataBuf out;
		dao.Clear();
		dao.SetParam(":VEPARCHY_CODE",eparchyCode);	   
		dao.SetParam(":VSEQ_NAME",seqName);
		dao.SetParam(":VNUM",size);
		string sql = "SELECT f_sys_getseqid(:VEPARCHY_CODE,:VSEQ_NAME) ID FROM TD_A_ACYCPARA \
					 WHERE ROWNUM <=:VNUM ";
		dao.execQueryInternal(out,sql);
		
		if(out.IsFldExist("ID")){
			int c=out.GetCount("ID");
			for(int i=0;i<c;i++){
				vSeqId.push_back(out.GetString("ID",i));
			}
			
		}
		
	}
	
	
	//��������ѯ
	int getDifTime(const string &time)
	{
		DataBuf out;
		dao.Clear();
		dao.SetParam(":VTIME", time);
		dao.execQueryInternal(out,"SELECT (sysdate - to_date(to_char(:VTIME),'yyyy-mm-dd hh24:mi:ss')) * 100000  OUTINT FROM DUAL");

		if(out.IsFldExist("OUTINT")){
			return out.GetInt("OUTINT");
		}else{
			return 0;
		}

	}
	

private:
	StaticDAO& dao;

};


} // namespace MiscDom

#endif /* __DualDAO_H_ */
