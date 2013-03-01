
#include "stdafx.h"
#include "StaticDAO.h"

using namespace std;
using namespace oracle::occi;


StaticDAO* StaticDAO::_inst=NULL;
string StaticDAO::configFileName("config.ini");

StaticDAO& StaticDAO::getDAO()
{
	if(_inst==NULL){
		_inst=new StaticDAO;
	}
	return *_inst;
}

StaticDAO::StaticDAO()
{
	initConfig();
	initDb();
}

void StaticDAO::initConfig()
{
	char dirBuf[100]={0};
	GetCurrentDirectory(100,dirBuf);
	curDir=dirBuf;
	
	char userBuf[100]={0};
	configFileName="config.ini";
	string fileName=curDir+"\\"+"config.ini"; //configFileName;
	GetPrivateProfileString("DB_CONFIG","username","",userBuf,100,fileName.c_str());
	username=userBuf;

	char passBuf[100]={0};
	GetPrivateProfileString("DB_CONFIG","pass","",passBuf,100,fileName.c_str());
	pass=passBuf;

	char urlBuf[100]={0};
	GetPrivateProfileString("DB_CONFIG","dburl","",urlBuf,100,fileName.c_str());
	dburl=urlBuf;

}

StaticDAO::~StaticDAO()
{
	if(con&&env){
		env->terminateConnection(con);
	}
	if(env){
		Environment::terminateEnvironment(env);
	}
	delete _inst;
}

void StaticDAO::initDb()
{
	env=Environment::createEnvironment();
	con=env->createConnection(username,pass,dburl);
}

string StaticDAO::getConfigFile()
{
	return curDir+"\\"+configFileName;
}

void StaticDAO::select( DataBuf& out,string sql,... )
{
	//1、首先定义va_list型的变量，这个变量是指向参数的指针。

	//	2、然后用va_start宏初始化刚定义的va_list变量，这个宏的第二个参数是“第一个可变参数的前一个参数”。
	//	3、再用va_arg得到可变参数，第二个参数是“可变参数”的类型。
	//	4、最后用va_end宏结束可变参数的获取。如果有多个可变参数，依次调用va_list获取各个参数。
	va_list vaList;
	va_start(vaList,sql);
	

	try
	{
		Statement* stmt=con->createStatement();
		stmt->setSQL(sql);
		int i=1;
		while(1){
			string name=va_arg(vaList,string);
			if(name=="END"){
				break;
			}else{
				stmt->setString(i,name);
			}
			i++;
		}
		
		va_end( vaList );

		ResultSet* rs=stmt->executeQuery();

		ResultSet2DataBuf(rs, out);

	
		con->terminateStatement(stmt);
		

	}catch (SQLException &e)
	{
		cout<<"Code :"<<e.getErrorCode()<<endl;
		cout<<"What :"<<e.what()<<endl;
	}
}

void StaticDAO::resovleParams(string& originalSql)
{
	vector<string> partSql;
	map_Data params=paramValues.GetData()[0];
	int i = 1;

	cout<<"originalSql :"<<originalSql<<endl;;


	unsigned int sqlIndex = 0;
	while (sqlIndex < originalSql.size()) {
		unsigned int keyBegin = originalSql.find_first_of(':', sqlIndex);
		if (keyBegin == string::npos) {
			break;
		}
		int keyEnd = originalSql.find_first_of(" ,)(\n\t", keyBegin + 1);
		partSql.push_back(originalSql.substr(sqlIndex, keyBegin - sqlIndex));

		string key(originalSql.substr(keyBegin, keyEnd - keyBegin));
		
		if (params.find(key) != params.end()) {
			//map_Data::iterator iter=params.find(key);
			char buf[10] = { 0 };
			sprintf(buf, ":%d", i);
			partSql.push_back(string(buf));
			cout << "[" << key << " is " << params[key]->ToString()  <<"]" ;
			sortedParamList.push_back(params[key]);
			i++;
		} else {
			partSql.push_back(originalSql.substr(keyBegin, keyEnd - keyBegin ));
		}


		sqlIndex = keyEnd ==-1?originalSql.size():keyEnd;
	}

	cout<<endl;

	partSql.push_back(originalSql.substr(sqlIndex));

	string finalSql = accumulate(partSql.begin(), partSql.end(), string(""));

	cout << "resolvedSql :"<<finalSql << endl; 

}
long StaticDAO::execQueryInternal( DataBuf&out,char* sql ){
	return execQueryInternal(out,string(sql));
}
long StaticDAO::execQueryInternal( DataBuf&out,string& sql )
{
	int num=0;
	try{
		Statement* stmt=con->createStatement();
		stmt->setSQL(sql);
		for(UINT i=0;i<sortedParamList.size();i++){
			stmt->setString(i+1,sortedParamList[i]->ToString());
		}
		ResultSet* rs=stmt->executeQuery();
		num=ResultSet2DataBuf(rs,out);

		con->terminateStatement(stmt);

	}catch(SQLException& e){
		cout<<"Code :"<<e.getErrorCode()<<endl;
		cout<<"What :"<<e.what()<<endl;
		throw e;
	}

	return num;
}

