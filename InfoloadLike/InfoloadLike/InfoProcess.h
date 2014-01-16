#ifndef _InfoProcess_H_
#define _InfoProcess_H_


#include "stdafx.h"


class RecordProcess;
class DestRuleParse;
class DbInterface;
class MmInterface;

/**
* InfoProcess:������Ϣ������,�ṩ���϶�����¼����Ĺ��ܡ�
* 	InfoProcess:		������Ϣ�����๹�캯��
*	~InfoProcess:		������Ϣ��������������
*	init:				������Ϣ������ĳ�ʼ����,����װ�ر���Ϣ����Ŀ�����������ָ��
*	processTrade:		���(����������)��������Ϣ���봦��
* 	getRecordProcess:	���ݱ������Ϣ���ͻ�ȡָ��������Ϣ��¼�������ָ��
*/
class InfoProcess
{
	public:
		
		/**
		* InfoProcess:������Ϣ�����๹�캯��
		* @param	��
		* @return
		*/
		InfoProcess();
		
		/**
		* ~InforProcess:������Ϣ��������������
		* @param	��
		* @return
		*/		
		~InfoProcess();
		
		/**
		* init:������Ϣ������ĳ�ʼ����,����װ�ر���Ϣ����Ŀ�����������ָ��
		* @param	loadtables		ָ��������Ҫװ�ص�������Ϣ���鼯�ϵ�ָ��
		* @param	destRuleParse	ָ������Ŀ������������ָ��
		* @param	dbisPro			ʡ���Ŀ����϶�Ӧ���������ݿ�
		* @param	mmisPro			ʡ���Ŀ����϶�Ӧ���ڴ����ݿ�
		* @return	��
		*/
		void init(LoadTable *loadtables, DestRuleParse *destRuleParse, vector<DbInterface*> *dbisPro, vector<MmInterface*> *mmisPro);
		
		/**
		* processTrade:���(����������)��������Ϣ���봦��(���쳣)
		* @param	chgid	�����ˮ��
		* @param	info	���α���޸ĵ���������������
		* @return	��
		*/	
		void processTrade(string &chgid, int &infotype, const int &srcdbnum, Info *info);
		
		
	private:
		
		/**
		* getRecordProcess:���ݱ������Ϣ���ͻ�ȡָ��������Ϣ��¼�������ָ��
		* @param	infotype	�������Ϣ����
		* @return	ָ��������Ϣ��¼�������ָ��
		*/		
		RecordProcess *getRecordProcess(const int &infotype);
		
		
	private:
		
		//ָ��������Ҫװ�ص�������Ϣ���鼯�ϵ�ָ��
		LoadTable					*m_loadtables;
		//ָ������Ŀ������������ָ��
		DestRuleParse				*m_destRuleParse;
		//�����Ϣ���ͺ����ϼ�¼����ָ�������
		map<int, RecordProcess*>	m_recordProcessMap;
		
		//ʡ���Ŀ����϶�Ӧ���������ݿ�
		vector<DbInterface*>		*m_dbisPro;
		//ʡ���Ŀ����϶�Ӧ���ڴ����ݿ�
		vector<MmInterface*>		*m_mmisPro;
};


#endif

