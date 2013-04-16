// ComicUpdater2Dlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ComicUpdater2.h"
#include "ComicUpdater2Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CComicUpdater2Dlg �Ի���




CComicUpdater2Dlg::CComicUpdater2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CComicUpdater2Dlg::IDD, pParent)
	, m_url(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
CComicUpdater2Dlg::~CComicUpdater2Dlg()
{
	if(m_pConnection->State){
		m_pConnection->Close();
		m_pConnection= NULL;
	}
	for(map<CString,Downloader*>::iterator it=m_downKey.begin();it!=m_downKey.end();it++){
		delete (*it).second;
	}

	
	m_msgDlg.DestroyWindow();
}
void CComicUpdater2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_URL, m_url);
	DDX_Control(pDX, IDC_COMIC_LIST, m_comicList);
}

BEGIN_MESSAGE_MAP(CComicUpdater2Dlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_COMIC_LIST, &CComicUpdater2Dlg::OnNMCustomdrawComicList)
//	ON_NOTIFY(HDN_ITEMDBLCLICK, 0, &CComicUpdater2Dlg::OnHdnItemdblclickComicList)
ON_NOTIFY(NM_DBLCLK, IDC_COMIC_LIST, &CComicUpdater2Dlg::OnNMDblclkComicList)
ON_NOTIFY(NM_CLICK, IDC_COMIC_LIST, &CComicUpdater2Dlg::OnNMClickComicList)
ON_BN_CLICKED(IDC_ANALYSE, &CComicUpdater2Dlg::OnBnClickedAnalyse)
ON_BN_CLICKED(IDC_ADD, &CComicUpdater2Dlg::OnBnClickedAdd)
ON_BN_CLICKED(IDC_DEL, &CComicUpdater2Dlg::OnBnClickedDel)
ON_BN_CLICKED(IDC_UPDATE, &CComicUpdater2Dlg::OnBnClickedUpdate)
ON_EN_CHANGE(IDC_URL, &CComicUpdater2Dlg::OnEnChangeUrl)
END_MESSAGE_MAP()


// CComicUpdater2Dlg ��Ϣ�������

BOOL CComicUpdater2Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	isInited=FALSE;
	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	
	//�ؼ�
	//�б��ʼ��
	m_comicList.InsertColumn(NAME_INDEX,_T("Name"),LVCFMT_LEFT,130);
	m_comicList.InsertColumn(LASTVOL_INDEX,_T("LastVol"),LVCFMT_LEFT,160);
	m_comicList.InsertColumn(LASTDATE_INDEX,_T("LastDate"),LVCFMT_LEFT,130);
	m_comicList.InsertColumn(UPDATEDATE_INDEX,_T("UpdateDate"),LVCFMT_LEFT,130);
	m_comicList.InsertColumn(URL_INDEX,_T("url"),LVCFMT_LEFT,300);
	m_comicList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	//��ť��ʼ��
	GetDlgItem(IDC_ANALYSE)->EnableWindow(FALSE);
	
	
	//down��ʼ��
	m_downKey.insert(map<CString,Downloader*>::value_type(_T("www.dm5.com"),new Dm5Downloader));
	m_downKey.insert(map<CString,Downloader*>::value_type(_T("manhua.178.com"),new Acg178Downloader));
    m_downKey.insert(map<CString,Downloader*>::value_type(_T("www.dmzj.com"),new Acg178Downloader));
    m_downKey.insert(map<CString,Downloader*>::value_type(_T("comic.xxbh.net"),new XxbhDownloader));
	m_downKey.insert(map<CString,Downloader*>::value_type(_T("www.manhua1.com"),new Manhua8Downloader));
	m_downKey.insert(map<CString,Downloader*>::value_type(_T("www.imanhua.com"),new IManhuaDownloader));
	

	//���ݳ�ʼ��
	if(!AfxOleInit()){
		MessageBox(_T("OLE��ʼ��ʧ��"));
		EndDialog(-1);
	}
	
	//��Ϣ�����ʼ��
	m_msgDlg.Create(MAKEINTRESOURCE(IDD_MSG),this);
	m_msgDlg.ShowWindow(SW_HIDE);

	InitDB();

	CString t;
	GetWindowText(t);
	UINT n=m_comicList.GetItemCount();
	t.Format(_T("%s   �ܹ� :%d"),t,n);
	SetWindowText(t);
	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CComicUpdater2Dlg::OnPaint()
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
HCURSOR CComicUpdater2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CComicUpdater2Dlg::OnNMCustomdrawComicList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	

	switch(pNMCD->dwDrawStage) {
		case CDDS_PREPAINT :
			*pResult= CDRF_NOTIFYITEMDRAW;
			return ;

		case CDDS_ITEMPREPAINT:
			
			/* At this point, you can change the background colors for the item
			and any subitems and return CDRF_NEWFONT. If the list-view control
			is in report mode, you can simply return CDRF_NOTIFYSUBITEMREDRAW
			to customize the item's subitems individually */
			
			*pResult= CDRF_NOTIFYSUBITEMDRAW;
			return ;
			
		case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
			{
				DWORD_PTR pn=pNMCD->dwItemSpec;
				
				if(isInited&&comicCache.at(pn).isNew){
					LPNMLVCUSTOMDRAW lplvcd = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
					lplvcd->clrText = RGB(0xff, 0x00, 0x00);
				}
		
			}
			
			
			/* This notification is received only if you are in report mode and
			returned CDRF_NOTIFYSUBITEMREDRAW in the previous step. At
			this point, you can change the background colors for the
			subitem and return CDRF_NEWFONT.*/
			
			*pResult= CDRF_NEWFONT;
			return ;
		
	}
	*pResult = 0;
}



