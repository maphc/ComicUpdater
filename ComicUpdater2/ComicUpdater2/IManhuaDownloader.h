#pragma once
#include "downloader.h"

class IManhuaDownloader :public Downloader
{
public:
	IManhuaDownloader(void);
	virtual ~IManhuaDownloader(void);

private:
	static CRegexpT<TCHAR> titleRegex;
	static CRegexpT<TCHAR> lastDateRegex;
	static CRegexpT<TCHAR> lastVolRegex;
	static CRegexpT<TCHAR> volRegex;
	static CRegexpT<TCHAR> isMainRegex;
   

public:
	virtual vector<CString> GetPicUrls(CString& url);
	virtual Downloader* CreateNewInst();

protected:
	virtual CRegexpT<TCHAR>& GetIsMainRegex();
	virtual CRegexpT<TCHAR>& GetTitleRegex();
	virtual CRegexpT<TCHAR>& GetLastVolRegex();
	virtual CRegexpT<TCHAR>& GetLastDateRegex();
	virtual CRegexpT<TCHAR>& GetVolRegex();
    virtual CString GetReferer(CString volUrl,int index,CString picUrl);

	virtual VOID GetVolUrlNamePair(CString& line,pair<CString,CString>& p);
	//CString GetSingleUrl( CString strid, int& total, int i );
	virtual VOID PostParse();
	virtual CString EncodeUrlIfNecessery(CString& picUrl){
		CHAR buf[500]={0};
		UINT len=Ansi2Utf8(picUrl,buf);
		CString r=UrlEncode(buf,len);
		return r;
	}
	virtual CString EncodeFileNameIfNecessery(CString& fileName){ 
		CString f(fileName);
		f.Replace(_T("%20"),_T("_"));
		return f;
	}

	
};
