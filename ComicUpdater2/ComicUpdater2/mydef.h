#pragma once
#include "stdafx.h"

//×Ô¶¨Òå
struct ComicInfo{
	UINT id;
	CString comicName;
	CString lastVol;
	CString lastDate;
	CString updateDate;
	CString url;
	BOOL isNew;
};



const UINT NAME_INDEX=0;
const UINT LASTVOL_INDEX=1;
const UINT LASTDATE_INDEX=2;
const UINT UPDATEDATE_INDEX=3;
const UINT URL_INDEX=4;