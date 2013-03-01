#pragma once


// CMyDlg2 对话框

class CMyDlg2 : public CDialog
{
	DECLARE_DYNAMIC(CMyDlg2)

public:
	CMyDlg2(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMyDlg2();

// 对话框数据
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
