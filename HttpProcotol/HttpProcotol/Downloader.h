#pragma once
#include "stdafx.h"

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

	CInternetSession session;
	CString url;
	CString lastDate;
	CString lastVol;
	CString title;
	vector<CString> volList;
	map<CString,CString> volCache;

public:
	LPCTSTR GetLastDate();
	LPCTSTR GetLastVol();
	LPCTSTR GetTitle();
	LPCTSTR GetUrl();
	BOOL SavePicAsFile(CString picUrl,CString fileName);
	virtual vector<CString> GetPicUrls(CString& url)=0;
	vector<CString>* GetVolList();
	map<CString,CString>* GetVolCache();
	BOOL Parse();
	BOOL Parse(CString url);
	BOOL GetSimpleResponse( CString& url,CString& content,__out CString& response );
	CString CreateNecessaryPath(LPCTSTR volName);
	virtual Downloader* CreateNewInst()=0;

protected:
	BOOL SavePicAsFileReally(CString picUrl,CString path);
	CString  UTF8ToUnicode(LPCSTR szU8);
	vector<CString> Split(CString& str,LPTSTR token);
	VOID CloseCHttpFile(CHttpFile* html);
	
	BOOL IsMatched(CString& str,CString& regex);
	virtual CRegexpT<TCHAR>& GetIsMainRegex()=0;
	virtual CRegexpT<TCHAR>& GetTitleRegex()=0;
	virtual CRegexpT<TCHAR>& GetLastVolRegex()=0;
	virtual CRegexpT<TCHAR>& GetLastDateRegex()=0;
	virtual CRegexpT<TCHAR>& GetVolRegex()=0;
	virtual VOID GetVolUrlNamePair(CString& line,pair<CString,CString>& p)=0;
	virtual CString GetMatchedStr(CRegexpT<TCHAR>& param1, CString& line );
	BOOL GetSimpleGet(CString strid,CString& resp);
	
};
