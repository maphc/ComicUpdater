#include "stdafx.h"


VOID aaa(){
	Downloader* down=new Acg178Downloader;

	down->Parse("http://acg.178.com/mh/s/sszqdzjy.shtml");
	//http://acg.178.com/mh/s/sszqdzjy/8672.shtml
	//http://acg.178.com/mh/imgs/s/史上最强弟子兼一/史上最强弟子兼一_第386话/01.jpg
	MessageBox(NULL,"注意","注意",MB_OK);
	///mh/imgs/s/%E5%8F%B2%E4%B8%8A%E6%9C%80%E5%BC%BA%E5%BC%9F%E5%AD%90%E5%85%BC%E4%B8%80/%E5%8F%B2%E4%B8%8A%E6%9C%80%E5%BC%BA%E5%BC%9F%E5%AD%90%E5%85%BC%E4%B8%80_%E7%AC%AC386%E8%AF%9D/01.jpg
	down->SavePicAsFile(CString("http://acg.178.com/mh/imgs/s/%E5%8F%B2%E4%B8%8A%E6%9C%80%E5%BC%BA%E5%BC%9F%E5%AD%90%E5%85%BC%E4%B8%80/%E5%8F%B2%E4%B8%8A%E6%9C%80%E5%BC%BA%E5%BC%9F%E5%AD%90%E5%85%BC%E4%B8%80_%E7%AC%AC386%E8%AF%9D/01.jpg"),CString("z:\\files\\"));


	delete down;
}

UINT Ansi2Utf8(LPCSTR s,LPSTR szU8){
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

CString UrlEncode(LPSTR szU8,UINT n){
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

int WINAPI _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{	
	LPCTSTR s="http://acg.178.com/mh/imgs/s/史上最强弟子兼一/史上最强弟子兼一_第386话/01.jpg";
	CHAR buf[500]={0};
	UINT len=Ansi2Utf8(s,buf);

	CString r=UrlEncode(buf,len);

	return 0;
}



