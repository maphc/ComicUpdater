#ifndef _RecordProcess_H_
#define _RecordProcess_H_


#include "stdafx.h"

class DbInterface;
class MmInterface;


/**
* RecordProcess:���ϼ�¼������,�ṩ���ϵ�����¼����Ĺ��ܡ�
* 	RecordProcess:		���ϼ�¼�����๹�캯��
*	~RecordProcess:		���ϼ�¼��������������
*	setLoadTable:		�����������϶�Ӧ��Ŀ������Ϣ
*	setLoadDatabase:	�����������϶�Ӧ��Ŀ�����Ϣ
*	processRecord:		���(����������)�������ϼ�¼���봦��
*	clearIT:			���(������ȫ��)�ؼ����漰���¼ɾ��
*/
class RecordProcess
{
	public:
		
		/**
		* RecordProcess:���ϼ�¼�����๹�캯��
		* @param	��
		* @return
		*/
		RecordProcess();
		
		/**
		* RecordProcess:���ϼ�¼�����๹�캯��
		* @param	loadtable	�������϶�Ӧ��Ŀ������Ϣ
		* @return
		*/
		RecordProcess(LoadTable* loadtable);
		
		/**
		* ~RecordProcess:���ϼ�¼��������������
		* @param	��
		* @return
		*/
		~RecordProcess();
		
		/**
		* setLoadTable:�����������϶�Ӧ��Ŀ������Ϣ
		* @param	loadtable	�������϶�Ӧ��Ŀ������Ϣ
		* @return	��
		*/
		void setLoadTable(LoadTable* loadtable);
		
		/**
		* setLoadDatabase:�����������϶�Ӧ��Ŀ�����Ϣ
		* @param	objdbis		ָ��Ŀ���������ݿ�����༯�ϵ�ָ��
		* @param	objmmis		ָ��Ŀ���ڴ����ݿ�����༯�ϵ�ָ��
		* @param	objdbisPro	ʡ���Ŀ���Ҫ����Ķ�Ӧ��Ŀ������⼯��Ϣ
		* @param	objmmisPro	ʡ���Ŀ���Ҫ����Ķ�Ӧ��Ŀ���ڴ�⼯��Ϣ		
		* @return	��
		*/
		void setLoadDatabase(vector<DbInterface*>* objdbis, vector<MmInterface*>* objmmis, vector<DbInterface*>* objdbisPro, vector<MmInterface*>* objmmisPro);
		
		/**
		* processRecord:���(����������)�������ϼ�¼���봦��(���쳣)
		* @param	record		ָ��(����������)�������ϼ�¼��ָ��
		* @return	��
		*/
		void processRecord(Record* record);
		
		/**
		* clearIT:���(������ȫ��)�ؼ����漰���¼ɾ��(���쳣)
		* @param	allobjdbis	�����������漰�����е�Ŀ���������ݿ�����༯��
		* @param	allobjmmis	�����������漰�����е�Ŀ���ڴ����ݿ�����༯��
		* @param	keyid		���������ϵĹؼ���
		* @return	��
		*/
		void clearIT(Record* record);

		/**
		* clearIT_N:���(������ȫ��)�ؼ����漰���¼ɾ��(���쳣)
		* @param	allobjdbis	�����������漰�����е�Ŀ���������ݿ�����༯��
		* @param	allobjmmis	�����������漰�����е�Ŀ���ڴ����ݿ�����༯��
		* @param	keyid		���������ϵĹؼ���
		* @return	��
		*/
		void clearIT_N(Record* record);
		
		
		void clearKeySet();
				
	private:
	
		/**
		* batchInsert:��������ȫ����Ϣ
		* @param	filetype	�����ļ����� 0:���������� 1:������ȫ�� 2:ȫ����ȫ��;���ֻ���ܳ���1,2
		* @param	info		ָ��(��/ȫ����ȫ��)�������ϼ�¼��ָ��
		* @return	��
		*/
		void batchInsert(Info* info);
		
