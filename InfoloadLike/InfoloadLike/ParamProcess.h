#ifndef _ParamProcess_H_
#define _ParamProcess_H_


#include "InfoPublic.h"


class DbInterface;
class MmInterface;


/**
* ParamProcess:�������ݴ�����,��ɲ�����ȫ�����ݵ����봦��
* 	ParamProcess:	�������ݴ����๹�캯��
*	~ParamProcess:	�������ݴ�������������
*	init:			�������ݴ�����ĳ�ʼ����,��������Դ���������ݿ������Ŀ����ڴ����ݿ⼯��
*	process:		�����������봦��
*/
class ParamProcess
{
	public:
		
		/**
		* ParamProcess:�������ݴ����๹�캯��
		* @param	��
		* @return
		*/
		ParamProcess();
		
		/**
		* ~ParamProcess:�������ݴ�������������
		* @param	��
		* @return
		*/
		~ParamProcess();

		/**
		* init:�������ݴ�����ĳ�ʼ����,��������Դ���������ݿ������Ŀ����ڴ����ݿ⼯��
		* @param	srcdbi	ָ������Դ���������ݿ�������ָ��
		* @param	objmmis	ָ������Ŀ����ڴ����ݿ�����༯�ϵ�ָ��
		* @return	��
		*/
		void init(DbInterface *srcdbi, vector<MmInterface*> *objmmis);

		/**
		* process:�����������봦��(���쳣)
		* @param	dealtype	���뷽ʽ 0:��TRUNCATE,Ȼ��ȫ������INSERT���롣1:ֻ��UPDATE��INSERT��������������UPDATE��û�е�����INSERT��
		* @param	selectsql	Դ���ݲ�ѯSQL:��ͨ��ѯSQL���������뷽ʽ��1��ʱ�����WHERE��������ض���UPDATE_TIME>=to_date(:1,'yyyymmddhh24miss') AND UPDATE_TIME<to_date(:2,'yyyymmddhh24miss')
		* @param	tablename	Ŀ�����
		* @param	updatesql	Ŀ�����SQL�����涨���ʽΪ��indexname[column1,column2,��,columnN]| expr[column1>=:1 AND .. AND columnN>:N]| update[column1,column2,��,columnN]
		* @param	starttime	��ʼʱ��,��ʽ"yyyymmddhh24miss"
		* @param	endtime		��ֹʱ��,��ʽ"yyyymmddhh24miss"
		* @return	��
		*/
		void process(const char& dealtype, const string& selectsql, const string& tablename, 
					 const string& updatesql, const string& starttime, const string& endtime);
    //�����ύ�ӿ�  (ֻʹ����IPC���⣩
    void  processbatch (const char& dealtype, const string& selectsql, const string& tablename, 
					 const string& updatesql, const string& starttime, const string& endtime);
   //��Ϣ��ʾ
    void  showPromtMessage(const char* cMsg);
    void  showErrorMessage(const char* cMsg );
   
	private:
		
		//ָ������Դ���������ݿ�������ָ��
		DbInterface				*m_srcdbi;
		//ָ������Ŀ����ڴ����ݿ�����༯�ϵ�ָ��
		vector<MmInterface*>	*m_objmmis;

};


#endif

