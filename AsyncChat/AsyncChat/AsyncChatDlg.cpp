// AsyncChatDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AsyncChat.h"
#include "AsyncChatDlg.h"

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


// CAsyncChatDlg �Ի���




CAsyncChatDlg::CAsyncChatDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAsyncChatDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAsyncChatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAsyncChatDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_RECV,OnRecvData)
	ON_BN_CLICKED(IDOK, &CAsyncChatDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CAsyncChatDlg ��Ϣ�������

BOOL CAsyncChatDlg::OnInitDialog()
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
	
	InitSocket();


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CAsyncChatDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CAsyncChatDlg::OnPaint()
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
HCURSOR CAsyncChatDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CAsyncChatDlg::OnRecvData( WPARAM wParam,LPARAM lParam)
{
	//MessageBox("���յ����� !");
	if(FD_READ==LOWORD(lParam)){
		
		char command[2];
		char recvBuf[255];
		WSABUF wsaBuf[2];
		wsaBuf[0].len=1;
		wsaBuf[0].buf=command;
		wsaBuf[1].len=255;
		wsaBuf[1].buf=recvBuf;
		memset(command,0,sizeof(command));
		//memset(recvBuf,0,sizeof(recvBuf));
		DWORD readSize;
		DWORD flag=0;
		SOCKADDR_IN addrFrom;
		
		int len=sizeof(addrFrom);
		if(SOCKET_ERROR==WSARecvFrom(m_socket,wsaBuf,2,&readSize,&flag,(SOCKADDR*)&addrFrom,&len,NULL,NULL)){
			CString errTxt;
			errTxt.Format("��������ʧ�� :%d"+WSAGetLastError());
			MessageBox(errTxt);
			return -1;
		}
		
		char hostName[512];
		getnameinfo((SOCKADDR*)&addrFrom,len,hostName,512,NULL,4,NI_NUMERICSERV);
		//MessageBox(hostName);
		CString temp;
		//temp.Format("%s ˵ : %s,%s",inet_ntoa(addrFrom.sin_addr),wsaBuf[1].buf,wsaBuf[0].buf);///
		temp.Format("%s ˵ : %s--%s",hostName,wsaBuf[1].buf,wsaBuf[0].buf);

		CString edit;
		GetDlgItemText(IDC_RECV,edit);
		temp+="\r\n";
		temp+=edit;
		
		SetDlgItemText(IDC_RECV,temp);
		//GetDlgItem(IDC_SEND)->SetFocus();

	}
	return 0;
}

void CAsyncChatDlg::InitSocket()
{
	WORD wVersionRequested;
	wVersionRequested=MAKEWORD(2,2);
	WSADATA wsaData;
	WSAStartup(wVersionRequested,&wsaData);

	m_socket=WSASocket(AF_INET,SOCK_DGRAM,0,NULL,NULL,0);

	if(INVALID_SOCKET==m_socket){
		CString errTxt;
		errTxt.Format("��ȡSOCKETʧ�� :%d"+WSAGetLastError());
		MessageBox(errTxt);
		return ;
	}

	SOCKADDR_IN addr;
	addr.sin_family=AF_INET;
	addr.sin_port=htons(1210);
	addr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);

	if(SOCKET_ERROR==bind(m_socket,(SOCKADDR*)&addr,sizeof(addr))){
		CString errTxt;
		errTxt.Format("��SOCKETʧ�� :%d"+WSAGetLastError());
		MessageBox(errTxt);
		return ;
	}

	if(SOCKET_ERROR==WSAAsyncSelect(m_socket,m_hWnd,UM_RECV,FD_READ)){
		CString errTxt;
		errTxt.Format("�����첽SOCKETʧ�� :%d"+WSAGetLastError());
		MessageBox(errTxt);
		return ;
	}

	((CIPAddressCtrl*)GetDlgItem(IDC_IPADDR))->SetAddress(ntohl(inet_addr("127.0.0.1")));
	GetDlgItem(IDC_SEND)->SetFocus();
	
}

CAsyncChatDlg::~CAsyncChatDlg()
{	
	if(m_socket){
		closesocket(m_socket);
		WSACleanup();
	}
	
}
void CAsyncChatDlg::OnBnClickedOk()
{
	//OnOK();

	SOCKADDR_IN addrTo;
	addrTo.sin_family=AF_INET;
	addrTo.sin_port=htons(1210);
	DWORD wIP;
	((CIPAddressCtrl*)GetDlgItem(IDC_IPADDR))->GetAddress(wIP);
	addrTo.sin_addr.S_un.S_addr=htonl(wIP);
	
	CString edit;
	GetDlgItemText(IDC_SEND,edit);
	WSABUF wsaBuf[2];
	memset(wsaBuf,0,sizeof(wsaBuf));
	wsaBuf[0].buf="W";
	wsaBuf[0].len=strlen(wsaBuf[0].buf);
	wsaBuf[1].len=edit.GetLength()+1;
	wsaBuf[1].buf=edit.GetBuffer();
	DWORD sendSize;
	
	//CString dbg;
	//dbg.Format("IP :%s\r\n���� :%s",inet_ntoa(addrTo.sin_addr),wsaBuf.buf);

	if(SOCKET_ERROR==WSASendTo(m_socket,wsaBuf,2,&sendSize,0,(SOCKADDR*)&addrTo,sizeof(addrTo),NULL,NULL)){
		CString errTxt;
		errTxt.Format("��������ʧ�� :%d"+WSAGetLastError());
		MessageBox(errTxt);
		return ;
	}
	
	SetDlgItemText(IDC_SEND,NULL);

}
