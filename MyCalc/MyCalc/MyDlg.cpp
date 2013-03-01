// MyDlag.cpp : 实现文件
//

#include "stdafx.h"
#include "MyCalc.h"
#include "MyDlg.h"
#include "MyUtil.h"



// CMyDlg 对话框

IMPLEMENT_DYNAMIC(CMyDlg, CDialog)

CMyDlg::CMyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMyDlg::IDD, pParent)
	, addFirst(_T(""))
	, addSecond(_T(""))
	, addResult(_T(""))
	, minusFirst(_T(""))
	, minusSecond(_T(""))
	, minusResult(_T(""))
	, multiplyFirst(_T(""))
	, multiplySecond(_T(""))
	, multiplyResult(_T(""))
	, divFirst(_T(""))
	, divSecond(_T(""))
	, divResult(_T(""))
	, radix(0)
{

}

CMyDlg::~CMyDlg()
{
}

void CMyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ADD_FISRT, addFirst);
	DDV_MaxChars(pDX, addFirst, 8);
	DDX_Text(pDX, IDC_ADD_SECOND, addSecond);
	DDV_MaxChars(pDX, addSecond, 8);
	DDX_Text(pDX, IDC_ADD_RESULT, addResult);
	DDX_Text(pDX, IDC_MINUS_FISRT, minusFirst);
	DDV_MaxChars(pDX, minusFirst, 8);
	DDX_Text(pDX, IDC_MINUS_SECOND, minusSecond);
	DDV_MaxChars(pDX, minusSecond, 8);
	DDX_Text(pDX, IDC_MINUS_RESULT, minusResult);
	DDX_Text(pDX, IDC_MULTIPLY_FIRST, multiplyFirst);
	DDV_MaxChars(pDX, multiplyFirst, 8);
	DDX_Text(pDX, IDC_MULTIPLY_SECOND, multiplySecond);
	DDV_MaxChars(pDX, multiplySecond, 8);
	DDX_Text(pDX, IDC_MULTIPLY_RESULT, multiplyResult);
	DDX_Text(pDX, IDC_DIV_FIRST, divFirst);
	DDV_MaxChars(pDX, divFirst, 8);
	DDX_Text(pDX, IDC_DIV_SECOND, divSecond);
	DDV_MaxChars(pDX, divSecond, 8);
	DDX_Text(pDX, IDC_DIV_RESULT, divResult);
	DDX_Radio(pDX, IDC_RADIO1, radix);
}


BEGIN_MESSAGE_MAP(CMyDlg, CDialog)
	ON_BN_CLICKED(IDC_CLEAR, &CMyDlg::OnBnClickedClear)
	ON_EN_CHANGE(IDC_ADD_FISRT, &CMyDlg::OnEnChangeAddFisrt)
	ON_EN_CHANGE(IDC_ADD_SECOND, &CMyDlg::OnEnChangeAddSecond)
	ON_BN_CLICKED(IDC_RADIO2, &CMyDlg::OnBnClickedRadio2)
END_MESSAGE_MAP()


// CMyDlg 消息处理程序

void CMyDlg::OnBnClickedClear()
{
	addFirst="";
	addSecond="";
	addResult="";
	minusFirst="";
	minusSecond="";
	minusResult="";
	multiplyFirst="";
	multiplySecond="";
	multiplyResult="";
	divFirst="";
	divSecond="";
	divResult="";
	radix=0;
	UpdateData(FALSE);
}

void CMyDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialog::OnOK();
}

void CMyDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	GetParent()->GetParent()->PostMessage(WM_QUIT);
	//CDialog::OnCancel();
}

void CMyDlg::OnEnChangeAddFisrt()
{
	UpdateData(TRUE);
	char r[20];
	MyUtil util;

	if (radix==0)
	{
		long a=atol(addFirst);
		long b=atol(addSecond);
		
		_ltoa_s(a+b,r,20,10);
	} 
	else
	{
		
		//char dec[20];
		long a=util.hToD(addFirst);
		long b=util.hToD(addSecond);
		_ltoa_s(a+b,r,20,16);

		
	}
	
	addResult=r;
	
	addResult.MakeUpper();
	UpdateData(FALSE);
}

void CMyDlg::OnEnChangeAddSecond()
{
	UpdateData(TRUE);
	char r[20];
	MyUtil util;
	

	if (radix==0)
	{
		long a=atol(addFirst);
		long b=atol(addSecond);
		_ltoa_s(a+b,r,20,10);
	} 
	else
	{
		//char dec[20];
		long a=util.hToD(addFirst);
		long b=util.hToD(addSecond);
		_ltoa_s(a+b,r,20,16);

	}

	addResult=r;
	
	addResult.MakeUpper();
	UpdateData(FALSE);
}

void CMyDlg::OnBnClickedRadio2()
{
	OnEnChangeAddFisrt();
}
