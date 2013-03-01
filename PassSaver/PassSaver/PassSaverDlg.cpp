// PassSaverDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PassSaver.h"
#include "PassSaverDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPassSaverDlg �Ի���




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


// CPassSaverDlg ��Ϣ�������

BOOL CPassSaverDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	
	if(!AfxOleInit()){
		MessageBox(_T("OLE��ʼ��ʧ��"));
		EndDialog(-1);
	}
	
	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_List.InsertColumn(0,_T("���"),LVCFMT_LEFT,80);
	m_List.InsertColumn(1,_T("URL"),LVCFMT_LEFT,280);
	m_List.InsertColumn(2,_T("�û���"),LVCFMT_LEFT,80);
	
	HRESULT hr;
	try{
		hr = m_pConnection.CreateInstance("ADODB.Connection");///����Connection����
		if(SUCCEEDED(hr))
		{
			hr = m_pConnection->Open("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=./data.mdb","","",adModeUnknown);///�������ݿ�
				//����һ���������ִ��е�Provider�����ACCESS2000�����ģ�����ACCESS97��
				//��Ҫ��Ϊ��Provider=Microsoft.Jet.OLEDB.3.51;
			
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
			MessageBox(_T("���ݿ�����ʧ��"));
			EndDialog(-1);
		}
	}
	catch(_com_error e)///��׽�쳣
	{
		CString errormessage;
		errormessage.Format(_T("�������ݿ�ʧ��!\r\n������Ϣ:%s"),e.ErrorMessage());
		AfxMessageBox(errormessage);///��ʾ������Ϣ
	} 


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CPassSaverDlg::OnPaint()
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
HCURSOR CPassSaverDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

