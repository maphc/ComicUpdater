// MyCalcDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "MyDlg.h"
#include "MyDlg2.h"

// CMyCalcDlg �Ի���
class CMyCalcDlg : public CDialog
{
// ����
public:
	CMyCalcDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MYCALC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
