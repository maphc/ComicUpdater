// AsyncChatDlg.h : ͷ�ļ�
//

#pragma once

#define UM_RECV WM_USER+1

// CAsyncChatDlg �Ի���
class CAsyncChatDlg : public CDialog
{
// ����
public:
	CAsyncChatDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CAsyncChatDlg();
// �Ի�������
	enum { IDD = IDD_ASYNCCHAT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnRecvData(WPARAM,LPARAM);
	DECLARE_MESSAGE_MAP()

private:
	SOCKET m_socket;
	void InitSocket();
public:
	afx_msg void OnBnClickedOk();
};
