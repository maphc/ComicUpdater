#include "stdafx.h"

extern PEUtil util;
extern HINSTANCE hInst;

typedef struct {
	UINT ID_RVA;
	UINT ID_SIZE;
} DD_EDIT;


DD_EDIT edit_array[]={
	{IDC_DD_RVA_EXPORT,IDC_DD_SIZE_EXPORT},
	{IDC_DD_RVA_IMPORT,IDC_DD_SIZE_IMPORT},
	{IDC_DD_RVA_RES,IDC_DD_SIZE_RES},
	{IDC_DD_RVA_EXCEP,IDC_DD_SIZE_EXCEP},
	{IDC_DD_RVA_SECURITY,IDC_DD_SIZE_SECURITY},
	{IDC_DD_RVA_RELOC,IDC_DD_SIZE_RELOC},
	{IDC_DD_RVA_DEBUG,IDC_DD_SIZE_DEBUG},
	{IDC_DD_RVA_ARCHI,IDC_DD_SIZE_ARCHI},
	{IDC_DD_RVA_GLOBAL,IDC_DD_SIZE_GLOBAL},
	{IDC_DD_RVA_TLS,IDC_DD_SIZE_TLS},
	{IDC_DD_RVA_LOAD_CONFIG,IDC_DD_SIZE_LOAD_CONFIG},
	{IDC_DD_RVA_BOUND_IMPORT,IDC_DD_SIZE_BOUND_IMPORT},
	{IDC_DD_RVA_IAT,IDC_DD_SIZE_IAT},
	{IDC_DD_RVA_DELAY,IDC_DD_SIZE_DELAY},
	{IDC_DD_RVA_COM,IDC_DD_SIZE_COM}

};

INT_PTR CALLBACK DataDirProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam){
	UINT itmId=LOWORD(wParam);

	switch(uMsg){
		case WM_INITDIALOG:
			LoadDataDir(hwndDlg);
			return TRUE;
		case WM_COMMAND:
			switch(itmId){
				case IDOK:
					EndDialog(hwndDlg,0);
					return FALSE;
				case IDC_BTN_IMPORT:
					DialogBox(hInst,MAKEINTRESOURCE(IDD_IMPORT),hwndDlg,ImportProc);
					return TRUE;
			}
			return TRUE;
	
		case WM_CLOSE:
			EndDialog(hwndDlg,0);
			return FALSE;
	}
	

	return FALSE;

}
void LoadDataDir(HWND hwnd){
	TCHAR buf[256];
	for(int i=0;i<15;i++){
		wsprintf(buf,_T("%08lX"),util.getDataDirectory(i)->VirtualAddress);
		SetDlgItemText(hwnd,edit_array[i].ID_RVA,buf);
		
		wsprintf(buf,_T("%08lX"),util.getDataDirectory(i)->Size);
		SetDlgItemText(hwnd,edit_array[i].ID_SIZE,buf);

	}
}