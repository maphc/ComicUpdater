// InfoloadLike.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
using namespace std;




void testDb(){
	DbInterface m_sysdbi;
	m_sysdbi.connect("uop_act3","123456","133.224.202.168/act630");
	string sql,m_lastdealfilename;
	sql= "SELECT LTRIM(RTRIM(VALUE_S)) FROM TD_B_SYSPARAM WHERE SYS_PARAM_CODE='DATALOAD_LAST_FILENAME'  AND USE_TAG='1' AND ROWNUM<2 ";

	try
	{
		m_sysdbi.execStaticSql(sql);
		while(m_sysdbi.getNext())
		{
			m_lastdealfilename = m_sysdbi.getColumnStringValue(1);
			cout<<""<<m_lastdealfilename<<endl;
		}
	}
	catch(CException e)
	{
		m_sysdbi.disconnect();
		cout<<e.GetString();


	}
}
DbDesc	m_sysDbDesc;
DbInterface						m_sysdbi;
//物理数据库、内存数据库描述信息集合
map<string, DbDesc>				m_dbDescMap;
//map<string, MmDesc>				m_mmDescMap;
map<string, DbInterface*>		m_dbiMap;
map<string, MmInterface*>		m_mmiMap;

vector<DbInterface*>			m_dbisPro;
vector<MmInterface*>			m_mmisPro;
//资料信息列数、目标规则集合
int								m_infolens[MAX_INFOTYPE_VALUE+1];
LoadTable						m_loadtables[MAX_INFOTYPE_VALUE+1];
vector<DestRule>				m_destRuleVector;
//文件处理类、目标库分析类、信息处理类、参数处理类
FileDeal						m_fileDeal;
DestRuleParse					m_destRuleParse;
InfoProcess						m_infoProcess;
//ParamProcess					m_paramProcess;
//当前时间、最后一次处理的文件名称
string							m_currtime;
string							m_lastdealfilename;
//文件路径、备份文件路径、错误文件路径、文件名称、文件分隔符号、休眠时间
string							m_filepath;
string							m_bakfilepath;
string							m_errfilepath;
string							m_filename;
string							m_seperator;
int							    m_sleeptime;
int									m_tag;
//允许的最大错误记录数
int									m_errnum;
int              m_Batchmaxrecordsize;
string           m_hostName;
/* CRM到ACT无缝全量专用  xueyf@lianchuang.com*/
map<string, DbDesc>         m_dbcrmDescMap;
map<string, DbInterface*>   m_dbcrmMap;
int                         m_crmnum;
int                         m_actnum;
BatchInDBProcess m_BatchInDbProcess ; 