long StaticDAO::ResultSet2DataBuf( ResultSet* rs, DataBuf &out )
{
	vector<MetaData> metaList=rs->getColumnListMetaData();

	int row=0;
	while(rs->next()){

		for(UINT i=0;i<metaList.size();i++){
			string colName(metaList[i].getString(MetaData::ATTR_NAME));
			int colType=metaList[i].getInt(MetaData::ATTR_DATA_TYPE);
			//printf("Column[%s]Type[%d]\n",colName.c_str(),colType);
			string colValue=rs->getString(i+1);
			out.SetString(colName,colValue,row);

		}
		row++;
	}
	
	return row;
}

long StaticDAO::jselect( DataBuf& buf, const string& sqlRef, const string& tabName )
{
	string sql(findSqlByRef(sqlRef,tabName));
	resovleParams(sql);
	
	return execQueryInternal(buf,sql);
}

string StaticDAO::findSqlByRef( const string& sqlRef,const string&tabName )
{
	string sqlKey(sqlRef+"#"+tabName);
	if(sqlCache.find(sqlKey)!=sqlCache.end()){
		printf("find sqlRef[%s]tabName[%s] in cache !",sqlRef.c_str(),tabName.c_str());
		return sqlCache[sqlKey];
	}else{
		printf("sqlRef[%s]tabName[%s] not in cache, select ",sqlRef.c_str(),tabName.c_str());
		DataBuf out;
		string sql("select sql_stmt from code_code a where a.sql_ref=:1 and a.tab_name=:2");
		select(out,sql,sqlRef,tabName,string("END"));
		if(!out.IsFldExist("SQL_STMT")){
			throw new string("no sql "+sqlRef+" "+tabName);
		}
		sqlCache.insert(make_pair(sqlKey,out.GetString("SQL_STMT")));
		return out.GetString("SQL_STMT");
	}

}

void StaticDAO::SetParam( string k,string v )
{
	paramValues.SetString(k,v);
}

void StaticDAO::SetParam( string k, long v )
{
	paramValues.SetLong(k,v);
}

void StaticDAO::SetParam( string k, int v )
{
	paramValues.SetInt(k,v);
}

void StaticDAO::Clear()
{
	paramValues.Clear();
	sortedParamList.clear();
}

long StaticDAO::execUpdateInternal(const char* sql )
{
	int num=0;
	try{
		Statement* stmt=con->createStatement();
		stmt->setSQL(sql);
		for(UINT i=0;i<sortedParamList.size();i++){
			stmt->setString(i+1,sortedParamList[i]->ToString());
		}

		num = stmt->executeUpdate();
		
		con->terminateStatement(stmt);

	}catch(SQLException& e){
		cout<<"Code :"<<e.getErrorCode()<<endl;
		cout<<"What :"<<e.what()<<endl;
		throw e;
	}
	return num;
}

long StaticDAO::execUpdateInternal( string& sql )
{
	return execUpdateInternal(sql.c_str());
}

long StaticDAO::jinsert( const string& sqlRef, const string& tabName, int times/*=1*/ )
{
	string sql(findSqlByRef(sqlRef,tabName));
	resovleParams(sql);

	return execUpdateInternal(sql);
}
