#ifndef _DbInterface_H_
#define _DbInterface_H_

#include "stdafx.h"

using namespace oracle::occi;

/**
* DbInterface:�������ݿ������,�ṩ���������ݿ�ĸ��ֲ�����
* 	DbInterface:			�������ݿ�����๹�캯��
*	~DbInterface:			�������ݿ��������������
*	connect:				�������������ݿ������
*	disconnect:				�Ͽ����������ݿ������
*	beginTrans:				��������
*	commitTrans:			�ύ����
*	rollbackTrans:			�ع�����
*	execStaticSql:			ִ�о�̬SQL
*	execDynamicSql:			ִ�ж�̬SQL
*	execProcedure:			ִ�д洢����
*	getNext:				��ȡһ�м�¼
*	getColumnNum:			��ȡ���β�ѯ�ļ�¼������
*	getColumnIntValue:		��ȡ��¼�е�����ֵ
*	getColumnLongValue:		��ȡ��¼�еĳ�����ֵ
*	getColumnFloatValue:	��ȡ��¼�еĸ���ֵ
*	getColumnDateValue:		��ȡ��¼�е�����ֵ
*	getColumnStringValue:	��ȡ��¼�е��ַ�ֵ 
*	getColumnToStringValue: ��ȡ��¼�е�ֵ��ת��Ϊ�ַ�ֵ
*/

class DbInterface
{
	public:
		/**
		* DbInterface:�������ݿ�����๹�캯��
		* @param	��
		* @return
		*/
		DbInterface(const Environment::Mode &mode=Environment::OBJECT);

		/**
		* ~DbInterface:�������ݿ��������������
		* @param	��
		* @return
		*/		
		~DbInterface();
		
		/**
		* connect:�������������ݿ������(���쳣)
		* @param	��
		* @return
		*/		
		void connect(const string& username, const string& password, const string& servname);

		/**
		* disconnect:�Ͽ����������ݿ������(���쳣)
		* @param	��
		* @return
		*/		
		void disconnect();

		/**
		* beginTrans:��������
		* @param	��
		* @return
		*/
		void beginTrans();

		/**
		* commitTrans:�ύ����(���쳣)
		* @param	��
		* @return
		*/				
		void commitTrans();

		/**
		* rollbackTrans:�ع�����(���쳣)
		* @param	��
		* @return
		*/		
		void rollbackTrans();
		
		/**
		* execStaticSql:ִ�о�̬SQL(���쳣)
		* @param	��
		* @return
		*/
		int execStaticSql(const string& staticsql);
		
		/**
		* execDynamicSql:ִ�ж�̬SQL(���쳣);params�����seq˳������dynamicsql�������ֵ�˳��һ��,��MM����ܴ�
		* @param	��
		* @return
		*/
		int execDynamicSql(const string& dynamicsql, const vector<DbParam>& params);
				
		/**
		* execProcedure:ִ�д洢����(���쳣)
		* @param	��
		* @return
		*/
		void execProcedure(const string& procedurename, vector<DbParam>& params);
		
		/**
		* getNext:��ȡһ�м�¼(���쳣)
		* @param	��
		* @return
		*/
		bool getNext();
		
		/**
		* getColumnNum:��ȡ���β�ѯ�ļ�¼������
		* @param	��
		* @return
		*/
		int	 getColumnNum();

		/**
		* getColumnIntValue:��ȡ��¼�е�����ֵ,pos��СΪ1(���쳣)
		* @param	��
		* @return
		*/		
		int getColumnIntValue(const unsigned int& pos);

		/**
		* getColumnLongValue:��ȡ��¼�еĳ�����ֵ,pos��СΪ1(���쳣)
		* @param	��
		* @return
		*/
		long getColumnLongValue(const unsigned int& pos);
				
		/**
		* getColumnFloatValue:��ȡ��¼�еĸ���ֵ,pos��СΪ1(���쳣)
		* @param	��
		* @return
		*/
		float getColumnFloatValue(const unsigned int& pos);

		/**
		* getColumnDateValue:��ȡ��¼�е�����ֵ,pos��СΪ1(���쳣)
		* @param	��
		* @return
		*/		
		string getColumnDateValue(const unsigned int& pos);
		
		/**
		* getColumnStringValue:��ȡ��¼�е��ַ�ֵ,pos��СΪ1(���쳣)
		* @param	��
		* @return
		*/		
		string getColumnStringValue(const unsigned int& pos);
		
