#pragma once
#include "downloader.h"

class Acg178Downloader :public Downloader
{
public:
	Acg178Downloader(void);
	virtual ~Acg178Downloader(void);

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
	virtual VOID GetVolUrlNamePair(CString& line,pair<CString,CString>& p);
	//CString GetSingleUrl( CString strid, int& total, int i );

};
