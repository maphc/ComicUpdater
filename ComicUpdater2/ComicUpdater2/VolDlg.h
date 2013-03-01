#pragma once
#include "stdafx.h"
#include "resource.h"
#include "afxcmn.h"
#include "MsgDlg.h"

// CVolDlg 对话框

class CVolDlg : public CDialog
{
	DECLARE_DYNAMIC(CVolDlg)

public:
	CVolDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CVolDlg();
	
// 对话框数据
	enum { IDD = IDD_VOL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()


//自定义
private:
	
	ComicInfo* m_oldInfo;
	
public:
	BOOL isSuccess;
	Downloader* m_down;
	VOID init(Downloader* down);
	VOID setOldInfo(ComicInfo* oldInfo);
	virtual BOOL OnInitDialog();
	CListCtrl m_volList;
	afx_msg void OnBnClickedDown();
	CMsgDlg m_msgDlg;
	afx_msg void OnBnClickedDeAll();
    afx_msg void OnBnClickedSeAll();
};


struct DownInfo{
	CVolDlg* dlg;
	CString volName;
	CString volUrl;
};