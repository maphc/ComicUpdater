#include "StdAfx.h"
#include "Downloader.h"

//静态初始化
const UINT Downloader::RETRY=5;		//读取失败后重试次数
const DWORD Downloader::SUCESS_DELAY=500;	//成功后延迟的时间
const DWORD Downloader::FAIL_DELAY=500;		//失败后延迟的时间
const UINT Downloader::BUFFER_SIZE=1024*2;			//一次读取的大小
const UINT Downloader::USE_UNICODE=0;
const UINT Downloader::USE_UTF8=1;
const BOOL Downloader::USE_REAL_NAME=FALSE; //是否使用原始文件名

const BOOL USE_ZIP=TRUE;
const BOOL IS_DEBUG=FALSE;  //是否真实下载


int Downloader::httpgzdecompress(CFile* html, CString& res){

	char buffer[BUFFER_SIZE*2]={0};
	char desBuffer[BUFFER_SIZE*10+1]={0};
	int err = 0;
	z_stream d_stream = {0}; /* decompression stream */
	uLong read=0;

	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;
	d_stream.next_in  = (Bytef*)buffer;
	d_stream.avail_in = 0;
	d_stream.next_out = (Bytef*)desBuffer;
	if(inflateInit2(&d_stream, 47) != Z_OK) 
		return -1;

	char* p=buffer;
	uLong remain=0;
	while((read=html->Read(buffer+remain,BUFFER_SIZE))&&read>0){
		ZeroMemory(desBuffer,BUFFER_SIZE*10);
		d_stream.next_in  = (Bytef*)buffer;
		d_stream.avail_in = read+remain;

		d_stream.next_out = (Bytef*)desBuffer;
		d_stream.avail_out=BUFFER_SIZE*10;

		err=inflate(&d_stream,Z_NO_FLUSH);
		if(err==Z_OK){
			res+=desBuffer;
		}else if(err==Z_STREAM_END){
			res+=desBuffer;
			break;
		}else{
			CString t;
			t.Format(_T("httpgzdecompress err :%d\n"),err);
			TRACE0(t);
		}
		remain=d_stream.avail_in;

		//char tmp[BUFFER_SIZE]={0};
		//memcpy(tmp,buffer+BUFFER_SIZE-remain-1,remain);
		//ZeroMemory(buffer,BUFFER_SIZE*2);
		//memcpy(buffer,tmp,remain);
		memcpy(buffer,d_stream.next_in,remain);
		ZeroMemory(buffer+remain,BUFFER_SIZE*2-remain);
		p+=d_stream.avail_in;

	}
	inflateEnd(&d_stream);
	return 0;

}

CString addTail(CString url){
	CString u;
	srand((unsigned)time(0));
	int ran_num=rand() ;
	if(url.Find(_T('?'))>0){
		//u.Format(_T("%s&temptail=%d"),url,ran_num);
	}else{
		//u.Format(_T("%s?temptail=%d"),url,ran_num);
	}

	return url;
}

Downloader::Downloader()
{
	//
}

Downloader::Downloader(LPTSTR url):url(url)
{
	this->url.MakeLower();
	//session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT,1000);
	//session.SetOption(INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, 1000);
	//session.SetOption(INTERNET_OPTION_DATA_SEND_TIMEOUT, 1000);
	//session.SetOption(INTERNET_OPTION_CONNECT_BACKOFF,1000);
	//session.SetOption(INTERNET_OPTION_CONNECT_RETRIES,2);
	
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
	//const ULONG SIZE=1024*1024;
	ULONG SIZE=strlen(szU8)*10;
	//UTF8 to Unicode
	//由于中文直接复制过来会成乱码，编译器有时会报错，故采用16进制形式
	//char* szU8 = "abcd1234\xe4\xbd\xa0\xe6\x88\x91\xe4\xbb\x96\x00";
	//预转换，得到所需空间的大小
	//WCHAR wszString[SIZE];
	WCHAR* wszString=new WCHAR[SIZE];
	ZeroMemory(wszString,SIZE*2);

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
	delete []wszString;
	return s;
}

