// VolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ComicUpdater2.h"
#include "VolDlg.h"


// CVolDlg 对话框

IMPLEMENT_DYNAMIC(CVolDlg, CDialog)

CVolDlg::CVolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVolDlg::IDD, pParent)
{
	m_down=NULL;
	m_oldInfo=NULL;
	
	
}

CVolDlg::~CVolDlg()
{
	m_msgDlg.DestroyWindow();
}

void CVolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VOL_LIST, m_volList);
}


BEGIN_MESSAGE_MAP(CVolDlg, CDialog)
	ON_BN_CLICKED(IDC_DOWN, &CVolDlg::OnBnClickedDown)
	ON_BN_CLICKED(IDC_DE_ALL, &CVolDlg::OnBnClickedDeAll)
    ON_BN_CLICKED(IDC_SE_ALL, &CVolDlg::OnBnClickedSeAll)
END_MESSAGE_MAP()

VOID CVolDlg::init( Downloader* down)
{
	m_down=down;
}


// CVolDlg 消息处理程序


BOOL CVolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	//不要再构造函数中创建，构造函数中还没有this指针
	m_msgDlg.Create(MAKEINTRESOURCE(IDD_MSG),this);
	m_msgDlg.ShowWindow(SW_HIDE);
	
	m_volList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES);
	m_volList.InsertColumn(0,_T("名称"),LVCFMT_RIGHT,180);
	
	vector<CString>* list=m_down->GetVolList();
	//SetDlgItemInt(IDC_COUNT,list->size());
	CString volTitle;
	volTitle.Format(_T("%s  总共 :%d"),m_down->GetTitle(),list->size());
	SetWindowText(volTitle);

	CString buf;
	buf.Format(_T("%s  %s"),m_down->GetLastVol(),m_down->GetLastDate());
	SetDlgItemText(IDC_INFO,buf);

	BOOL isNew=FALSE;
	
	UINT n=list->size();
	for(UINT i=0;i<n;i++){
		m_volList.InsertItem(0,list->at(n-i-1));
		
		if(isNew){
			m_volList.SetCheck(0,TRUE);
		}

		if(m_oldInfo&&(strcmp(m_oldInfo->lastVol,list->at(n-i-1))==0)){
			isNew=TRUE;
			
		}
		
	}

	if(!isNew){
		m_volList.SetCheck(0,TRUE);
	}

	isSuccess=TRUE;
	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

VOID CVolDlg::setOldInfo( ComicInfo* oldInfo )
{
	m_oldInfo=oldInfo;
}



