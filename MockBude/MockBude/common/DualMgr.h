#ifndef DualMGR_H_HEADER_INCLUDED_BE13A0DE
#define DualMGR_H_HEADER_INCLUDED_BE13A0DE

#include <iostream>
#include <string>
#include <stdio.h>
#include "../base/config-all.h"

USING_NAMESPACE(std)

/**
 *  ������ˮ��
 */
extern "C" void GenerateID(CFmlBuf &inBuf,CFmlBuf &outBuf);

/**
 *  ��ȡ��ǰϵͳʱ��
 */
extern "C" void GetSysDate(CFmlBuf &inBuf,CFmlBuf &outBuf);

/**
 *  ��ȡ���µ�һ���ʱ��
 */
extern "C" void GetFirstDayOfNextMonth(CFmlBuf &inBuf,CFmlBuf &outBuf);

/**
 *  ��ȡ�������һ���ʱ��
 */
extern "C" void GetLastDayOfCurMonth(CFmlBuf &inBuf,CFmlBuf &outBuf);

/**
 *  ��ȡ����
 */
extern "C" void GetAcycId(CFmlBuf &inBuf,CFmlBuf &outBuf);

/**
 *  ����Ա����ȡ��������        jianghp@20050601 add
 */
extern "C" void GetDepartidByStaffid(CFmlBuf &inBuf,CFmlBuf &outBuf);

/**
 *  ���ݲ��Ż�ȡ����ҵ����        jianghp@20050601 add
 */
extern "C" void GetCitycodeByDepartid(CFmlBuf &inBuf,CFmlBuf &outBuf);

/** @class DualMgr
 *  @brief Dual������
 *  �ṩ�����ݿ�Dual�����в������������ȡ���С���ǰ���ڵȡ�
 */
//##ModelId=41EC613D001E
class DualMgr
{
    public:
        /**
         * ���ݴ����ID����ͨ�����ݿ���������ɸ���ID
         * @param eparchyCode ������б����ַ���������
         * @param SequenceName �������������ַ���������
         * @param SequenceId ������к��ַ���������
         * @return �ޣ�ʧ��ʱ�׳�����
         */
        //##ModelId=41EC62590239
        void GenerateID(string &eparchyCode, string &sequenceName, string &sequenceId);

        /**
         *  ��ȡ����ʱ����ϵͳʱ���ֵ
         */
        static void GetOddsDays(const string &strCurdate,string &strStartDateTim);

        /**
         * ��ȡϵͳʱ��
         * @param strSysdate ���ϵͳʱ���ַ���������
         * @return �ޣ�ʧ��ʱ�׳�����
         */
        void GetSysDate(string &strSysdate);

        /**
         * ��ȡ����
         * @param iAcycId ������ڵ�����
         * @param getMode ��ȡ��ʽ��0-��ȡ��ǰ���ڣ�1-��ȡ��Сδ�������ڣ�2-����ʱ���ȡ����
         * @param parameter getMode=2ʱ������ʱ��
         * @return �ޣ�ʧ��ʱ�׳�����
         */
        void GetAcycId(int &iAcycId, const int &getMode, const string &parameter);

        /**
         * ��ȡ���µ�һ��
         * @param strDateTime ���ʱ���ַ���������
         * @return �ޣ�ʧ��ʱ�׳�����
         */
        static void GetFirstDayOfNextMonth(string &strDateTime);

        /**
         * ��ȡָ��ʱ�䣫n���ʱ��
         */
        static void AddDays(const string &strStartDateTime,const int &iDays, string &strResultDateTime);

        /**
         * ��ȡָ��ʱ�䣫n���ʱ��
         */
        static void AddDays(const string &strStartDateTime,const float &fDays, string &strResultDateTime);
        
        /**
         * ��ȡָ��ʱ�䣫n���·�
         */
        void AddMonths(const string &strStartDateTime,const int &iMonths, string &strResultDateTime);

        /**
         * ��ȡָ��ʱ�䣫n���·�(��ȷ����)
         */
        void AddMonthsExact(const string &strStartDateTime,const int &iMonths, string &strResultDateTime);


        /**
         * ��ȡ�������һ��
         * @param strDateTime ���ʱ���ַ���������
         * @return �ޣ�ʧ��ʱ�׳�����
         */
        void GetLastDayOfCurMonth(string &strDateTime);

        /**
         * ���ݴ����Ա�������ȡ��������   jianghp@20050601 add
         * @param strStaffId ����Ա������
         * @param strDepartId �����������
         * @return �ޣ�ʧ��ʱ�׳�����
         */
        void GetDepartidByStaffid(const string &strStaffId, string &strDepartId);

        /**
         * ���ݴ���Ĳ��ű����ȡ����ҵ����   jianghp@20050601 add
         * @param strDepartId ���벿�ű���
         * @param strCityCode �������ҵ����
         * @return �ޣ�ʧ��ʱ�׳�����
         */
        void GetCitycodeByDepartid(const string &strDepartId, string &strCityCode);
        
		/**
		 *  ��ȡN��ĩʱ��	add by zhoush @ 2005-11-17
		 */
		void GetLastDayOfNMonths(const string &strInputDate,const int &iMonths,string &strResultDateTime);
		/**  
		 *���ڼ�N����
 		*/  
	  	void GetAcycIdOfNMonths(int &iAcycId,const int &iMonths);
};



#endif /* DualMGR_H_HEADER_INCLUDED_BE13A0DE */