		/**
		* getColumnToStringValue:��ȡ��¼�е�ֵ��ת��Ϊ�ַ�ֵ,pos��СΪ1(���쳣)
		* @param	��
		* @return
		*/	
    string getColumnToStringValue(const unsigned int& pos);
public:    
//--------------- ��������(���� ���� ɾ��)��ؽӿ� wangfc@lianchuang.com at 2010.03.29 
    /*
     * createStatement ��������SQL����
    */
    void  createStatement() ;
    /*
     * setSql  ����SQL���  
    */
    void  setSql(char* sSQL);
    /*
     * setMaxIteration �����������������Ŀ
    */
    void  setMaxIteration(const int &iMaxIteration);
    /*
     * addIteration     ������������
    */
    void  addIteration();
    /*
     * setMaxParamSize  ����ÿ�еĲ������Ĵ�С
    */
    void  setMaxParamSize(const int &riPosition, const int &riSize);
    /*
     * setParameter ����ÿһ�е���ֵ
    */
    void  setParameter(const int &iPosition, const int & iValue);
    void  setParameter(const int &iPosition, const char *sValue);
    void  setParameter(const int &iPosition, const string &sValue);
    void  setParameter(const int &iPosition, const long   &lValue);
    void  setParameter(const int &iPosition, const Date   &dataValue);
    //void  setParameter(const int &iPosition, const Number &nValue);
    void  setParameter(const int &iPosition, const double &dValue);    
    void  setParameter(const int &iPosition, const float  &fValue);
    /*
     *    setBatchParameter �������ݴ������� setMaxIteration setParameter��)
    */
    void  setBatchParameter(vector<BatchDbParam>& dbparams,int& rmaxrow);
   /*
    *     executeUpdate  ִ��SQL����
   */
    int   executeUpdate();
    /*
     * executeUpdate(string) ִ��SQL���� ����һ�� add xueyf 20101229;
    */
    void   executeUpdate(const string &str);
    /*
     *    executeQuery  ִ�в�ѯ(����)
    */
    void  executeQuery(const int rmaxrecordsize = DEFAULT_RECORD_NUM) ;
    /*
     *    SetResultDataBuffer ������ѯ���ڴ�ռ��
    */
    void  SetResultDataBuffer(const int& iPosition,void* buf ,const Type & coltype ,const unsigned int & size );
    /*
     *    SetResultDataBuffer ������ѯ���ڴ�ռ�� ����һ�� add xueyf 20101229
    */
    void  SetResultDataBuffer(const int& iPosition,void* buf ,const Type & coltype ,const unsigned int & size ,
                              ub2 *length = NULL,sb2 *ind = NULL, ub2 *rc = NULL);
    void  ResultDataBuffer(const int& iPosition,void* buf ,const Type & coltype ,const unsigned int & size ,
                              ub2 *length = NULL,sb2 *ind = NULL, ub2 *rc = NULL);
    //void  SetResultDataBuffer(const int& iPosition,void* buf ,const Type & coltype ,const unsigned int & size,ub2 *elementLength,sb2 *ind);
    /*
     *    BatchNext  �������ݼ�����
    */
    bool  BatchNext(int& rcount ,const int& rmaxrecordsize = DEFAULT_RECORD_NUM) ;
    /*
     *getColCount ȡ������Ŀ
    */
    int   getColCount();
    /*
     *  getColumnType  ȡ��ָ���е�����
    */
    int   getColumnType(const unsigned int& pos);
    //add xueyf ��ȡ�д�С
    int   getColumnSize(const unsigned int& pos);
    void  execArrayUpdate(const unsigned int& arry_num);
    void  getUpdateNum();
//--------------------------end---------------------------------------
	private:		
		/**
		* strcasestr:��ȡ�ڶ����ַ����ڵ�һ���ַ����״γ��ֵĵط�,����"ACCDDEE","CD";��ô���ص�ָ��ָ��"CDDEE"
		* @param	��
		* @return
		*/
		char* strcasestr(const char *str1,const char *str2);
		
	public:
		//ָ��OCCI��������ݿ⻷�����ָ��
		Environment			*m_env;
		//ָ��OCCI��������ݿ��������ָ��
		Connection			*m_conn;
		//ָ��OCCI��������ݿ�SQLִ�����ָ��
		Statement			*m_stmt;
		//ָ��OCCI��������ݿ��ѯ������ָ��
		ResultSet			*m_rs;
		//���β�ѯ�ļ�¼������
		int					m_columnnum;
		//�Ƿ��������־ 0:������ 1:������
		int       			m_transflag;
	public://�������Ӳ��� wangfc@lianchuang.com 2010.04.10
	  string          m_objalisename;
	  //���ñ���
	  void            setobjalisename(string& objname ) 
	  {
	    this->m_objalisename = objname ;
	  }
	  string          getobjalisename() 
	  {
	    return this->m_objalisename;
	  }
		
};


#endif

