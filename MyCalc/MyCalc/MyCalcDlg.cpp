// MyCalcDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyCalc.h"
#include "MyCalcDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMyCalcDlg �Ի���




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


// CMyCalcDlg ��Ϣ�������

BOOL CMyCalcDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	
	//TAB��ʼ��
	m_dlg[0]=&dlg1;
	m_dlg[1]=&dlg2;
	m_dlg[0]->Create(IDD_DIALOG1,&m_tab);
	m_dlg[1]->Create(IDD_DIALOG2,&m_tab);
	
	m_tab.InsertItem(0,"��һҳ");
	m_tab.InsertItem(1,"�ڶ�ҳ");
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

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMyCalcDlg::OnPaint()
{

	
	

	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMyCalcDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMyCalcDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int cur=m_tab.GetCurSel();
	m_dlg[0]->ShowWindow(SW_HIDE);
	m_dlg[1]->ShowWindow(SW_HIDE);
	m_dlg[cur]->ShowWindow(SW_SHOW);
	


	*pResult = 0;
}

void CMyCalcDlg::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���

	//CDialog::OnOK();
}
