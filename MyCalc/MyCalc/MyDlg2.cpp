// MyDlg2.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyCalc.h"
#include "MyDlg2.h"


// CMyDlg2 �Ի���

IMPLEMENT_DYNAMIC(CMyDlg2, CDialog)

CMyDlg2::CMyDlg2(CWnd* pParent /*=NULL*/)
	: CDialog(CMyDlg2::IDD, pParent)
{

}

CMyDlg2::~CMyDlg2()
{
}

void CMyDlg2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMyDlg2, CDialog)
	ON_BN_CLICKED(IDOK, &CMyDlg2::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMyDlg2::OnBnClickedCancel)
END_MESSAGE_MAP()


// CMyDlg2 ��Ϣ�������

void CMyDlg2::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//OnOK();
}

void CMyDlg2::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//OnCancel();
	GetParent()->GetParent()->PostMessage(WM_QUIT);
}
