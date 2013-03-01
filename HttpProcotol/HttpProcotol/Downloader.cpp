#include "StdAfx.h"
#include "Downloader.h"

//静态初始化
const UINT Downloader::RETRY=3;		//读取失败后重试次数
const DWORD Downloader::SUCESS_DELAY=500;	//成功后延迟的时间
const DWORD Downloader::FAIL_DELAY=500;		//失败后延迟的时间
const UINT Downloader::BUFFER_SIZE=1024*4;			//一次读取的大小

Downloader::Downloader():session(_T("abc"))
{
	
}

Downloader::Downloader(LPTSTR url):url(url)
{
	this->url.MakeLower();
}

Downloader::~Downloader(void)
{
}

vector<CString> Downloader::Split( CString& str,LPTSTR token )
{
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

VOID Downloader::CloseCHttpFile( CHttpFile* html )
{
	if(html){
		html->Close();
		delete html;
		html=NULL;

	}
}

LPCTSTR Downloader::GetLastDate()
{
	return lastDate;
}

LPCTSTR Downloader::GetLastVol()
{
	return lastVol;
}

LPCTSTR Downloader::GetTitle()
{
	return title;
}
LPCTSTR Downloader::GetUrl()
{
	return url;
}

CString  Downloader::UTF8ToUnicode(LPCSTR szU8)
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

BOOL Downloader::SavePicAsFileReally(CString picUrl,CString path){
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
			CloseCHttpFile(html);
			TRACE("Could not open file (error %d)\n", GetLastError());
			return FALSE; 
		}

		UINT n;
		while((n=html->Read(bs,BUFFER_SIZE))>0){
			DWORD written;
			if(!WriteFile(hFile,bs,n,&written,NULL)){
				TRACE("Could not write file (error %d)\n", GetLastError());
				return FALSE; 
			}
		}

		CloseCHttpFile(html);
		CloseHandle(hFile);

		return TRUE;
	}catch(CInternetException* e){
		TCHAR error[200]={0};
		UINT ctxId;
		e->GetErrorMessage(error,200,&ctxId);
		TRACE("CInternetException in init : %s ,%d\n",error,ctxId);
		e->Delete();

		CloseCHttpFile(html);
		CloseHandle(hFile);
		
		return FALSE;
	}
}
BOOL Downloader::SavePicAsFile(CString picUrl,CString path){
	int retry=RETRY;

	while(!SavePicAsFileReally(picUrl,path)&&retry>0){
		
		retry--;
		Sleep(FAIL_DELAY);
	}

	Sleep(SUCESS_DELAY);
	return TRUE;

}

CString Downloader::CreateNecessaryPath(LPCTSTR volName){
	
	TCHAR tchar[1024]={0};
	GetCurrentDirectory(1023,tchar);
	CString basePath(tchar);
	basePath+=_T("/down");

	CreateDirectory(basePath,NULL);
	CString filePath=basePath+_T("/")+title;
	CreateDirectory(filePath,NULL);
	filePath+=_T("/");
	filePath+=volName;
	CreateDirectory(filePath,NULL);
	return filePath;
}

BOOL Downloader::GetSimpleResponse( CString& url,CString& content,__out CString& response )
{
	Sleep(200);
	response.Empty();
	CString postUrl(url);

	
	postUrl.Replace(_T("http://"),"");
	CString server= postUrl.Left(postUrl.Find(_T("/")));
	CString req=postUrl.Mid(postUrl.Find(_T("/")));


	CHttpConnection* pServer = session.GetHttpConnection(server);
	CHttpFile* pFile = pServer -> OpenRequest(CHttpConnection::HTTP_VERB_POST, req, NULL, 1, NULL, NULL, INTERNET_FLAG_EXISTING_CONNECT);
	CString headers ;//=_T("Host: ")+server;
	//CString headers=_T("Host: www.dm5.com");
	try{
		//"Content-Type: application/x-www-form-urlencoded"
		//if(pFile->SendRequest(headers,(DWORD)headers.GetLength(), (LPVOID)(LPCTSTR)content, (DWORD)content.GetLength()) == 0)
		if(pFile->SendRequest(headers, (LPVOID)(LPCTSTR)content, (DWORD)content.GetLength()) == 0)
		{
			CloseCHttpFile(pFile);

			pServer->Close();
			delete pServer;
			pServer=NULL;

			return FALSE;
		}else{

			CString line;
			while(pFile->ReadString(line)){
				response+=line;
			}

			CloseCHttpFile(pFile);

			pServer->Close();
			delete pServer;
			pServer=NULL;

			return TRUE;

		}
	}catch(CInternetException e){
		TCHAR error[200]={0};
		UINT ctxId;
		e.GetErrorMessage(error,200,&ctxId);
		TRACE("CInternetException in init : %s ,%d\n",error,ctxId);
		e.Delete();
		CloseCHttpFile(pFile);
		return FALSE;
	}

	
}

