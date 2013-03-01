#include "stdafx.h"

extern PEUtil util;
extern HINSTANCE hInst;

INT_PTR CALLBACK ImportProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam){
	WORD itmId=LOWORD(wParam);
	WORD notifyId=HIWORD(wParam);
	switch(uMsg){
		case WM_INITDIALOG:
			InitImportDlg(hwndDlg);
			InitImportData(hwndDlg);
			return TRUE;
		case WM_NOTIFY:
			if(itmId==IDC_IMPORT_LIST&& ((LPNMHDR)lParam)->code ==LVN_ITEMCHANGED){//&&notifyId==LVN_ITEMACTIVATE
				LPNMLISTVIEW pnmv = (LPNMLISTVIEW) lParam; 
				if(pnmv->uNewState){
					TCHAR buf[256];

					wsprintf(buf,_T("你选择了第%d行"),pnmv->iItem);
					//MessageBox(hwndDlg,buf,_T("Info"),MB_OK|MB_ICONASTERISK);
					ShowFuncList(hwndDlg,pnmv->iItem);
					return TRUE;
				}
				
			}
			return FALSE;
		case WM_CLOSE:
			EndDialog(hwndDlg,0);
			return FALSE;
	}

	return FALSE;

}

void InitImportDlg( HWND hwnd )
{
	SendDlgItemMessage(hwnd,IDC_IMPORT_LIST,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT);
	SendDlgItemMessage(hwnd,IDC_FUNC_LIST,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT);

	LVCOLUMN column;
	column.mask=LVCF_WIDTH|LVCF_TEXT;
	column.cx=100;

	column.pszText=_T("Dll Name");
	SendDlgItemMessage(hwnd,IDC_IMPORT_LIST,LVM_INSERTCOLUMN,0,(LPARAM)&column);
	column.pszText=_T("OriginalFirstThunk");
	SendDlgItemMessage(hwnd,IDC_IMPORT_LIST,LVM_INSERTCOLUMN,1,(LPARAM)&column);
	column.pszText=_T("TimeDateStamp");
	SendDlgItemMessage(hwnd,IDC_IMPORT_LIST,LVM_INSERTCOLUMN,2,(LPARAM)&column);
	column.pszText=_T("ForwarderChain");
	SendDlgItemMessage(hwnd,IDC_IMPORT_LIST,LVM_INSERTCOLUMN,3,(LPARAM)&column);
	column.pszText=_T("FirstThunk");
	SendDlgItemMessage(hwnd,IDC_IMPORT_LIST,LVM_INSERTCOLUMN,4,(LPARAM)&column);


	column.pszText=_T("ThunkRVA");
	SendDlgItemMessage(hwnd,IDC_FUNC_LIST,LVM_INSERTCOLUMN,0,(LPARAM)&column);
	column.pszText=_T("ThunkValue");
	SendDlgItemMessage(hwnd,IDC_FUNC_LIST,LVM_INSERTCOLUMN,1,(LPARAM)&column);
	column.pszText=_T("Hint");
	SendDlgItemMessage(hwnd,IDC_FUNC_LIST,LVM_INSERTCOLUMN,2,(LPARAM)&column);
	column.pszText=_T("FunctionName");
	SendDlgItemMessage(hwnd,IDC_FUNC_LIST,LVM_INSERTCOLUMN,3,(LPARAM)&column);

}

