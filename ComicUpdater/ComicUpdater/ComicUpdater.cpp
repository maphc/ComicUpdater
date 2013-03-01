// ComicUpdater.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"

#define UM_MY WM_USER+1

struct MyParam{
	HWND hwnd;
	LPARAM lParam;

};

struct OldComicInfo{
	LPCTSTR oldLastVol;
	Dm5Downloader* down;
};
struct ComicInfo{
	TCHAR title[100];
	TCHAR lastVol[100];
	TCHAR lastDate[100];
	TCHAR updateDate[100];
	TCHAR url[100];

};

const TCHAR CONFIG[]=_T("./config.ini");
Dm5Downloader* down;
HINSTANCE hInst;
BOOL isInited=FALSE;
vector<BOOL> isNew;

INT_PTR CALLBACK DlgProc( HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
INT_PTR CALLBACK VolProc( HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
INT_PTR CALLBACK MsgProc( HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
VOID initComicList(HWND hComic);
VOID initVolList(HWND hwndDlg);
BOOL doAnalyse(HWND hwndDlg);
VOID setListViewText(HWND hComic,LPTSTR buf,UINT i,UINT j);
VOID getListViewText(HWND hComic,LPTSTR buf,UINT i,UINT j);
VOID writeComicListToFile(ComicInfo& info);
VOID writeAllInfoToFile(HWND hComic);
static const UINT TITLE_INDEX=0;
static const UINT LASTVOL_INDEX=1;
static const UINT LASTDATE_INDEX=2;
static const UINT UPDATEDATE_INDEX=3;
static const UINT URL_INDEX=4;


DWORD WINAPI ThreadProc(LPVOID lpParameter);
//
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	hInst=hInstance;

	DialogBox(hInstance,MAKEINTRESOURCE(IDD_MAIN),NULL,DlgProc);

	if(down){
		delete down;
	}
	return 0;
}


INT_PTR CALLBACK DlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam,LPARAM lParam){
	int wmId=LOWORD(wParam);
	int wmEvent=HIWORD(wParam);
	HWND hComic=GetDlgItem(hwndDlg,IDC_COMIC_LIST);
	
	
	
	switch(uMsg){
		case WM_INITDIALOG:
			{
				isInited=FALSE;
				LVCOLUMN lv={0};
				
				lv.mask=LVCF_WIDTH|LVCF_TEXT;

				lv.pszText=_T("名称");
				lv.cx=120;
				ListView_InsertColumn(hComic,TITLE_INDEX,&lv);
				lv.pszText=_T("最后访问VOL");
				lv.cx=160;
				ListView_InsertColumn(hComic,LASTVOL_INDEX,&lv);
				lv.pszText=_T("最后更新时间");
				lv.cx=120;
				ListView_InsertColumn(hComic,LASTDATE_INDEX,&lv);
				lv.pszText=_T("最后访问时间");
				lv.cx=120;
				ListView_InsertColumn(hComic,UPDATEDATE_INDEX,&lv);
				lv.pszText=_T("地址");
				lv.cx=160;
				ListView_InsertColumn(hComic,URL_INDEX,&lv);

				ListView_SetExtendedListViewStyle (hComic,LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES );

				initComicList(hComic);
				
				UINT n=ListView_GetItemCount(hComic);
				for(UINT i=0;i<n;i++){
					isNew.push_back(FALSE);
				}
				isInited=TRUE;
				
			}
			
			break;

		case WM_COMMAND:
			switch(wmId){
				case IDC_ANALYSE:
					{
						if(down){
							delete down;
							down=NULL;
						}

						TCHAR url[1024]={0};

						GetDlgItemText(hwndDlg,IDC_URL,url,1024);
						if(strlen(url)==0){
							MessageBox(hwndDlg,"请输入地址","错误",MB_OK);
							return FALSE;
						}

						
						Dm5Downloader* down= new Dm5Downloader(url);
						
						if(!doAnalyse(hwndDlg)){
							return TRUE;
						}
						
						DialogBoxParam(hInst,MAKEINTRESOURCE(IDD_VOL),hwndDlg,VolProc,(LPARAM)down);
						delete down ;
						down =NULL;
					}
					
					break;
				case IDC_ADD:
					{
						TCHAR section[100]={0};
						UINT n=ListView_GetItemCount(hComic);
						if(down){
							delete down;
							down=NULL;
						}
						TCHAR url[1024]={0};

						GetDlgItemText(hwndDlg,IDC_URL,url,1024);
						if(strlen(url)==0){
							MessageBox(hwndDlg,"请输入地址","错误",MB_OK);
							return FALSE;
						}


						down=new Dm5Downloader(url);
						if(!doAnalyse(hwndDlg)){
							
							return TRUE;
						}
						for(UINT i=0;i<n;i++){
							LVITEM item={0};
							item.iItem=i;
							item.mask=LVIF_TEXT;
							item.cchTextMax=100;
							item.pszText=section;
							ListView_GetItem(hComic,&item);
							if(strcmp(section,down->getTitle())==0){
								if(IDCANCEL==MessageBox(hwndDlg,_T("已经存在此项，确定重新添加吗?"),_T("Caution"),MB_OKCANCEL)){
									delete down;
									down=NULL;
									return TRUE;
								}else{
									ListView_DeleteItem(hComic,i);
									break;
								}
								
							}

						}
						
						LVITEM item={0};
						item.mask=LVIF_TEXT;
						item.iItem=TITLE_INDEX;

						item.pszText=(LPSTR)down->getTitle();
						ListView_InsertItem(hComic,&item);

						item.pszText=(LPSTR)down->getLastVol();
						item.iSubItem=LASTVOL_INDEX;
						ListView_SetItem(hComic,&item);

						item.pszText=(LPSTR)down->getLastDate();
						item.iSubItem=LASTDATE_INDEX;
						ListView_SetItem(hComic,&item);

						item.pszText=(LPSTR)down->getUrl();
						item.iSubItem=URL_INDEX;
						ListView_SetItem(hComic,&item);

						WritePrivateProfileString(down->getTitle(),_T("LastVol"),down->getLastVol(),CONFIG);
						WritePrivateProfileString(down->getTitle(),_T("LastDate"),down->getLastDate(),CONFIG);
						WritePrivateProfileString(down->getTitle(),_T("UpdateDate"),down->getLastDate(),CONFIG);
						WritePrivateProfileString(down->getTitle(),_T("URL"),down->getUrl(),CONFIG);
						
						isNew.insert(isNew.begin(),FALSE);

						delete down;
						down=NULL;
					}
					
					break;
				case IDC_DEL:
					{
						
						UINT n=ListView_GetItemCount(hComic);
						
						for(UINT i=0;i<n;i++){
							LVITEM item={0};
							TCHAR section[100]={0};
							item.iItem=i;
							item.mask=LVIF_STATE |LVIF_TEXT;
							item.stateMask=LVIS_SELECTED;
							item.pszText=section;
							item.cchTextMax=100;
							ListView_GetItem(hComic,&item);
							if(item.state==LVIS_SELECTED){
								WritePrivateProfileString(section,NULL,NULL,CONFIG);
								isNew.erase(isNew.begin()+i);

							}

						}

						isInited=FALSE;
						initComicList(hComic);
						isInited=TRUE;
	
					}
					break;
				case IDC_UPDATE:
					{
						EnableWindow(GetDlgItem(hwndDlg,IDC_UPDATE),FALSE);
						UINT n=ListView_GetItemCount(hComic);

						for(UINT i=0;i<n;i++){
							LVITEM item={0};
							TCHAR url[100]={0},lastVol[100]={0};
							item.iItem=i;
							item.iSubItem=URL_INDEX;
							item.mask=LVIF_TEXT;
							item.pszText=url;
							item.cchTextMax=100;
							ListView_GetItem(hComic,&item);

							item.iSubItem=LASTVOL_INDEX;
							item.pszText=lastVol;
							ListView_GetItem(hComic,&item);


							if(down){
								delete down;
								down=NULL;
							}
							
							down=new Dm5Downloader(url);
							down->init();
							if(strcmp(down->getLastVol(),lastVol)==0){
								isNew.at(i)=FALSE;
							}else{

								//setListViewText(hComic,down->getLastVol(),i,LASTVOL_INDEX);
								//setListViewText(hComic,down->getLastDate(),i,LASTDATE_INDEX);

								isNew.at(i)=TRUE;
								
								/*ComicInfo info={0};
								UINT l=strlen(down->getTitle());
								strcpy_s(info.title,l+1,down->getTitle());
								strcpy_s(info.lastVol,strlen(down->getLastVol())+1,down->getLastVol());
								strcpy_s(info.lastDate,strlen(down->getLastDate())+1,down->getLastDate());
								strcpy_s(info.updateDate,time.GetLength()+1,time);
								strcpy_s(info.url,strlen(down->getUrl())+1,down->getUrl());*/
								//writeComicListToFile(info);
								
							}

							CTime ctime=CTime::GetCurrentTime();
							CString time=ctime.Format(_T("%Y-%m-%d %H:%M:%S"));
							setListViewText(hComic,(LPTSTR)(LPCTSTR)time,i,UPDATEDATE_INDEX);
							WritePrivateProfileString(down->getTitle(),_T("UpdateDate"),time,CONFIG);


							if(down){
								delete down;
								down=NULL;
							}

						}
						EnableWindow(GetDlgItem(hwndDlg,IDC_UPDATE),TRUE);
						RECT rect;
						GetWindowRect(hComic,&rect);
						InvalidateRect(hComic,NULL,FALSE);
					}
					break;

			}
			break;
		case WM_NOTIFY:
			
			
			if(wmId==IDC_COMIC_LIST){
				NMHDR* nmhdr=((LPNMHDR)lParam);
				if(nmhdr->code==NM_DBLCLK){
					TCHAR url[100]={0};
					UINT row=((LPNMLISTVIEW) lParam)->iItem;
					LVITEM item={0};
					item.iItem=((LPNMLISTVIEW) lParam)->iItem;
					item.iSubItem=URL_INDEX;
					item.mask=LVIF_TEXT;
					item.cchTextMax=100;
					item.pszText=url;
					ListView_GetItem(hComic,&item);
					SetDlgItemText(hwndDlg,IDC_URL,url);
					
					TCHAR oldLastVol[100]={0};
					getListViewText(hComic,oldLastVol,row,LASTVOL_INDEX);

					CTime ctime=CTime::GetCurrentTime();
					CString time=ctime.Format(_T("%Y-%m-%d %H:%M:%S"));
					setListViewText(hComic,(LPTSTR)(LPCTSTR)time,row,UPDATEDATE_INDEX);
					
					down= new Dm5Downloader(url);
					down->init();

					if(!doAnalyse(hwndDlg)){
						return TRUE;
					}
					setListViewText(hComic,(LPTSTR)down->getLastVol(),row,LASTVOL_INDEX);
					setListViewText(hComic,(LPTSTR)down->getLastDate(),row,LASTDATE_INDEX);
					
					ComicInfo info={0};
					UINT l=strlen(down->getTitle());
					strcpy_s(info.title,l+1,down->getTitle());
					strcpy_s(info.lastVol,strlen(down->getLastVol())+1,down->getLastVol());
					strcpy_s(info.lastDate,strlen(down->getLastDate())+1,down->getLastDate());
					strcpy_s(info.updateDate,time.GetLength()+1,time);
					strcpy_s(info.url,strlen(down->getUrl())+1,down->getUrl());
					writeComicListToFile(info);
					
					OldComicInfo oci={0};
					oci.down=down;
					oci.oldLastVol=oldLastVol;

					DialogBoxParam(hInst,MAKEINTRESOURCE(IDD_VOL),hwndDlg,VolProc,(LPARAM)&oci);

					if(down){
						delete down ;
						down =NULL;
					}
					

					isNew.at(row)=FALSE;

				}else{
					LPNMLISTVIEW  pnm    = (LPNMLISTVIEW)lParam;
					
					///if(pnm->iItem==0)
					switch (pnm->hdr.code){
						
						case NM_CUSTOMDRAW:

						LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)lParam;

						switch(lplvcd->nmcd.dwDrawStage) {

							case CDDS_PREPAINT :
								
								SetWindowLong(hwndDlg,DWL_MSGRESULT,(LPARAM)CDRF_NOTIFYITEMDRAW);
								return TRUE;
								
								return CDRF_NOTIFYITEMDRAW;

							case CDDS_ITEMPREPAINT:
								
								/* At this point, you can change the background colors for the item
								and any subitems and return CDRF_NEWFONT. If the list-view control
								is in report mode, you can simply return CDRF_NOTIFYSUBITEMREDRAW
								to customize the item's subitems individually */
								
								SetWindowLong(hwndDlg,DWL_MSGRESULT,(LPARAM)CDRF_NOTIFYSUBITEMDRAW);
								return TRUE;

								//return CDRF_NOTIFYSUBITEMDRAW;
								//  or return CDRF_NOTIFYSUBITEMREDRAW;

							case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
								{
									
									DWORD_PTR pn=lplvcd->nmcd.dwItemSpec;
									
									if(isInited&&isNew.at(pn)){
										lplvcd->clrText = RGB(0xff, 0x00, 0x00);
									}
									

					
								}
								
								
								/* This notification is received only if you are in report mode and
								returned CDRF_NOTIFYSUBITEMREDRAW in the previous step. At
								this point, you can change the background colors for the
								subitem and return CDRF_NEWFONT.*/
								
								//return CDRF_NEWFONT; 
								SetWindowLong(hwndDlg,DWL_MSGRESULT,(LPARAM)CDRF_NEWFONT);
								return TRUE;

						}

					}

				}

			}
			
			break;
		case WM_CLOSE:
			EndDialog(hwndDlg,0);
			break;
	}

	
	return FALSE;
}