BOOL Downloader::Parse()
{
	return Parse(url);
}

BOOL Downloader::Parse(CString reqUrl)
{
	reqUrl.MakeLower();
	this->url=reqUrl;
	CString ctx;
	CHttpFile* html=NULL;

	try{
		html=(CHttpFile*)session.OpenURL(url);

		CString line;
		BOOL isMain=FALSE;
		while(html->ReadString(line)){

			if(!isMain){
				if(GetIsMainRegex().Match(line).IsMatched()){
					isMain=TRUE;
				}
				continue;
			}

			if(title.IsEmpty()){
				title=GetMatchedStr(GetTitleRegex(),line);
			}

			if(lastDate.IsEmpty()){
				lastDate=GetMatchedStr(GetLastDateRegex(),line);
			}

			if(lastVol.IsEmpty()){
				lastVol=GetMatchedStr(GetLastVolRegex(),line);
			}
			

			if(GetVolRegex().Match(line).IsMatched()){
				pair<CString,CString> p;
				GetVolUrlNamePair(line,p);
				volCache.insert(p);
				volList.push_back(p.first);
			}

			

		}
	}catch(CInternetException e){
		TCHAR error[200]={0};
		UINT ctxId;
		e.GetErrorMessage(error,200,&ctxId);
		TRACE("CInternetException in init : %s ,%d\n",error,ctxId);
		e.Delete();
		CloseCHttpFile(html);
		return FALSE;
	}


	CloseCHttpFile(html);
	return TRUE;

}

BOOL Downloader::IsMatched(CString& str,CString& regex){
	CRegexpT< TCHAR > regexp(regex);
	MatchResult mrRet = regexp.Match( str );

	if(mrRet.IsMatched()){
		return TRUE;
	}else{
		return FALSE;
	}
}

CString Downloader::GetMatchedStr( CRegexpT<TCHAR>& regex, CString& line )
{
	CString nul;
	MatchResult mrRet = regex.Match( line );
	if(mrRet.IsMatched()){
		//CString v=UTF8ToUnicode(line);

		UINT a=mrRet.GetGroupStart(1);
		UINT b=mrRet.GetGroupEnd(1)-mrRet.GetGroupStart(1);
		CString value=line.Mid(a,b);
		return UTF8ToUnicode(value);
		//return v.Mid(a,b);
	}else{
		return nul;
	}
	
}

vector<CString>* Downloader::GetVolList()
{
	return &volList;
}

map<CString,CString>* Downloader::GetVolCache()
{
	return &volCache;
}

BOOL Downloader::GetSimpleGet(CString strid,CString& resp){
	CHttpFile* html=NULL;
	resp.Empty();
	try{
		html=(CHttpFile*)session.OpenURL(strid);

		CString line;

		while(html->ReadString(line)){
			//CString u(UTF8ToUnicode(line));
			resp+=line;

		}
	}catch(CInternetException e){
		TCHAR error[200]={0};
		UINT ctxId;
		e.GetErrorMessage(error,200,&ctxId);
		TRACE("CInternetException in init : %s ,%d\n",error,ctxId);
		e.Delete();
		CloseCHttpFile(html);
		return FALSE;
	}


	CloseCHttpFile(html);
	return TRUE;
}