void CComicUpdater2Dlg::OnNMDblclkComicList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMItemActivate = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	UINT n=pNMItemActivate->iItem;
	
	ComicInfo* oldInfo=&comicCache.at(n);
	m_url=m_comicList.GetItemText(n,URL_INDEX);
	GetDlgItem(IDC_ANALYSE)->EnableWindow(TRUE);
	UpdateData(FALSE);
	
	

	Downloader* down=GetDownloaderByKey(m_url);
	if(down){
		
		CVolDlg dlg;
		dlg.setOldInfo(oldInfo);
		dlg.init(down);
		dlg.DoModal();
		if(dlg.isSuccess==TRUE){	
			comicCache.at(n).isNew=FALSE;
			UpdateComicInfo(n,down,LASTDATE_MASK|LASTVOL_MASK);
			m_comicList.Invalidate();
		}
		delete down;
	}else{
		MessageBox(_T("URL����"));
	}

	*pResult = 0;
}

void CComicUpdater2Dlg::OnNMClickComicList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	UINT n=pNMItemActivate->iItem;
	m_url=m_comicList.GetItemText(n,URL_INDEX);
	GetDlgItem(IDC_ANALYSE)->EnableWindow(TRUE);
	UpdateData(FALSE);

	

	*pResult = 0;
}

void CComicUpdater2Dlg::OnBnClickedAnalyse()
{
	
	UpdateData(TRUE);

	Downloader* down=GetDownloaderByKey(m_url);
	if(down){
		CString info;
		info.Format(_T("LASTVOL[%s]VOLNUM[%d]LASTDATE[%s]"),down->GetLastVol(),down->GetVolList()->size(),down->GetLastDate());
		CVolDlg dlg;
		dlg.init(down);
		dlg.DoModal();

		delete down;
	}else{
		MessageBox(_T("URL����"));
	}
	
	
}

