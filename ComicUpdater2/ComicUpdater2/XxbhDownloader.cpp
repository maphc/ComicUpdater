#include "stdafx.h"
#include "V8Helper.h"
#include "PyHelper.h"
//<h2>��Ӱ����</h2>
CRegexpT<TCHAR> XxbhDownloader::titleRegex(_T("<h2>([^>]*?)</h2>"));
//<li>����ʱ�䣺2014-02-05</li>
CRegexpT<TCHAR> XxbhDownloader::lastDateRegex(_T("����ʱ�䣺(\\d{4}-\\d{1,2}-\\d{1,2})"));
//<li><a href="/comic/12912/121236.html" title="��553��" target="_blank" class="new">��553��<em></em></a></li>
//<li class="ul7_li2"><a title="��663��" href="/comic/12913/128079.html" target="_blank">��663��</a></li>
CRegexpT<TCHAR> XxbhDownloader::lastVolRegex(_T("target=\"_blank\">([^\"]+?)</a></li>"));
//<li><a class="f_red" href="http://comic.xxbh.net/201104/189718.html" target="_blank" title="����֮�� ��1��">��1��</a></li>
//<li class="ul7_li2"><a title="��663��" href="/comic/12913/128079.html" target="_blank">��663��</a></li>
//<li class="ul7_li1"><a title="��662��" href="/comic/12913/127186.html" target="_blank">��662��</a></li>
CRegexpT<TCHAR> XxbhDownloader::volRegex(_T("<a[^>]*? href=\"([^\"]+?)\" [^>]*?>([^\"]+)</a></li>"));
//<div class="left2">
CRegexpT<TCHAR> XxbhDownloader::isMainRegex(_T("<div class=\"left2\">"));

VOID Get_images_arr(CString& t,vector<CString>& imgs ){

	

}

UINT XxbhDownloader::GetPageEncoding(){
	return USE_UTF8;
}
VOID Get_img_s(CString& t,UINT& n){
    CString img_s_reg(_T("var img_s = (\\d+);"));
    CString img_s=Downloader::GetMatchedStr(img_s_reg,t);
    n=atoi(img_s);
}
VOID Get_img_svraa(CString& t,vector<CString>& servs ){
    CString regStr(_T("img_svrab\\[(\\d+)\\]\\s*=\\s*\"(http://.+?)\""));
	CRegexpT<TCHAR> regex(regStr);
    //Downloader::GetMatchedList(regStr,t,servs);
	MatchResult mrRet = regex.Match( t );
	while(mrRet.IsMatched()){
		UINT a=mrRet.GetGroupStart(2);
		UINT b=mrRet.GetGroupEnd(2)-mrRet.GetGroupStart(2);
		CString value=t.Mid(a,b);
		UINT m=mrRet.GetGroupStart(1);
		UINT n=mrRet.GetGroupEnd(1)-mrRet.GetGroupStart(1);
		CString key=t.Mid(m,n);

		if(value[0]==_T('/')){
			value=_T("http://comic.xxbh.net/")+value;
		}
		UINT index=atoi(key);
		if(index>=servs.size()){
			servs.resize(index+1);
		}
		servs.at(index)=value;
		//servs.push_back(value);
		mrRet = regex.Match(t, mrRet.GetEnd());

	}
	
	

}

XxbhDownloader::XxbhDownloader(void):Downloader()
{
    SetPageEncoding(USE_UTF8);
}

XxbhDownloader::~XxbhDownloader(void)
{
}

vector<CString> XxbhDownloader::GetPicUrls(CString& strid ){
	
	CString resp;
	if(strid.Left(1)==_T("/")){
		strid=_T("http://mh.xxbh.net")+strid;
	}
	GetSimpleGet(strid,resp);
	CString pics=PyHelper::getXxbhPics(resp);
	vector<CString> a=this->Split(pics,",");

	return a;
}

vector<CString> XxbhDownloader::GetPicUrls_Old( CString& strid )
{
    

	vector<CString> a;
    vector<CString> jsList;
	
	CString resp;
	
	if(strid.Left(1)==_T("/")){
		strid=_T("http://mh.xxbh.net")+strid;
	}
	GetSimpleGet(strid,resp);
	
    //CString jsRex(_T("<script type=\"text/javascript\" src=\"(http://.*?)\"></script>"));
    CString jsRex(_T("<script type=\"text/javascript\" src=\"(.*?)\"></script>"));
	GetMatchedList(jsRex,resp,jsList);


	CString js0,js1,js2,jssvr;
	UINT readIndex=0;
    GetSimpleGet(jsList.at(readIndex++),js0,strid);
	if(js0.Find(_T("404 Not Found"))>-1||js0.Find(_T("Bad Request"))>-1){
		GetSimpleGet(jsList.at(readIndex++),js0,strid);
	}

	GetSimpleGet(jsList.at(readIndex++),js1,strid);
	GetSimpleGet(jsList.at(readIndex++),js2,strid);

	vector<CString> images_arr;

	UINT img_s;
	vector<CString> msgList;

	if(js0.Find(_T("eval"))==-1){
		//Get_images_arr(js0,images_arr);
		CString msgs=Downloader::GetMatchedStr(_T("var msg='([^']+)'"),js0);//
		msgList=Downloader::Split(msgs,_T("|"));
		Get_img_s(js0,img_s);

	}else{
		//�·�ʽ
		CString msgs;
		V8Helper::getXxbhInfo(js0,img_s, msgs);
		msgList=Downloader::Split(msgs,_T("|"));

	}
	vector<CString> servs;
	js2.Remove(_T('\r'));
	js2.Remove(_T('\n'));
	CString svrListJs=Downloader::GetMatchedStr(_T("\ttsvrJs\\s*=\\s*'(.*?)';"),js2);
	if(svrListJs=="" || svrListJs.Find(_T("http://"))!=0){
		Get_img_svraa(js1,servs);
		if(servs.size()==0){
			AfxMessageBox(_T("δ�ҵ������б�"));
			return a;
		}
		//
		//
	}else{
		GetSimpleGet(svrListJs,jssvr,strid);
		
		Get_img_svraa(jssvr,servs);
	}
	
	


	for(UINT i=0;i<msgList.size();i++){
		CString aUrl=servs.at(img_s)+msgList.at(i);
		a.push_back(aUrl);
	}
    

	return a;
}

Downloader* XxbhDownloader::CreateNewInst()
{
	return new XxbhDownloader;
}

CRegexpT<TCHAR>& XxbhDownloader::GetIsMainRegex()
{
return isMainRegex;
}

CRegexpT<TCHAR>& XxbhDownloader::GetTitleRegex()
{
	return titleRegex;
}

CRegexpT<TCHAR>& XxbhDownloader::GetLastVolRegex()
{
	return lastVolRegex;
}

CRegexpT<TCHAR>& XxbhDownloader::GetLastDateRegex()
{
	return lastDateRegex;
}

CRegexpT<TCHAR>& XxbhDownloader::GetVolRegex()
{
	return volRegex;
}

VOID XxbhDownloader::GetVolUrlNamePair( CString& line,pair<CString,CString>& p )
{
	MatchResult mrRet=GetVolRegex().Match( line );
	if(mrRet.IsMatched()){
		p.first=UTF8ToUnicode(line.Mid(mrRet.GetGroupStart(2),mrRet.GetGroupEnd(2)-mrRet.GetGroupStart(2)));
		p.second=line.Mid(mrRet.GetGroupStart(1),mrRet.GetGroupEnd(1)-mrRet.GetGroupStart(1));
	}
}

VOID XxbhDownloader::PostParse()
{
	

}
