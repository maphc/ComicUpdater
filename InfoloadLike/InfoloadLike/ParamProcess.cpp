#include "stdafx.h"

extern  Application * const theApp ;
extern ErrorMessages * const theErrorMessages; 
const unsigned int  INFO_PROMT_MSG  = INFOLOAD_ERROR_BASE + 26;
const unsigned int  ERR_ACTMDB_MSG  = INFOLOAD_ERROR_BASE + 27;

// ��ʾ��ʾ��Ϣ
void  ParamProcess::showPromtMessage(const char* cMsg)
{
    theErrorMessages->insert(INFO_PROMT_MSG  , cMsg);
    theApp->writeAlert();
}
// ��ʾ������Ϣ
void  ParamProcess::showErrorMessage(const char* cMsg )
{
    theErrorMessages->insert(ERR_ACTMDB_MSG  , cMsg);
    theApp->writeAlert();
}


ParamProcess::ParamProcess()
{
	m_srcdbi = NULL;
	m_objmmis = NULL;
}
		
		
ParamProcess::~ParamProcess()
{
}

		
void ParamProcess::init(DbInterface *srcdbi, vector<MmInterface*> *objmmis)
{
	m_srcdbi = srcdbi;
	m_objmmis = objmmis;
}

void  ParamProcess::processbatch (const char& dealtype, const string& selectsql, const string& tablename, 
					 const string& updatesql, const string& starttime, const string& endtime)
{
  
   long  nCommitRowCnt = 0L;
   long  nErrRowCnt    = 0L;
   int   nRow      = 0 ;
   char  cBuf[201] = {0};
   vector<DataParam> vBatchDbPars;
   map<int,string>   tmaperror;
   time_t startTime = time(NULL); 

   // (1).����SQL��ѯ���ݣ����������ڶ�ȡ��ʽ
   try
    {
        m_srcdbi->createStatement();
        m_srcdbi->executeUpdate ( "ALTER SESSION SET NLS_DATE_FORMAT = 'YYYYMMDDHH24MISS'" );
        memset( cBuf ,0x00 ,sizeof(cBuf));
        sprintf( cBuf , " Begin Query %s ",tablename.c_str());
        showPromtMessage(cBuf);
        m_srcdbi ->setSql((char*)selectsql.c_str());
        m_srcdbi ->executeQuery(MAX_RECORD_NUM);
        sprintf( cBuf , " End Query %s ",tablename.c_str());
        showPromtMessage(cBuf);
    }
    catch(CException e)
   {
	  	throw e;
   }
   
   //  (2).����������ȡ�ռ�
   vBatchDbPars.clear();
   int  iColCnt = m_srcdbi->getColCount();
   vBatchDbPars.reserve( iColCnt );
   for(int i=1;  i <= iColCnt ; ++i )
   {
       int tcoltype = ((m_srcdbi->m_rs->getColumnListMetaData())[i-1]).getInt(MetaData::ATTR_DATA_TYPE);
       DataParam BatchDbPar;
       BatchDbPar.m_dataElemSize = new short[MAX_RECORD_NUM];
       BatchDbPar.m_seq = i;        
       oracle::occi::Type iType = OCCI_SQLT_STR;
       switch ( tcoltype )
       {         
          case OCCI_SQLT_NUM:
          case OCCI_SQLT_LNG:
                 BatchDbPar.m_size = sizeof(long);
                 BatchDbPar.m_type = VAR_TYPE_LONG ; 
                 BatchDbPar.m_data = new long[MAX_RECORD_NUM];
                 iType = OCCIINT;
                 break;
          case OCCIINT :
                 BatchDbPar.m_size = sizeof(int);
                 BatchDbPar.m_type = VAR_TYPE_INT ; 
                 BatchDbPar.m_data = new int[MAX_RECORD_NUM];  
                 iType = OCCIINT;
                 break;
          case OCCIFLOAT:                  
                 BatchDbPar.m_size = sizeof(float);
                 BatchDbPar.m_type = VAR_TYPE_REAL; 
                 BatchDbPar.m_data = new float[MAX_RECORD_NUM];
                 iType =  OCCIFLOAT;
                 break;
          default :
                 BatchDbPar.m_size = 100 + 1 ;
                 BatchDbPar.m_type = VAR_TYPE_VSTR ;
                 BatchDbPar.m_data = new char[BatchDbPar.m_size* MAX_RECORD_NUM];
       }
       m_srcdbi->SetResultDataBuffer(BatchDbPar.m_seq ,(void*)BatchDbPar.m_data ,iType,BatchDbPar.m_size,0,reinterpret_cast<sb2*>(BatchDbPar.m_dataElemSize));
       memset((char*) BatchDbPar.m_data,0x00, BatchDbPar.m_size* MAX_RECORD_NUM);
       vBatchDbPars.push_back(BatchDbPar); 
   }  
   
   //  (3).��ȫ���,ȫ������(ֻȡ����IPC���⣬����ʱע�⣡��)
   vector<MmInterface*>::iterator iter = m_objmmis->begin();
   MdbBatchInterface* mdbBatchIntf = new MdbBatchInterface();
   if ( iter != m_objmmis->end() )
   {
        if(dealtype == '3')
           (*iter)->truncate(tablename);
        mdbBatchIntf->initialize( (*iter)->m_session , tablename.c_str() ) ;        
   }    
   
   //  (4).������ȡ�������ڴ��    
   tmaperror.clear(); 
   register int k ,i ;    
   map<int, string>::iterator iterd ;
   //  Add by wangfc 2012-2-1 19:09:55 for NULL issue
   char cNullBuffer[512] = {0};
   memset( cNullBuffer , 0x00, sizeof(cNullBuffer ) );
   //  ---end
   while(m_srcdbi->BatchNext( nRow, MAX_RECORD_NUM ) )
   {
        //  (4.1).�����ڴ�⻺�� [��ѭ��������ѭ����Ч��Ҫ��һ��]      
        for ( k = 0 ; k< iColCnt;++k )
        { 
            // Modify by wangfc 2012-2-1 19:10:28 for NULL issue
            short* ind =  vBatchDbPars[k].m_dataElemSize ;
            for ( i = 0 ; i <  nRow ; ++ i )
            { 
               if  (  ind[i] == -1 )    // �����NULL�Ļ� 
                   mdbBatchIntf->setDataValue( k, vBatchDbPars[k].m_type, i  ,cNullBuffer );    
               else
                   mdbBatchIntf->setDataValue( k, vBatchDbPars[k].m_type, i  ,(char*)vBatchDbPars[k].m_data  + vBatchDbPars[k].m_size * i );                 
            }
        }    
        
        //  (4.2).�����ύ���� ,�д��������
        try
        {
           mdbBatchIntf->ArrayCommit( nRow,tmaperror );          
        }catch(Mdb_Exception &e)
        {
		    		for(  i = 0 ;i < vBatchDbPars.size();++i )
		    		{
		        	 delete[] vBatchDbPars[i].m_data ;
		         	 delete[] vBatchDbPars[i].m_dataElemSize ;     
		   	    }
    				mdbBatchIntf->uninitialize();
     				throw CException(__FILE__, __LINE__, "�����ռ�ʧ��,��Ϣ%s",e.GetString());
       }
       
       //   (4.3).�����ύ�������д�����,�������־
        for (iterd = tmaperror.begin(); iterd != tmaperror.end(); ++iterd)
        {
            showPromtMessage( iterd->second.c_str() );
            nErrRowCnt++ ;
        }
        nCommitRowCnt += nRow;
        
        //  (4.4).��ʾ������������Ŀ�仯
//       if ( nCommitRowCnt % 100000 == 0 ) 
//       {
//           memset( cBuf ,0x00,sizeof(cBuf));     
//           sprintf(cBuf , "commit row = %ld " , nCommitRowCnt ); 
//           showPromtMessage( cBuf );
//       }
   }  
   
   //  (5).������ʾ
   time_t endTime = time(NULL); 
   memset( cBuf ,0x00,sizeof(cBuf));
   sprintf(cBuf , "RowCount= %ld ,ErrCount=%ld ,elapsed_time= %.1f seconds" , nCommitRowCnt , nErrRowCnt, float(difftime(endTime, startTime)) );     
   showPromtMessage( cBuf ); 
   
    // (6).�ͷŷ���Ŀռ�
    for( int i = 0 ;i < vBatchDbPars.size();++i )
    {
         delete[] vBatchDbPars[i].m_data ;
         delete[] vBatchDbPars[i].m_dataElemSize ;     
    }
    mdbBatchIntf->uninitialize();
        
}		
void ParamProcess::process(const char& dealtype, const string& selectsql, const string& tablename, 
					 	   const string& updatesql, const string& starttime, const string& endtime)
{
	int recnum = 0;
	
	//(1)��ѯ��¼
	try
	{
		//���뷽ʽ,0:��TRUNCATE,Ȼ��ȫ������INSERT���롣
		if (dealtype=='0' || dealtype=='2')
		{		
			m_srcdbi->execStaticSql(selectsql);
		}
		//Add by �����ύ 2012-1-7 10:10:48
		else if (dealtype=='3' || dealtype=='4')
    {
        return processbatch(dealtype, selectsql, tablename, updatesql, starttime, endtime);
    }
		//���뷽ʽ,1:ֻ��UPDATE��INSERT��������������UPDATE��û�е�����INSERT��
		else
		{
			vector<DbParam> params;
			params.clear();
			DbParam param;
			param.m_seq = 1;
			param.m_action = PARAM_ACT_IN;
			param.m_type = PARAM_TYPE_DATE;
			strcpy(param.m_dateValue, starttime.c_str());
			params.push_back(param);
			param.m_seq = 2;
			param.m_action = PARAM_ACT_IN;
			param.m_type = PARAM_TYPE_DATE;
			strcpy(param.m_dateValue, endtime.c_str());
			params.push_back(param);
			
			m_srcdbi->execDynamicSql(selectsql, params);
		}
	}	
	catch (CException e)
	{
		throw e;
	}	

	//(2)ִ������+����
	//(2.1)���뷽ʽ,0:��TRUNCATE,Ȼ��ȫ������INSERT���롣
	if (dealtype=='0' || dealtype=='2')
	{
		try
		{
			//(2.1.1)��ȫ���,ȫ������
			for (vector<MmInterface*>::iterator iter=m_objmmis->begin(); iter!=m_objmmis->end(); ++iter)
			{
				if(dealtype != '2')
				{
					(*iter)->truncate(tablename);
				}
				(*iter)->beginTrans(tablename, 0);
			}
			
			//(2.1.2)��¼�����ڴ����ݿ�
			vector<string> record;
			while(m_srcdbi->getNext())
			{
				++recnum;
				
				//��ȡһ��ԭʼ��¼
				record.clear();
				for (int i=1; i<=m_srcdbi->getColumnNum(); ++i)
				{
					record.push_back(m_srcdbi->getColumnStringValue(i));
        }
			
				//��¼�����ڴ����ݿ���
				for (vector<MmInterface*>::iterator iter=m_objmmis->begin(); iter!=m_objmmis->end(); ++iter)
				{
						(*iter)->insert(tablename, record);
				}
				
			   //��¼������Ĭ��ֵ,�ύ
				if (recnum%MAX_RECCOMMIT_NUM == 0)
				{
					for (vector<MmInterface*>::iterator iter=m_objmmis->begin(); iter!=m_objmmis->end(); ++iter)
					{	
						try
						{
							(*iter)->commitTrans();
						}
						catch (CException e)
				    {
							(*iter)->rollbackTrans();	
							(*iter)->beginTrans(tablename, 0);				
//							m_theErrorMessages->insert(DB_ERROR_NOCFG, "�����ظ�|"+record[0]+"|"+record[1]+"|"+record[2]+"|���ʵ��¼");
							continue;
				    }
						(*iter)->beginTrans(tablename, 0);
					}
				}
			}

			//(2.1.3)�ύ�ڴ����ݿ�
			for (vector<MmInterface*>::iterator iter=m_objmmis->begin(); iter!=m_objmmis->end(); ++iter)
			{
				try
				{
					(*iter)->commitTrans();
				}
				catch (CException e)
				{
					(*iter)->rollbackTrans();	
//					m_theErrorMessages->insert(DB_ERROR_NOCFG, "�����ظ�|"+record[0]+"|"+record[1]+"|"+record[2]+"|���ʵ��¼");
				}
			}
		}
		catch (CException e)
		{
			for (vector<MmInterface*>::iterator iter=m_objmmis->begin(); iter!=m_objmmis->end(); ++iter)
			{
				(*iter)->rollbackTrans();
			}
			throw e;
		}
	}
	//(2.2)����+����(ע�⣺��ѯ��SQL�ֶα�������£��������Ҫ���ֶ�˳����ȫһ��)
	else
	{		
		try
		{
			//(2.2.1)���¼������
			for (vector<MmInterface*>::iterator iter=m_objmmis->begin(); iter!=m_objmmis->end(); ++iter)
			{
				(*iter)->beginTrans(tablename, 1);
			}
			
			//(2.2.2)��¼�����ڴ����ݿ�
			vector<string> record;
			while(m_srcdbi->getNext())
			{
				++recnum;
				
				//��ȡһ��ԭʼ��¼
				record.clear();
				for (int i=1; i<=m_srcdbi->getColumnNum(); ++i)
				{
					record.push_back(m_srcdbi->getColumnToStringValue(i));
				}
				
				//��¼�����ڴ����ݿ���
				for (vector<MmInterface*>::iterator iter=m_objmmis->begin(); iter!=m_objmmis->end(); ++iter)
				{
					//���û�и��µ�����,��ô��������
					if ((*iter)->update(tablename, updatesql, record) == 0)
					{
						(*iter)->insert(tablename, record);
					}
				}
				
				//��¼������Ĭ��ֵ,�ύ
				if (recnum%MAX_RECCOMMIT_NUM == 0)
				{
					for (vector<MmInterface*>::iterator iter=m_objmmis->begin(); iter!=m_objmmis->end(); ++iter)
					{	
						(*iter)->commitTrans();
						(*iter)->beginTrans(tablename, 1);
					}
				}
			}
			
			//(2.2.3)�ύ�ڴ����ݿ�
			for (vector<MmInterface*>::iterator iter=m_objmmis->begin(); iter!=m_objmmis->end(); ++iter)
			{
				(*iter)->commitTrans();
			}
		}
		catch (CException e)
		{
			for (vector<MmInterface*>::iterator iter=m_objmmis->begin(); iter!=m_objmmis->end(); ++iter)
			{
				(*iter)->rollbackTrans();
			}
			throw e;
		}
	
	}
}