Downloader* CComicUpdater2Dlg::GetDownloaderByKey(CString& url){
	url.MakeLower();

	if(url.GetLength()<8||url.Left(7)!=_T("http://")){
		return NULL;
	}

	CString server= url.Mid(7,url.Find(_T("/"),8)-7);
	
	map<CString,Downloader*>::iterator it=m_downKey.find(server);

	if(it!=m_downKey.end()){
		Downloader* d=(*it).second->CreateNewInst();
		d->SetServer(server);
		if(d->Parse(url)){
			return d;
		}else{
			delete d;
			return NULL;
		}
		
	}else{
		return NULL;
	}


}
void CComicUpdater2Dlg::OnBnClickedAdd()
{
	UpdateData(TRUE);
	Downloader* down=GetDownloaderByKey(m_url);
	if(down){
		if(strlen(down->GetTitle())==0||strlen(down->GetLastDate())==0||strlen(down->GetLastVol())==0){
			CString buf;
			buf.Format(_T("һ���������Դ��󣬼���?\nTitle : %s\nLastVol : %s\nLastDate : %s"),down->GetTitle(),down->GetLastVol(),down->GetLastDate());
			if(IDCANCEL==MessageBox(buf,_T("Caution"),MB_OKCANCEL)){
				delete down;
				return;
			}
		}
		
		UINT n=m_comicList.GetItemCount();
		for(UINT i=0;i<n;i++){
			CString name=m_comicList.GetItemText(i,NAME_INDEX);
			CString comicUrl=m_comicList.GetItemText(i,URL_INDEX);
			if(name==down->GetTitle()||comicUrl==down->GetUrl()){
				CString buf;
				buf.Format(_T("��������Ѿ����ڣ�����?\nTitle : %s\nLastVol : %s\nLastDate : %s"),down->GetTitle(),down->GetLastVol(),down->GetLastDate());
				if(IDCANCEL==MessageBox(buf,_T("Caution"),MB_OKCANCEL)){
					delete down;
					return;
				}else{
					break;
				}
			}

		}

		m_pRecordset.CreateInstance(__uuidof(Recordset));
		try
		{
			m_pRecordset->Open("SELECT * FROM comic where status ='0' order by comicname",// ��ѯDemoTable���������ֶ�
				m_pConnection.GetInterfacePtr(),  // ��ȡ��ӿ��IDispatchָ��
				adOpenDynamic,
				adLockOptimistic,
				adCmdText);

			m_pRecordset->MoveLast();
			m_pRecordset->AddNew();
			m_pRecordset->PutCollect(_T("comicname"),_variant_t(down->GetTitle()));
			m_pRecordset->PutCollect(_T("lastvol"),_variant_t(down->GetLastVol()));
			m_pRecordset->PutCollect(_T("lastdate"),_variant_t(down->GetLastDate()));
			m_pRecordset->PutCollect(_T("status"),_variant_t(_T("0")));

			CTime t=CTime::GetCurrentTime();
			CString time=t.Format(_T("%Y-%m-%d %H:%M:%S"));
			m_pRecordset->PutCollect(_T("updatedate"),_variant_t(time));
			m_pRecordset->PutCollect(_T("url"),_variant_t(down->GetUrl()));
			m_pRecordset->Update();
			
			m_pRecordset->Close();
		}
		catch(_com_error e)
		{
			CString errormessage;
			errormessage.Format(_T("�������ݿ�ʧ��!\r\n������Ϣ:%s"),e.ErrorMessage());
			AfxMessageBox(errormessage);///��ʾ������Ϣ

		}


	
	}else{
		MessageBox(_T("URL����"));
	}
	
	delete down;

	InitDB();
}



