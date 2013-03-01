// ComicUpdater2Dlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"


// CComicUpdater2Dlg �Ի���
class CComicUpdater2Dlg : public CDialog
{
// ����
public:
	CComicUpdater2Dlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CComicUpdater2Dlg();

// �Ի�������
	enum { IDD = IDD_COMICUPDATER2_DIALOG };

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

//�Զ���
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
