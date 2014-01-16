#include "stdafx.h"
/*
  ��Ҫ������ʡ����ģʽ.
*/

BatchInDBProcess::BatchInDBProcess()
{
  
  m_ifProvinceMode  = false ;//Ĭ�Ϸ�ʡ����ģʽ
}

BatchInDBProcess::~BatchInDBProcess()
{
  
}

void BatchInDBProcess::setProvinceMode(bool bisProv)
{
  m_ifProvinceMode = bisProv;
  
}

void BatchInDBProcess::setMaxBatchRecordNum(int & maxrum)
{
  m_maxBatchRecordNum = maxrum ;
  
}
		
void BatchInDBProcess::init(LoadTable *loadtables,DestRuleParse *destRuleParse, vector<DbInterface*> *dbisPro)
{
   m_loadtables    = loadtables ;
   m_destRuleParse = destRuleParse ;
   m_dbisPro       = dbisPro ;
   
}

//
void BatchInDBProcess::setfilename(const char* filename )
{
  m_filename = filename ;
  
}
    
void BatchInDBProcess::processRecord(string &chgid, int &infotype, int &srcdbnum, Info *info)
{   
  // (1.1) �ж�infotype�Ƿ���Ҫ�������
  int obj_db_deal  = m_destRuleParse->getobj_db_deal( infotype , srcdbnum );
  if ( obj_db_deal == 0 ) return ;
    
#ifdef _DEBUG_		   
   printf("(����)��Ӽ�¼:processRecord(��ʼ)\n");
#endif      

 //  (1.2) �ж϶�Ӧ��Ŀ�����
  DestRule destrule ;
  m_destRuleParse->getDestRule( infotype , srcdbnum , destrule );
  
 //  (1.3)  ͬһinfotype�����ݴﵽ���������Ŀʱ���Զ������⹦�ܣ�����inserttable
 //         �������¿���һƬ�ڴ��Ÿò��ּ�¼���������¼����
  for ( int  i = 0 ; i < info->size() ; ++i )
  {
     Record *record = (*info)[i];  
     
     //  (1.3.1 )  ͬһ��ļ�¼�ﵽ����¼ֵʱ��������ʱ��
     if ( destrule.getCurRecordsize() >= m_maxBatchRecordNum  ) 
        insertTable( infotype,srcdbnum ,false);
        
     //  (1.3.2)   ͬһ��ļ�¼�ﵽ����¼ֵʱ��������ʱ��ʡ���Ŀ⣩
     if ( destrule.getCurRecordProvsize() >= m_maxBatchRecordNum  ) 
        insertTable( infotype,srcdbnum ,true);
        
     //  (1.3.3�� ����¼��ŵ����ػ�����             
     //           ����������뱾�������ݵĴ����ǻ����
      if ( isProvinceInfo(record ))       
         destrule.AddRecordProvData ( m_channelNo , record,m_loadtables[infotype].m_dbcolumns,m_loadtables[infotype].m_dbcolumnnum );         
      else 
         destrule.AddRecordData ( m_channelNo ,record,m_loadtables[infotype].m_dbcolumns,m_loadtables[infotype].m_dbcolumnnum );      
   }  
   
#ifdef _DEBUG_		   
        printf("����)��Ӽ�¼:processRecord(����) CurRecordsize = %ld,CurProvRecordsize = %ld \n",destrule.getCurRecordsize(),destrule.getCurRecordProvsize());
#endif  
  
}