VOID CComicUpdater2Dlg::InitDB(){
	comicCache.clear();
	isInited=FALSE;
	m_comicList.DeleteAllItems();

	HRESULT hr;
	_variant_t var;
	try{
		hr = m_pConnection.CreateInstance("ADODB.Connection");///����Connection����
		if(SUCCEEDED(hr))
		{
			hr = m_pConnection->Open("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=./data.mdb","","",adModeUnknown);///�������ݿ�

			m_pRecordset.CreateInstance(__uuidof(Recordset));//
			m_pRecordset->Open("SELECT * FROM comic where status='0' order by comicname",m_pConnection.GetInterfacePtr(),adOpenDynamic,adLockOptimistic,adCmdText);
			m_pRecordset->MoveFirst();
			int i=0;
			while(!m_pRecordset->adoEOF){
				ComicInfo comicInfo;

				var=m_pRecordset->GetCollect(_T("id"));
				if(var.vt!=VT_NULL){
					comicInfo.id=var;
				}

				var=m_pRecordset->GetCollect(_T("comicname"));
				if(var.vt!=VT_NULL){
					m_comicList.InsertItem(i,(_bstr_t)var);
					comicInfo.comicName=var;
				}

				var=m_pRecordset->GetCollect(_T("lastvol"));
				if(var.vt!=VT_NULL){
					m_comicList.SetItemText(i,LASTVOL_INDEX,(_bstr_t)var);
					comicInfo.lastVol=var;
				}

				var=m_pRecordset->GetCollect(_T("lastdate"));
				if(var.vt!=VT_NULL){
					m_comicList.SetItemText(i,LASTDATE_INDEX,(_bstr_t)var);
					comicInfo.lastDate=var;
				}

				var=m_pRecordset->GetCollect(_T("updatedate"));
				if(var.vt!=VT_NULL){
					m_comicList.SetItemText(i,UPDATEDATE_INDEX,(_bstr_t)var);
					comicInfo.updateDate=var;
				}

				var=m_pRecordset->GetCollect(_T("url"));
				if(var.vt!=VT_NULL){
					m_comicList.SetItemText(i,URL_INDEX,(_bstr_t)var);
					comicInfo.url=var;
				}

				comicInfo.isNew=FALSE;
				comicCache.push_back(comicInfo);

				m_pRecordset->MoveNext();
				i++;
			}

			m_pRecordset->Close();
			isInited=TRUE;

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
}
void CComicUpdater2Dlg::OnBnClickedDel()
{
	CString buf(_T("ѡ����� "));
	UINT n=m_comicList.GetItemCount();
	for(UINT i=0;i<n;i++){
		
		UINT state=m_comicList.GetItemState(i,LVIS_SELECTED );
		if(state&LVIS_SELECTED ){
			UINT id=comicCache.at(i).id;
			CString buf;
			buf.Format(_T("�Ƿ�ȷ��ɾ�� %s \nid :%d"),comicCache.at(i).comicName,id);
			if(IDCANCEL==MessageBox(buf,_T("Caution"),MB_OKCANCEL)){
				continue;
			}
			
			try{
				HRESULT hr = m_pConnection.CreateInstance("ADODB.Connection");///����Connection����
				if(SUCCEEDED(hr))
				{
					m_pConnection->Open("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=./data.mdb","","",adModeUnknown);///�������ݿ�
					
					_CommandPtr  pCommand; 
					
					pCommand.CreateInstance(__uuidof(Command)); 
					pCommand->ActiveConnection = m_pConnection;

					CString cmd;
					cmd.Format(_T("delete from comic where id=%d"),id);
					pCommand->CommandText=_bstr_t(cmd);
					pCommand->Execute(NULL,NULL,adCmdText);
					
					
				}


			}catch(_com_error e)///��׽�쳣
			{
				CString errormessage;
				errormessage.Format(_T("�������ݿ�ʧ��!\r\n������Ϣ:%s"),e.ErrorMessage());
				AfxMessageBox(errormessage);///��ʾ������Ϣ
			}

		}
		
	
	}

	InitDB();
	
}

BOOL CComicUpdater2Dlg::UpdateComicInfo( UINT n ,Downloader* down,UINT mask)
{
	CString t=CTime::GetCurrentTime().Format(_T("%Y-%m-%d %H:%M:%S"));
	
	ComicInfo* oldInfo=&comicCache.at(n);
	
	
	if(mask&LASTVOL_MASK){
		oldInfo->lastVol=down->GetLastVol();
		m_comicList.SetItemText(n,LASTVOL_INDEX,down->GetLastVol());
	}
	if(mask&LASTDATE_MASK){
		oldInfo->lastDate=down->GetLastDate();
		m_comicList.SetItemText(n,LASTDATE_INDEX,down->GetLastDate());
	}
	//if(mask&UPDATEDATE_MASK){
	oldInfo->updateDate=t;
	m_comicList.SetItemText(n,UPDATEDATE_INDEX,t);
	
	CString sql;
	sql.Format(_T("select * from comic where id=%d"),oldInfo->id);
	
	m_pRecordset.CreateInstance(__uuidof(Recordset));
	try
	{
		m_pRecordset->Open(_variant_t(sql),// ��ѯDemoTable���������ֶ�
			m_pConnection.GetInterfacePtr(),  // ��ȡ��ӿ��IDispatchָ��
			adOpenDynamic,
			adLockOptimistic,
			adCmdText);

		m_pRecordset->PutCollect(_T("updatedate"),_variant_t(t));

		if(mask&LASTVOL_MASK){
			m_pRecordset->PutCollect(_T("lastvol"),_variant_t(down->GetLastVol()));
		}
		
		if(mask&LASTDATE_MASK){
			m_pRecordset->PutCollect(_T("lastdate"),_variant_t(down->GetLastDate()));
		}
		
		m_pRecordset->Update();

		m_pRecordset->Close();
	}
	catch(_com_error e)
	{
		CString errormessage;
		errormessage.Format(_T("�������ݿ�ʧ��!\r\n������Ϣ:%s"),e.ErrorMessage());
		TRACE(errormessage);

		return FALSE;
	}
	return TRUE;
}


UINT UpdateComicThreadProc( LPVOID pParam ){


	CComicUpdater2Dlg* comicDlg=reinterpret_cast<CComicUpdater2Dlg*>(pParam);
	comicDlg->m_msgDlg.ShowWindow(SW_SHOW);
	comicDlg->EnableWindow(FALSE);

	CListCtrl* m_comicList=&comicDlg->m_comicList;

	comicDlg->isInited=FALSE;
	comicDlg->GetDlgItem(IDC_UPDATE)->EnableWindow(FALSE);
	UINT n=m_comicList->GetItemCount();

	for(UINT i=0;i<n;i++){

		if(comicDlg->m_msgDlg.isTerminated){
            break;
			//return 0;
		}
        
        Downloader* down=NULL;
        try{
            Sleep(200);

            CString title=m_comicList->GetItemText(i,NAME_INDEX);
            CString comicUrl=m_comicList->GetItemText(i,URL_INDEX);
            CString oldLastVol=m_comicList->GetItemText(i,LASTVOL_INDEX);

            CString buf;
            buf.Format(_T("���ڸ���\nTitle : %s\n URL : %s"),title,comicUrl);
            comicDlg->m_msgDlg.SetDlgItemText(IDC_MSG,buf);

            down=comicDlg->GetDownloaderByKey(comicUrl);

            if(down){
                if(strcmp(oldLastVol,down->GetLastVol())==0){
                    delete down;
                    down=NULL;
                    continue;
                }else{
                    comicDlg->comicCache.at(i).isNew=TRUE;
                    //comicDlg->UpdateComicInfo(i,down,NULL);
                }

                delete down;
                down=NULL;
            }

            
        }catch(CException* e){
            TCHAR buf[500]={0};
            UINT n=0;
            e->GetErrorMessage(buf,500,&n);
            CString msg;
            msg.Format("%s,%d",buf,n);
            AfxMessageBox(msg);
            e->Delete();
            
        }
        if(down){
            delete down;
            down=NULL;
        }

		
	}

	comicDlg->isInited=TRUE;
    
	comicDlg->GetDlgItem(IDC_UPDATE)->EnableWindow(TRUE);
	m_comicList->Invalidate();


	comicDlg->m_msgDlg.ShowWindow(SW_HIDE);
    comicDlg->m_msgDlg.isTerminated=FALSE;
	comicDlg->EnableWindow(TRUE);
	return 0;

}
void CComicUpdater2Dlg::OnBnClickedUpdate()
{
	AfxBeginThread(UpdateComicThreadProc,this);\

	//UpdateComicThreadProc(this);
}

void CComicUpdater2Dlg::OnEnChangeUrl()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	UpdateData(TRUE);

	if(m_url.IsEmpty()){
		GetDlgItem(IDC_ANALYSE)->EnableWindow(FALSE);
	}else{
		GetDlgItem(IDC_ANALYSE)->EnableWindow(TRUE);
	}
}
