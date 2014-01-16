// MockBude.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "log4cpp/Logger.h"
#include "commondao/StaticDAO.h"
#include "inputoutput/InputOutput.h"
#include "Test.h"
#include "common/StrTable.h"
#include "commondao/DualDAO.h"
#include "UniCommonMgr/UniDataMgr.h"
#include "common/DualMgr.h"
#include "UniCommonMgr/UniDataMgr.h"
#include "commondao/CParamDAO.h"
#include "TradeMgr/TradeCheckBeforeTrade.h"
#include "TradeMgr/TradeCheckAfterTrade.h"


using namespace std;

void testDataBufLog(){
	DataBuf buf;

	buf.SetString("A","12345");

	cout<<"Buf :"<<buf.ToString()<<endl;
	cout<<"A :"<<buf.GetString("A")<<endl;

	using namespace log4cpp;
	static Logger& log = Logger::getLogger("gboss.crm.CustServ");

	LOG_TRACE(log,"abc");
	LOG_TRACE_P1(log, "Hi%d",123);

	LOG_DEBUG(log,"ohter");
	LOG_DEBUG_P2(log,"another[%s][%d]","debug",2);

	StaticDAO& dao = StaticDAO::getDAO();
	DataBuf out;

	string s;
	

	cout<<"Result :"<<out.ToString()<<endl;
}

void start(){
	DataBuf out;
	FileInput(out,"");
	
	ofstream f("z:\\output");
	f<<out.GetString("X_CODING_STR",1)<<endl;
	f<<out.GetInt("X_RESULTCODE")<<endl;
	f<<out.GetInt("X_RECORDNUM")<<endl;
	f<<out.GetInt("X_LCU_LOGSTEP")<<endl;
	f<<out.GetInt("X_LAST_RESULTCODE")<<endl;
	f<<out.GetString("ROUTE_EPARCHY_CODE")<<endl;
	f<<out.GetString("X_TRANS_CODE")<<endl;
	f<<out.GetString("X_RESULTINFO")<<endl;
	f<<out.GetString("X_CODING_STR")<<endl;
	
	f.close();

}
void testStrTable(){
	StrTable stbOutPrompt;
	stbOutPrompt.AddTable("Prmp", 1);
	stbOutPrompt.AddField("Prmp", "TradeCheck_CheckBlackUser:黑名单客户，建议终止业务的办理！\n是否要继续业务的办理？选择【确定】继续办理业务，选择【取消】终止办理业务。");

	string strResult;
	stbOutPrompt.ToString(strResult);
	cout<<"strResult "<<strResult<<endl;
}
void testDualMgr(){
	DataBuf in;
	GetSysDate(in,in);
	cout<<"DualDAO GetSysDate"<<in.GetString("X_SYSDATE")<<endl;
}
void testCParam(){
	int n=0;
	StaticDAO& dao=StaticDAO::getDAO();
	dao.Clear();
	DataBuf out;
	string key("TradeType");
	dao.SetParam(":VKEY",key);
	n=dao.jselect(out,key,"TD_S_CPARAM");
	if(n>0){
		map<string,string> item;
		for(UINT i=0;i<out.GetCount("KEY");i++){
			item.insert(make_pair(out.GetString("VALUE1",i)+"#"+out.GetString("VALUE2",i),out.GetString("VRESULT",i)));
		}
		
	}
}
void testTradeCheckBeforeTrade()
{

	try{
		DataBuf in;
		in.SetString("ID_TYPE","1");
		in.SetString("ID","3890486224960330");
		in.SetString("NET_TYPE_CODE","33");
		in.SetString("TRADE_EPARCHY_CODE","0010");
		in.SetString("TRADE_TYPE_CODE","120");
		in.SetString("TRADE_STAFF_ID","v-yuyan35-x");
		in.SetString("TRADE_DEPART_ID","TES00");
		in.SetString("TRADE_CITY_CODE","0010");
		TradeCheckBeforeTrade t;
		t.CheckBeforeTrade(in,in);
	}catch(exception& e){
		cout<<e.what()<<endl;
	}
}


int _tmain(int argc, _TCHAR* argv[])

{

	
	//testCParam();
	testTradeCheckBeforeTrade();
	


	return 0;
}

