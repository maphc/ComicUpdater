#include "stdafx.h"
#include "PyHelper.h"
#include "V8Helper.h"
//<h1>死神</h1>
//<strong>火影忍者漫画</strong>
CRegexpT<TCHAR> FzdmDownloader::titleRegex(_T("<strong>(.+?)漫画</strong>"));
//<a href="../t/tianjiangzhiwu/6772.shtml" >第39话</a>&nbsp;<br /><span class="update2">2010-05-30</span>	
CRegexpT<TCHAR> FzdmDownloader::lastDateRegex(_T("<span class=\"update2\">(\\d{4}-\\d{1,2}-\\d{1,2})</span>"));
//<li><a href="555/" title="死神555话">死神555话</a></li>
CRegexpT<TCHAR> FzdmDownloader::lastVolRegex(_T("<li><a href=\".+?\" title=\"(.+?)\""));
//<li><a href="555/" title="死神555话">死神555话</a></li>
CRegexpT<TCHAR> FzdmDownloader::volRegex(_T("<li><a href=\"(.+?)\" title=\"(.+?)\""));
//<div id="content">
CRegexpT<TCHAR> FzdmDownloader::isMainRegex(_T("<div id=\"content\">"));




FzdmDownloader::FzdmDownloader(void):Downloader()
{
    SetPageEncoding(USE_UNICODE);
}

FzdmDownloader::~FzdmDownloader(void)
{
}

CString FzdmDownloader::GetReferer(CString volUrl,int index,CString picUrl){
	CString ref;
	ref.Format(_T("http://manhua.fzdm.com/7/%sindex_%d.html"),volUrl,index);
	return ref;
}
VOID FzdmDownloader::AfterGetPic(){
	Sleep(1000);
}

vector<CString> FzdmDownloader::GetPicUrls( CString& strid )
{
	static CString prefix(_T("http://manhua.fzdm.com/"));
	CString volUrl=this->url+strid;
	
	vector<CString> a;
	Sleep(1000);
	CString resp;
	GetSimpleGet(volUrl,resp);
	CString picHtmlUrl;
	int num=0;
	CString lastPicUrl;
	CString picUrl;
	do 
	{
		bool isDup=false;

		picHtmlUrl.Format("%sindex_%d.html",volUrl,num++);
		TRACE1(_T("searching url: %s\n"),picHtmlUrl);
		CString picHtml;
		GetSimpleGet(picHtmlUrl,picHtml);
		if(picHtml.Find(_T("404"))>-1){
			break;
		}
		vector<CString> lines=Split(picHtml,_T("\n"));
		CString picUrl;
		for (UINT i=0;i<lines.size();i++)
		{
			CString l=lines.at(i).Trim();
			//        document.writeln('<img src="http://183.60.142.171/manhua/2014/01/151657120.jpg" id="mhpic" alt="火影忍者661话" />');
			//picUrl=GetMatchedStr(_T("document.writeln\\('<img src=\"(.*?)\".*"),l);
			//<li><img src="http://s2.fzdm.org/2014/02/051659450.jpg" id="mhpic" alt="火影忍者663话" />
			picUrl=GetMatchedStr(_T("<li><img src=\"(.*?)\" id=\"mhpic\" .*"),l);
			

			if(picUrl==""){
				continue;
			}else if(picUrl==lastPicUrl){
				isDup=true;
				break;
			}else{
				a.push_back(picUrl);
				break;
			}
		}
		if(isDup){
			break;
		}
		
		if(picUrl==_T("")){
			AfxMessageBox(_T("未找到picUrl"));
			return vector<CString>();
		}
		lastPicUrl=picUrl;
		Sleep(1000);
	} while (num<=1000);
	

	
	return a;
}

Downloader* FzdmDownloader::CreateNewInst()
{
	return new FzdmDownloader;
}

CRegexpT<TCHAR>& FzdmDownloader::GetIsMainRegex()
{
	return isMainRegex;
}

CRegexpT<TCHAR>& FzdmDownloader::GetTitleRegex()
{
	return titleRegex;
}

CRegexpT<TCHAR>& FzdmDownloader::GetLastVolRegex()
{
	return lastVolRegex;
}

CRegexpT<TCHAR>& FzdmDownloader::GetLastDateRegex()
{
	return lastDateRegex;
}

CRegexpT<TCHAR>& FzdmDownloader::GetVolRegex()
{
	return volRegex;
}


VOID FzdmDownloader::GetVolUrlNamePair( CString& line,pair<CString,CString>& p )
{
	MatchResult mrRet=GetVolRegex().Match( line );
	if(mrRet.IsMatched()){
		p.first=UTF8ToUnicode(line.Mid(mrRet.GetGroupStart(2),mrRet.GetGroupEnd(2)-mrRet.GetGroupStart(2)));
		p.second=line.Mid(mrRet.GetGroupStart(1),mrRet.GetGroupEnd(1)-mrRet.GetGroupStart(1));
	}
}

VOID FzdmDownloader::PostParse()
{
	if (lastDate==_T(""))
    {
		lastDate=CTime::GetCurrentTime().Format(_T("%Y-%m-%d %H:%M:%S"));
    }
	
}