VOID initComicList(HWND hComic){
	
	ListView_DeleteAllItems(hComic);
	UINT len=0,p=0,i=0;
	TCHAR comicBuf[4096]={0};

	GetPrivateProfileString(NULL,NULL,NULL,comicBuf,4096,CONFIG);
	
	while(*(comicBuf+p)!='\0'&&*(comicBuf+p+1)!='\0'){

		TCHAR section[100]={0},buf[100]={0};

		len=strlen(comicBuf+p);
		if(len==0){
			break;
		}
		strcpy_s(section,len+1,comicBuf+p);


		LVITEM item={0};
		item.mask=LVIF_TEXT;
		item.iItem=TITLE_INDEX;

		item.pszText=section;
		ListView_InsertItem(hComic,&item);

		GetPrivateProfileString(section,_T("LastVol"),NULL,buf,100,CONFIG);
		item.pszText=buf;
		item.iSubItem=LASTVOL_INDEX;
		ListView_SetItem(hComic,&item);

		GetPrivateProfileString(section,_T("LastDate"),NULL,buf,100,CONFIG);
		item.pszText=buf;
		item.iSubItem=LASTDATE_INDEX;
		ListView_SetItem(hComic,&item);

		GetPrivateProfileString(section,_T("UpdateDate"),NULL,buf,100,CONFIG);
		item.pszText=buf;
		item.iSubItem=UPDATEDATE_INDEX;
		ListView_SetItem(hComic,&item);

		GetPrivateProfileString(section,_T("Url"),NULL,buf,100,CONFIG);
		item.pszText=buf;
		item.iSubItem=URL_INDEX;
		ListView_SetItem(hComic,&item);

		p+=len+1;
		i++;
		
		
	}

}

