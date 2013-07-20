#include "stdafx.h"
#include <v8.h>
#include "PyHelper.h"
#pragma once
#pragma comment(lib,"v8.lib")


namespace V8Helper{
	CString get178VolPages(CString h);
	CString getIManhuaVolPages(CString h);
	void getXxbhInfo(CString js0,UINT& img_s,CString& msgs);

}