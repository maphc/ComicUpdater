// MyOracle.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

using namespace std;
using namespace oracle::occi;


#pragma comment(lib,"oraocci11.lib")



int _tmain(int argc, _TCHAR* argv[]){

	string userName = "uop_busi_91"; 
	string password = "abc123"; 
	const string connectString = "132.35.76.131/essdev"; 
	try{

		Environment *env = Environment::createEnvironment(Environment::DEFAULT); 

		Connection *conn = env->createConnection(userName, password, connectString); 
		Statement *stmt = conn->createStatement("select * from td_s_brand "); 
		ResultSet *rs = stmt->executeQuery(); 
		while(rs->next()){
			string* i =&rs->getString(1);
			//int i=rs->getInt(1);
			cout << "Length of string : " <<i<<endl;
		}
		
		stmt->closeResultSet(rs); 
		conn->terminateStatement(stmt); 
		env->terminateConnection(conn); 

		Environment::terminateEnvironment(env); 
	}catch(SQLException& e ){
		cout<<"ERROR Code:"<<e.getErrorCode()<<endl;
		cout<<"ERROR MSG:"<<e.what()<<endl;
	}

	

	return 0;

}