VOID writeComicListToFile(ComicInfo& info){
	WritePrivateProfileString(info.title,_T("LastVol"),info.lastVol,CONFIG);
	WritePrivateProfileString(info.title,_T("LastDate"),info.lastDate,CONFIG);
	WritePrivateProfileString(info.title,_T("UpdateDate"),info.updateDate,CONFIG);
	WritePrivateProfileString(info.title,_T("URL"),info.url,CONFIG);

}

VOID writeAllInfoToFile(HWND hComic){
	UINT n=ListView_GetItemCount(hComic);

	for(UINT i=0;i<n;i++){
		ComicInfo info={0};
		LVITEM item={0};
		item.iItem=i;
		item.mask=LVIF_TEXT;
		item.pszText=info.title;
		item.cchTextMax=100;
		ListView_GetItem(hComic,&item);
		
		item.iSubItem=LASTVOL_INDEX;
		item.pszText=info.lastVol;
		ListView_GetItem(hComic,&item);

		item.iSubItem=LASTDATE_INDEX;
		item.pszText=info.lastDate;
		ListView_GetItem(hComic,&item);

		item.iSubItem=UPDATEDATE_INDEX;
		item.pszText=info.updateDate;
		ListView_GetItem(hComic,&item);

		item.iSubItem=URL_INDEX;
		item.pszText=info.url;
		ListView_GetItem(hComic,&item);
		

		writeComicListToFile(info);

	}


}

