#include "stdafx.h"

extern  Application * const theApp ;
extern ErrorMessages * const theErrorMessages; 
const unsigned int  INFO_PROMT_MSG  = INFOLOAD_ERROR_BASE + 26;
const unsigned int  ERR_ACTMDB_MSG  = INFOLOAD_ERROR_BASE + 27;

// 显示提示信息
void  ParamProcess::showPromtMessage(const char* cMsg)
{
    theErrorMessages->insert(INFO_PROMT_MSG  , cMsg);
    theApp->writeAlert();
}
// 显示错误信息
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

   // (1).根据SQL查询数据，并设置日期读取格式
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
   
   //  (2).分配批量读取空间
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
   
   //  (3).完全清空,全表锁定(只取本机IPC单库，配置时注意！！)
   vector<MmInterface*>::iterator iter = m_objmmis->begin();
   MdbBatchInterface* mdbBatchIntf = new MdbBatchInterface();
   if ( iter != m_objmmis->end() )
   {
        if(dealtype == '3')
           (*iter)->truncate(tablename);
        mdbBatchIntf->initialize( (*iter)->m_session , tablename.c_str() ) ;        
   }    
   
   //  (4).批量读取并加载内存库    
   tmaperror.clear(); 
   register int k ,i ;    
   map<int, string>::iterator iterd ;
   //  Add by wangfc 2012-2-1 19:09:55 for NULL issue
   char cNullBuffer[512] = {0};
   memset( cNullBuffer , 0x00, sizeof(cNullBuffer ) );
   //  ---end
   while(m_srcdbi->BatchNext( nRow, MAX_RECORD_NUM ) )
   {
        //  (4.1).设置内存库缓存 [长循环放在内循环，效率要高一点]      
        for ( k = 0 ; k< iColCnt;++k )
        { 
            // Modify by wangfc 2012-2-1 19:10:28 for NULL issue
            short* ind =  vBatchDbPars[k].m_dataElemSize ;
            for ( i = 0 ; i <  nRow ; ++ i )
            { 
               if  (  ind[i] == -1 )    // 如果是NULL的话 
                   mdbBatchIntf->setDataValue( k, vBatchDbPars[k].m_type, i  ,cNullBuffer );    
               else
                   mdbBatchIntf->setDataValue( k, vBatchDbPars[k].m_type, i  ,(char*)vBatchDbPars[k].m_data  + vBatchDbPars[k].m_size * i );                 
            }
        }    
        
        //  (4.2).批量提交数据 ,有错误则输出
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
     				throw CException(__FILE__, __LINE__, "分配表空间失败,信息%s",e.GetString());
       }
       
       //   (4.3).批量提交过程中有错误发生,则输出日志
        for (iterd = tmaperror.begin(); iterd != tmaperror.end(); ++iterd)
        {
            showPromtMessage( iterd->second.c_str() );
            nErrRowCnt++ ;
        }
        nCommitRowCnt += nRow;
        
        //  (4.4).提示倒换过程中数目变化
