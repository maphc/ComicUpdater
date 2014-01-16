// zenmebuhaoshi.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <occi.h>
#include <iostream>
using namespace oracle::occi;
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
    try
    {
        Environment* env=Environment::createEnvironment();
        string userName = "uop_busi_91"; 
		string password = "abc123"; 
		const string connectString = "132.35.76.131/essdev";  
        Connection* con=env->createConnection(userName,password,connectString);
        
        Statement *stmt = con->createStatement("select * from td_s_brand "); 
		ResultSet *rs = stmt->executeQuery(); 
		while(rs->next()){
			string brandCode=rs->getString(1);
			string brand=rs->getString(2);
			//int i=rs->getInt(1);
			cout <<brandCode<<"  "<<brand<<endl;
		} 
        con->terminateStatement(stmt);
        env->terminateConnection(con);
        Environment::terminateEnvironment(env);


    }
    catch (SQLException &e)
    {
        cout<<"Code :"<<e.getErrorCode()<<endl;
        cout<<"What :"<<e.what()<<endl;
        

    }
    
	return 0;
}