BOOL doAnalyse(HWND hwndDlg){
	
	
	
	if(!down->init()){
		MessageBox(hwndDlg,_T("链接错误"),_T("错误"),MB_OK);
		
		return FALSE;
	}
	
	if(!down->getTitle()||!down->getLastDate()||!down->getLastVol()){
		TCHAR buf[300]={0};
		wsprintf(buf,_T("一个或多个内容获取错误，是否继续?\nTitle :%s\nLastDate :%s\nLastVol :%s"),down->getTitle(),down->getLastDate(),down->getLastVol());
		if(IDCANCEL==MessageBox(hwndDlg,buf,_T("Caution"),MB_OKCANCEL)){
			return FALSE;
		}
	}
	
	return TRUE;
	
	
}

INT_PTR CALLBACK VolProc( HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam){
	int wmId=LOWORD(wParam);
	int wmEvent=HIWORD(wParam);
	
	
	
	switch(uMsg){
		case WM_INITDIALOG:
		
			{

				OldComicInfo* oci=(OldComicInfo*)lParam;
				HWND hListHwnd=GetDlgItem(hwndDlg,IDC_VOL_LIST);
				down=oci->down;

				map<CString,CString>* volCache=down->getVolCache();

				SetWindowText(hwndDlg,down->getTitle());
				initVolList(hListHwnd);
				
				BOOL isNewVol=FALSE;
				if(!volCache->empty()){
					int i=0;
					SetDlgItemInt(hwndDlg,IDC_VOL_COUNT,volCache->size(),FALSE);

					for(map<CString,CString>::iterator it=volCache->begin();it!=volCache->end();it++,i++){
						LVITEM item={0};
						TCHAR buf[100]={0};

						item.iItem=0;
						item.mask=LVIF_TEXT;

						wsprintf (buf,"%s",(*it).second);
						item.pszText=buf;
						ListView_InsertItem(hListHwnd,&item);

						wsprintf (buf,"%s",(*it).first);
						item.pszText=buf;
						item.iSubItem=1;
						ListView_SetItem(hListHwnd,&item);


						if(isNewVol){
							ListView_SetCheckState(hListHwnd,0,TRUE);
						}

						if(!isNewVol&&strcmp((LPTSTR)(LPCTSTR)(*it).second,oci->oldLastVol)==0){
							
							isNewVol=TRUE;

						}




					}

				}

			}
			
			
			break;

		case WM_COMMAND:
			switch(wmId){
				case IDC_CHECK:
					{
						HWND hListHwnd=GetDlgItem(hwndDlg,IDC_VOL_LIST);
						UINT n=ListView_GetItemCount(hListHwnd);
						for(UINT i=0;i<n;i++){
							ListView_SetCheckState(hListHwnd,i,TRUE);
						}
					}
					break;
				case IDC_DESELECT:
					{
						HWND hListHwnd=GetDlgItem(hwndDlg,IDC_VOL_LIST);
						UINT n=ListView_GetItemCount(hListHwnd);
						for(UINT i=0;i<n;i++){
							ListView_SetCheckState(hListHwnd,i,!ListView_GetCheckState(hListHwnd,i));
						}
					}
					break;

				case IDC_DE_ALL:
					{
						HWND hListHwnd=GetDlgItem(hwndDlg,IDC_VOL_LIST);
						UINT n=ListView_GetItemCount(hListHwnd);
						for(UINT i=0;i<n;i++){
							ListView_SetCheckState(hListHwnd,i,FALSE);
						}
					}
					break;
				case IDOK:

					{
						HWND hListHwnd=GetDlgItem(hwndDlg,IDC_VOL_LIST);
						//ListView_GetCheckState ListView_SetCheckState. ListView_GetItemCount
						HWND hMsgDlg=CreateDialog(hInst,MAKEINTRESOURCE(IDD_MSG),hwndDlg,MsgProc);
						
						UINT n=ListView_GetItemCount(hListHwnd);
						for(UINT i=0;i<n;i++){
							if(ListView_GetCheckState(hListHwnd,i)){
								EnableWindow(hwndDlg,FALSE);
								TCHAR strid[100]={0},volName[100]={0};

								LVITEM item;
								ZeroMemory(&item,sizeof(item));
								item.mask=LVIF_TEXT;
								item.iItem=i;


								item.pszText=volName;
								item.cchTextMax=100;
								ListView_GetItem(hListHwnd,&item);

								item.iSubItem=1;
								item.pszText=strid;
								item.cchTextMax=100;
								ListView_GetItem(hListHwnd,&item);


								CString* ps=new CString(strid);
								
								SendMessage(hMsgDlg,UM_MY,0,(LPARAM)ps);
								ListView_SetCheckState(hListHwnd,i,FALSE);
								
							}
						}

						
					}
					
					break;

				case IDCANCEL:
					EndDialog(hwndDlg,0);

					break;
			}
			break;
		
		case WM_CLOSE:
			EndDialog(hwndDlg,0);
			break;
	}

	return FALSE;


}

