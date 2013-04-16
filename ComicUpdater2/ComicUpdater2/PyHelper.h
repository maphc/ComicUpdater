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
	
	void printExceptionMessage();
	void init();
public:
	void initMulti();
	~PyHelper();

	static PyHelper& getInstance();

	void printDict(PyObject* o);
	void printList(PyObject* l);

	CString get178VolPages(CString h);
	CString getIManhuaVolPages(CString h);

};