bool init(){

	//(1)获取系统默认数据库信息
	m_sysDbDesc.m_username=GetSimpleCfgParam("sys_db_username");
	m_sysDbDesc.m_password=GetSimpleCfgParam("sys_db_password");
	m_sysDbDesc.m_servname=GetSimpleCfgParam("sys_db_servname");

	//(2)获取系统定义的所有数据库信息
	//(2.1)所有物理库信息
	int dbnum=atoi(GetSimpleCfgParam("obj_db_num").c_str());
	for(int i=0;i<dbnum;i++){
		char ci[4];
		sprintf(ci, "%d", i);
		string si = ci;
		DbDesc dbDesc;

		string sParamName = "obj_db_name_"+si;
		string objdbname = GetSimpleCfgParam(sParamName);
		sParamName = "obj_db_username_"+si;
		dbDesc.m_username =GetSimpleCfgParam(sParamName);
		sParamName = "obj_db_password_"+si;
		dbDesc.m_password =GetSimpleCfgParam(sParamName);
		sParamName = "obj_db_servname_"+si;
		dbDesc.m_servname =GetSimpleCfgParam(sParamName);

		
		m_dbDescMap.insert(map<string, DbDesc>::value_type(objdbname, dbDesc));
		DbInterface *dbi = new DbInterface();
		dbi->setobjalisename( objdbname );
		m_dbiMap.insert(map<string, DbInterface*>::value_type(objdbname, dbi));
	}

	//(2.2)所有内存库信息
	//skip

	//(3)获取文件路径、休眠时间等信息
	m_filepath=GetSimpleCfgParam("file_path");
	m_bakfilepath=GetSimpleCfgParam("bak_file_path");
	m_errfilepath=GetSimpleCfgParam("err_file_path");
	if(GetSimpleCfgParam("allow_max_errnum")==""){
		m_errnum = -1;
	}else{
		m_errnum = atoi(GetSimpleCfgParam("allow_max_errnum").c_str());
	}
	
	string sep=GetSimpleCfgParam("separator_sign");
	char c1[2];
	char c2[2];
	c1[0] = sep[2];
	c1[1] = '\0';
	c2[0] = sep[3];
	c2[1] = '\0';
	m_seperator=atoi(c1)*16+atoi(c2);
	
	//(5)从默认数据库获取:最后更新时间,最后一次处理错误的文件名称,各类信息长度,各类信息划分规则
	//(5.1)连接默认数据库
	//DbInterface sysdbi;
	try
	{
		m_sysdbi.connect(m_sysDbDesc.m_username, m_sysDbDesc.m_password, m_sysDbDesc.m_servname);
	}
	catch (CException e)
	{
		cout<<"连接数据库失败"<<e.GetString()<<endl;
		return false;
	}

	//(5.11)获取省中心入库的数据名称

	//(5.2)获取最后一次处理的文件名称

	//(5.4)获取各类信息长度
	string strSql;
	memset(m_infolens, 0x0, (MAX_INFOTYPE_VALUE+1)*sizeof(int));
	memset(m_loadtables, 0x0, (MAX_INFOTYPE_VALUE+1)*sizeof(LoadTable));
	strSql =  "	SELECT	INFO_TYPE,INFO_FIELD_NUM,INFO_KEY_FIELDPOS,DB_TABLE_NAME,DB_COLUMN_NUM,DB_KEY_COLUMNPOS,DB_COLUMN_DESC,		";
	strSql += "			MM_TABLE_NAME,MM_COLUMN_NUM,MM_KEY_INDEXNAME,MM_KEY_COLUMNPOS,MM_INDEXNAME_PK,MM_COLUMN_DESC,TRIGGER_FLAG	";
	strSql += "	FROM	TD_B_INFO_LOADRULE			";

	try
	{
		m_sysdbi.execStaticSql(strSql);
		while(m_sysdbi.getNext())
		{			
			int infotype = m_sysdbi.getColumnIntValue(1);
			int infolen = m_sysdbi.getColumnIntValue(2);

			LoadTable loadTable;
			loadTable.m_infotype = m_sysdbi.getColumnIntValue(1);
			loadTable.m_infofieldnum = m_sysdbi.getColumnIntValue(2);
			loadTable.parseinfokeyfieldpos(m_sysdbi.getColumnStringValue(3));
			loadTable.m_dbtablename = m_sysdbi.getColumnStringValue(4);
			loadTable.m_dbcolumnnum = m_sysdbi.getColumnIntValue(5);
			loadTable.parsedbkeyfieldpos(m_sysdbi.getColumnStringValue(6));
			loadTable.parseDbColumns(m_sysdbi.getColumnStringValue(7));
			loadTable.m_mmtablename = m_sysdbi.getColumnStringValue(8);
			loadTable.m_mmcolumnnum = m_sysdbi.getColumnIntValue(9);
			loadTable.m_mmkeyindexname = m_sysdbi.getColumnStringValue(10);
			loadTable.parsemmkeyfieldpos(m_sysdbi.getColumnStringValue(11));
			loadTable.m_mmindexnamepk = m_sysdbi.getColumnStringValue(12);
			loadTable.parseMmColumns(m_sysdbi.getColumnStringValue(13));
			loadTable.m_triggerflag = (m_sysdbi.getColumnStringValue(14))[0];

			m_infolens[infotype] = infolen;
			m_loadtables[infotype] = loadTable;
		}
	}
	catch(CException e)
	{
		m_sysdbi.disconnect();

		cout<<"(5.4)获取各类信息长度 error "<<e.GetString()<<endl;
		return false;
	}

	//(5.5)获取目标分拣规则,增加批量入库配置和MDB ACT分离 兼容广东模式 wangfc@lianchuang.com 2010.04.22
  	m_destRuleVector.clear();
  	strSql =  " SELECT INFO_TYPE,EPARCHY_CODE,OBJ_DB_NAMES,OBJ_MM_NAMES,OBJ_DB_DEALTAG,OBJ_DB_IN_TABLENAMES,OBJ_DB_IN_PROCS,";
  	strSql += " OBJ_PROVDB_IN_TABLENAMES,OBJ_PROVDB_IN_PROCS FROM TD_B_INFO_DESTRULE  ";//order by 1,2
  	strSql += " ORDER BY 1,2 ";
    m_Batchmaxrecordsize               = MAX_BATCH_COMMIT_RECORD_SIZE ;//默认写死。如有需要，可以参数配置 wangfc@lianchuang.com 2010-03.19
  	map<int, DestRule>                 m_DisctntInfoTypeDescMap;
  	try
  	{
  		m_sysdbi.execStaticSql(strSql);	
  		int destinfotype = 0 ;
  		while(m_sysdbi.getNext())
  		{		
  			DestRule destRule;
  			destRule.m_infotype = m_sysdbi.getColumnIntValue(1);
  			
  			destRule.m_srcdbnum = atoi((m_sysdbi.getColumnStringValue(2)).c_str());
  			destRule.m_objdbnames = m_sysdbi.getColumnStringValue(3);
  			destRule.m_objmmnames = m_sysdbi.getColumnStringValue(4);
        /*
           批量增加部分 wangfc@lianchuang.com 2010.03.29
        */
        destRule.m_objdeal               =  m_sysdbi.getColumnIntValue(5);
  	  	destRule.m_obj_db_tablenames     =  m_sysdbi.getColumnStringValue(6);
  		  destRule.m_obj_db_procs          =  m_sysdbi.getColumnStringValue(7);
  		  destRule.m_obj_provdb_tablenames =  m_sysdbi.getColumnStringValue(8);
  		  destRule.m_obj_provdb_procs      =  m_sysdbi.getColumnStringValue(9);		 
  		  destinfotype                     =  destRule.m_infotype; 
  		  if ( destRule.m_objdeal == 1 )
  		  {
  		   /*
  		     一个增量通道目前的设计模式，之可能是处理一个库的增量文件，而此段代码
  		     将分配了多次的内存（同一个infotype）
  		   */
          printf("读取批量入库参数并加载\n");
  		  LoadTable destloadTable          = m_loadtables[destinfotype] ;
    	  destRule.m_MaxRecordCount        = m_Batchmaxrecordsize ;
    		  //如果没有分配的话，直接分配  2010-6-23 12:21:25 修正多次分配内存BUG
    		  {
    		      printf("读取批量入库参数并加载 infotype=%d\n",destinfotype); 
        		  destRule.setBatchParam    ( destloadTable.m_dbcolumns  ,destloadTable.m_dbcolumnnum,m_Batchmaxrecordsize );
        		  destRule.setBatchProvParam( destloadTable.m_dbcolumns  ,destloadTable.m_dbcolumnnum,m_Batchmaxrecordsize );
        		  destRule.ResetBatchParam ();	
    		  }
  	   
          printf("读取批量入库参数并加载(完)\n");
  
  		  }		
  			//----------end
  			m_destRuleVector.push_back(destRule);
  		}
  		
  	}
  	catch(CException e)
  	{
  		m_sysdbi.disconnect();
  		
  		cout<<"TD_B_INFO_DESTRULE error"<<e.GetString()<<endl;
  		return false;
  	}
  	partial_sort(m_destRuleVector.begin(), m_destRuleVector.end(), m_destRuleVector.end());
  
  	//(6)关闭系统数据库连接
  	
  
  	//(7)初始化对应的类成员
  	m_fileDeal.setfilepath(m_filepath.c_str());
  	m_fileDeal.setbakfilepath(m_bakfilepath.c_str());
  	m_fileDeal.setinfolen(m_infolens);
  	m_fileDeal.setseperator(m_seperator.c_str());
  	m_fileDeal.setloadtables(m_loadtables);
  
	return true;
	
}
void getcurrtime()
{
	char currtime[15];
	time_t tNowTime;
	struct tm *stNowTime = NULL;
	time(&tNowTime);
	stNowTime=localtime(&tNowTime);

	sprintf(currtime, "%04d%02d%02d%02d%02d%02d",
		stNowTime->tm_year+1900,stNowTime->tm_mon+1,stNowTime->tm_mday,stNowTime->tm_hour,stNowTime->tm_min,stNowTime->tm_sec);
	m_currtime = currtime;
}
void addTime(string &baseTimeStr, const int& addSeconds)
{		
	char baseTime[15];
	strncpy(baseTime, baseTimeStr.c_str(), 14);
	int second = 0;
	int minute = 0;
	int hour = 0;
	int day = 0;
	int month = 0;
	int year = 0;

	char tmp_s3[3];
	char tmp_s5[5];
	memset(tmp_s3, 0x0, 3);
	memset(tmp_s5, 0x0, 5);

	time_t  timeTTT;
	struct tm timeTm;
	strncpy(tmp_s5, baseTime, 4);
	timeTm.tm_year = atoi(tmp_s5) - 1900;
	strncpy(tmp_s3, baseTime+4, 2); 
	timeTm.tm_mon = atoi(tmp_s3) - 1;
	strncpy(tmp_s3, baseTime+6, 2); 
	timeTm.tm_mday = atoi(tmp_s3);
	strncpy(tmp_s3, baseTime+8, 2); 
	timeTm.tm_hour = atoi(tmp_s3);
	strncpy(tmp_s3, baseTime+10, 2); 
	timeTm.tm_min = atoi(tmp_s3);
	strncpy(tmp_s3, baseTime+12, 2); 
	timeTm.tm_sec = atoi(tmp_s3);
	timeTTT = mktime(&timeTm);
	timeTTT += addSeconds;
	struct tm *ptime = localtime(&timeTTT);
	sprintf(baseTime, "%04d%02d%02d%02d%02d%02d",
		ptime->tm_year+1900,ptime->tm_mon+1,ptime->tm_mday,ptime->tm_hour,ptime->tm_min,ptime->tm_sec);
	baseTimeStr = baseTime;
}

