#include "stdafx.h"

int const Dm5Downloader::BUFFER_SIZE=1024*4;
const CString SERVIER_NAME(_T("http://www.dm5.com/"));

CRegexpT<TCHAR> Dm5Downloader::titleReg(_T("-&gt;&gt;&nbsp;<font color=red>(.+)[<\\s]"));
//<span class="time">2010-6-11 10:35:00</span>		
CRegexpT<TCHAR> Dm5Downloader::lastDateReg(_T("<span class=\"time\">(\\d{4}-\\d{1,2}-\\d{1,2} \\d{1,2}:\\d{1,2}:\\d{1,2})</span>"));
CRegexpT<TCHAR> Dm5Downloader::lastVolReg(_T("<a href=\"showcomic\\d+\" target=\"\\w+\">(.+?)</a>"));
//<a href="showcomic74922" target="_blank">Bleach死神_CH407</a>
CRegexpT<TCHAR> Dm5Downloader::volReg(_T("<a href=\"showcomic(\\d+)\" [^>]+>(.+)</a>"));

static CString  isMainReg=_T("<a href=\"index.aspx\">");

vector<CString> split(CString& str,LPTSTR token){
	vector<CString> result;
	int curPos = 0;

	CString resToken=str.Tokenize(token,curPos);
	while (resToken != _T(""))
	{
		result.push_back(resToken);
		//_tprintf_s(_T("Resulting token: %s\n"), resToken);
		resToken = str.Tokenize(token, curPos);
	};  

	return result;
}
void enc(CString& str){
	// return s.toString().replace(/\%/g, "%26").replace(/=/g, "%3D").replace(/\+/g, "%2B")
	str.Replace(_T("%"),_T("%26"));
	str.Replace(_T("+"),_T("%2B"));
	str.Replace(_T("="),_T("%3D"));
}

Dm5Downloader::Dm5Downloader(CString url):url(url),session(_T("Dm5Downloader")),volCache(300)
{
}
Dm5Downloader::Dm5Downloader():session(_T("Dm5Downloader"))
{

}

Dm5Downloader::~Dm5Downloader(void)
{
	session.Close();
}

VOID Dm5Downloader::closeCHttpFile(CHttpFile* html){
	if(html){
		html->Close();
		delete html;
		html=NULL;

	}
}

BOOL Dm5Downloader::init()
{
	
	if(url.MakeLower().Left(32)!=_T("http://www.dm5.com/type.aspx?id=")){
		return FALSE;
	}
	
	CString ctx;
	CHttpFile* html=NULL;

	try{
		html=(CHttpFile*)session.OpenURL(url);

		CString line;
		BOOL isMain=FALSE;
		while(html->ReadString(line)){

			if(!isMain){
				if(isMatched(line,isMainReg)){
					isMain=TRUE;
				}
				continue;
			}

			if(title.IsEmpty()){
				CString t=getField(line,titleReg);
				t.Remove(_T('['));
				t.Remove(_T(']'));
				title=t;
			}

			if(lastDate.IsEmpty()){
				lastDate=getField(line,lastDateReg);
			}

			if(lastVol.IsEmpty()){
				lastVol=getField(line,lastVolReg);
			}

			MatchResult mrRet=volReg.Match( line );
			if(mrRet.IsMatched()){
				CString value=line.Mid(mrRet.GetGroupStart(2),mrRet.GetGroupEnd(2)-mrRet.GetGroupStart(2));
				pair<CString,CString> p(line.Mid(mrRet.GetGroupStart(1),mrRet.GetGroupEnd(1)-mrRet.GetGroupStart(1)),
					UTF8ToUnicode(value.GetBuffer()));
				value.ReleaseBuffer();
				volCache.insert(p);
				
			}



		}
	}catch(CInternetException e){
		TCHAR error[200]={0};
		UINT ctxId;
		e.GetErrorMessage(error,200,&ctxId);
		TRACE("CInternetException in init : %s ,%d\n",error,ctxId);
		e.Delete();
		closeCHttpFile(html);
		return FALSE;
	}
	
	
	closeCHttpFile(html);
	return TRUE;

	
	

}

LPCTSTR Dm5Downloader::getLastDate()
{
	return lastDate;
}

LPCTSTR Dm5Downloader::getLastVol()
{
	return lastVol;
}

LPCTSTR Dm5Downloader::getTitle()
{
	return title;
}
LPCTSTR Dm5Downloader::getUrl()
{
	return url;
}

map<CString,CString>* Dm5Downloader::getVolCache(){
	return &volCache;
}
BOOL Dm5Downloader::isMatched(CString& str,CString& regex){
	CRegexpT< TCHAR > regexp(regex);
	MatchResult mrRet = regexp.Match( str );

	if(mrRet.IsMatched()){
		return TRUE;
	}else{
		return FALSE;
	}
}

