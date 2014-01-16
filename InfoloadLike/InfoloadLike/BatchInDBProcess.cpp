#include "stdafx.h"
/*
  需要参数化省中心模式.
*/

BatchInDBProcess::BatchInDBProcess()
{
  
  m_ifProvinceMode  = false ;//默认非省中心模式
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
  // (1.1) 判断infotype是否需要批量入库
  int obj_db_deal  = m_destRuleParse->getobj_db_deal( infotype , srcdbnum );
  if ( obj_db_deal == 0 ) return ;
    
#ifdef _DEBUG_		   
   printf("(批量)添加记录:processRecord(开始)\n");
#endif      

 //  (1.2) 判断对应的目标规则
  DestRule destrule ;
  m_destRuleParse->getDestRule( infotype , srcdbnum , destrule );
  
 //  (1.3)  同一infotype的数据达到配置最大数目时，自动完成入库功能，调用inserttable
 //         否则，则新开辟一片内存存放该部分记录，并将其记录下来
  for ( int  i = 0 ; i < info->size() ; ++i )
  {
     Record *record = (*info)[i];  
     
     //  (1.3.1 )  同一表的记录达到最大记录值时，插入临时表
     if ( destrule.getCurRecordsize() >= m_maxBatchRecordNum  ) 
        insertTable( infotype,srcdbnum ,false);
        
     //  (1.3.2)   同一表的记录达到最大记录值时，插入临时表（省中心库）
     if ( destrule.getCurRecordProvsize() >= m_maxBatchRecordNum  ) 
        insertTable( infotype,srcdbnum ,true);
        
     //  (1.3.3） 将记录存放到本地缓存中             
     //           跨地市数据与本地市数据的处理是互斥的
      if ( isProvinceInfo(record ))       
         destrule.AddRecordProvData ( m_channelNo , record,m_loadtables[infotype].m_dbcolumns,m_loadtables[infotype].m_dbcolumnnum );         
      else 
         destrule.AddRecordData ( m_channelNo ,record,m_loadtables[infotype].m_dbcolumns,m_loadtables[infotype].m_dbcolumnnum );      
   }  
   
#ifdef _DEBUG_		   
        printf("批量)添加记录:processRecord(结束) CurRecordsize = %ld,CurProvRecordsize = %ld \n",destrule.getCurRecordsize(),destrule.getCurRecordProvsize());
#endif  
  
}


bool BatchInDBProcess::executedbproc(DbInterface* dbi ,int& infotype,int& srcdbnum,string& procname )
{
  
       // (1.1) 存储过程参数准备
      vector<DbParam> procParam;
      DbParam dbparam;
      //  通道号
      dbparam.m_seq = 1;
      dbparam.m_action   = PARAM_ACT_IN;
      dbparam.m_type     = PARAM_TYPE_INT;
      dbparam.m_intValue = m_channelNo ;               
      procParam.push_back( dbparam ) ;
       //  文件名 
      dbparam.m_seq    = 2;
      dbparam.m_action = PARAM_ACT_IN;
      dbparam.m_type   = PARAM_TYPE_STR;
      strcpy(dbparam.m_strValue,m_filename.c_str());
      procParam.push_back( dbparam ) ;
      //    错误
      dbparam.m_seq      = 3;
      dbparam.m_action   = PARAM_ACT_OUT;
      dbparam.m_type     = PARAM_TYPE_INT;
      dbparam.m_intValue = infotype;                
      procParam.push_back( dbparam ) ;
      //   错误描述
      dbparam.m_seq      = 4;
      dbparam.m_action   = PARAM_ACT_OUT;
      dbparam.m_type     = PARAM_TYPE_STR;
      strcpy(dbparam.m_strValue,"" );
      procParam.push_back( dbparam ) ;
      dbi->beginTrans();
      
      //  (1.2) 调用存储过程,需要程序提交事务
      try
      {
        dbi->execProcedure( procname ,procParam);
          
      }catch(CException e)
    	{
    	   dbi->rollbackTrans();
    	   throw e;
    	}
    	
    	// (1.3) 存储过程执行过程中错误判断
      if ( procParam[2].m_intValue == -1 )
      {       
        dbi->rollbackTrans();   
        string szDBName = dbi->getobjalisename();
        throw CException(__FILE__, __LINE__, "[%s]执行存储过程失败 ：%d ,%s",szDBName.c_str(), procParam[2].m_intValue, procParam[3].m_strValue);
      }
      dbi->commitTrans();
    	return true;
    	
}

