// ChatDlg.h : 头文件
//

#pragma once

#define WM_RECEIVE WM_USER+1

// CChatDlg 对话框
class CChatDlg : public CDialog
{
// 构造
public:
	CChatDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CHAT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	SOCKET m_socket;

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg LRESULT OnRecvData(WPARAM wParam,LPARAM lParam);

	DECLARE_MESSAGE_MAP()

public:
	static DWORD WINAPI RecvProc(LPVOID lpParameter);
	
	afx_msg void OnBnClickedOk();
};
