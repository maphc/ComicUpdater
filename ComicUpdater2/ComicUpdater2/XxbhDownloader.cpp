#include "stdafx.h"


//<li><h1>白银之匙</h1></li>
CRegexpT<TCHAR> XxbhDownloader::titleRegex(_T("<li><h1>(.+)</h1></li>"));
//<li><b>更新</b>2011-04-05 02:16:46</li>
CRegexpT<TCHAR> XxbhDownloader::lastDateRegex(_T("<li><b>.*?</b>(\\d{4}-\\d{1,2}-\\d{1,2} \\d{1,2}:\\d{1,2}:\\d{1,2})</li>"));
//<li><a class="f_red" href="http://comic.xxbh.net/201104/189718.html" target="_blank" title="白银之匙 第1话">第1话</a></li>
CRegexpT<TCHAR> XxbhDownloader::lastVolRegex(_T("<li><a[^>]*? target=\"_blank\" [^>]*?>([^\"]+?)</a></li>"));
//<li><a class="f_red" href="http://comic.xxbh.net/201104/189718.html" target="_blank" title="白银之匙 第1话">第1话</a></li>
//<li><a href="http://comic.xxbh.net/201103/187880.html" target="_blank" title="死神 441话">441话</a></li>
CRegexpT<TCHAR> XxbhDownloader::volRegex(_T("<li><a[^>]*? href=\"([^\"]+?)\" [^>]*?>([^\"]+)</a></li>"));
//<div class="ar_list_coc">
CRegexpT<TCHAR> XxbhDownloader::isMainRegex(_T("<div class=\"ar_list_coc\">"));

VOID Get_images_arr(CString& t,vector<CString>& imgs ){

	

}

UINT XxbhDownloader::GetPageEncoding(){
	return USE_UNICODE;
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

vector<CString> XxbhDownloader::GetPicUrls( CString& strid )
{
    

	vector<CString> a;
    vector<CString> jsList;
	
	CString resp;
	
	if(strid.Left(1)==_T("/")){
		strid=_T("http://comic.xxbh.net")+strid;
	}
	GetSimpleGet(strid,resp);
	
    //CString jsRex(_T("<script type=\"text/javascript\" src=\"(http://.*?)\"></script>"));
    CString jsRex(_T("<script type=\"text/javascript\" src=\"(.*?)\"></script>"));
	GetMatchedList(jsRex,resp,jsList);


    CString js0,js1,js2,jssvr;
	UINT readIndex=0;
    GetSimpleGet(jsList.at(readIndex++),js0,strid);
	if(js0.FindOneOf(_T("404 Not Found"))>-1){
		GetSimpleGet(jsList.at(readIndex++),js0,strid);
	}
    GetSimpleGet(jsList.at(readIndex++),js1,strid);
    GetSimpleGet(jsList.at(readIndex++),js2,strid);
	GetSimpleGet(_T("http://img_v1.dm08.com/img_v1/cn_130117.js"),jssvr,strid);
    
    vector<CString> images_arr;
    //Get_images_arr(js0,images_arr);
	CString msgs=Downloader::GetMatchedStr(_T("var msg='([^']+)'"),js0);
	vector<CString> msgList=Downloader::Split(msgs,_T("|"));

    UINT img_s;
    Get_img_s(js0,img_s);

    vector<CString> servs;
    Get_img_svraa(jssvr,servs);

	
	for(UINT i=0;i<msgList.size();i++){
		CString aUrl=servs.at(img_s)+msgList.at(i);
		a.push_back(aUrl);
	}

	//if(sList.size()==2&&!sList.at(1).IsEmpty()){
	//	
	//	CString req,res;
	//	req.Format(_T("http://comic.xxbh.net/a/showbaidu.php?coid=%s&%s=%s"),coid,sList.at(0),servs.at(atoi(sList.at(1))-1));
	//	//http://comic.xxbh.net/a/showbaidu.php?coid=194224&s=http://222.218.156.16/h9/
	//	//http://comic.xxbh.net/a/showbaidu.php?coid=194224&s=http%3A%2F%2F222.218.156.16%2Fh8%2F
	//	//GetSimpleGet(req,res);
	//	TRACE(_T("Response : %s"),res);
	//	images_arr=Downloader::Split(res,_T(","));

	//	for(UINT i=0;i<images_arr.size();i++){
	//		CString picUrl=servs.at(img_s-1)+images_arr.at(i);
	//		a.push_back(picUrl);
	//	}
	//}
 //   
    

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