void BatchInDBProcess::indb(int& srcdbNum)
{
  
  vector<DestRule> destrule;
  m_destRuleParse->getAllDestRule( srcdbNum , destrule );
  
  // vector已经是排好了顺序的，如需在排序的话，
  // 建议::partial_sort(m_destRuleVector.begin(), m_destRuleVector.end(), m_destRuleVector.end());
  if ( destrule.size() == 0 ) return  ;
#ifdef _DEBUG_		   
        printf("入库:indb(开始)\n");
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

      // (1.1) 将未入库的记录入库（包括省中心库的）
      if (destrule[i].getCurRecordsize() > 0 )   
         insertTable( destrule[i].m_infotype ,srcdbNum,false ) ;
      if (destrule[i].getCurRecordProvsize() > 0 )         
           insertTable( destrule[i].m_infotype ,srcdbNum,true ) ;
      //如果批量缓存中没有数据的话，不应该执行存储过程
	   	if ( *(destrule[i].m_isExistsRecordProv)  == true )
	   	{
#ifdef _DEBUG_		   
        printf("indb: infotype(省中心模式) = %d\n" , destrule[i].m_infotype);
#endif 	   	  
          // (1.2) 调用数据库的存储过程，完成批量入库（省中心库)
    	   	// (1.3) 调用省中心库存储过程	   	  
    	   	for (vector<DbInterface*>::iterator iter=m_dbisPro->begin(); iter!=m_dbisPro->end(); ++iter)
    			{
    				  //  解析DestRule配置的信息，放置因为四个infoload同时入临时表相互影响
    				  //  如果是省中心库的话，这个字段一定要设置成不同的临时表和存储过程
    				  //  现在的做法是以通道为划分依据。
    				  procname = "";
    				  dbname   = (*iter)->getobjalisename(); 
    				  destrule[i].getProvActProcName( dbname, procname );
    					executedbproc((*iter),destrule[i].m_infotype,srcdbNum,procname);
    			}
    	}
    	// 正常模式下的各自库的入库过程
    	else if ( 	*(destrule[i].m_isExistsRecord)  == true )
    	{
#ifdef _DEBUG_		   
        printf("indb: infotype(正常模式) = %d\n" , destrule[i].m_infotype);
#endif     	  
    	    // (1.4)正常调用入库存储过程
          vector<DbInterface*>	objdbis;
    	   	m_destRuleParse->getobjdbi(destrule[i].m_infotype, srcdbNum, objdbis);
    	    for (vector<DbInterface*>::iterator iter=objdbis.begin(); iter!=objdbis.end(); ++iter)
    	    {
    			 // 解析DestRule配置的信息，放置因为四个infoload同时入临时表相互影响
    			 // 如果是省中心库的话，这个字段一定要设置成不同的临时表和存储过程
    			 // 现在的做法是以通道为划分依据。
    				  procname = "";
    				  dbname   = (*iter)->getobjalisename() ; 
    				  destrule[i].getActProcName( dbname,procname );		              
    			    executedbproc( (*iter),destrule[i].m_infotype,srcdbNum,procname ) ;	      
    	    }	 
	    } 
	  }
  }
  
#ifdef _DEBUG_		   
        printf("入库:indb(结束)\n");
#endif    

}

