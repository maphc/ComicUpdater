#pragma once


// CMyDlag 对话框

class CMyDlg : public CDialog
{
	DECLARE_DYNAMIC(CMyDlg)

public:
	CMyDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMyDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CString addFirst;
	CString addSecond;
	CString addResult;
	CString minusFirst;
	CString minusSecond;
	CString minusResult;
	CString multiplyFirst;
	CString multiplySecond;
	CString multiplyResult;
	CString divFirst;
	CString divSecond;
	CString divResult;
	int radix;
public:
	afx_msg void OnBnClickedClear();
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnEnChangeAddFisrt();
	afx_msg void OnEnChangeAddSecond();
	afx_msg void OnBnClickedRadio2();
};
