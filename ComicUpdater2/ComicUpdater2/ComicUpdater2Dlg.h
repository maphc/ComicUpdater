// ComicUpdater2Dlg.h : 头文件
//

#pragma once
#include "afxcmn.h"


// CComicUpdater2Dlg 对话框
class CComicUpdater2Dlg : public CDialog
{
// 构造
public:
	CComicUpdater2Dlg(CWnd* pParent = NULL);	// 标准构造函数
	~CComicUpdater2Dlg();

// 对话框数据
	enum { IDD = IDD_COMICUPDATER2_DIALOG };

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

//自定义
private:
	
	_ConnectionPtr m_pConnection;
	_RecordsetPtr m_pRecordset;

	static const UINT LASTVOL_MASK=1;
	static const UINT LASTDATE_MASK=2;
	static const UINT UPDATEDATE_MASK=4;

public:
	map<CString,Downloader*> m_downKey;
	vector<ComicInfo> comicCache;
	BOOL isInited;
	Downloader* GetDownloaderByKey(CString& url);
	BOOL UpdateComicInfo(UINT n ,Downloader* down,UINT mask);
	CMsgDlg m_msgDlg;

	CString m_url;
	CListCtrl m_comicList;
	VOID InitDB();
	afx_msg void OnNMCustomdrawComicList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkComicList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickComicList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedAnalyse();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedDel();
	afx_msg void OnBnClickedUpdate();

	
	afx_msg void OnEnChangeUrl();
};