		/**
		* deleteRec:���ݵ������ϼ�¼����ɾ��
		* @param	record	ָ��(����������)�������ϼ�¼��ָ��
		* @return	��
		*/
		void deleteRec(Record* record);
		
		/**
		* insert:���ݵ������ϼ�¼���в���
		* @param	record	ָ��(����������)�������ϼ�¼��ָ��
		* @return	��
		*/
		void insert(Record* record);
		
		/**
		* update:���ݵ������ϼ�¼���и���
		* @param	record	ָ��(����������)�������ϼ�¼��ָ��
		* @return	��
		*/
		void update(Record* record);
		
		/**
		* isExist:�жϸü�¼�����ݿ����Ƿ����
		* @param	dbtype	���ݿ����� 0:�������ݿ� 1:�ڴ����ݿ�
		* @param	record	ָ���¼��ָ��
		* @param	dbi		ָ���������ݿ�������ָ��
		* @param	mmi		ָ���ڴ����ݿ�������ָ��						
		* @return	���ظü�¼�����ݿ����Ƿ���ڵĽ�� true:���� false:������
		*/		
		bool isExist(const int &dbtype, Record* record, DbInterface* dbi, MmInterface* mmi);
		
		/**
		* getdbparamvalue:���������͡���ȡֵ��ʽ������Դֵ����Ӧ�������������ݸ�ֵ
		* @param	dbcolumntype	������
		* @param	dbgetvalue		��ȡֵ��ʽ
		* @param	dbcolumnvalue	��Դֵ
		* @param	dbparam			��������		
		* @return	��
		*/
		void getdbparamvalue(const PROCPARAM_TYPE &dbcolumntype, const char &dbgetvalue, const string &dbcolumnvalue, DbParam &dbparam);
		
		/**
		* getmmparamvalue:���������͡���ȡֵ��ʽ������Դֵ����Ӧ�������������ݸ�ֵ
		* @param	mmcolumntype	������
		* @param	mmgetvalue		��ȡֵ��ʽ
		* @param	mmcolumnvalue	��Դֵ
		* @param	mmparam			�ڴ�����		
		* @return	��
		*/
		//void getmmparamvalue(const COLUMN_VAR_TYPE &mmcolumntype, const char &mmgetvalue, const string &mmcolumnvalue, MmParam &mmparam);
		
		/**
		* isProvinceInfo:�жϸü�¼�Ƿ��ǿ���м��ſͻ���Ϣ
		* @param	record	ָ��(����������)�������ϼ�¼��ָ��	
		* @return	true:����м�����Ϣ false:������Ϣ
		*/		
		bool isProvinceInfo(Record* record);

		
	protected:
		
		//��Ҫ��������϶�Ӧ��Ŀ������Ϣ
		LoadTable				*m_loadtable;
		//��Ҫ��������϶�Ӧ��Ŀ������⼯��Ϣ
		vector<DbInterface*>	*m_objdbis;
		//��Ҫ��������϶�Ӧ��Ŀ���ڴ�⼯��Ϣ
		vector<MmInterface*>	*m_objmmis;
		
		//ʡ���Ŀ���Ҫ����Ķ�Ӧ��Ŀ������⼯��Ϣ
		vector<DbInterface*>	*m_objdbisPro;
		//ʡ���Ŀ���Ҫ����Ķ�Ӧ��Ŀ���ڴ�⼯��Ϣ
		vector<MmInterface*>	*m_objmmisPro;
		
		set<string>				m_keySet;
	 public :// �������Ӳ��� wangfc@lianchuang.com
	  bool              m_batchIntoBilling ;//�Ƿ�����������־
	  bool              m_ifProvinceMode ;
	  void              setbatchIntoBillingMode(bool batchIntoBilling)
	  {
	    m_batchIntoBilling = batchIntoBilling;
	  }
	  bool              getbatchIntoBillingMode()
	  {
	    return m_batchIntoBilling ;
	  }
	  //ʡ����ģʽ
  	void setProvinceMode(bool bisProv)
  	 {
  	   m_ifProvinceMode = bisProv;
  	 }
    
};


#endif

