// ChatDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Chat.h"
#include "ChatDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CChatDlg �Ի���




CChatDlg::CChatDlg(CWnd* pParent /*=NULL*/)
: CDialog(CChatDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CChatDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_RECEIVE,OnRecvData)

	ON_BN_CLICKED(IDOK, &CChatDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CChatDlg ��Ϣ�������

BOOL CChatDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	((CIPAddressCtrl*)GetDlgItem(IDC_IPADDR))->SetAddress(ntohl(inet_addr("127.0.0.1")));

	if(0==AfxSocketInit()){
		int err=WSAGetLastError();
		CString errTxt;
		errTxt.Format("���ִ��� :%d",err);

		MessageBox(errTxt,"����",MB_OK);
		return FALSE;
	}

	m_socket=socket(AF_INET,SOCK_DGRAM,0);
	if(INVALID_SOCKET==m_socket){
		int err=WSAGetLastError();
		CString errTxt;
		errTxt.Format("���ִ��� :%d",err);
		MessageBox(errTxt,"����",MB_OK);
		return FALSE;
	}

	SOCKADDR_IN addr;
	addr.sin_family=AF_INET;
	addr.sin_port=htons(1210);
	addr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
	if(SOCKET_ERROR==bind(m_socket,(SOCKADDR*)&addr,sizeof(addr))){
		int err=WSAGetLastError();
		CString errTxt;
		errTxt.Format("���ִ��� :%d",err);

		MessageBox(errTxt,"����",MB_OK);
		return FALSE;
	}


	recv_data* recv=new recv_data;
	recv->sock=m_socket;
	recv->hwnd=m_hWnd;
	HANDLE handle=CreateThread(NULL,0,RecvProc,recv,0,NULL);
	CloseHandle(handle);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CChatDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CChatDlg::OnPaint()
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
HCURSOR CChatDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


DWORD WINAPI CChatDlg::RecvProc(LPVOID lpParameter)
{
	recv_data* data=(recv_data*)lpParameter;
	SOCKET s=data->sock;
	HWND hwnd=data->hwnd;
	SOCKADDR_IN addrFrom;
	int len=sizeof(addrFrom);

	char recvData[256];
	char tempData[512];
	while(1){

		if(SOCKET_ERROR==recvfrom(s,recvData,255,0,(SOCKADDR*)&addrFrom,&len)){
			//::MessageBox(hwnd,"��������ʧ��");
			int err=WSAGetLastError();
			CString errTxt;
			errTxt.Format("���ִ��� :%d",err);
			::MessageBox(hwnd,errTxt,"����",MB_OK);
			return 0;
		}

		//::MessageBox(hwnd,"���յ�����","Info",MB_OK);
		sprintf_s(tempData,511,"%s ˵ : %s",inet_ntoa(addrFrom.sin_addr),recvData);

		//GetDlgItem(IDC_RECV)->SetWindowText(tempData);
		::PostMessage(hwnd,WM_RECEIVE,NULL,(LPARAM)tempData);
	}
	return 0;
}

LRESULT CChatDlg::OnRecvData(WPARAM wParam,LPARAM lParam)
{
	CString txt=(char*)lParam;
	CString edit;
	GetDlgItemText(IDC_RECV,edit);
	edit+="\r\n";
	edit+=txt;
	SetDlgItemText(IDC_RECV,edit);


	return 0;
}

void CChatDlg::OnBnClickedOk()
{
	//OnOK();
	//MessageBox("Get Char !");
	DWORD wIP;

	SOCKADDR_IN addrTo;
	addrTo.sin_family=AF_INET;
	addrTo.sin_port=htons(1210);
	((CIPAddressCtrl*)GetDlgItem(IDC_IPADDR))->GetAddress(wIP);
	addrTo.sin_addr.S_un.S_addr=htonl(wIP);
	int len=sizeof(addrTo);

	CString edit;
	//char temp[500];
	GetDlgItemText(IDC_SEND,edit);
	if(edit.GetLength()>0){
		CString dbgInfo;
		dbgInfo.Format("���͵�ip :%s\r\n���͵����� :%s",inet_ntoa(addrTo.sin_addr),edit);	//inet_ntoa(addrTo.sin_addr)
		//MessageBox(dbgInfo);

		if(SOCKET_ERROR==sendto(m_socket,edit,edit.GetLength()+1,0,(SOCKADDR*)&addrTo,len)){
			int err=WSAGetLastError();
			CString errTxt;
			errTxt.Format("���ִ��� :%d",err);
			MessageBox(errTxt,"����",MB_OK);
			return ;
		}

	}


}
