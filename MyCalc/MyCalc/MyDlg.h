#pragma once


// CMyDlag �Ի���

class CMyDlg : public CDialog
{
	DECLARE_DYNAMIC(CMyDlg)

public:
	CMyDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMyDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
