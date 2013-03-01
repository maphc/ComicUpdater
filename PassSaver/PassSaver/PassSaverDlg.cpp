// PassSaverDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PassSaver.h"
#include "PassSaverDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPassSaverDlg 对话框




CPassSaverDlg::CPassSaverDlg(CWnd* pParent /*=NULL*/)
: CDialog(CPassSaverDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPassSaverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PASS_LIST, m_List);
}

BEGIN_MESSAGE_MAP(CPassSaverDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CPassSaverDlg 消息处理程序

BOOL CPassSaverDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	
	if(!AfxOleInit()){
		MessageBox(_T("OLE初始化失败"));
		EndDialog(-1);
	}
	
	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_List.InsertColumn(0,_T("类别"),LVCFMT_LEFT,80);
	m_List.InsertColumn(1,_T("URL"),LVCFMT_LEFT,280);
	m_List.InsertColumn(2,_T("用户名"),LVCFMT_LEFT,80);
	
	HRESULT hr;
	try{
		hr = m_pConnection.CreateInstance("ADODB.Connection");///创建Connection对象
		if(SUCCEEDED(hr))
		{
			hr = m_pConnection->Open("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=./data.mdb","","",adModeUnknown);///连接数据库
				//上面一句中连接字串中的Provider是针对ACCESS2000环境的，对于ACCESS97，
				//需要改为：Provider=Microsoft.Jet.OLEDB.3.51;
			
			m_pRecordset.CreateInstance(__uuidof(Recordset));//
			m_pRecordset->Open("SELECT a.*,b.type FROM user_info a , category b where a.category=b.id",m_pConnection.GetInterfacePtr(),adOpenDynamic,adLockOptimistic,adCmdText);
			m_pRecordset->MoveFirst();
			int i=0;
			while(!m_pRecordset->adoEOF){
				//_variant_t c=m_pRecordset->GetCollect(_T("category"));
				//_variant_t u=m_pRecordset->GetCollect(_T("url"));
				//_variant_t user=m_pRecordset->GetCollect(_T("user"));
				m_List.InsertItem(i,(_bstr_t)m_pRecordset->GetCollect(_T("type")));
				m_List.SetItemText(i,1,(_bstr_t)m_pRecordset->GetCollect(_T("url")));
				m_List.SetItemText(i,2,(_bstr_t)m_pRecordset->GetCollect(_T("user")));
				m_pRecordset->MoveNext();

				i++;
			}

			m_pRecordset->Close();

		}else{
			MessageBox(_T("数据库连接失败"));
			EndDialog(-1);
		}
	}
	catch(_com_error e)///捕捉异常
	{
		CString errormessage;
		errormessage.Format(_T("连接数据库失败!\r\n错误信息:%s"),e.ErrorMessage());
		AfxMessageBox(errormessage);///显示错误信息
	} 


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPassSaverDlg::OnPaint()
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
HCURSOR CPassSaverDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

