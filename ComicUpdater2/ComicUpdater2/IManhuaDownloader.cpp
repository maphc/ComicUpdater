#include "stdafx.h"
#include "PyHelper.h"
#include "V8Helper.h"

//<h1>伊克西翁</h1>
CRegexpT<TCHAR> IManhuaDownloader::titleRegex(_T("<h1>(.*?)</h1>"));
//</span>原作者：Capcom | 字母索引：<a href="/comic/Y">Y</a> | 加入时间：2013/4/15 | 更新时间：2013/4/15 </p>
CRegexpT<TCHAR> IManhuaDownloader::lastDateRegex(_T("：(\\d+/\\d+/\\d+) </p>"));
//<li><a href="/comic/54/list_80538.html" title="火影忍者627话正式版" target="_blank" class="new">火影忍者627话正式版<em></em></a></li>
CRegexpT<TCHAR> IManhuaDownloader::lastVolRegex(_T("<li><a href=\".*?html\" title=\"(.*?)\" target=\"_blank\"( class=\"new\")?"));
//<li><a href="/comic/54/list_80538.html" title="火影忍者627话正式版" target="_blank" class="new">火影忍者627话正式版<em></em></a></li>
CRegexpT<TCHAR> IManhuaDownloader::volRegex(_T("<li[^>]*><a href=\"([^>]*?.html)\" title=\"(.*?)\" target=\"_blank\""));
//<h2 class="bar"><strong class="position">
CRegexpT<TCHAR> IManhuaDownloader::isMainRegex(_T("<h2 class=\"bar\"><strong class=\"position\">"));



IManhuaDownloader::IManhuaDownloader(void):Downloader()
{
    SetPageEncoding(USE_UNICODE+1);
}

IManhuaDownloader::~IManhuaDownloader(void)
{
}

vector<CString> IManhuaDownloader::GetPicUrls( CString& strid )
{
	//static CString server=_T("http://www.imanhua.com/");
	//http://www.imanhua.com/comic/54/
	//http://www.imanhua.com/comic/4411/list_80734.html

	vector<CString> a;

	CString url(strid);

	CString resp;
	CString ctx;

	GetSimpleGet(url,resp);
	
	//pages
	//CString pagesTag1(_T("pages = '["));
	//CString pagesTag2(_T("]';"));
	//UINT pagesStart=resp.Find(pagesTag1);
	//UINT pagesEnd=resp.Find(pagesTag2,pagesStart+1);
	
	//CString arr=resp.Mid(pagesStart+pagesTag1.GetLength(),pagesEnd-pagesStart-pagesTag1.GetLength());
	PyHelper pyHelper;
	//CString arr=pyHelper.getIManhuaVolPages(resp);
	CString arr=V8Helper::getIManhuaVolPages(resp);
	if(arr==_T("")){
		MessageBox(NULL,_T("没有找到图片列表,网络么问题的话就是改版了..."),_T("异常"),MB_OK);
		return vector<CString>();
	}
	
	vector<CString> pics=Split(arr,_T(","));

	return pics;
}

Downloader* IManhuaDownloader::CreateNewInst()
{
	return new IManhuaDownloader;
}

CRegexpT<TCHAR>& IManhuaDownloader::GetIsMainRegex()
{
return isMainRegex;
}

CRegexpT<TCHAR>& IManhuaDownloader::GetTitleRegex()
{
	return titleRegex;
}

CRegexpT<TCHAR>& IManhuaDownloader::GetLastVolRegex()
{
	return lastVolRegex;
}

CRegexpT<TCHAR>& IManhuaDownloader::GetLastDateRegex()
{
	return lastDateRegex;
}

CRegexpT<TCHAR>& IManhuaDownloader::GetVolRegex()
{
	return volRegex;
}


VOID IManhuaDownloader::GetVolUrlNamePair( CString& line,pair<CString,CString>& p )
{
	MatchResult mrRet=GetVolRegex().Match( line );
	if(mrRet.IsMatched()){
		p.first=UTF8ToUnicode(line.Mid(mrRet.GetGroupStart(2),mrRet.GetGroupEnd(2)-mrRet.GetGroupStart(2)));
		p.second=line.Mid(mrRet.GetGroupStart(1),mrRet.GetGroupEnd(1)-mrRet.GetGroupStart(1));
	}
}

VOID IManhuaDownloader::PostParse()
{
    for(map<CString,CString>::iterator iter=volCache.begin();iter!=volCache.end();iter++){
		CString cs;
		cs.Format(_T("http://%s%s"),this->server,iter->second);
		iter->second=cs;
	}
}