VOID initVolList(HWND lHwnd ){

	LVCOLUMN lv;
	ZeroMemory(&lv,sizeof(lv));
	lv.mask=LVCF_WIDTH|LVCF_TEXT;
	lv.cx=160;
	lv.pszText="列表";
	ListView_InsertColumn(lHwnd,0,&lv);
	lv.pszText="strid";
	lv.cx=80;
	ListView_InsertColumn(lHwnd,1,&lv);

	ListView_SetExtendedListViewStyle (lHwnd,LVS_EX_CHECKBOXES|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES );

}

DWORD WINAPI ThreadProc(LPVOID lpParameter){
	MyParam* p=(MyParam* )lpParameter;

	CString volName,savePath;
	
	

	HWND hwndDlg= p->hwnd;
	CString* pStrid=(CString*)p->lParam;
	TCHAR buf[300]={0};

	wsprintf(buf,"正在获取图片地址...%s",*pStrid);
	SetDlgItemText(hwndDlg,IDC_MSG,buf);
	vector<CString>  picList=down->getPicUrls(*pStrid);

	volName=(*down->getVolCache()->find(*pStrid)).second;

	wsprintf(buf,"正在创建路径...%s",volName);
	SetDlgItemText(hwndDlg,IDC_MSG,buf);
	down->createNecessaryPath(volName);

	savePath=down->getSavePath(*pStrid,volName);
	UINT total=picList.size();
	for(UINT i=0;i<total;i++){
		CString url=picList.at(i);
		wsprintf(buf,"正在下载 %s(%d/%d) -> %s",volName,i+1,total,url);
		down->savePicAsFile(url,savePath);
		SetDlgItemText(hwndDlg,IDC_MSG,buf);
	}
	
	
	ShowWindow(hwndDlg,SW_HIDE);
	EnableWindow(GetParent(hwndDlg),TRUE);
	
	delete pStrid;
	delete p;
	return 0;
}
INT_PTR CALLBACK MsgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam,LPARAM lParam){
	int wmId=LOWORD(wParam);
	int wmEvent=HIWORD(wParam);
	MyParam* p;
	

	switch(uMsg){
		case WM_INITDIALOG:
			break;
		case UM_MY:
			ShowWindow(hwndDlg,SW_SHOW);
			
			p=new MyParam;
			p->hwnd=hwndDlg;
			p->lParam=lParam;
			
			CloseHandle(CreateThread(NULL,0,ThreadProc,p,NULL,NULL));
			
			return TRUE;

		case WM_COMMAND:
			break;
		case WM_CLOSE:
			EndDialog(hwndDlg,0);
			break;
	}


	return FALSE;
}

VOID setListViewText(HWND hComic,LPTSTR buf,UINT i,UINT j){
	LVITEM item={0};
	item.mask=LVIF_TEXT;
	item.iItem=i;
	item.iSubItem=j;
	item.pszText=buf;
	

	
	ListView_SetItem(hComic,&item);
}

VOID getListViewText(HWND hComic,LPTSTR buf,UINT i,UINT j){
	LVITEM item={0};
	item.mask=LVIF_TEXT;
	item.iItem=i;
	item.iSubItem=j;
	item.pszText=buf;
	item.cchTextMax=100;

	ListView_GetItem(hComic,&item);
}