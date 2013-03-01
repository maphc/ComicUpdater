// PEFileDemo.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"



PEUtil util;
HINSTANCE hInst;
INT_PTR CALLBACK MainProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
INT_PTR CALLBACK AboutProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
void ClearCtrlData(HWND hwnd);
void ShowPEInfo(HWND hwnd,LPTSTR buf);
void ShowFileHeader(HWND hwnd);
void ShowOptHeader(HWND hwnd);
int WINAPI WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd ){
	
	hInst=hInstance;
	DialogBox(hInstance,MAKEINTRESOURCE(IDD_MAIN),NULL,MainProc);

	return TRUE;
}

INT_PTR CALLBACK AboutProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam){
	WORD itmId=LOWORD(wParam);
	switch(uMsg){
		case WM_INITDIALOG:
			return TRUE;
		case WM_COMMAND:
			if(itmId==IDOK){
				SendMessage(hwndDlg,WM_CLOSE,wParam,lParam);
				return TRUE;
			}
			break;
		case WM_CLOSE:
			EndDialog(hwndDlg,0);
			break;
	}
	return FALSE;

}
INT_PTR CALLBACK MainProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam){
	
	WORD itmId=LOWORD(wParam);
	HICON hIcon;
	switch(uMsg){
		case WM_INITDIALOG:
				  hIcon=LoadIcon(hInst,MAKEINTRESOURCE(IDI_SMALL));
				 SendMessage(hwndDlg,WM_SETICON,ICON_SMALL,(LPARAM)hIcon);
				 return TRUE;
		case WM_COMMAND:
			switch(itmId){
				case IDC_BTN_ABOUT:
					DialogBox(hInst,MAKEINTRESOURCE(IDD_ABOUTBOX),hwndDlg,AboutProc);
					return TRUE;
				case IDC_BTN_SECTIONS:
					DialogBox(hInst,MAKEINTRESOURCE(IDD_SECTIONS),hwndDlg,SectionsProc);
					return TRUE;
				case IDC_BTN_DATADIR:
					DialogBox(hInst,MAKEINTRESOURCE(IDD_DATA_DIR),hwndDlg,DataDirProc);

			}
				 return FALSE;

		case WM_DROPFILES:
			
			TCHAR buf[512];
			memset(buf,0,512);
			DragQueryFile((HDROP)wParam,0,buf,sizeof(buf));
			//MessageBox(NULL,buf,_T("Info"),MB_OK);
			ShowPEInfo(hwndDlg,buf);
			return TRUE;
		case WM_CLOSE:
			EndDialog(hwndDlg,0);
			break;
		
	}
	return FALSE;

}

void ClearCtrlData( HWND hwnd )
{
	
}

void ShowPEInfo( HWND hwnd,LPTSTR buf )
{
	//MessageBox(hwnd,buf,_T("Info"),MB_OK);
	ClearCtrlData(hwnd);
	util.unloadFile();
	util.loadFile(buf);
	if(!util.isPEFile()){
		MessageBox(hwnd,_T("这不是一个PE文件"),_T("Error"),MB_OK|MB_ICONERROR);
		EnableWindow(GetDlgItem(hwnd,IDC_BTN_SECTIONS),FALSE);
		EnableWindow(GetDlgItem(hwnd,IDC_BTN_DATADIR),FALSE);
	}else{
		MessageBox(hwnd,_T("发现PE格式"),_T("Info"),MB_OK|MB_ICONINFORMATION);
	
		ShowFileHeader(hwnd);
		ShowOptHeader(hwnd);
		EnableWindow(GetDlgItem(hwnd,IDC_BTN_SECTIONS),TRUE);
		EnableWindow(GetDlgItem(hwnd,IDC_BTN_DATADIR),TRUE);
	}
	
	


	//PIMAGE_SECTION_HEADER pSH=IMAGE_FIRST_SECTION(util.getNtHeaders());

}

