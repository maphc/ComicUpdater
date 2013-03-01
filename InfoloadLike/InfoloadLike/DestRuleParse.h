#ifndef _DestRuleParse_H_
#define _DestRuleParse_H_


#include "stdafx.h"

class DbInterface;
class MmInterface;


/**
* DestRuleParse:����Ŀ���ж���,�������Ͼ������ϵ�Ŀ�����Ϣ��
* 	DestRuleParse:	����Ŀ���ж��๹�캯��
*	~DestRuleParse:	����Ŀ���ж�����������
*	init:			����Ŀ���ж���ĳ�ʼ����,����Ŀ����򼯺Ϻ�Ŀ�������༯��ָ��
*	getallobjdbis:	�����������ͻ�ȡ���е�Ŀ������������
*	getallobjmmis:	�����������ͻ�ȡ���е�Ŀ���ڴ�������
*	getobjdbi:		�����������͡����б����ȡ���е�Ŀ������������
*	getobjmmi:		�����������͡����б����ȡ���е�Ŀ���ڴ�������
*/
class DestRuleParse
{
	public:

		/**
		* InfoProcess:������Ϣ�����๹�캯��
		* @param	��
		* @return
		*/		
		DestRuleParse();

		/**
		* InfoProcess:������Ϣ�����๹�캯��
		* @param	��
		* @return
		*/		
		~DestRuleParse();

		/**
		* InfoProcess:������Ϣ�����๹�캯��
		* @param	��
		* @return
		*/		
		void init(map<string, DbInterface*> *objdbiMap, map<string, MmInterface*> *objmmiMap, vector<DestRule> *destruleVector);		

		/**
		* InfoProcess:������Ϣ�����๹�캯��
		* @param	��
		* @return
		*/
		//void getallobjdbis(const int &infotype, vector<DbInterface*> &objdbis);

		/**
		* InfoProcess:������Ϣ�����๹�캯��
		* @param	��
		* @return
		*/		
		//void getallobjmmis(const int &infotype, vector<MmInterface*> &objmmis);

		/**
		* InfoProcess:������Ϣ�����๹�캯��
		* @param	��
		* @return
		*/		
		void getobjdbi(const int &infotype, const int &srcdbnum, vector<DbInterface*> &objdbis);

		/**
		* InfoProcess:������Ϣ�����๹�캯��
		* @param	��
		* @return
		*/		
		void getobjmmi(const int &infotype, const int &srcdbnum, vector<MmInterface*> &objmmis);
		
//modify by chenxt 20081024
//private:
	public:
		//��������ƺ������������Ӧ��ϵ����
		map<string, DbInterface*>	*m_objdbiMap;
		//�ڴ�����ƺ��ڴ��������Ӧ��ϵ����
		map<string, MmInterface*>	*m_objmmiMap;
		//��������Ŀ����򼯺�(�Ѿ�����)
		vector<DestRule>			*m_destruleVector;
  public:		
    //�������Ӳ���  wangfc@lianchuang.com 2010.03.29
    /*
     *@getobj_db_deal ȡĳ��infotype�����Ŀ���Ĵ���ʽ  0 ���������  1 ��������� 
    */
    int  getobj_db_deal(int& infotype,const int& srcdbnum );
    /*
      @getAllDestRule ȡĳ���������е�����Ŀ�����
    */
    void getAllDestRule (const int &srcdbnum,vector<DestRule> &destrule);
    /*
      @getDestRule ����infotype��srcdbnumȡ��ָ��destrule
    */
    void getDestRule(int& infotype ,int& srcdbnum,DestRule& destrule );
    
};


#endif

