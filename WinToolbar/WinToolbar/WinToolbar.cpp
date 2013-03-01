// WinToolbar.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "WinToolbar.h"

INT_PTR CALLBACK DialogProc(          HWND hwndDlg,
                            UINT uMsg,
                            WPARAM wParam,
                            LPARAM lParam
                            );
INT_PTR CALLBACK AboutProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
HWND CreateAToolBar(HWND hwndParent) ;

HINSTANCE hInst;
int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    hInst=hInstance;
    DialogBox(hInstance,MAKEINTRESOURCE(IDD_MAIN),NULL,DialogProc);

    return 0;

}

INT_PTR CALLBACK DialogProc(          HWND hwndDlg,
                            UINT uMsg,
                            WPARAM wParam,
                            LPARAM lParam
                            )
{
    WORD uId;
    switch(uMsg){
        case WM_INITDIALOG:
            CreateAToolBar(hwndDlg) ;
            break;
        case WM_COMMAND:
            uId=LOWORD(wParam);
            switch(uId){
        case IDOK:
        case IDM_ABOUT:
            DialogBox(hInst,MAKEINTRESOURCE(IDD_ABOUTBOX),hwndDlg,AboutProc);
            break;
        case IDCANCEL:
            EndDialog(hwndDlg,1);
            break;

            }

            break;
        case WM_CLOSE:
            EndDialog(hwndDlg,0);
            break;

    }

    return FALSE;
}

INT_PTR CALLBACK AboutProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:

        return TRUE;

    case WM_COMMAND:
        //IDM_ABOUT
        EndDialog(hDlg,1);



        break;
    case WM_CLOSE:
        EndDialog(hDlg, 0);
        break;
    }
    return FALSE;
}

// CreateAToolBar creates a toolbar and adds a set of buttons to it.
// The function returns the handle to the toolbar if successful, 
// or NULL otherwise. 
// hwndParent is the handle to the toolbar's parent window. 
HWND CreateAToolBar(HWND hwndParent) 
{ 
    //http://blog.csdn.net/Y___Y/archive/2007/12/20/1955919.aspx
    HWND hWndToolbar;
    TBBUTTON tbb[5]={0};

    HIMAGELIST hImageList;
    DWORD abc;
    TCHAR dbg[50];

    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC  = ICC_BAR_CLASSES;
    InitCommonControlsEx(&icex);

    abc=GetLastError();
    _tprintf(dbg,_T("%s"),abc);
    //MessageBox(hwndParent,dbg,_T(""),MB_OK);

    hWndToolbar=CreateWindowEx(WS_EX_TOOLWINDOW,
        TOOLBARCLASSNAME,
        //TEXT(""),
        NULL,
        //WS_CHILD|WS_VISIBLE|WS_BORDER|TBSTYLE_FLAT|CCS_NOMOVEY,
        WS_CHILD |WS_VISIBLE|TBSTYLE_FLAT|CCS_NOMOVEY ,
        0,0,0,0,
        hwndParent,
        (HMENU)ID_TOOLBAR,
        hInst,
        NULL);
    if(!hWndToolbar)
        return 0;
    //EnableWindow(hWndToolbar,FALSE);
    //TB_BUTTONSTRUCTSIZE 
    SendMessage(hWndToolbar,TB_BUTTONSTRUCTSIZE,(WPARAM)sizeof(TBBUTTON),0);

    hImageList=ImageList_Create(32,32,ILC_COLORDDB|ILC_MASK,5,2);
    ImageList_AddIcon(hImageList,LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON1)));
    ImageList_AddIcon(hImageList,LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON2)));
    ImageList_AddIcon(hImageList,LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON3)));
    ImageList_AddIcon(hImageList,LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON4)));

    //TB_SETIMAGELIST
    SendMessage(hWndToolbar,TB_SETIMAGELIST,0,(LPARAM)hImageList);
    //SendMessage(hWndToolbar,TB_SETHOTIMAGELIST,0,(LPARAM)hImageList);
    //SendMessage(hWndToolbar,TB_SETDISABLEDIMAGELIST,0,(LPARAM)hImageList);

    TCHAR szS0[]=_T("123456");
    tbb[0].iBitmap=MAKELONG(0,0);
    tbb[0].idCommand=IDOK;
    tbb[0].fsState =TBSTATE_ENABLED ;
    //tbb[0].fsStyle=TBSTYLE_TOOLTIPS|TBSTYLE_LIST|BTNS_BUTTON|BTNS_SHOWTEXT ;
    tbb[0].fsStyle= TBSTYLE_BUTTON|BTNS_AUTOSIZE ;
    tbb[0].iString=(INT_PTR)_T("123456");

    TCHAR szS1[]=_T("234567");
    tbb[1].iBitmap=MAKELONG(1,0);
    tbb[1].idCommand=IDOK;
    tbb[1].fsState =TBSTATE_ENABLED;
    //tbb[1].fsStyle=TBSTYLE_TOOLTIPS|TBSTYLE_LIST|BTNS_BUTTON|BTNS_SHOWTEXT ;
    tbb[1].fsStyle= TBSTYLE_BUTTON|BTNS_AUTOSIZE ;
    tbb[1].iString=(INT_PTR)_T("234567");

    TCHAR szS2[]=_T("345678");
    tbb[2].iBitmap=MAKELONG(2,0);
    tbb[2].idCommand=IDOK;
    tbb[2].fsState =TBSTATE_ENABLED;
    //tbb[2].fsStyle=TBSTYLE_TOOLTIPS|TBSTYLE_LIST|BTNS_BUTTON|BTNS_SHOWTEXT ;
    tbb[2].fsStyle= TBSTYLE_BUTTON|BTNS_AUTOSIZE ;
    tbb[2].iString=(INT_PTR)szS2;

    TCHAR szS3[]=_T("456789");
    tbb[3].iBitmap=MAKELONG(3,0);
    tbb[3].idCommand=IDCANCEL;
    tbb[3].fsState =TBSTATE_ENABLED;
    //tbb[3].fsStyle=TBSTYLE_TOOLTIPS|TBSTYLE_LIST|BTNS_BUTTON|BTNS_SHOWTEXT ;
    tbb[3].fsStyle= TBSTYLE_BUTTON|BTNS_AUTOSIZE ;
    tbb[3].iString=(INT_PTR)szS3;

    tbb[4].iBitmap=0;
    tbb[4].idCommand=0;
    tbb[4].fsState =TBSTATE_ENABLED;
    tbb[4].fsStyle=BTNS_SEP;
    tbb[4].iString=0;

    //TB_ADDBUTTONS
    SendMessage(hWndToolbar,TB_ADDBUTTONS, sizeof(tbb)/sizeof(TBBUTTON), (WPARAM)&tbb);

    //RECT rect;
    //GetClientRect(hwndParent,&rect);
    //WM_SIZE SIZE_RESTORED
    SendMessage(hWndToolbar,WM_SIZE,SIZE_RESTORED,0);

    //SetWindowPos(hWndToolbar,NULL,rect.top,rect.left,rect.right-rect.left,60,SWP_NOZORDER|SWP_NOACTIVATE);
    //TBBUTTON tb={0};
    //SendMessage(hWndToolbar,TB_GETBUTTON,0,(LPARAM) &tb);




    return hWndToolbar;


} 