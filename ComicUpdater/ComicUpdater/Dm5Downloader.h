#pragma once
#include "stdafx.h"



class Dm5Downloader
{
private:
	int static const BUFFER_SIZE;
	static const CString SERVIER_NAME;
	CInternetSession session;
	CString url;
	CString lastDate;
	CString lastVol;
	CString title;
	static CRegexpT<TCHAR> titleReg;
	static CRegexpT<TCHAR> lastDateReg;
	static CRegexpT<TCHAR> lastVolReg;
	static CRegexpT<TCHAR> volReg;
	CString basePath;
	map<CString,CString> volCache;

private:
	BOOL isMatched(CString& str,CString& regex);
	CString getGroup1(CString& str,CString& regex);
	CString  UTF8ToUnicode(LPSTR szU8);
	CString getField(CString& line,CRegexpT<TCHAR>& cReg);
	BOOL savePicAsFileReally(CString picUrl,CString path);
	VOID closeCHttpFile(CHttpFile* html);
public:

	BOOL init();
	LPCTSTR getLastDate();
	LPCTSTR getLastVol();
	LPCTSTR getTitle();
	LPCTSTR getUrl();
	map<CString,CString>* getVolCache(); 
	BOOL savePicAsFile(CString picUrl,CString fileName);
	BOOL getSimpleResponse(CString& url,CString& content,CString& response);
	BOOL saveVol(CString url,CString volName,CString basePath);
	BOOL saveVol( CString url);
	vector<CString> getPicUrls(CString& url);
	
	CString getSingleUrl(CString& strid,int& total,int strpage);
	CString getSavePath( CString url,CString volName );
	VOID Dm5Downloader::createNecessaryPath(CString volName);
	//vector<CString>
public:
	Dm5Downloader();
	Dm5Downloader(CString url);
	~Dm5Downloader(void);

};