void InitImportData( HWND hwnd )
{
	PIMAGE_IMPORT_DESCRIPTOR pIID=util.getIID();
	if(!pIID){
		MessageBox(hwnd,_T("未找到导入表信息"),_T("Error"),MB_OK|MB_ICONERROR);
		return ;
	}
	
	LVITEM item={0};
	item.mask=LVIF_TEXT;
	
	TCHAR buf[256];
	

	for(int i=0;pIID->Name;i++,pIID++){

		memset(&item,0,sizeof(item));
		item.mask=LVIF_TEXT;
		
		LPVOID lsrc=util.rva2ra(pIID->Name);
		int len=strlen((LPTSTR)lsrc);
		//memcpy(&(item.pszText),lsrc,len);
		item.pszText=(LPTSTR)lsrc;
		item.iItem=i;
		SendDlgItemMessage(hwnd,IDC_IMPORT_LIST,LVM_INSERTITEM,0,(LPARAM)&item);
		
		wsprintf(buf,_T("%08lX"),pIID->OriginalFirstThunk);
		item.pszText=buf;
		item.iSubItem=1;
		//SendDlgItemMessage(hwnd,IDC_IMPORT_LIST,LVM_SETITEM,0,(LPARAM)&item);
		ListView_SetItemText(GetDlgItem(hwnd,IDC_IMPORT_LIST),i,1,buf);
		
		wsprintf(buf,_T("%08lX"),pIID->TimeDateStamp);
		item.pszText=buf;
		item.iSubItem=2;
		SendDlgItemMessage(hwnd,IDC_IMPORT_LIST,LVM_SETITEM,0,(LPARAM)&item);

		wsprintf(buf,_T("%08lX"),pIID->ForwarderChain);
		item.pszText=buf;
		item.iSubItem=3;
		SendDlgItemMessage(hwnd,IDC_IMPORT_LIST,LVM_SETITEM,0,(LPARAM)&item);

		wsprintf(buf,_T("%08lX"),pIID->FirstThunk);
		item.pszText=buf;
		item.iSubItem=4;
		SendDlgItemMessage(hwnd,IDC_IMPORT_LIST,LVM_SETITEM,0,(LPARAM)&item);
	}

}

void ShowFuncList( HWND hwnd,UINT row )
{
	SendDlgItemMessage(hwnd,IDC_FUNC_LIST,LVM_DELETEALLITEMS,0,0);
	PIMAGE_THUNK_DATA pITD=(PIMAGE_THUNK_DATA) util.rva2ra( ((PIMAGE_IMPORT_DESCRIPTOR)(util.getIID()+row))->OriginalFirstThunk);
	
	
	
	
	TCHAR buf[256]={0};
	LVITEM item={0};
	item.mask=LVIF_TEXT;

	for(int i=0;pITD->u1.Ordinal;i++,pITD++){

		memset(&item,0,sizeof(item));
		item.mask=LVIF_TEXT;
		

		wsprintf(buf,_T("%08lX"),pITD);
		item.pszText=buf;
		item.iItem=i;
		SendDlgItemMessage(hwnd,IDC_FUNC_LIST,LVM_INSERTITEM,0,(LPARAM)&item);

		wsprintf(buf,_T("%08lX"),pITD->u1.Ordinal);
		item.pszText=buf;
		item.iSubItem=1;
		SendDlgItemMessage(hwnd,IDC_FUNC_LIST,LVM_SETITEM,0,(LPARAM)&item);
		
	

		if(pITD->u1.Ordinal&0x80000000){
			wsprintf(buf,_T("-"));
			item.pszText=buf;
			item.iSubItem=2;
			SendDlgItemMessage(hwnd,IDC_FUNC_LIST,LVM_SETITEM,0,(LPARAM)&item);

			wsprintf(buf,_T("id :%08lX"),pITD->u1.Ordinal&0x7fffffff);
			item.pszText=buf;
			item.iSubItem=3;
			SendDlgItemMessage(hwnd,IDC_FUNC_LIST,LVM_SETITEM,0,(LPARAM)&item);
		}else{

			PIMAGE_IMPORT_BY_NAME pIN=(PIMAGE_IMPORT_BY_NAME)util.rva2ra(pITD->u1.AddressOfData);
			wsprintf(buf,_T("%08lX"),pIN->Hint);
			item.pszText=buf;
			item.iSubItem=2;
			SendDlgItemMessage(hwnd,IDC_FUNC_LIST,LVM_SETITEM,0,(LPARAM)&item);

			item.pszText=(LPTSTR)pIN->Name;
			item.iSubItem=3;
			SendDlgItemMessage(hwnd,IDC_FUNC_LIST,LVM_SETITEM,0,(LPARAM)&item);

		}
		
	}
	
}