UINT Downloader::Ansi2Utf8(LPCSTR s,LPSTR szU8){
	int wcsLen = ::MultiByteToWideChar(CP_ACP, NULL, s, strlen(s), NULL, 0);
	wchar_t* wszString = new wchar_t[wcsLen + 1];
	::MultiByteToWideChar(CP_ACP, NULL, s, strlen(s), wszString, wcsLen);
	wszString[wcsLen] = '\0';


	UINT u8Len = ::WideCharToMultiByte(CP_UTF8, NULL, wszString, wcsLen, NULL, 0, NULL, NULL);

	::WideCharToMultiByte(CP_UTF8, NULL, wszString, wcslen(wszString), szU8, u8Len, NULL, NULL);
	//最后加上'\0'
	szU8[u8Len] = '\0';
	delete []wszString;

	return u8Len;

}

CString Downloader::UrlEncode(LPSTR szU8,UINT n){
	CString r;

	for(UINT i=0;i<n;i++){
		if(szU8[i]&0x80){
			CHAR buf[4]={0};
			r.AppendFormat("%%%X",szU8[i]&0xff);
		}else{
			r+=szU8[i];
		}
	}

	return r;

}

BOOL Downloader::SavePicAsFileReally(CString volUrl,CString picUrl,CString path){
	CHttpFile* html=NULL;
	HANDLE hFile=NULL;
	try{
		CString fileName=picUrl.Mid(picUrl.ReverseFind(_T('/'))+1);

		byte bs[BUFFER_SIZE];

		CString fullPathName;

		if(USE_REAL_NAME){
			fullPathName=EncodeFileNameIfNecessery(path+_T("/")+fileName);
		}else{
			fullPathName=EncodeFileNameIfNecessery(path);
		}

		if(IS_DEBUG){
			TRACE("Down %s To %s\n",picUrl,fullPathName);
		}else{
			CString strReferer(volUrl);
			strReferer.Insert(0, _T("Referer:"));
			//strReferer.Append(_T("\nHost: 222.218.156.24"));

			html=(CHttpFile*)session.OpenURL(addTail(EncodeUrlIfNecessery(picUrl)),1,INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_RELOAD|INTERNET_FLAG_DONT_CACHE, strReferer, strReferer.GetLength());
			//hFile=CreateFile(fullPathName,GENERIC_WRITE,NULL,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
			hFile=CreateFile(fullPathName,GENERIC_WRITE,FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
			TRACE("pic url : %s\n",picUrl);
			TRACE("write file :%s\n",fullPathName);

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
		}
		CloseCHttpFile(html);
		CloseHandle(hFile);

		return TRUE;
	}catch(CInternetException* e){
		TCHAR error[200]={0};
		UINT ctxId;
		e->GetErrorMessage(error,200,&ctxId);
		TRACE("CInternetException in SavePicAsFileReally : %s ,%d\n",error,ctxId);
		e->Delete();

		CloseCHttpFile(html);
		CloseHandle(hFile);

		return FALSE;
	}
}

BOOL Downloader::SavePicAsZipReally(CString volUrl,CString picUrl,CString path){
	CHttpFile* html=NULL;
	zipFile newZipFile=NULL;

	try{
		CString fileName;
		UINT n1=path.ReverseFind(_T('/'));
		UINT n2=path.Left(n1).ReverseFind(_T('/'));
		CString filaNamePath=path.Left(n2+1);
		CString pureZipName=path.Mid(n2+1,n1-n2-1);
		if(pureZipName.GetAt(0)>=_T('0')&&pureZipName.GetAt(0)<=_T('9')){
			//pureZipName=pureZipName.Mid(2);
			pureZipName=_T("第")+pureZipName;
		}


		pureZipName+=_T(".zip");
		CString zipFileName=filaNamePath+pureZipName;


		byte bs[BUFFER_SIZE];

		CString fullPathName;

		if(USE_REAL_NAME){
			fileName=picUrl.Mid(picUrl.ReverseFind(_T('/'))+1);

		}else{
			fileName=path.Mid(path.Left(path.ReverseFind(_T('/'))-1).ReverseFind(_T('/'))+1);
		}
		fileName.Replace(_T("/"),_T("_"));

		if(IS_DEBUG){
			TRACE("Down %s To %s in %s\n",picUrl,fileName,zipFileName);
		}else{
			//http://comic.xxbh.net/colist_130631.html
			//http://comic.xxbh.net/201002/153506.html
			//CString strReferer(picUrl);
			CString strReferer(volUrl);
			strReferer.Insert(0, _T("Referer: "));
			strReferer.Append(_T("\nUser-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.17 (KHTML, like Gecko) Chrome/24.0.1312.56 Safari/537.17"));

			html=(CHttpFile*)session.OpenURL(addTail(EncodeUrlIfNecessery(picUrl)),1,INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_RELOAD|INTERNET_FLAG_DONT_CACHE, strReferer, strReferer.GetLength());

			DWORD dwStatusCode=0;
			html->QueryInfoStatusCode(dwStatusCode);
			if(dwStatusCode>399){
				CString errTxt;
				errTxt.Format(_T("SavePicAsZipReally %s abnormal status code :%d"),picUrl,dwStatusCode);
				LogError(errTxt);
				CloseCHttpFile(html);
				return FALSE;
				
			}

			//hFile=CreateFile(fullPathName,GENERIC_WRITE,NULL,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

			if(_access(zipFileName,0)==-1){
				newZipFile = zipOpen((zipFileName), APPEND_STATUS_CREATE);
			}else{
				newZipFile = zipOpen((zipFileName), APPEND_STATUS_ADDINZIP);
			}

			TRACE("pic url : %s\n",picUrl);
			TRACE("write file :%s in %s\n",fileName,zipFileName);

			//初始化写入zip的文件信息  
			zip_fileinfo zi;  
			zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =  
				zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;  
			zi.dosDate = 0;  
			zi.internal_fa = 0;  
			zi.external_fa = 0; 


			UINT n;
			struct afragment{
				byte context[BUFFER_SIZE];
				UINT n;
			};
			vector<afragment> bsList;
			while((n=html->Read(bs,BUFFER_SIZE))>0){
				struct afragment temp={0};
				memcpy(temp.context,bs,n);
				temp.n=n;
				bsList.push_back(temp);
			}
			zipOpenNewFileInZip(newZipFile, fileName, &zi, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION);
			for(vector<afragment>::iterator iter=bsList.begin();iter!=bsList.end();iter++){
				//zipWriteInFileInZip(newZipFile, bs, n);
				zipWriteInFileInZip(newZipFile, iter->context, iter->n);
			}

			zipCloseFileInZip(newZipFile); 

		}
		CloseCHttpFile(html);
		zipClose(newZipFile, NULL);

		return TRUE;
	}catch(CInternetException* e){
		TCHAR error[200]={0};
		UINT ctxId;
		e->GetErrorMessage(error,200,&ctxId);
		CString errTxt;
		errTxt.Format("CInternetException in SavePicAsZipReally : %s ,%d\n",error,ctxId);
		TRACE(errTxt);
		LogError(errTxt);

		e->Delete();

		CloseCHttpFile(html);

		zipClose(newZipFile, NULL);

		return FALSE;
	}
}

BOOL Downloader::SavePicAsFile(CString volUrl,CString picUrl,CString path){
	int retry=RETRY;

	while(!(USE_ZIP?SavePicAsZipReally(volUrl,picUrl,path):SavePicAsFileReally(volUrl,picUrl,path))&&retry>0){
		
		retry--;

		CString errTxt;
		errTxt.Format("SavePicAsFile %s fail, remain %d ,retry...",picUrl,retry);
		//TRACE(errTxt);
		LogError(errTxt);

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
	DWORD dd=GetLastError();
	filePath+=_T("/");
	filePath+=volName;
	if(USE_ZIP){

	}else{
		CreateDirectory(filePath,NULL);
	}

	return filePath;
}

BOOL Downloader::GetSimplePost( CString& url,CString& content,__out CString& response )
{
	Sleep(50);
	response.Empty();
	CString postUrl(url);


	postUrl.Replace(_T("http://"),"");
	CString server= postUrl.Left(postUrl.Find(_T("/")));
	CString req=postUrl.Mid(postUrl.Find(_T("/")));


	CHttpConnection* pServer = session.GetHttpConnection(server);
	CHttpFile* pFile = pServer -> OpenRequest(CHttpConnection::HTTP_VERB_POST, req, NULL, 1, NULL, NULL, INTERNET_FLAG_EXISTING_CONNECT);
	CString headers=_T("Host: ")+server;

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
	}catch(CInternetException* e){
		TCHAR error[200]={0};
		UINT ctxId;
		e->GetErrorMessage(error,200,&ctxId);

		CString errTxt;
		errTxt.Format("CInternetException in GetSimplePost : %s ,%d\n",error,ctxId);
		TRACE(errTxt);
		LogError(errTxt);

		e->Delete();
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
	CString ctx,resp;
	
	SetProxy();
	try{

		if(!GetSimpleGet(reqUrl,resp)){
			return FALSE;
		}
		if(GetPageEncoding()==USE_UNICODE){
			resp=UTF8ToUnicode(resp);
		}

		vector<CString> lines=Split(resp,_T("\n"));
		BOOL isMain=FALSE;
		for(vector<CString>::iterator it=lines.begin();it!=lines.end();it++){
			CString line(*it);
			//if(GetPageEncoding()==USE_UNICODE){
			//	line=UTF8ToUnicode(*it);
			//}else{
			//	line=*it;
			//}

			if(!isMain){
				MatchResult mainMatch=GetIsMainRegex().Match(line);
				if(mainMatch.IsMatched()){
					UINT b=mainMatch.GetGroupEnd(0);
					line=line.Mid(b);
					isMain=TRUE;
				}else{
					continue;
				}

			}

			if(title.IsEmpty()){
				title=GetMatchedStr(GetTitleRegex(),line);
			}

			if(lastDate.IsEmpty()){
				lastDate=GetMatchedStr(GetLastDateRegex(),line);
			}

			if(lastVol.IsEmpty()){
				lastVol=GetMatchedStr(GetLastVolRegex(),line);
				if(lastVol&&!lastVol.IsEmpty()){
					lastVol.Remove(_T(':'));
					lastVol.Remove(_T('\\'));
					lastVol.Remove(_T('/'));
				}
			}

			//if(GetVolRegex().Match(line).IsMatched()){
			//pair<CString,CString> p;
			//GetVolUrlNamePair(line,p);
			//volCache.insert(p);
			//volList.push_back(p.first);
			//}

			//匹配统一进行.如果有其他情况实现不了,再由子类实现
			MatchResult mrRet=GetVolRegex().Match( line );
			while(mrRet.IsMatched()){
				pair<CString,CString> p;
				CString t(line.Mid(mrRet.GetGroupStart(2),mrRet.GetGroupEnd(2)-mrRet.GetGroupStart(2)));
				p.first=t;//UTF8ToUnicode(t);
				p.second=line.Mid(mrRet.GetGroupStart(1),mrRet.GetGroupEnd(1)-mrRet.GetGroupStart(1));
				volCache.insert(p);
				volList.push_back(p.first);
				mrRet = GetVolRegex().Match(line, mrRet.GetEnd());

			}
		}

		

		PostParse();
	}catch(CInternetException* e){
		TCHAR error[200]={0};
		UINT ctxId;
		e->GetErrorMessage(error,200,&ctxId);
		TRACE("CInternetException in Parse : %s ,%d\n",error,ctxId);
		e->Delete();

		return FALSE;
	}



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
		value.Trim();

		return value;
		//return UTF8ToUnicode(value);
		//return v.Mid(mrRet.GetGroupStart(1),mrRet.GetGroupEnd(1)-mrRet.GetGroupStart(1));
	}else{
		return nul;
	}

}
CString Downloader::GetMatchedStr( CString regStr, CString& line )
{
	CRegexpT<TCHAR> regex(regStr);
	return GetMatchedStr(regex, line );
}

VOID Downloader::GetMatchedList(CString& regStr, CString& t,vector<CString>& jsList){
	CRegexpT<TCHAR> regex(regStr);

	GetMatchedList(regex,t,jsList);
}
VOID Downloader::GetMatchedList(CRegexpT<TCHAR>& regex, CString& t,vector<CString>& jsList){

	MatchResult mrRet = regex.Match( t );
	while(mrRet.IsMatched()){
		UINT a=mrRet.GetGroupStart(1);
		UINT b=mrRet.GetGroupEnd(1)-mrRet.GetGroupStart(1);
		CString value=t.Mid(a,b);
		if(value[0]==_T('/')){
			value=_T("http://comic.xxbh.net/")+value;
		}
		jsList.push_back(value);
		mrRet = regex.Match(t, mrRet.GetEnd());

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

BOOL Downloader::GetSimpleGet(CString strid,CString& resp,CString customRef){
	CHttpFile* html=NULL;
	resp.Empty();
	try{

		CString Referer("Referer: ");
		if(customRef&&!customRef.IsEmpty()){
			Referer+=customRef;
		}else{
			Referer+=strid;
		}

		CString hostStr(strid);
		hostStr.Replace(_T("http://"),"");
		CString hostIP= hostStr.Left(hostStr.Find(_T("/")));
		CString hostHeader=_T("Host: ")+hostIP;
		Referer=Referer+"\n"+hostHeader;
		
		//INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_RELOAD|INTERNET_FLAG_DONT_CACHE
		html=(CHttpFile*)session.OpenURL(addTail(strid),1,INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_RELOAD|INTERNET_FLAG_DONT_CACHE, Referer, Referer.GetLength());
		
		//CString postUrl(strid);
		//postUrl.Replace(_T("http://"),"");
		//CString server= postUrl.Left(postUrl.Find(_T("/")));
		//CString req=postUrl.Mid(postUrl.Find(_T("/")));
		//CHttpConnection* pServer = session.GetHttpConnection(server);
		//CHttpFile* pFile = pServer -> OpenRequest(CHttpConnection::HTTP_VERB_GET, req, NULL, 1, NULL, NULL, INTERNET_FLAG_EXISTING_CONNECT);
		SetProxyPass(html);
		//BOOL isSucess=html->AddRequestHeaders(Referer,HTTP_ADDREQ_FLAG_REPLACE,Referer.GetLength());
		DWORD err=GetLastError();

		CString rspHeaders;

		html-> SendRequest(NULL); 
		html->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF,rspHeaders);

		if(rspHeaders.Find(_T("gzip"))>-1){
			httpgzdecompress(html,resp);

		}else{

			CString line;
			while(html->ReadString(line)){
				resp+=line;
				resp+=_T('\n');
			}
		}

	}catch(CInternetException* e){
		TCHAR error[200]={0};
		UINT ctxId;
		e->GetErrorMessage(error,200,&ctxId);
		CString errTxt;
		errTxt.Format("CInternetException in GetSimpleGet : %s ,%d\n",error,ctxId);
		TRACE(errTxt);
		LogError(errTxt);

		e->Delete();
		CloseCHttpFile(html);
		return FALSE;
	}


	CloseCHttpFile(html);
	return TRUE;
}

UINT Downloader::GetPageEncoding(){
	return pageEncoding;
}
VOID Downloader::SetPageEncoding(UINT n){
	this->pageEncoding=n;
}

VOID Downloader::Convert2Unicode(){
	title=UTF8ToUnicode(title);
	lastVol=UTF8ToUnicode(lastVol);

	map<CString,CString> rightVolCache;
	for(map<CString,CString>::iterator it=volCache.begin();it!=volCache.end();it++){
		CString k(UTF8ToUnicode(it->first));
		CString v(it->second);
		rightVolCache.insert(pair<CString,CString>(k,v));
	}
	volCache.clear();
	volCache.insert(rightVolCache.begin(),rightVolCache.end());

	for(vector<CString>::iterator it=volList.begin();it!=volList.end();it++){
		*it=UTF8ToUnicode(*it);
	}
}

VOID Downloader::LogError( CString err )
{
	CString logName("log.txt");
	CString txt;
	CTime starttime=CTime::GetCurrentTime();
	CString time=starttime.Format(_T("%Y-%m-%d %H:%M:%S"));
	TCHAR tchar[1024]={0};
	GetCurrentDirectory(1023,tchar);
	logName.Format(_T("%s/down/%s/log.txt"),tchar,GetTitle());

	CString logtext=time+_T("  ")+GetUrl()+_T(" :")+err;

	ofstream fout(logName,ios_base::out|ios_base::app);
	fout<<logtext<<endl;
	fout.close();

}

VOID Downloader::SetProxy()
{

	isUserProxy=FALSE;

	CString proxyini;
	GetCurrentDirectory(500,proxyini.GetBuffer(500));
	proxyini.ReleaseBuffer();
	proxyini+=_T("\\proxy.ini");
	if(_access(proxyini,0)!=-1){
		CString useProxy;
		GetPrivateProfileString(_T("proxy"),_T("use_proxy"),_T(""),useProxy.GetBuffer(20),20,proxyini);
		useProxy.ReleaseBuffer();
		//GetPrivateProfileString(NULL,NULL,NULL,rtn,500,proxyini);
		if(useProxy.MakeLower()==_T("yes")){
			isUserProxy=TRUE;
			GetPrivateProfileString(_T("proxy"),_T("proxy_url"),_T(""),proxyUrl.GetBuffer(500),500,proxyini);
			proxyUrl.ReleaseBuffer();
			if(proxyUrl==_T("")){
				isUserProxy=FALSE;
			}else{
				
				GetPrivateProfileString(_T("proxy"),_T("user"),_T(""),proxyUser.GetBuffer(50),50,proxyini);
				proxyUser.ReleaseBuffer();
				if(proxyUser!=_T("")){
					GetPrivateProfileString(_T("proxy"),_T("pass"),_T(""),proxyPass.GetBuffer(50),50,proxyini);
					
				}
				TRACE1(_T("proxy.ini [%s]"),proxyUrl);
				TRACE2(_T("proxy.ini [%s][%s]"),proxyUser,proxyPass);
			}

		}else{
			isUserProxy=FALSE;
		}
	}

	if(isUserProxy){
		INTERNET_PROXY_INFO proxyinfo;

		proxyinfo.dwAccessType = INTERNET_OPEN_TYPE_PROXY;
		proxyinfo.lpszProxy = proxyUrl;
		proxyinfo.lpszProxyBypass = NULL;

		BOOL b = session.SetOption(INTERNET_OPTION_PROXY, (LPVOID)&proxyinfo, sizeof(INTERNET_PROXY_INFO), 0);
		UINT n=GetLastError();
		TRACE1(_T("Set Proxy :%s\n"),proxyUrl);
	}
}

VOID Downloader::SetProxyPass( CHttpFile* f )
{
	if(isUserProxy){
		TRACE2(_T("Proxy [%s][%s]\n"),proxyUser,proxyPass);
		BOOL b=f->SetOption(INTERNET_OPTION_PROXY_USERNAME, (LPVOID)(LPCTSTR)proxyUser, proxyUser.GetLength()+ 1, 0);
		UINT n=GetLastError();
		b=f->SetOption(INTERNET_OPTION_PROXY_PASSWORD, (LPVOID)(LPCTSTR)proxyPass,  proxyPass.GetLength() + 1, 0);
		n=GetLastError();

	}
}

void Downloader::SetServer( CString server )
{
	this->server=server;
}