bool BatchInDBProcess::executedbproc(DbInterface* dbi ,int& infotype,int& srcdbnum,string& procname )
{
  
       // (1.1) �洢���̲���׼��
      vector<DbParam> procParam;
      DbParam dbparam;
      //  ͨ����
      dbparam.m_seq = 1;
      dbparam.m_action   = PARAM_ACT_IN;
      dbparam.m_type     = PARAM_TYPE_INT;
      dbparam.m_intValue = m_channelNo ;               
      procParam.push_back( dbparam ) ;
       //  �ļ��� 
      dbparam.m_seq    = 2;
      dbparam.m_action = PARAM_ACT_IN;
      dbparam.m_type   = PARAM_TYPE_STR;
      strcpy(dbparam.m_strValue,m_filename.c_str());
      procParam.push_back( dbparam ) ;
      //    ����
      dbparam.m_seq      = 3;
      dbparam.m_action   = PARAM_ACT_OUT;
      dbparam.m_type     = PARAM_TYPE_INT;
      dbparam.m_intValue = infotype;                
      procParam.push_back( dbparam ) ;
      //   ��������
      dbparam.m_seq      = 4;
      dbparam.m_action   = PARAM_ACT_OUT;
      dbparam.m_type     = PARAM_TYPE_STR;
      strcpy(dbparam.m_strValue,"" );
      procParam.push_back( dbparam ) ;
      dbi->beginTrans();
      
      //  (1.2) ���ô洢����,��Ҫ�����ύ����
      try
      {
        dbi->execProcedure( procname ,procParam);
          
      }catch(CException e)
    	{
    	   dbi->rollbackTrans();
    	   throw e;
    	}
    	
    	// (1.3) �洢����ִ�й����д����ж�
      if ( procParam[2].m_intValue == -1 )
      {       
        dbi->rollbackTrans();   
        string szDBName = dbi->getobjalisename();
        throw CException(__FILE__, __LINE__, "[%s]ִ�д洢����ʧ�� ��%d ,%s",szDBName.c_str(), procParam[2].m_intValue, procParam[3].m_strValue);
      }
      dbi->commitTrans();
    	return true;
    	
}

void BatchInDBProcess::indb(int& srcdbNum)
{
  
  vector<DestRule> destrule;
  m_destRuleParse->getAllDestRule( srcdbNum , destrule );
  
  // vector�Ѿ����ź���˳��ģ�����������Ļ���
  // ����::partial_sort(m_destRuleVector.begin(), m_destRuleVector.end(), m_destRuleVector.end());
  if ( destrule.size() == 0 ) return  ;
#ifdef _DEBUG_		   
        printf("���:indb(��ʼ)\n");
#endif        
  string procname = "" ;//
  string dbname   = "";
  for( int i = 0 ; i< destrule.size() ;++ i ) 
  {    
     
    if ( destrule[i].m_objdeal == 1 )
    {
#ifdef _DEBUG_		   
        printf("indb: infotype = %d\n" , destrule[i].m_infotype);
#endif      

      // (1.1) ��δ���ļ�¼��⣨����ʡ���Ŀ�ģ�
      if (destrule[i].getCurRecordsize() > 0 )   
         insertTable( destrule[i].m_infotype ,srcdbNum,false ) ;
      if (destrule[i].getCurRecordProvsize() > 0 )         
           insertTable( destrule[i].m_infotype ,srcdbNum,true ) ;
      //�������������û�����ݵĻ�����Ӧ��ִ�д洢����
	   	if ( *(destrule[i].m_isExistsRecordProv)  == true )
	   	{
#ifdef _DEBUG_		   
        printf("indb: infotype(ʡ����ģʽ) = %d\n" , destrule[i].m_infotype);
#endif 	   	  
          // (1.2) �������ݿ�Ĵ洢���̣����������⣨ʡ���Ŀ�)
    	   	// (1.3) ����ʡ���Ŀ�洢����	   	  
    	   	for (vector<DbInterface*>::iterator iter=m_dbisPro->begin(); iter!=m_dbisPro->end(); ++iter)
    			{
    				  //  ����DestRule���õ���Ϣ��������Ϊ�ĸ�infoloadͬʱ����ʱ���໥Ӱ��
    				  //  �����ʡ���Ŀ�Ļ�������ֶ�һ��Ҫ���óɲ�ͬ����ʱ��ʹ洢����
    				  //  ���ڵ���������ͨ��Ϊ�������ݡ�
    				  procname = "";
    				  dbname   = (*iter)->getobjalisename(); 
    				  destrule[i].getProvActProcName( dbname, procname );
    					executedbproc((*iter),destrule[i].m_infotype,srcdbNum,procname);
    			}
    	}
    	// ����ģʽ�µĸ��Կ��������
    	else if ( 	*(destrule[i].m_isExistsRecord)  == true )
    	{
#ifdef _DEBUG_		   
        printf("indb: infotype(����ģʽ) = %d\n" , destrule[i].m_infotype);
#endif     	  
    	    // (1.4)�����������洢����
          vector<DbInterface*>	objdbis;
    	   	m_destRuleParse->getobjdbi(destrule[i].m_infotype, srcdbNum, objdbis);
    	    for (vector<DbInterface*>::iterator iter=objdbis.begin(); iter!=objdbis.end(); ++iter)
    	    {
    			 // ����DestRule���õ���Ϣ��������Ϊ�ĸ�infoloadͬʱ����ʱ���໥Ӱ��
    			 // �����ʡ���Ŀ�Ļ�������ֶ�һ��Ҫ���óɲ�ͬ����ʱ��ʹ洢����
    			 // ���ڵ���������ͨ��Ϊ�������ݡ�
    				  procname = "";
    				  dbname   = (*iter)->getobjalisename() ; 
    				  destrule[i].getActProcName( dbname,procname );		              
    			    executedbproc( (*iter),destrule[i].m_infotype,srcdbNum,procname ) ;	      
    	    }	 
	    } 
	  }
  }
  
#ifdef _DEBUG_		   
        printf("���:indb(����)\n");
#endif    

}