//       if ( nCommitRowCnt % 100000 == 0 ) 
//       {
//           memset( cBuf ,0x00,sizeof(cBuf));     
//           sprintf(cBuf , "commit row = %ld " , nCommitRowCnt ); 
//           showPromtMessage( cBuf );
//       }
   }  
   
   //  (5).总数提示
   time_t endTime = time(NULL); 
   memset( cBuf ,0x00,sizeof(cBuf));
   sprintf(cBuf , "RowCount= %ld ,ErrCount=%ld ,elapsed_time= %.1f seconds" , nCommitRowCnt , nErrRowCnt, float(difftime(endTime, startTime)) );     
   showPromtMessage( cBuf ); 
   
    // (6).释放分配的空间
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
	
	//(1)查询记录
	try
	{
		//载入方式,0:先TRUNCATE,然后全部数据INSERT进入。
		if (dealtype=='0' || dealtype=='2')
		{		
			m_srcdbi->execStaticSql(selectsql);
		}
		//Add by 批量提交 2012-1-7 10:10:48
		else if (dealtype=='3' || dealtype=='4')
    {
        return processbatch(dealtype, selectsql, tablename, updatesql, starttime, endtime);
    }
		//载入方式,1:只做UPDATE和INSERT操作，已有数据UPDATE，没有的数据INSERT。
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

	//(2)执行清理+插入
	//(2.1)载入方式,0:先TRUNCATE,然后全部数据INSERT进入。
	if (dealtype=='0' || dealtype=='2')
	{
		try
		{
			//(2.1.1)完全清空,全表锁定
			for (vector<MmInterface*>::iterator iter=m_objmmis->begin(); iter!=m_objmmis->end(); ++iter)
			{
				if(dealtype != '2')
				{
					(*iter)->truncate(tablename);
				}
				(*iter)->beginTrans(tablename, 0);
			}
			
			//(2.1.2)记录插入内存数据库
			vector<string> record;
			while(m_srcdbi->getNext())
			{
				++recnum;
				
				//获取一行原始记录
				record.clear();
				for (int i=1; i<=m_srcdbi->getColumnNum(); ++i)
				{
					record.push_back(m_srcdbi->getColumnStringValue(i));
        }
			
				//记录插入内存数据库中
				for (vector<MmInterface*>::iterator iter=m_objmmis->begin(); iter!=m_objmmis->end(); ++iter)
				{
						(*iter)->insert(tablename, record);
				}
				
			   //记录数超过默认值,提交
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
//							m_theErrorMessages->insert(DB_ERROR_NOCFG, "主键重复|"+record[0]+"|"+record[1]+"|"+record[2]+"|请核实记录");
							continue;
				    }
						(*iter)->beginTrans(tablename, 0);
					}
				}
			}

			//(2.1.3)提交内存数据库
			for (vector<MmInterface*>::iterator iter=m_objmmis->begin(); iter!=m_objmmis->end(); ++iter)
			{
				try
				{
					(*iter)->commitTrans();
				}
				catch (CException e)
				{
					(*iter)->rollbackTrans();	
//					m_theErrorMessages->insert(DB_ERROR_NOCFG, "主键重复|"+record[0]+"|"+record[1]+"|"+record[2]+"|请核实记录");
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
	//(2.2)更新+插入(注意：查询的SQL字段必须与更新＋插入的需要的字段顺序完全一致)
	else
	{		
		try
		{
			//(2.2.1)表记录事务开启
			for (vector<MmInterface*>::iterator iter=m_objmmis->begin(); iter!=m_objmmis->end(); ++iter)
			{
				(*iter)->beginTrans(tablename, 1);
			}
			
			//(2.2.2)记录插入内存数据库
			vector<string> record;
			while(m_srcdbi->getNext())
			{
				++recnum;
				
				//获取一行原始记录
				record.clear();
				for (int i=1; i<=m_srcdbi->getColumnNum(); ++i)
				{
					record.push_back(m_srcdbi->getColumnToStringValue(i));
				}
				
				//记录插入内存数据库中
				for (vector<MmInterface*>::iterator iter=m_objmmis->begin(); iter!=m_objmmis->end(); ++iter)
				{
					//如果没有更新到数据,那么插入数据
					if ((*iter)->update(tablename, updatesql, record) == 0)
					{
						(*iter)->insert(tablename, record);
					}
				}
				
				//记录数超过默认值,提交
				if (recnum%MAX_RECCOMMIT_NUM == 0)
				{
					for (vector<MmInterface*>::iterator iter=m_objmmis->begin(); iter!=m_objmmis->end(); ++iter)
					{	
						(*iter)->commitTrans();
						(*iter)->beginTrans(tablename, 1);
					}
				}
			}
			
			//(2.2.3)提交内存数据库
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