int _tmain(int argc, _TCHAR* argv[])
{
	
	if(!init()){
		cout<<"初始化失败"<<endl;
	}
	
	getcurrtime();
	int m_channelNo=1;
	char   sql[2000] = "\0";
	string strSql;
	if ( m_crmnum == 0 )
	{
		//(3)全量(MDB ACT)参数导入处理
		//(3.1)获取装载参数
		vector<TableLoadRule> tableLoadRuleVector;
		tableLoadRuleVector.clear();
		sprintf(sql,"SELECT LOAD_TYPE,SRC_DB_NAME,SRC_SEL_SQL,OBJ_MM_NAMES,TABLE_NAME,OBJ_UPDATE_SQL,TRIGGER_METHOD,TRIGGER_TAG,INTERVAL_TIME,UPDATE_TIME,LOADRULE_ID FROM TD_B_TABLE_LOADRULE WHERE CHANNEL_NO=%d ORDER BY LOADRULE_ID",m_channelNo);
		strSql = sql;
		try
		{
			m_sysdbi.execStaticSql(strSql);
			while(m_sysdbi.getNext())
			{
				//进行资料装载
				TableLoadRule tableLoadRule;

				tableLoadRule.m_loadtype = (m_sysdbi.getColumnStringValue(1))[0];
				tableLoadRule.m_srcdbname = m_sysdbi.getColumnStringValue(2);
				tableLoadRule.m_srcselsql = m_sysdbi.getColumnStringValue(3);
				tableLoadRule.m_objmmnames = m_sysdbi.getColumnStringValue(4);
				tableLoadRule.m_tablename = m_sysdbi.getColumnStringValue(5);
				tableLoadRule.m_objupdatesql = m_sysdbi.getColumnStringValue(6);
				tableLoadRule.m_triggermethod = (m_sysdbi.getColumnStringValue(7))[0];
				tableLoadRule.m_triggertag = (m_sysdbi.getColumnStringValue(8))[0];
				tableLoadRule.m_intervaltime = m_sysdbi.getColumnIntValue(9);
				tableLoadRule.m_updatetime = m_sysdbi.getColumnDateValue(10);
				tableLoadRule.m_addupdatetime = m_sysdbi.getColumnDateValue(10);
				tableLoadRule.m_loadruleid = m_sysdbi.getColumnIntValue(11);
				addTime(tableLoadRule.m_addupdatetime, tableLoadRule.m_intervaltime);

				tableLoadRuleVector.push_back(tableLoadRule);
			}
		}		
		catch(CException e)
		{
			m_sysdbi.disconnect();

			cout<<"3.1)获取装载参数"<<e.GetString()<<endl;
			return false;
		}

		//(3.2)进行数据装载
		for (vector<TableLoadRule>::iterator iterx=tableLoadRuleVector.begin(); iterx!=tableLoadRuleVector.end(); ++iterx)
		{
			if ((iterx->m_triggermethod=='1' && iterx->m_triggertag=='0') ||
				(iterx->m_triggermethod=='2' && iterx->m_addupdatetime<m_currtime))
			{
				//获取目标内存数据库操作类指针集合
				vector<string>			objmmnameVector;
				StringUtil::split(iterx->m_objmmnames, ";", objmmnameVector, false);

				//更新装载标志为成功装载
				
			}
		}
	}

	//"e:\\LNDATA\\Billing_测试\\billdata\\infolad\\1\\1_20111130082406_000000.CTBII";
	m_filename="1_20111130091544_000005.CTBII";

	string chgid = "";
	string oldChgid = "";
	int infotype = 0;
	int srcdbNum = 0;
	int recordnum = 0;
	char line[MAX_FILERECORD_LEN] = "\0";

	Info info;
	m_fileDeal.setfilename(m_filename.c_str());	
	srcdbNum = m_fileDeal.getsrcdbnum();
	int totalrecnum = 0;
	int trycount = 0;

	try
	{
		m_destRuleParse.init(&m_dbiMap, &m_mmiMap, &m_destRuleVector);
		m_infoProcess.init(m_loadtables, &m_destRuleParse, &m_dbisPro, &m_mmisPro);
		//ResetBatchdestRuleParam();
		for( int j = 0 ; j < m_destRuleVector.size() ; ++j )
		{
			if (  m_destRuleVector[j].m_objdeal == 1 )
				m_destRuleVector[j].ResetBatchParam();
		}
		
		m_BatchInDbProcess.setMaxBatchRecordNum( m_Batchmaxrecordsize );
		m_BatchInDbProcess.init( m_loadtables, &m_destRuleParse, &m_dbisPro );
		m_BatchInDbProcess.setchannelno ( m_channelNo );//设置通道号
		m_BatchInDbProcess.setfilename ( m_filename.c_str() );
		m_BatchInDbProcess.setProvinceMode (  m_dbisPro.size() != 0 ) ;//是否是省中心模式 

		m_fileDeal.open();
		int errnum = 0;
		while((recordnum=m_fileDeal.getInfo(chgid, infotype, info, line)) > 0)
		{
			// (4.3.1) 根据info_type类型判断是否要批量操作，如果是的话，则调用相关函数 wangfc@lianchuang.com 2010.03.30
			try
			{

					// 批量入临时表处理子过程：某类型下的记录数达到1000行，自己提交到数据库
					m_BatchInDbProcess.processRecord(chgid ,infotype,srcdbNum, & info);
		      
			}catch (CException e)
			{
				//程序退出，等待人工干预
				throw CException(__FILE__,__LINE__,"批量入库时，增加临时表出错,文件=%s,seqid=%s,infotype=%d,err=%s", m_filename.c_str(),chgid.c_str(),infotype,e.GetString());
			}
			//	(4.3.2) 正常文件处理，按流水号+infotype处理记录	    
			try
			{
				m_infoProcess.processTrade(chgid, infotype, srcdbNum, &info);
				totalrecnum += recordnum;
			}
			catch (CException e)
			{
				cout<<e.GetString()<<endl;
			} 
		}



	}catch (CException e)
	{
		info.clear();
		m_fileDeal.close();
		m_sysdbi.disconnect();

		cout<<"处理文件失败"<<e.GetString()<<endl;
		return false;
	}
	

	try
	{
		m_sysdbi.disconnect();
	}
	catch (CException e)
	{
		cout<<"关闭链接失败"<<endl;
		return false;
	}
	return 0;
}

