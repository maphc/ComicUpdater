// PL/SQL Developer Plug-In demo
// Copyright 1999 Allround Automations
// support@allroundautomations.nl
// http://www.allroundautomations.nl
// Demo Developed by David Maisonave (david@axter.com)

// This demo shows an elementary interface

// To debug the demo, change the project property settings in the LINK tab
// Change the [Output File Name] field from
// Debug/DemoWin32.dll
// To the following:
// C:\Program Files\PLSQL Developer\PlugIns/DemoWin32.dll
// Change the above directory to reflect the correct location of the PLSQL Developer directory.
// The above change will make the compiler compile the code to the PlugIns directory

// The project property settings in the debug tab should also be changed to reflect correct PLSQL Dev directory
// Check the following field: [Project]->[Settings]->[Debug](tab)->[Executable For Debug Session](field)

// DemoWin32.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "dllmain.h"
#include "PlSql_functions.h"
#include <string>
#include <sstream>
#include "TinyReplacer.h"

TinyReplacer tr;

BOOL APIENTRY DllMain( HANDLE hModule, 
	DWORD  ul_reason_for_call, 
	LPVOID lpReserved
	)
{
	return TRUE;
}

bool UseAutoSetCursorPosition = false;

const char* IntToStr(int n)
{
	static std::string s;
	std::stringstream ss;
	ss << n;
	s = ss.str();
	return s.c_str();
}

void ShowMessage(const char* Data)
{
	MessageBox(NULL,Data, "Msg from AddIn",0);
}
char* MenuItemInterface(int Index, bool CreateMenuItem_);


//---------------------------------------------------------------------------
// Creating menus
char* CreateMenuItem(int Index)
{
	return MenuItemInterface(Index, true);
}

//---------------------------------------------------------------------------
// Show configuration dialog
void  Configure()
{
	ShowMessage("No configue menu");
}

//---------------------------------------------------------------------------
// Plug-In identification, a unique identifier is received and
// the description is returned
char* IdentifyPlugIn(int ID)
{
	PlugInID = ID;
	return Desc;
}
//---------------------------------------------------------------------------
void OnMenuClick(int Index)
{
	MenuItemInterface(Index, false);
}
//---------------------------------------------------------------------------
// Called when someone logs on or off
void OnConnectionChange()
{
}
//---------------------------------------------------------------------------
// Called when another item in the browser gets selected
void OnBrowserChange()
{
	char *Type, *Owner, *Name;

	IDE_GetBrowserInfo(&Type, &Owner, &Name);
}
//---------------------------------------------------------------------------
// Called when the Plug-In is created
void OnCreate()
{ 

}
//---------------------------------------------------------------------------
// Called when the Plug-In is activated
void OnActivate()
{
	OnBrowserChange();
	OnConnectionChange();
	OnWindowChange();
	tr.init();
}
//---------------------------------------------------------------------------
// Called when the Plug-In is destroyed
void OnDestroy()
{
}
//---------------------------------------------------------------------------
void DoSQLForm()
{
}

//---------------------------------------------------------------------------
void ShowBrowserObject()
{
	char *Type, *Owner, *Name;
	char s[1024]={0};

	IDE_GetBrowserInfo(&Type, &Owner, &Name);
	sprintf_s(s,1024,"%s\n%s\n%s",Type,Owner,Name);
	ShowMessage(s);
}
//---------------------------------------------------------------------------

bool GetClientText(string& s){
	int H = IDE_GetEditorHandle();
	if (H > 0)
	{
		char buffer[4096]={0};
		int len=GetWindowTextLength( (HWND)H);
		GetWindowText((HWND)H,buffer,len+1);
		s=(buffer);

		int sel_start=0, sel_end=0; 
		SendMessage((HWND)H,EM_GETSEL,(WPARAM)&sel_start,(LPARAM)&sel_end);
		if(sel_start==sel_end){
			return false;
		}else{
			s= s.substr(sel_start,sel_end-sel_start);
			return true;
		}
	}
	return false;
}