//求解SQL插入语句,在loadrule配置一下,临时表需要的字段信息
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
			//求解SQL中列信息
		  columnnames = columnnames + "," + dbcolumnname;
			//求解SQL中值信息
			valuenames = valuenames + ",:" + dbcolumnname;
	}	
			
  string strSql = "INSERT INTO " + tablenames + " (" + columnnames + ") VALUES (" + valuenames + ") ";
  return strSql ;
}

//入临时表
void BatchInDBProcess::insertTable(int& infotype,int& srcdbnum ,bool isPro)
{
  //  (1.1) 参数声明
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
        printf("入中间表:insertTable(开始)\n");
#endif	
  string                      sql ;
  string                      tablename;
  string                      dbname ;
  LoadTable * m_loadtable =  &m_loadtables[infotype];
 
  //  (1.2) 判断是否是省中心模式
  //  (1.3) 批量入库数据准备
 	if (isPro)
	{
			for (vector<DbInterface*>::iterator iter=m_dbisPro->begin(); iter!=m_dbisPro->end(); ++iter)
			{
			  // 日期类型二次转化，因为OCCI中的Date需要绑定系统环境变量的（数据库接口已封装）
			  (*iter)->beginTrans();
				dbname = (*iter)->getobjalisename();
				tablename = "";//取得表名
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
			  // 日期类型二次转化，因为OCCI中的Date需要绑定系统环境变量的（数据库接口已封装）
	      (*iter)->beginTrans();
				dbname = (*iter)->getobjalisename();
				tablename = "";//取得表名
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
        printf("入中间表:insertTable(结束)\n");
#endif	
	
}

//
bool BatchInDBProcess::isProvinceInfo(Record* record)
{
  
  if ( !m_ifProvinceMode ) return false;
#ifdef _DEBUG_		   
        printf("判断是否是省中心模式：isProvinceInfo\n");
#endif	    
	int infotype = atoi((*record)[1]);
	switch(infotype)
	{
		//客户
		case 100:
			//判断CUST_CLASS_TYPE,集团客户级别归属：0－省级、1－市级、2－区县级
			if (strlen((*record)[INFO_ADDLEN+23])>0 && (*record)[INFO_ADDLEN+23][0]=='0')
			{
				return true;
			}
			break;
		//集团客户
		case 102:
			//判断CUST_CLASS_TYPE,集团客户级别归属：0－省级、1－市级、2－区县级
			if (strlen((*record)[INFO_ADDLEN+5])>0 && (*record)[INFO_ADDLEN+5][0]=='0')
			{
				return true;
			}
			break;
		//账户
		case 200:
		//用户
		case 300:
		//用户_成员
		case 301:
		//用户_重要
		case 302:
		//用户_其他
		case 307:
			//以99打头的账户、用户标识是涉及跨地市的账户、用户
			if (strncmp((*record)[INFO_ADDLEN], "99" ,2) == 0)
			{
				return true;
			}
			break;
		//用户_服务
		case 304:
		//用户_ 服务状态
		case 306:
		//账户托收银行
		case 203:
			//以99打头的账户、用户标识是涉及跨地市的账户、用户
			if (strncmp((*record)[INFO_ADDLEN+1], "99" ,2) == 0)
			{
				return true;
			}
			break;
		//资费实例
		case 401:
			//以99打头的账户、用户标识是涉及跨地市的账户、用户
			if (strncmp((*record)[INFO_ADDLEN+2], "99" ,2)==0 && ((*record)[INFO_ADDLEN+1][0]=='0'||(*record)[INFO_ADDLEN+1][0]=='1'))
			{
				return true;
			}
			break;
		//付费关系
		case 313:
			//以99打头的账户、用户标识是涉及跨地市的账户、用户
			if (strncmp((*record)[INFO_ADDLEN+1], "99", 2)==0 || strncmp((*record)[INFO_ADDLEN+9], "99", 2)==0)
			{
				return true;
			}
			break;
		//银行代扣表
		case 319:
			//以99打头的账户、用户标识是涉及跨地市的账户、用户
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