void ShowFileHeader( HWND hwnd )
{
	PIMAGE_FILE_HEADER pFH=util.getFileHeader();
	TCHAR temp[256]={0};

	wsprintf(temp,_T("%04lX"),pFH->Machine);
	SetDlgItemText(hwnd,IDC_FH_MACHINE,temp);

	wsprintf(temp,_T("%04lX"),pFH->NumberOfSections);
	SetDlgItemText(hwnd,IDC_FH_NUMOFSECTIONS,temp);

	wsprintf(temp,_T("%08lX"),pFH->TimeDateStamp);
	SetDlgItemText(hwnd,IDC_FH_TDS,temp);

	wsprintf(temp,_T("%08lX"),pFH->PointerToSymbolTable);
	SetDlgItemText(hwnd,IDC_FH_PT2SYMB,temp);

	wsprintf(temp,_T("%08lX"),pFH->NumberOfSymbols);
	SetDlgItemText(hwnd,IDC_FH_NUMOFSYMBOLS,temp);

	wsprintf(temp,_T("%04lX"),pFH->SizeOfOptionalHeader);
	SetDlgItemText(hwnd,IDC_FH_SIZEOFOPTHEADER,temp);

	wsprintf(temp,_T("%04lX"),pFH->Characteristics);
	SetDlgItemText(hwnd,IDC_FH_CHARACTERISTICS,temp);
}

void ShowOptHeader( HWND hwnd )
{
	
	TCHAR buf[256]={0};
	PIMAGE_OPTIONAL_HEADER pOH=util.getOptHeader();
	
	wsprintf(buf,_T("%04lX"),pOH->Magic);
	SetDlgItemText(hwnd,IDC_OH_MAGIC,buf);

	wsprintf(buf,_T("%d.%d"),pOH->MajorLinkerVersion,pOH->MinorLinkerVersion);
	SetDlgItemText(hwnd,IDC_OH_LINKERVER,buf);

	wsprintf(buf,_T("%08lX"),pOH->SizeOfCode);
	SetDlgItemText(hwnd,IDC_OH_SIZEOFCODE,buf);

	wsprintf(buf,_T("%08lX"),pOH->AddressOfEntryPoint);
	SetDlgItemText(hwnd,IDC_OH_ENTRYPOINT,buf);

	wsprintf(buf,_T("%08lX"),pOH->BaseOfCode);
	SetDlgItemText(hwnd,IDC_OH_BASEOFCODE,buf);

	wsprintf(buf,_T("%08lX"),pOH->BaseOfData);
	SetDlgItemText(hwnd,IDC_OH_BASEOFDATA,buf);

	wsprintf(buf,_T("%08lX"),pOH->ImageBase);
	SetDlgItemText(hwnd,IDC_OH_IMAGEBASE,buf);

	wsprintf(buf,_T("%08lX"),pOH->SectionAlignment);
	SetDlgItemText(hwnd,IDC_OH_SECTIONALIGN,buf);

	wsprintf(buf,_T("%08lX"),pOH->FileAlignment);
	SetDlgItemText(hwnd,IDC_OH_FILEALIGN,buf);

	wsprintf(buf,_T("%08lX"),pOH->AddressOfEntryPoint);
	SetDlgItemText(hwnd,IDC_OH_ENTRYPOINT,buf);

	wsprintf(buf,_T("%08lX"),pOH->SizeOfImage);
	SetDlgItemText(hwnd,IDC_OH_SIZEOFIMAGE,buf);

	wsprintf(buf,_T("%08lX"),pOH->SizeOfHeaders);
	SetDlgItemText(hwnd,IDC_OH_SIZEOFHEADERS,buf);

	wsprintf(buf,_T("%08lX"),pOH->CheckSum);
	SetDlgItemText(hwnd,IDC_OH_CHECKSUM,buf);

	wsprintf(buf,_T("%04lX"),pOH->Subsystem);
	SetDlgItemText(hwnd,IDC_OH_SUBSYSTEM,buf);

	wsprintf(buf,_T("%04lX"),pOH->DllCharacteristics);
	SetDlgItemText(hwnd,IDC_OH_DLLCHAR,buf);

	wsprintf(buf,_T("%08lX"),pOH->NumberOfRvaAndSizes);
	SetDlgItemText(hwnd,IDC_OH_NUMOFRAV,buf);

}