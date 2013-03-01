// MyDlg2.cpp : 实现文件
//

#include "stdafx.h"
#include "MyCalc.h"
#include "MyDlg2.h"


// CMyDlg2 对话框

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


// CMyDlg2 消息处理程序

void CMyDlg2::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnOK();
}

void CMyDlg2::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnCancel();
	GetParent()->GetParent()->PostMessage(WM_QUIT);
}
