// PassSaverDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"


// CPassSaverDlg �Ի���
class CPassSaverDlg : public CDialog
{
// ����
public:
	CPassSaverDlg(CWnd* pParent = NULL);	// ��׼���캯��
	//~CPassSaverDlg();

// �Ի�������
	enum { IDD = IDD_PASSSAVER_DIALOG };

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

private :
	_ConnectionPtr m_pConnection;
	_RecordsetPtr m_pRecordset;

public:
	CListCtrl m_List;
};
