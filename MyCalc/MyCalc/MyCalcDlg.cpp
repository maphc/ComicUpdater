// MyCalcDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyCalc.h"
#include "MyCalcDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMyCalcDlg 对话框




CMyCalcDlg::CMyCalcDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMyCalcDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyCalcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab);
}

BEGIN_MESSAGE_MAP(CMyCalcDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CMyCalcDlg::OnTcnSelchangeTab1)
END_MESSAGE_MAP()


// CMyCalcDlg 消息处理程序

BOOL CMyCalcDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	
	//TAB初始化
	m_dlg[0]=&dlg1;
	m_dlg[1]=&dlg2;
	m_dlg[0]->Create(IDD_DIALOG1,&m_tab);
	m_dlg[1]->Create(IDD_DIALOG2,&m_tab);
	
	m_tab.InsertItem(0,"第一页");
	m_tab.InsertItem(1,"第二页");
	m_tab.SetCurSel(0);
	m_dlg[0]->ShowWindow(SW_SHOW);
	
	CRect rect;
	
	GetClientRect(&rect);
	m_tab.MoveWindow(&rect);
	
	m_tab.GetClientRect(&rect);
	rect.top+=22; 
	//rect.bottom-=4; 
	//rect.left+=4; 
	//rect.right-=4; 
	m_dlg[0]->MoveWindow(&rect);
	m_dlg[1]->MoveWindow(&rect);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMyCalcDlg::OnPaint()
{

	
	

	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMyCalcDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMyCalcDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	int cur=m_tab.GetCurSel();
	m_dlg[0]->ShowWindow(SW_HIDE);
	m_dlg[1]->ShowWindow(SW_HIDE);
	m_dlg[cur]->ShowWindow(SW_SHOW);
	


	*pResult = 0;
}

void CMyCalcDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialog::OnOK();
}