CString Dm5Downloader::getGroup1(CString& str,CString& regex){
	CRegexpT< TCHAR > regexp(regex);
	MatchResult mrRet = regexp.Match( str );
	return str.Mid(mrRet.GetGroupStart(1),mrRet.GetGroupEnd(1)-mrRet.GetGroupStart(1)+1);
	

}

 CString Dm5Downloader::getField(CString& line,CRegexpT<TCHAR>& cReg){
	CString nullStr;
	MatchResult mrRet=cReg.Match(line);
	if(mrRet.IsMatched()){
		return UTF8ToUnicode(line.Mid(mrRet.GetGroupStart(1),mrRet.GetGroupEnd(1)-mrRet.GetGroupStart(1)).GetBuffer());

	}else{
		return nullStr;
	}
}

CString  Dm5Downloader::UTF8ToUnicode(LPSTR szU8)
{
	//UTF8 to Unicode
	//由于中文直接复制过来会成乱码，编译器有时会报错，故采用16进制形式
	//char* szU8 = "abcd1234\xe4\xbd\xa0\xe6\x88\x91\xe4\xbb\x96\x00";
	//预转换，得到所需空间的大小
	WCHAR wszString[1024];
	ZeroMemory(wszString,1024*2);

	int wcsLen = ::MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), NULL, 0);
	//分配空间要给'\0'留个空间，MultiByteToWideChar不会给'\0'空间
	//wchar_t* wszString = new wchar_t[wcsLen + 1];
	//转换
	::MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), wszString, wcsLen);
	//最后加上'\0'
	wszString[wcsLen] = '\0';
	//unicode版的MessageBox API
	//::MessageBoxW(GetSafeHwnd(), wszString, wszString, MB_OK);

	CString s(wszString);
	return s;
}
BOOL Dm5Downloader::savePicAsFileReally(CString picUrl,CString path){
	CHttpFile* html;
	HANDLE hFile;
	try{
		CString fileName=picUrl.Mid(picUrl.ReverseFind(_T('/'))+1);

		html=(CHttpFile*)session.OpenURL(picUrl);

		byte bs[BUFFER_SIZE];
		CString fullPathName=path+_T("/")+fileName;
		TRACE("write file :%s\n",fullPathName);
		hFile=CreateFile(fullPathName,GENERIC_WRITE,NULL,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		
		if (hFile == INVALID_HANDLE_VALUE) 
		{ 
			closeCHttpFile(html);
			TRACE("Could not open file (error %d)\n", GetLastError());
			return FALSE; 
		}

		UINT n;
		while((n=html->Read(bs,BUFFER_SIZE))>0){
			DWORD written;
			if(!WriteFile(hFile,bs,n,&written,NULL)){
				TRACE("Could not write file (error %d)\n", GetLastError());
				closeCHttpFile(html);
				CloseHandle(hFile);
				return FALSE; 
			}


		}
		
		closeCHttpFile(html);
		CloseHandle(hFile);
		return TRUE;
	}catch(CInternetException* e){
		TCHAR error[200]={0};
		UINT ctxId;
		e->GetErrorMessage(error,200,&ctxId);
		TRACE("CInternetException in init : %s ,%d\n",error,ctxId);
		e->Delete();
		closeCHttpFile(html);
		
		if(hFile){
			CloseHandle(hFile);
		}
		return FALSE;
	}
}
BOOL Dm5Downloader::savePicAsFile(CString picUrl,CString path){
	int retry=3;

	while(!savePicAsFileReally(picUrl,path)&&retry>3){
		savePicAsFileReally(picUrl,path);
		retry--;
		Sleep(1000);
	}

	Sleep(200);
	return TRUE;

}
BOOL Dm5Downloader::saveVol( CString url){
	CString shortUrl;
	if(url.Left(28)==_T("http://www.dm5.com/showcomic")){
		shortUrl=url.Mid(28);
		
	}else{
		shortUrl=url;
	}
	
	CString volName=volCache.find(shortUrl)->second;

	saveVol(shortUrl,volName,_T(""));
	return TRUE;
}


BOOL Dm5Downloader::saveVol( CString url,CString volName,CString basePath )
{	
	TRACE("title :%s\n",title);

	if(basePath.IsEmpty()){
		
		TCHAR tchar[1024];
		ZeroMemory(tchar,1024);
		GetCurrentDirectory(1023,tchar);
		basePath=tchar;
		basePath+=_T("/down");
	}
	
	CreateDirectory(basePath,NULL);
	CString filePath=basePath+_T("/")+title;
	CreateDirectory(filePath,NULL);
    filePath=filePath+_T("/")+volName;
	CreateDirectory(filePath,NULL);

	vector<CString> urlList=getPicUrls(url);
	for(vector<CString>::iterator it=urlList.begin();it!=urlList.end();++it){
		savePicAsFile(*it,filePath);
	}
	

	return TRUE;


}
VOID Dm5Downloader::createNecessaryPath(CString volName){
	if(basePath.IsEmpty()){

		TCHAR tchar[1024]={0};
		GetCurrentDirectory(1023,tchar);
		basePath=tchar;
		basePath+=_T("/down");
	}

	CreateDirectory(basePath,NULL);
	CString filePath=basePath+_T("/")+title;
	CreateDirectory(filePath,NULL);
	filePath=filePath+_T("/")+volName;
	CreateDirectory(filePath,NULL);
}

CString Dm5Downloader::getSavePath( CString url,CString volName ){
	return basePath+_T("/")+title+_T("/")+volName;
}

vector<CString> Dm5Downloader::getPicUrls( CString& url )
{
	vector<CString> urlList;

	//http://www.dm5.com/showcomic74922
	CString strid(url);
	strid.Replace(_T("http://www.dm5.com/showcomic"),_T(""));
	
	int total=0;

	//第一张
	urlList.push_back(getSingleUrl(strid,total,0));
	if(total>1){
		for(int i=1;i<total;i++){
			urlList.push_back(getSingleUrl(strid,total,i));
		}
	}

	return urlList;
	

}

CString Dm5Downloader::getSingleUrl(CString& strid,int& total,int strpage){
	static CString url1=_T("http://www.dm5.com/ajax/DM5Func.FuncAjax,DM5Func.ashx?_method=ImageDes&_session=no");
	static CString url2=_T("http://www.dm5.com/ajax/ajax/DM5Func.FuncAjax,DM5Func.ashx?_method=UnImageDesTwo&_session=no");
	static CString url3=_T("http://www.dm5.com/ajax/ajax/DM5Func.FuncAjax,DM5Func.ashx?_method=UnImageDes&_session=no");
	
	CString resp,content,picUrl;;
	content.Format(_T("strid=%s&strpage=%d"),strid,strpage);
	TRACE(_T("%s\n"),content);
	BOOL is1stSucess=getSimpleResponse(url1,content,resp);
	TRACE(_T("%s\n"),UTF8ToUnicode(resp.GetBuffer()));

	resp.Remove(_T('['));
	resp.Remove(_T(']'));
	resp.Remove(_T('\''));

	vector<CString> arr1st=split(resp,_T(","));
	total=arr1st.size()-1;
	if(total>1){
		enc(arr1st[strpage]);
		enc(arr1st[strpage+1]);
		//date1=%s&date2=%s&key=%s
		//content.Format(_T("date1=%s&date2=%s&key=%s"),arr1st[0],arr1st[1],arr1st[total]); 
		content.Format(_T("date=%s&key=%s"),arr1st[strpage],arr1st[total]); 
		TRACE("content : %s\n",content);
		getSimpleResponse(url3,content,resp);
		
		resp.Remove(_T('['));
		resp.Remove(_T(']'));
		resp.Remove(_T('\''));
		
		picUrl=UTF8ToUnicode(resp.GetBuffer());
		resp.ReleaseBuffer();
		TRACE(_T("picUrl :%s\n"),picUrl);
		return picUrl;
	}
	
	
	return picUrl;

}


BOOL Dm5Downloader::getSimpleResponse( CString& url,CString& content,__out CString& response )
{
	Sleep(200);
	response.Empty();
	CString postUrl(url);
	postUrl.Replace(_T("http://www.dm5.com"),_T(""));

	CHttpConnection* pServer = session.GetHttpConnection("www.dm5.com");
	CHttpFile* pFile = pServer -> OpenRequest(CHttpConnection::HTTP_VERB_POST, postUrl, NULL, 1, NULL, NULL, INTERNET_FLAG_EXISTING_CONNECT);
	CString headers=_T("Host: www.dm5.com");
	
	if(pFile->SendRequest("Content-Type: application/x-www-form-urlencoded",(DWORD)headers.GetLength(), (LPVOID)(LPCTSTR)content, (DWORD)content.GetLength()) == 0)
	{
		MessageBox(NULL,"网络异常", "提示",MB_OK);
		
		pFile->Close();
		delete pFile;
		pFile=NULL;

		pServer->Close();
		delete pServer;
		pServer=NULL;

		return FALSE;
	}else{
		
		CString line;
		while(pFile->ReadString(line)){
			response+=line;
		}
	
		pFile->Close();
		delete pFile;
		pFile=NULL;

		pServer->Close();
		delete pServer;
		pServer=NULL;

		return TRUE;

	}

}