void SetClientText(string s,bool isSel){
	int H = IDE_GetEditorHandle();
	if(isSel){
		SendMessage((HWND)H, EM_REPLACESEL, true, (int)s.c_str());
	}else{
		SetWindowText((HWND)H,s.c_str());
	}
	
}
void string_replace(std::string& strBig, const std::string & strsrc, const std::string &strdst)
{
	std::string::size_type pos = 0;
	while( (pos = strBig.find(strsrc, pos)) != string::npos)
	{
		strBig.replace(pos, strsrc.length(), strdst);
		pos += strdst.length();
	}
}
void ReplaceColon(){
	string s;
	bool isSel=GetClientText(s);
	string_replace(s,":V","&V");
	SetClientText(s,isSel);
}

void ShowConnectionInfo()
{
	char *Username, *Password, *Database;
	char s[1024]={0};

	IDE_GetConnectionInfo(&Username, &Password, &Database);
	sprintf_s(s,1024,"%s\n%s\n%s",Username,Password,Database);
	ShowMessage(s);
}

//---------------------------------------------------------------------------
void AddSchema()
{
	char *Username, *Password, *Database;
	IDE_GetConnectionInfo(&Username, &Password, &Database);
	
	tr.setUser(Username);
	tr.setConn(Database);

	string s;
	bool isSel=GetClientText(s);
	SetClientText(tr.replace(s),isSel);
	
}
//---------------------------------------------------------------------------
void MaximizeWindow()
{
	int H = IDE_GetChildHandle();
	int M = IDE_GetClientHandle();
	if (H > 0)
	{
		SendMessage((HWND)M, WM_MDIMAXIMIZE, H, (int)0);
	}
}
void SaveAndExecute()
{
	IDE_SaveFile();
	IDE_Perform(pcmd_Execute);
}

DWORD GetCurrentCusorPosition(int &x, int &y)
{
	HWND H = (HWND)IDE_GetEditorHandle();
	DWORD StartingPos;
	SendMessage(H, EM_GETSEL, (WPARAM) (LPDWORD) &StartingPos, NULL);
	DWORD StartingPos_x = StartingPos;
	y = SendMessage(H, EM_LINEFROMCHAR, (WPARAM) StartingPos, NULL);
	x = StartingPos - SendMessage(H, EM_LINEINDEX, y, NULL);
	return StartingPos_x;
}

int OnWindowClose(int WindowType, BOOL Changed)
{
	int x,y;
	DWORD StartingPos = GetCurrentCusorPosition(x,y);
	std::string FileName = IDE_Filename();
	return 0;
}

bool WindowJustOpened = false;

void OnWindowCreate(int WindowType)
{
	WindowJustOpened = true;
}
void OnWindowChange()
{

}

void DisplayCurrentCusorLineNumber(void)
{
	int x,y;
	DWORD StartingPos = GetCurrentCusorPosition(x,y);
	std::stringstream ss;
	ss << "Current line number = " << x << " " <<  y << " " << StartingPos ;
	ShowMessage(ss.str().c_str());
}

char* MenuItemInterface(int Index, bool CreateMenuItem_)
{
	switch (Index)
	{
	case 1 : 
		if (CreateMenuItem_) return "Plugins / &Plug-In 2 Demo / Show connection info";
		ShowConnectionInfo();
		return NULL;
	case 2 : 
		if (CreateMenuItem_) return "Plugins / &Plug-In 2 Demo / Test display Cursor position";
		DisplayCurrentCusorLineNumber();
		return NULL;
	case 3 :
		if (CreateMenuItem_) return "Plugins / &Plug-In 2 Demo / &Maximize Window";
		MaximizeWindow();
		return NULL;
	case 4 :
		if (CreateMenuItem_) return "Plugins / &Plug-In 2 Demo / &ReadOnly / &On";
		IDE_SetReadOnly(true);
		return NULL;
	case 5 :
		if (CreateMenuItem_) return "Plugins / &Plug-In 2 Demo / &ReadOnly / O&ff";
		IDE_SetReadOnly(false);
		return NULL;
	case 6 :
		if (CreateMenuItem_) return "Plugins / &Plug-In 2 Demo / &Editor/ ShowHandle...";
		ShowMessage(IntToStr(IDE_GetEditorHandle()));
		return NULL;
	case 7 :
		if (CreateMenuItem_) return "Plugins / &Plug-In 2 Demo / AddSchema";
		AddSchema();
		return NULL;
	case 8 :
		if (CreateMenuItem_) return "Plugins / &Plug-In 2 Demo / ReplaceColon ";
		ReplaceColon();
		return NULL;

	}
	return "";
}
