// PassSaver.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CPassSaverApp:
// �йش����ʵ�֣������ PassSaver.cpp
//

class CPassSaverApp : public CWinApp
{
public:
	CPassSaverApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CPassSaverApp theApp;