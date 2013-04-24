#include "stdafx.h"
#pragma once

using namespace std;

class PyThreadStateLock{
public:
	PyThreadStateLock(void){
		state = PyGILState_Ensure( );
	}
	~PyThreadStateLock(void){
		PyGILState_Release( state );
	}
private:
	PyGILState_STATE state;
};

class PyHelper{
private:
	map< CString , PyObject* > moduleMap;
private:
	
	static void printExceptionMessage();
	void init();
public:
	void initMulti();
	~PyHelper();

	static PyHelper& getInstance();

	static void printDict(PyObject* o);
	static void printList(PyObject* l);

	CString get178VolPages(CString h);
	CString getIManhuaVolPages(CString h);
	static string getSimpleResp(CString input,CString moduleName,CString funcName);
};

