#pragma once
#include "stdafx.h"

INT_PTR CALLBACK ImportProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
void InitImportDlg(HWND hwnd);
void InitImportData(HWND hwnd);
void ShowFuncList(HWND hwnd,UINT row);