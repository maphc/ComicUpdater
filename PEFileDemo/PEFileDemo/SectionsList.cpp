#pragma once
#include "stdafx.h"

void InitColumnInfo(HWND hwnd);
void InitItemInfo(HWND hwnd);

extern PEUtil util;

INT_PTR CALLBACK SectionsProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam){
	WORD itmId=LOWORD(wParam);

	switch(uMsg){
		case WM_INITDIALOG:
			InitColumnInfo(hwndDlg);
			InitItemInfo(hwndDlg);
			return TRUE;

		case WM_COMMAND:


			return FALSE;

		case WM_CLOSE:

			EndDialog(hwndDlg,0);
			return FALSE;

	}
	return FALSE;

}

void InitColumnInfo( HWND hwnd )
{
	LVCOLUMN column={0};
	//memset(&column,0,sizeof(column));

	SendDlgItemMessage(hwnd,IDC_SECTIONTS_LIST,LVM_SETEXTENDEDLISTVIEWSTYLE,0,(LPARAM)LVS_EX_FULLROWSELECT  );// LVS_EX_CHECKBOXES
	
	column.mask=LVCF_TEXT|LVCF_WIDTH;
	column.pszText=_T("Name");
	column.cx=60;
	SendDlgItemMessage(hwnd,IDC_SECTIONTS_LIST,LVM_INSERTCOLUMN,0,(LPARAM)&column);

	column.mask=LVCF_TEXT|LVCF_WIDTH;
	column.pszText=_T("VAddress");
	column.cx=80;
	SendDlgItemMessage(hwnd,IDC_SECTIONTS_LIST,LVM_INSERTCOLUMN,1,(LPARAM)&column);

	column.mask=LVCF_TEXT|LVCF_WIDTH;
	column.pszText=_T("VSize");
	column.cx=80;
	SendDlgItemMessage(hwnd,IDC_SECTIONTS_LIST,LVM_INSERTCOLUMN,2,(LPARAM)&column);

	column.mask=LVCF_TEXT|LVCF_WIDTH;
	column.pszText=_T("RAddress");
	column.cx=80;
	SendDlgItemMessage(hwnd,IDC_SECTIONTS_LIST,LVM_INSERTCOLUMN,3,(LPARAM)&column);

	column.mask=LVCF_TEXT|LVCF_WIDTH;
	column.pszText=_T("RSize");
	column.cx=80;
	SendDlgItemMessage(hwnd,IDC_SECTIONTS_LIST,LVM_INSERTCOLUMN,4,(LPARAM)&column);

	column.mask=LVCF_TEXT|LVCF_WIDTH;
	column.pszText=_T("Flag");
	column.cx=80;
	SendDlgItemMessage(hwnd,IDC_SECTIONTS_LIST,LVM_INSERTCOLUMN,5,(LPARAM)&column);
}

void InitItemInfo( HWND hwnd )
{
	TCHAR buf[256];
	memset(buf,0,256);
	//wsprintf(buf,_T("%08lX"),util.getDosHeader()->e_magic);
	//MessageBox(hwnd,buf,_T("INFO"),MB_OK);

	int num=util.getFileHeader()->NumberOfSections;
	PIMAGE_SECTION_HEADER pSH=util.getFirstSectionHeader();

	
	LVITEM item={0};
	memset(&item,0,sizeof(item));
	item.mask=LVIF_TEXT;
	
	
	
	
	/*
	item.iSubItem=1;
	wsprintf(buf,_T("%08lX"),pSH->VirtualAddress);
	item.pszText=buf;
	SendDlgItemMessage(hwnd,IDC_SECTIONTS_LIST,LVM_SETITEM,0,(LPARAM)&item);
	*/
	for(int i=0;i<num;i++){

		memset(&item,0,sizeof(item));
		item.mask=LVIF_TEXT;
		item.iItem=i;
		memcpy(buf,pSH->Name,8);
		item.pszText=buf;
		SendDlgItemMessage(hwnd,IDC_SECTIONTS_LIST,LVM_INSERTITEM,0,(LPARAM)&item);
		
		item.iSubItem=1;
		wsprintf(buf,_T("%08lX"),pSH->VirtualAddress);
		item.pszText=buf;
		//ListView_SetItemText(GetDlgItem(hwnd,IDC_SECTIONTS_LIST),i,1,buf);
		SendDlgItemMessage(hwnd,IDC_SECTIONTS_LIST,LVM_SETITEM,0,(LPARAM)&item);

		item.iSubItem=2;
		wsprintf(buf,_T("%08lX"),pSH->Misc.VirtualSize);
		item.pszText=buf;
		SendDlgItemMessage(hwnd,IDC_SECTIONTS_LIST,LVM_SETITEM,0,(LPARAM)&item);


		item.iSubItem=3;
		wsprintf(buf,_T("%08lX"),pSH->PointerToRawData);
		item.pszText=buf;
		SendDlgItemMessage(hwnd,IDC_SECTIONTS_LIST,LVM_SETITEM,0,(LPARAM)&item);


		item.iSubItem=4;
		wsprintf(buf,_T("%08lX"),pSH->SizeOfRawData);
		item.pszText=buf;
		SendDlgItemMessage(hwnd,IDC_SECTIONTS_LIST,LVM_SETITEM,0,(LPARAM)&item);


		item.iSubItem=5;
		wsprintf(buf,_T("%08lX"),pSH->Characteristics);
		item.pszText=buf;
		SendDlgItemMessage(hwnd,IDC_SECTIONTS_LIST,LVM_SETITEM,0,(LPARAM)&item);
		
		++pSH;
	}
	


}