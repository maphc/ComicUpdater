#pragma once


// CMsgDlg �Ի���

class CMsgDlg : public CDialog
{
	DECLARE_DYNAMIC(CMsgDlg)

public:
	CMsgDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMsgDlg();

// �Ի�������
	enum { IDD = IDD_MSG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	BOOL isTerminated;
	afx_msg void OnBnClickedCancel();
	
	virtual BOOL OnInitDialog();
};
