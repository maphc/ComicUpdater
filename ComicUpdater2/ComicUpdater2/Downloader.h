#pragma once
#include "stdafx.h"
#include "PyHelper.h"

class Downloader
{


public:
	Downloader();
	Downloader(LPTSTR url);
	virtual ~Downloader(void);
protected:
	static const UINT RETRY;		//读取失败后重试次数
	static const DWORD SUCESS_DELAY;	//成功后延迟的时间
	static const DWORD FAIL_DELAY;		//失败后延迟的时间
	static const UINT BUFFER_SIZE;			//一次读取的大小
    static const UINT Downloader::USE_UNICODE;
    static const UINT Downloader::USE_UTF8;
    

	CInternetSession session;
    vector<CString> volList;
    map<CString,CString> volCache;

	CString server;
	CString url;
	CString lastDate;
	CString lastVol;
	CString title;
	UINT pageEncoding;

	BOOL isUserProxy;
	CString proxyUrl;
	CString proxyUser;
	CString proxyPass;

	//PyHelper& pyHelper;

public:
    static const BOOL USE_REAL_NAME;

public:
	virtual LPCTSTR GetLastDate();
	virtual LPCTSTR GetLastVol();
    virtual UINT GetPageEncoding();
    virtual VOID SetPageEncoding(UINT);
	virtual LPCTSTR GetTitle();
	LPCTSTR GetUrl();
	virtual void SetServer(CString server);
	BOOL SavePicAsFile(CString volUrl,CString picUrl,CString fileName);
	virtual vector<CString> GetPicUrls(CString& url)=0;
	vector<CString>* GetVolList();
	map<CString,CString>* GetVolCache();
	BOOL Parse();
	BOOL Parse(CString url);
	BOOL GetSimplePost( CString& url,CString& content,__out CString& response );
	BOOL GetSimpleGet(CString strid,CString& resp,CString Referer=_T(""));
	CString CreateNecessaryPath(LPCTSTR volName);
	virtual Downloader* CreateNewInst()=0;

    static CString GetMatchedStr(CRegexpT<TCHAR>& param1, CString& line );
    static CString GetMatchedStr( CString regStr, CString& line );

    static VOID GetMatchedList(CRegexpT<TCHAR>& regex, CString& t,vector<CString>& jsList);
    static VOID GetMatchedList(CString& regStr, CString& t,vector<CString>& jsList);

	int Downloader::httpgzdecompress(CFile* html,CString& res);

protected:
	BOOL SavePicAsFileReally(CString volUrl,CString picUrl,CString path);
	BOOL SavePicAsZipReally(CString volUrl,CString picUrl,CString path);
	CString  UTF8ToUnicode(LPCSTR szU8);
	UINT Ansi2Utf8(LPCSTR s,LPSTR szU8);
	CString UrlEncode(LPSTR szU8,UINT n);

	vector<CString> Split(CString& str,LPTSTR token);
	VOID CloseCHttpFile(CHttpFile* html);
	
	BOOL IsMatched(CString& str,CString& regex);
	virtual CRegexpT<TCHAR>& GetIsMainRegex()=0;
	virtual CRegexpT<TCHAR>& GetTitleRegex()=0;
	virtual CRegexpT<TCHAR>& GetLastVolRegex()=0;
	virtual CRegexpT<TCHAR>& GetLastDateRegex()=0;
	virtual CRegexpT<TCHAR>& GetVolRegex()=0;
	virtual VOID GetVolUrlNamePair(CString& line,pair<CString,CString>& p)=0;
	
	virtual VOID PostParse(){}
	virtual CString EncodeUrlIfNecessery(CString& picUrl){return picUrl;}
	virtual CString EncodeFileNameIfNecessery(CString& fileName){ return fileName;}
    virtual VOID Convert2Unicode();

	virtual VOID LogError(CString err);

	VOID SetProxy();
	VOID SetProxyPass(CHttpFile* f);

	
};
