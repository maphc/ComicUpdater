#include "stdafx.h"
#include "PyHelper.h"

//<H1><B>夫妻成长日记</B></H1>
CRegexpT<TCHAR> Manhua8Downloader::titleRegex(_T("<H1><B>(.+?)</B></H1>"));
//<DIV class=info><SPAN>完成状态[连载中]</SPAN>原作者：克·亚树 | 字母索引：F | 加入时间：2007-9-14 | 最近更新：2012-11-6 
CRegexpT<TCHAR> Manhua8Downloader::lastDateRegex(_T("<DIV class=info><SPAN>.+(\\d{4}-\\d{1,2}-\\d{1,2})\\s*$"));
//<LI class="alter"><A class="new" title="夫妻成长日记第54卷(日文)" HREF="/manhua/86/list_78470.htm" target="_blank">第54卷(日文)</A>
CRegexpT<TCHAR> Manhua8Downloader::lastVolRegex(_T("<A class=\"new\" title=\"(.+?)\" HREF="));
//<A title="夫妻成长日记第46卷" HREF="/manhua/86/list_28397.htm" target="_blank">第46卷</A><A title="联通" HREF="http://www.manhua2.com/manhua/86/list_28397.htm" target="_blank">联通</A></LI>
CRegexpT<TCHAR> Manhua8Downloader::volRegex(_T("<A .*? title=.*? HREF=\"(/.+?)\" [^>]*?>(.+?)</A>"));
//<H1 class=main_bar>
CRegexpT<TCHAR> Manhua8Downloader::isMainRegex(_T("<H1 class=main_bar>"));

UINT Manhua8Downloader::GetPageEncoding(){
	return USE_UNICODE+1;
}


CString decodeManhua8(CString& str){
	
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

Manhua8Downloader::Manhua8Downloader(void):Downloader()
{
    SetPageEncoding(USE_UNICODE);
}

Manhua8Downloader::~Manhua8Downloader(void)
{
}

vector<CString> Manhua8Downloader::GetPicUrls( CString& strid )
{
	static CString server=_T("http://www.manhua1.com");
	//http://www.manhua1.com/manhua/86/list_78470.htm
    vector<CString> a;

	CString volUrl(strid);
	if(strid.Left(1)==_T("/")){
		volUrl=server+strid;
	}
	

	CString resp;
	CString ctx;

	GetSimpleGet(volUrl,resp);
	
	//pages
	CString pagesTag1(_T("var picArray = new Array("));
	CString pagesTag2(_T(");var picNum ="));
	UINT pagesStart=resp.Find(pagesTag1);
	UINT pagesEnd=resp.Find(pagesTag2,pagesStart+1);
	
	CString arr=resp.Mid(pagesStart+pagesTag1.GetLength(),pagesEnd-pagesStart-pagesTag1.GetLength());
	
	if(arr==_T("")){
		MessageBox(NULL,_T("没有找到图片列表,网络么问题的话就是改版了..."),_T("异常"),MB_OK);
		return vector<CString>();
	}
	arr.Replace(_T("'"),"");
	vector<CString> pics=Split(arr,_T(","));

	

	return pics;
}

Downloader* Manhua8Downloader::CreateNewInst()
{
	return new Manhua8Downloader;
}

CRegexpT<TCHAR>& Manhua8Downloader::GetIsMainRegex()
{
return isMainRegex;
}

CRegexpT<TCHAR>& Manhua8Downloader::GetTitleRegex()
{
	return titleRegex;
}

CRegexpT<TCHAR>& Manhua8Downloader::GetLastVolRegex()
{
	return lastVolRegex;
}

CRegexpT<TCHAR>& Manhua8Downloader::GetLastDateRegex()
{
	return lastDateRegex;
}

CRegexpT<TCHAR>& Manhua8Downloader::GetVolRegex()
{
	return volRegex;
}


VOID Manhua8Downloader::GetVolUrlNamePair( CString& line,pair<CString,CString>& p )
{
	MatchResult mrRet=GetVolRegex().Match( line );
	if(mrRet.IsMatched()){
		p.first=UTF8ToUnicode(line.Mid(mrRet.GetGroupStart(2),mrRet.GetGroupEnd(2)-mrRet.GetGroupStart(2)));
		p.second=line.Mid(mrRet.GetGroupStart(1),mrRet.GetGroupEnd(1)-mrRet.GetGroupStart(1));
	}
}

VOID Manhua8Downloader::PostParse()
{

	CString server(_T("http://www.manhua1.com"));
	map<CString,CString>* volCache=GetVolCache();
	map<CString,CString> tmp(*volCache);
	volCache->clear();
	for(map<CString,CString>::iterator it=tmp.begin();it!=tmp.end();it++){
		if(it->second.Left(1)==_T("/")){
			volCache->insert(make_pair(it->first,server+it->second));
		}else{
			volCache->insert(*it);
		}
		
	}


}