//���SQL�������,��loadrule����һ��,��ʱ����Ҫ���ֶ���Ϣ
string BatchInDBProcess::getinsertsql(int& infotype,int& srcdbnum ,string& tablenames)
{
  
  LoadTable * m_loadtable =  &m_loadtables[infotype];
	int dbcolumnnum    = m_loadtable->m_dbcolumnnum;
	string columnnames = "CHANNELNO,SYNC_SEQUENCE,INFOTYPE,MODIFY_TAG";
	string valuenames  = ":CHANNELNO,:SYNC_SEQUENCE,:INFOTYPE,:MODIFY_TAG";
	
	for (int j=0; j < dbcolumnnum; ++j)
	{
			string dbcolumnname = (m_loadtable->m_dbcolumns[j]).m_columnname;
			PROCPARAM_TYPE dbcolumntype = (m_loadtable->m_dbcolumns[j]).m_columntype;	
			//���SQL������Ϣ
		  columnnames = columnnames + "," + dbcolumnname;
			//���SQL��ֵ��Ϣ
			valuenames = valuenames + ",:" + dbcolumnname;
	}	
			
  string strSql = "INSERT INTO " + tablenames + " (" + columnnames + ") VALUES (" + valuenames + ") ";
  return strSql ;
}

//����ʱ��
void BatchInDBProcess::insertTable(int& infotype,int& srcdbnum ,bool isPro)
{
  //  (1.1) ��������
  DestRule  destrule ;
  m_destRuleParse->getDestRule( infotype , srcdbnum , destrule );
  vector<DbInterface*>	objdbis;
  objdbis.clear();
  try
  {
    m_destRuleParse->getobjdbi(infotype, srcdbnum, objdbis);
  }catch(CException e)
	{
	 	throw e;
	}  
#ifdef _DEBUG_		   
        printf("���м��:insertTable(��ʼ)\n");
#endif	
  string                      sql ;
  string                      tablename;
  string                      dbname ;
  LoadTable * m_loadtable =  &m_loadtables[infotype];
 
  //  (1.2) �ж��Ƿ���ʡ����ģʽ
  //  (1.3) �����������׼��
 	if (isPro)
	{
			for (vector<DbInterface*>::iterator iter=m_dbisPro->begin(); iter!=m_dbisPro->end(); ++iter)
			{
			  // �������Ͷ���ת������ΪOCCI�е�Date��Ҫ��ϵͳ���������ģ����ݿ�ӿ��ѷ�װ��
			  (*iter)->beginTrans();
				dbname = (*iter)->getobjalisename();
				tablename = "";//ȡ�ñ���
				destrule.getProvTempTableName( dbname , tablename );
				sql = getinsertsql(infotype ,srcdbnum ,tablename);
				try
				{
  				(*iter)->createStatement();
  				(*iter)->setSql( (char*)sql.c_str() ); 
  				int nMax = destrule.getCurRecordProvsize();
  				(*iter)->setBatchParameter(destrule.m_provdata , nMax );			
  				(*iter)->executeUpdate(); 
  			  (*iter)->commitTrans();
			  }catch(CException e)
      	{
      	 	throw e;
      	}  
			}
			destrule.ResetBatchParambytype(true);
	}
	else
	{
			for (vector<DbInterface*>::iterator iter=objdbis.begin(); iter!=objdbis.end(); ++iter)
			{
			  // �������Ͷ���ת������ΪOCCI�е�Date��Ҫ��ϵͳ���������ģ����ݿ�ӿ��ѷ�װ��
	      (*iter)->beginTrans();
				dbname = (*iter)->getobjalisename();
				tablename = "";//ȡ�ñ���
				destrule.getTempTableName( dbname , tablename );
				sql = getinsertsql(infotype ,srcdbnum ,tablename);	
	    #ifdef _DEBUG_
	      printf("getinsertsql=%s\n",sql.c_str());
	    #endif 						
				try
				{	
  				(*iter)->createStatement();
  				(*iter)->setSql((char*)sql.c_str()); 
  				int nMax = destrule.getCurRecordsize() ;
  				(*iter)->setBatchParameter(destrule.m_data ,  nMax );
  				(*iter)->executeUpdate();
          (*iter)->commitTrans();
			  }catch(CException e)
      	{
	    #ifdef _DEBUG_
	      printf("insert db =%s\n",e.GetString());
	    #endif       	  
      	 	throw e;
      	}				
			}
			destrule.ResetBatchParambytype(false);
	}
#ifdef _DEBUG_		   
        printf("���м��:insertTable(����)\n");
#endif	
	
}

