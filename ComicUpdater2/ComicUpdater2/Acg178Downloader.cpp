#include "stdafx.h"
#include "PyHelper.h"

//<span class="anim_title_text"><a href="../s/sszqdzjy.shtml"><h1>史上最强弟子兼一</h1></a></span>
CRegexpT<TCHAR> Acg178Downloader::titleRegex(_T("<span class=\"anim_title_text\"><a href=.*><h1>(.+)</h1></a></span>"));
//<a href="../t/tianjiangzhiwu/6772.shtml" >第39话</a>&nbsp;<br /><span class="update2">2010-05-30</span>	
CRegexpT<TCHAR> Acg178Downloader::lastDateRegex(_T("<span class=\"update2\">(\\d{4}-\\d{1,2}-\\d{1,2})</span>"));
CRegexpT<TCHAR> Acg178Downloader::lastVolRegex(_T("<a href=.*?>(.+?)</a>&nbsp;<br /><span class=\"update2\">"));
//<a title="史上最强弟子兼一-第一卷" href="../s/sszqdzjy/657.shtml" >第一卷</a>
CRegexpT<TCHAR> Acg178Downloader::volRegex(_T("<a title=.*? href=\"(.+?)\" [^>]*?>(.+)</a>"));
//<div class="middleright_mr">
CRegexpT<TCHAR> Acg178Downloader::isMainRegex(_T("<div class=\"middleright_mr\">"));



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
    SetPageEncoding(USE_UNICODE);
}

Acg178Downloader::~Acg178Downloader(void)
{
}

vector<CString> Acg178Downloader::GetPicUrls( CString& strid )
{
	static CString server=_T("http://manhua.178.com");
	static CString prefix(_T("http://acg.178.com/mh/imgs/"));
	//manhua.178.com
    //http://acg.178.com/mh/y/yuanzhikong.shtml
    //http://manhua.178.com/m/mitumaooverrun.shtml
    

	vector<CString> a;

	CString url=server+strid;
	//20110603因格式改变修改
	//CString url=strid;

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
	CString arr=pyHelper.get178VolPages(resp);
	if(arr==_T("")){
		MessageBox(NULL,_T("没有找到图片列表,网络么问题的话就是改版了..."),_T("异常"),MB_OK);
		return vector<CString>();
	}
	arr.Replace(_T("\""),"");
	vector<CString> pics=Split(arr,_T(","));

	//is_hot_comic
	CString hotTag1(_T("is_hot_comic ="));
	CString hotTag2(_T(";"));
	UINT hotStart=resp.Find(hotTag1);
	UINT hotEnd=resp.Find(hotTag2,hotStart+1);
	CString isHot=resp.Mid(hotStart+hotTag1.GetLength(),hotEnd-hotStart-hotTag1.GetLength()).Trim();
	
	//is_fast_comic
	CString fastTag1(_T("is_fast_comic ="));
	CString fastTag2(_T(";"));
	UINT fastStart=resp.Find(fastTag1);
	UINT fastEnd=resp.Find(fastTag2,fastStart+1);
	CString isFast=resp.Mid(fastStart+fastTag1.GetLength(),fastEnd-fastStart-fastTag1.GetLength()).Trim();

	if(isFast=="true"){
		prefix=_T("http://imgfastd.manhua.178.com/");
	}
	if(isHot=="true"){
		//prefix=_T("http://hot.manhua.178.com/");
	}
	
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

VOID Acg178Downloader::PostParse()
{
    
	vector<CString> tmp(volList);
	volList.clear();
	for(vector<CString>::reverse_iterator it=tmp.rbegin();it!=tmp.rend();it++){
		volList.push_back(*it);
	}

}
