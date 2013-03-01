#include "stdafx.h"


//<span class="anim_title_text"><a href="../s/sszqdzjy.shtml"><h1>史上最强弟子兼一</h1></a></span>
CRegexpT<TCHAR> Acg178Downloader::titleRegex(_T("<span class=\"anim_title_text\"><a href=.*><h1>(.+)</h1></a></span>"));
//<a href="../t/tianjiangzhiwu/6772.shtml" >第39话</a>&nbsp;<br /><span class="update2">2010-05-30</span>	
CRegexpT<TCHAR> Acg178Downloader::lastDateRegex(_T("<span class=\"update2\">(\\d{4}-\\d{1,2}-\\d{1,2})</span>"));
CRegexpT<TCHAR> Acg178Downloader::lastVolRegex(_T("<a href=.*>(.+?)</a>&nbsp;<br /><span class=\"update2\">"));
//<a title="史上最强弟子兼一-第一卷" href="../s/sszqdzjy/657.shtml" >第一卷</a>
CRegexpT<TCHAR> Acg178Downloader::volRegex(_T("<a title=.* href=\"../(.+?)\" [^>]*>(.+)</a>"));
//<div class="anim_intro_ptext">
CRegexpT<TCHAR> Acg178Downloader::isMainRegex(_T("<div class=\"anim_intro_ptext\">"));

CString decode(CString& str){
	
	int pos=0;
	CString r;
	str.Replace("\\/","/");
	str.Replace(" ","%20");

	while(pos<=str.GetLength()){
		UINT a=str.Find("\\u",pos);
		if(a==-1){
			r.Append(str.Mid(pos));
			break;
		}

		r.Append(str.Mid(pos,a-pos));
		CString ch(str.Mid(a+2,4));
		//WCHAR wch={0};

		long wch={0};

		sscanf_s(ch,"%x",&wch);

		//CString abc(wch);
		r+=CString((WCHAR)wch);
		
		pos=a+6;
		

	}

	return r;
}

Acg178Downloader::Acg178Downloader(void):Downloader()
{
}

Acg178Downloader::~Acg178Downloader(void)
{
}

vector<CString> Acg178Downloader::GetPicUrls( CString& strid )
{
	static CString server=_T("http://acg.178.com/mh/");
	static CString prefix(_T("http://acg.178.com/mh/imgs/"));
	
	vector<CString> a;

	CString url=server+strid;

	CString resp;
	CString ctx;

	GetSimpleGet(url,resp);
	
	CString tag1(_T("var pages = '["));
	CString tag2(_T("]';"));
	UINT start=resp.Find(tag1);
	UINT end=resp.Find(tag2,start+1);
	
	CString arr=resp.Mid(start+tag1.GetLength(),end-start-tag1.GetLength());
	arr.Replace(_T("\""),"");
	vector<CString> pics=Split(arr,_T(","));
	
	for(UINT i=0;i<pics.size();i++){
		CString pu(prefix+decode(pics.at(i)));
		a.push_back(pu);
		
	}

	return a;
}

Downloader* Acg178Downloader::CreateNewInst()
{
	return new Acg178Downloader;
}

CRegexpT<TCHAR>& Acg178Downloader::GetIsMainRegex()
{
return isMainRegex;
}

CRegexpT<TCHAR>& Acg178Downloader::GetTitleRegex()
{
	return titleRegex;
}

CRegexpT<TCHAR>& Acg178Downloader::GetLastVolRegex()
{
	return lastVolRegex;
}

CRegexpT<TCHAR>& Acg178Downloader::GetLastDateRegex()
{
	return lastDateRegex;
}

CRegexpT<TCHAR>& Acg178Downloader::GetVolRegex()
{
	return volRegex;
}

VOID Acg178Downloader::GetVolUrlNamePair( CString& line,pair<CString,CString>& p )
{
	MatchResult mrRet=GetVolRegex().Match( line );
	if(mrRet.IsMatched()){
		p.first=UTF8ToUnicode(line.Mid(mrRet.GetGroupStart(2),mrRet.GetGroupEnd(2)-mrRet.GetGroupStart(2)));
		p.second=line.Mid(mrRet.GetGroupStart(1),mrRet.GetGroupEnd(1)-mrRet.GetGroupStart(1));
	}
}
