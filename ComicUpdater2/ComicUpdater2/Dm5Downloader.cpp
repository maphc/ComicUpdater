#include "StdAfx.h"
#include "Dm5Downloader.h"

//<div class="inbt"><h2>死神之拳</h2><h3>
CRegexpT<TCHAR> Dm5Downloader::titleRegex(_T("<div class=\"inbt\"><h2>(.+?)</h2><h3>"));
//<span class="lanzi">更新时间：2011-04-07 09:16:00</span>
CRegexpT<TCHAR> Dm5Downloader::lastDateRegex(_T("<span class=\"lanzi\">.+?(\\d{4}.\\d{1,2}.\\d{1,2} \\d{1,2}:\\d{1,2}:\\d{1,2})</span>"));
//<li><a class="tg" href="/m85106/" title="死神 第443话">
CRegexpT<TCHAR> Dm5Downloader::lastVolRegex(_T("<li><a class=\"tg\" href=\".*?\" title=\"(.*?)\">"));
//<li><a class="tg" href="/m816/" title="死神 第203话"> 第203话</a>（19页）</li>
CRegexpT<TCHAR> Dm5Downloader::volRegex(_T("<li><a class=\"tg\" href=\"(.*?)\" title=\"(.*?)\">"));
//<div class="innr91">
CRegexpT<TCHAR> Dm5Downloader::isMainRegex(_T("<div class=\"innr91\">"));



Dm5Downloader::Dm5Downloader(LPTSTR url):Downloader(url)
{
	SetPageEncoding(USE_UNICODE);
}

Dm5Downloader::Dm5Downloader()
{
    SetPageEncoding(USE_UNICODE);
}
Dm5Downloader::~Dm5Downloader(void)
{
}

vector<CString> Dm5Downloader::GetPicUrls( CString& url )
{
	vector<CString> urlList;

	//http://www.dm5.com/
    CString id(url);
	CString strid(url);
	strid.Insert(0,_T("http://www.dm5.com"));

	int total=0;
    id.Remove(_T('/'));
    id.Remove(_T('m'));
	
    CString reqUrl=strid+_T("showimage.ashx?d=Fri%20Apr%208%2021:50:14%20UTC+0800%202011");
    CString content;
    content.Format(_T("cid=%s&page=1"),id);
    
    CString resp;
    GetSimpleGet(CString(_T("http://www.dm5.com/m85099-p12/")),resp);
    resp.Empty();

    CString picUrl;
    
    
    GetSimpleGet(_T("http://www.dm5.com/userinfo.ashx?d=Fri%20Apr%208%2021:50:14%20UTC+0800%202011"),resp);
    resp.Empty();
    GetSimpleGet(_T("http://www.dm5.com/userinfo.ashx?d=Fri%20Apr%208%2021:50:14%20UTC+0800%202011"),resp);

    //GetSimplePost(reqUrl,content,picUrl);
    
    CString req(_T("http://www.dm5.com/m85099-p12/showimage.ashx?d=Fri%20Apr%208%2022:26:42%20UTC+0800%202011"));
    CString c(_T("cid=85099&page=12"));
    GetSimplePost(req,c,picUrl);
    picUrl=UTF8ToUnicode(picUrl);
    urlList.push_back(picUrl);

    //第一张
// 	urlList.push_back(GetSingleUrl(strid,total,0));
// 
// 	if(total>1){
// 		for(int i=1;i<total;i++){
// 			urlList.push_back(GetSingleUrl(strid,total,i));
// 		}
// 	}
	return urlList;
}
VOID Dm5Downloader::GetVolUrlNamePair(CString& line, pair<CString,CString>& p )
{
	MatchResult mrRet=GetVolRegex().Match( line );
	while(mrRet.IsMatched()){
		p.first=UTF8ToUnicode(line.Mid(mrRet.GetGroupStart(2),mrRet.GetGroupEnd(2)-mrRet.GetGroupStart(2)));
		p.second=line.Mid(mrRet.GetGroupStart(1),mrRet.GetGroupEnd(1)-mrRet.GetGroupStart(1));

        mrRet = GetVolRegex().Match(line, mrRet.GetEnd());

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
	
	BOOL is1stSucess=GetSimplePost(url1,content,resp);
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
		GetSimplePost(url3,content,resp);

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

