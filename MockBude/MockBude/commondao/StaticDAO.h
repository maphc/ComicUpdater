#ifndef _STATIC_DAO_H__
#define _STATIC_DAO_H__

#include <Windows.h>
#include <string>
#include <map>
#include <vector>
#include "../DataBuf.h"
#include "occi.h"



using namespace std;
using namespace oracle::occi;

//typedef struct Param{
//	string key;
//	int type;	//0--stirng 1--int
//	long lv;
//	string sv;
//
//	bool operator ==(struct Param& r){
//		return key==r.key&&type==r.type&&(type==0?sv==r.sv:lv==r.lv);
//	}
//} P;


class StaticDAO {
public:
	//!
	static StaticDAO& getDAO();
	void Clear();


	void SetParam(string k,string v);
	void SetParam(string k, long v);
	void SetParam(string k, int v);

	long jselect(DataBuf& buf);
	long jselect(DataBuf& buf, const string& sqlRef, const string& tabName);
	long jselect(DataBuf& buf, const string& sqlRef, const string& tabName, const string& swhere);
    long jselpage(DataBuf& buf, const string& sqlRef, const string& tabName, int page, int rowsPerPage);
	long jcount(const string& sqlRef, const string& tabName);
	long jcount(const string& sqlRef, const string& tabName, const string& swhere);
	long jupdate(const string& sqlRef, const string& tabName);
	long jdelete(const string& sqlRef, const string& tabName);
	long jinsert(const string& sqlRef, const string& tabName, int times=1);

    //! À©Õ¹SQLÓï¾äÖ´ÐÐº¯Êý
    long jexecext(const string& sqlRef, const string& tabName, int exTimes=1);
    
    long jselsqlpage(DataBuf& buf, const string&  sql, int page, int rowsPerPage);
	long jselsql(DataBuf& buf, const string&  sql,int tag);
	
	void select(DataBuf& out,string sql,...);

	long ResultSet2DataBuf( ResultSet* rs, DataBuf &out );
	long execQueryInternal(DataBuf&out,string& sql);
	long execQueryInternal(DataBuf&out,char* sql);
	long execUpdateInternal(const char* sql);
	long execUpdateInternal(string& sql);

private:
	//!
	StaticDAO() ;
	~StaticDAO();
	static StaticDAO* _inst; 
	
	void initConfig();
	void initDb();
	static string configFileName;
	string curDir;
	string getConfigFile();
	
	
	void resovleParams(string& sql);
	
	string findSqlByRef(const string& sqlRef,const string&tabName);

private:
	string username;
	string pass;
	string dburl;
	Environment * env;
	Connection* con;
	
	map<string,string> sqlCache;
	DataBuf paramValues;
	vector<PAnyData> sortedParamList;

	

};


#endif

