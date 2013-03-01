#pragma once
#include "downloader.h"

class Dm5Downloader :public Downloader
{
private:
	static CRegexpT<TCHAR> titleRegex;
	static CRegexpT<TCHAR> lastDateRegex;
	static CRegexpT<TCHAR> lastVolRegex;
	static CRegexpT<TCHAR> volRegex;
	static CRegexpT<TCHAR> isMainRegex;
    
public:
	Dm5Downloader();
	Dm5Downloader(LPTSTR url);
	~Dm5Downloader(void);

public:
	virtual vector<CString> GetPicUrls(CString& url);
	virtual Downloader* CreateNewInst();

protected:
	virtual CRegexpT<TCHAR>& GetIsMainRegex();
	virtual CRegexpT<TCHAR>& GetTitleRegex();
	virtual CRegexpT<TCHAR>& GetLastVolRegex();
	virtual CRegexpT<TCHAR>& GetLastDateRegex();
	virtual CRegexpT<TCHAR>& GetVolRegex();
    

	virtual VOID GetVolUrlNamePair(CString& line,pair<CString,CString>& p);
	CString GetSingleUrl( CString strid, int& total, int i );
	VOID enc( CString& str );
    
};
