#include "StdAfx.h"
#include "Dm5Downloader.h"

CRegexpT<TCHAR> Dm5Downloader::titleRegex(_T("-&gt;&gt;&nbsp;<font color=red>(.+)[<\\s]"));
//<span class="time">2010-6-11 10:35:00</span>		
CRegexpT<TCHAR> Dm5Downloader::lastDateRegex(_T("<span class=\"time\">(\\d{4}-\\d{1,2}-\\d{1,2} \\d{1,2}:\\d{1,2}:\\d{1,2})</span>"));
CRegexpT<TCHAR> Dm5Downloader::lastVolRegex(_T("<a href=\"showcomic\\d+\" target=\"\\w+\">(.+?)</a>"));
//<a href="showcomic74922" target="_blank">Bleach死神_CH407</a>
CRegexpT<TCHAR> Dm5Downloader::volRegex(_T("<a href=\"showcomic(\\d+)\" [^>]+>(.+)</a>"));
CRegexpT<TCHAR> Dm5Downloader::isMainRegex(_T("<a href=\"index.aspx\">"));

Dm5Downloader::Dm5Downloader(LPTSTR url):Downloader(url)
{
	
}

Dm5Downloader::Dm5Downloader():Downloader()
{

}
Dm5Downloader::~Dm5Downloader(void)
{
}

vector<CString> Dm5Downloader::GetPicUrls( CString& url )
{
	vector<CString> urlList;

	//http://www.dm5.com/showcomic74922
	CString strid(url);
	strid.Replace(_T("http://www.dm5.com/showcomic"),_T(""));

	int total=0;

	//第一张
	urlList.push_back(GetSingleUrl(strid,total,0));

	if(total>1){
		for(int i=1;i<total;i++){
			urlList.push_back(GetSingleUrl(strid,total,i));
		}
	}
	return urlList;
}
VOID Dm5Downloader::GetVolUrlNamePair(CString& line, pair<CString,CString>& p )
{
	MatchResult mrRet=GetVolRegex().Match( line );
	if(mrRet.IsMatched()){
		p.first=UTF8ToUnicode(line.Mid(mrRet.GetGroupStart(2),mrRet.GetGroupEnd(2)-mrRet.GetGroupStart(2)));
		p.second=line.Mid(mrRet.GetGroupStart(1),mrRet.GetGroupEnd(1)-mrRet.GetGroupStart(1));
	}
}


CRegexpT<TCHAR>& Dm5Downloader::GetIsMainRegex()
{
	return isMainRegex;
}

CRegexpT<TCHAR>& Dm5Downloader::GetTitleRegex()
{
	return titleRegex;
}

CRegexpT<TCHAR>& Dm5Downloader::GetLastVolRegex()
{
	return lastVolRegex;
}

CRegexpT<TCHAR>& Dm5Downloader::GetLastDateRegex()
{
	return lastDateRegex;
}

CRegexpT<TCHAR>& Dm5Downloader::GetVolRegex()
{
	return volRegex;
}

CString Dm5Downloader::GetSingleUrl( CString strid, int& total, int strpage )
{
	static CString url1=_T("http://www.dm5.com/ajax/DM5Func.FuncAjax,DM5Func.ashx?_method=ImageDes&_session=no");
	static CString url2=_T("http://www.dm5.com/ajax/ajax/DM5Func.FuncAjax,DM5Func.ashx?_method=UnImageDesTwo&_session=no");
	static CString url3=_T("http://www.dm5.com/ajax/ajax/DM5Func.FuncAjax,DM5Func.ashx?_method=UnImageDes&_session=no");

	CString resp,content,picUrl;;
	content.Format(_T("strid=%s&strpage=%d"),strid,strpage);
	TRACE(_T("%s\n"),content);
	
	BOOL is1stSucess=GetSimpleResponse(url1,content,resp);
	TRACE(_T("%s\n"),UTF8ToUnicode(resp.GetBuffer()));

	resp.Remove(_T('['));
	resp.Remove(_T(']'));
	resp.Remove(_T('\''));

	vector<CString> arr1st=Split(resp,_T(","));
	total=arr1st.size()-1;
	if(total>1){
		enc(arr1st[strpage]);
		enc(arr1st[strpage+1]);
		//date1=%s&date2=%s&key=%s
		//content.Format(_T("date1=%s&date2=%s&key=%s"),arr1st[0],arr1st[1],arr1st[total]); 
		content.Format(_T("date=%s&key=%s"),arr1st[strpage],arr1st[total]); 
		TRACE("content : %s\n",content);
		GetSimpleResponse(url3,content,resp);

		resp.Remove(_T('['));
		resp.Remove(_T(']'));
		resp.Remove(_T('\''));

		picUrl=UTF8ToUnicode(resp);
		
		TRACE(_T("picUrl :%s\n"),picUrl);
		return picUrl;
	}


	return picUrl;
}

void Dm5Downloader::enc( CString& str )
{
	// return s.toString().replace(/\%/g, "%26").replace(/=/g, "%3D").replace(/\+/g, "%2B")
	str.Replace(_T("%"),_T("%26"));
	str.Replace(_T("+"),_T("%2B"));
	str.Replace(_T("="),_T("%3D"));
}

Downloader* Dm5Downloader::CreateNewInst()
{
	return new Dm5Downloader;
}