//
bool BatchInDBProcess::isProvinceInfo(Record* record)
{
  
  if ( !m_ifProvinceMode ) return false;
#ifdef _DEBUG_		   
        printf("�ж��Ƿ���ʡ����ģʽ��isProvinceInfo\n");
#endif	    
	int infotype = atoi((*record)[1]);
	switch(infotype)
	{
		//�ͻ�
		case 100:
			//�ж�CUST_CLASS_TYPE,���ſͻ����������0��ʡ����1���м���2�����ؼ�
			if (strlen((*record)[INFO_ADDLEN+23])>0 && (*record)[INFO_ADDLEN+23][0]=='0')
			{
				return true;
			}
			break;
		//���ſͻ�
		case 102:
			//�ж�CUST_CLASS_TYPE,���ſͻ����������0��ʡ����1���м���2�����ؼ�
			if (strlen((*record)[INFO_ADDLEN+5])>0 && (*record)[INFO_ADDLEN+5][0]=='0')
			{
				return true;
			}
			break;
		//�˻�
		case 200:
		//�û�
		case 300:
		//�û�_��Ա
		case 301:
		//�û�_��Ҫ
		case 302:
		//�û�_����
		case 307:
			//��99��ͷ���˻����û���ʶ���漰����е��˻����û�
			if (strncmp((*record)[INFO_ADDLEN], "99" ,2) == 0)
			{
				return true;
			}
			break;
		//�û�_����
		case 304:
		//�û�_ ����״̬
		case 306:
		//�˻���������
		case 203:
			//��99��ͷ���˻����û���ʶ���漰����е��˻����û�
			if (strncmp((*record)[INFO_ADDLEN+1], "99" ,2) == 0)
			{
				return true;
			}
			break;
		//�ʷ�ʵ��
		case 401:
			//��99��ͷ���˻����û���ʶ���漰����е��˻����û�
			if (strncmp((*record)[INFO_ADDLEN+2], "99" ,2)==0 && ((*record)[INFO_ADDLEN+1][0]=='0'||(*record)[INFO_ADDLEN+1][0]=='1'))
			{
				return true;
			}
			break;
		//���ѹ�ϵ
		case 313:
			//��99��ͷ���˻����û���ʶ���漰����е��˻����û�
			if (strncmp((*record)[INFO_ADDLEN+1], "99", 2)==0 || strncmp((*record)[INFO_ADDLEN+9], "99", 2)==0)
			{
				return true;
			}
			break;
		//���д��۱�
		case 319:
			//��99��ͷ���˻����û���ʶ���漰����е��˻����û�
			if (strncmp((*record)[INFO_ADDLEN+3], "99", 2)==0 || strncmp((*record)[INFO_ADDLEN+4], "99", 2)==0)
			{
				return true;
			}
			break;
		default:
			break;
	}
	return false;
}

void BatchInDBProcess::setchannelno ( int & rchannelno )
{
  m_channelNo = rchannelno;
}