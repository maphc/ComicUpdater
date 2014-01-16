#ifndef _BATCHINDBPROCESS_H_
#define _BATCHINDBPROCESS_H_
#include "stdafx.h"
/**
 ** �����������ʱ�����ô洢������������������ȹ���
**/
class DestRuleParse;

class DbInterface;

class BatchInDBProcess
{
  
  public:
    BatchInDBProcess();
    ~BatchInDBProcess();
  private:
		//ָ������Ŀ������������ָ��
		DestRuleParse				    *m_destRuleParse;
		
		//ʡ���Ŀ����϶�Ӧ���������ݿ�
		vector<DbInterface*>		*m_dbisPro;
		
		LoadTable               *m_loadtables;//���ñ�
		
		string                   m_filename ;	//��ǰ������ļ���	
	public:	  
		/**
		* init:������Ϣ������ĳ�ʼ����,����Ŀ�����������ָ��
		* @param	destRuleParse	ָ������Ŀ������������ָ��
		* @param	dbisPro			ʡ���Ŀ����϶�Ӧ���������ݿ�
		* @return	��
		*/	  
	  void init(LoadTable *loadtables,DestRuleParse *destRuleParse, 
	            vector<DbInterface*> *dbisPro);
	
	  /**
		* processRecord:���(����������)��������Ϣ���봦��(���쳣)
		* @param	chgid	�����ˮ��
		* @param	info	���α���޸ĵ���������������
		* @return	��
		*/	
		
		void processRecord(string &chgid, int &infotype, int &srcdbnum, Info *info);
		/**
		* indb :����������ݵ���ʽ���(���쳣)
		* @param ��
		* @return ��
		**/	    
		
		void indb(int& srcdbNum);
		/**
		* inserttable :���������������ʱ�����쳣��
		* @param infotype �������
		* @param maxrecordsize ����ύ��¼
		**/
		
		void insertTable(int& infotype,int& srcdbnum ,bool isprov ) ;
		/**
		* isProvinceInfo :�ж��Ƿ���ʡ���Ŀ�
		* @param record ������¼
		**/		
		
		bool isProvinceInfo(Record* record);
		/*
		 * getinsertsql �������ʱ���insert���
		 * @infotype
		 * @srcdbnum
		 * @tablenames
		*/
		
		string getinsertsql(int& infotype,int& srcdbnum,string& tablenames );
		/*
		 * executedbproc ִ�д洢����
		 * @dbi :�����ʵ��
		 * @infotype 
		 * @srcdbnum    ���
		 * @procname    �洢��������
		*/
		
		bool executedbproc(DbInterface* dbi ,int& infotype,int& srcdbnum,string& procname );
	
		/*
		* setfilename           �����ļ���
		* @filename             �ļ���
		*/
		void  setfilename(const char* filename );
		/*
		 *setchannelno          ����ͨ����
		 *@ rchannelno          ͨ����
		*/
		void  setchannelno ( int & rchannelno );
		/*
		 * setMaxBatchRecordNum  �������������ύ��Ŀ
		 * @maxrum  ��Ŀ
		*/
		void  setMaxBatchRecordNum(int & maxrum);
		
		/*
		* �����Ƿ���ʡ����ģʽ
		* @bisProv �Ƿ���ʡ����ģʽ
		*/
		void  setProvinceMode(bool bisProv);
    
  private:
    
    bool  m_ifProvinceMode;       // �Ƿ���ʡ����ģʽ    
    int   m_maxBatchRecordNum;    // ����ύ����ʱ��ļ�¼��Ŀ   
    int   m_channelNo ;           // ͨ����    
};

#endif