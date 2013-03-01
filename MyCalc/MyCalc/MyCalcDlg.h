// MyCalcDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "MyDlg.h"
#include "MyDlg2.h"

// CMyCalcDlg 对话框
class CMyCalcDlg : public CDialog
{
// 构造
public:
	CMyCalcDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MYCALC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	CTabCtrl m_tab;
	CMyDlg dlg1;
	CMyDlg2 dlg2;
	CDialog* m_dlg[2];

public:
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual void OnOK();
};
