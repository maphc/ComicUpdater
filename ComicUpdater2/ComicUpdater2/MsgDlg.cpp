// MsgDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ComicUpdater2.h"
#include "MsgDlg.h"


// CMsgDlg 对话框

IMPLEMENT_DYNAMIC(CMsgDlg, CDialog)

CMsgDlg::CMsgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMsgDlg::IDD, pParent)
{

}

CMsgDlg::~CMsgDlg()
{
}

void CMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMsgDlg, CDialog)
	ON_BN_CLICKED(IDCANCEL, &CMsgDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CMsgDlg 消息处理程序

void CMsgDlg::OnBnClickedCancel()
{
	isTerminated=TRUE;
	
	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
	
}



BOOL CMsgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetDlgItemText(IDC_MSG,NULL);
	GetDlgItem(IDCANCEL)->EnableWindow(TRUE);
	isTerminated=FALSE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