UINT DownPicThreadProc( LPVOID pParam ){

	
	CVolDlg* volDlg=reinterpret_cast<CVolDlg*>(pParam);
	volDlg->m_msgDlg.ShowWindow(SW_SHOW);
	volDlg->EnableWindow(FALSE);
	
	CListCtrl* m_volList=&volDlg->m_volList;
	Downloader* down=volDlg->m_down;

	CString buf;
	int count=m_volList->GetItemCount();

	for(int i=count;--i>=0;){
	//for(UINT i=0;i<count;i++){
		BOOL isChecked=m_volList->GetCheck(i);
		if(isChecked){
			CString name=m_volList->GetItemText(i,0);
			

			CString logName;
			CTime starttime=CTime::GetCurrentTime();
			CString time=starttime.Format(_T("%Y-%m-%d %H:%M:%S"));

			TCHAR tchar[1024]={0};
			GetCurrentDirectory(1023,tchar);
			logName.Format(_T("%s/down/%s/log.txt"),tchar,down->GetTitle());
			CString logtext=time+_T("  ")+down->GetUrl()+name+_T("  ");
			
            try
            {
                CString volUrl=(*down->GetVolCache()->find(name)).second;

                buf.Format(_T("正在获取地址 :%s"),name);
                volDlg->m_msgDlg.SetDlgItemText(IDC_MSG,buf);	

				CString zipFileName;
				if(name.GetAt(0)>=_T('0')&&name.GetAt(0)<=_T('9')){
					zipFileName.Format(_T("%s/down/%s/第%s.zip"),tchar,down->GetTitle(),name);
					
				}else{
					zipFileName.Format(_T("%s/down/%s/%s.zip"),tchar,down->GetTitle(),name);
				}
				
				if(GetFileAttributes(zipFileName)!=INVALID_FILE_ATTRIBUTES){
					if(!DeleteFile(zipFileName)){
						AfxMessageBox(_T("删除文件出错"));
						return 0;
					}
				}
				if(volDlg->m_msgDlg.isTerminated){
					volDlg->m_msgDlg.ShowWindow(SW_HIDE);
					volDlg->EnableWindow(TRUE);
					volDlg->isSuccess=FALSE;
					return 0;
				}
                vector<CString> picList=down->GetPicUrls(volUrl);
				if(picList.size()==0){
					volDlg->m_msgDlg.ShowWindow(SW_HIDE);
					volDlg->EnableWindow(TRUE);
					volDlg->isSuccess=FALSE;
					return 0;
				}
				if(volDlg->m_msgDlg.isTerminated){
					volDlg->m_msgDlg.ShowWindow(SW_HIDE);
					volDlg->EnableWindow(TRUE);
					volDlg->isSuccess=FALSE;
					return 0;
				}
                buf.Format(_T("正在创建路径 :%s"),name);
                volDlg->m_msgDlg.SetDlgItemText(IDC_MSG,buf);	
                CString path=down->CreateNecessaryPath(name);
                UINT n=picList.size();
				for(UINT j=0;j<n;j++){

					if(volDlg->m_msgDlg.isTerminated){
						volDlg->m_msgDlg.ShowWindow(SW_HIDE);
						volDlg->EnableWindow(TRUE);
						volDlg->isSuccess=FALSE;
						return 0;
					}

                    if(Downloader::USE_REAL_NAME){
                        down->SavePicAsFile(volUrl,j,picList.at(j),path);
                    }else{
                        CString fullName;
                        CString extName=picList.at(j).Mid(picList.at(j).ReverseFind(_T('.')));
                        fullName.Format(_T("%s/%03d%s"),path,j,extName);
                        down->SavePicAsFile(volUrl,j,picList.at(j),fullName);
                    }
                    
                    buf.Format(_T("正在下载 (%d/%d):\n %s\n到 :\n%s"),j+1,n,picList.at(j),path);
                    volDlg->m_msgDlg.SetDlgItemText(IDC_MSG,buf);	

                }

				
                buf.Format(_T("下载完成"));
                volDlg->m_msgDlg.SetDlgItemText(IDC_MSG,buf);	

                m_volList->SetCheck(i,FALSE);

				logtext+=_T("SUCCESS");
            }
            catch (CException* e)
            {
                TCHAR buf[500]={0};
                UINT n=0;
                e->GetErrorMessage(buf,500,&n);
                CString msg;
                msg.Format("%s,%d",buf,n);
                AfxMessageBox(msg);
                e->Delete();
				volDlg->isSuccess=FALSE;
                logtext+=_T("FAIL");
            }
			
			ofstream fout(logName,ios_base::out|ios_base::app);
			fout<<logtext<<endl;
			fout.close();

			
		}

	}
	
	
	volDlg->m_msgDlg.ShowWindow(SW_HIDE);
	volDlg->EnableWindow(TRUE);
	AfxEndThread(0,FALSE);
	return 0;

}


void CVolDlg::OnBnClickedDown()
{
	
		
	AfxBeginThread(DownPicThreadProc,this);


}

void CVolDlg::OnBnClickedDeAll()
{
	UINT n=m_volList.GetItemCount();
	for(UINT i=0;i<n;i++){
		m_volList.SetCheck(i,FALSE);
	}
}

void CVolDlg::OnBnClickedSeAll()
{
    UINT n=m_volList.GetItemCount();
    for(UINT i=0;i<n;i++){
        m_volList.SetCheck(i,TRUE);
    }
}