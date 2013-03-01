// PythonAndCpp.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

void printDict(PyObject* o){
	if(!PyDict_Check(o)){
		printf("Not A Dict!");
		return;
	}

	PyObject *k,*keys;
	keys=PyDict_Keys(o);
	for(int i=0,n=PyList_GET_SIZE(keys);i<n;i++){
		k=PyList_GetItem(keys,i);
		char* m=PyString_AsString(k);
		char* v=PyString_AsString(PyDict_GetItem(o,k));
		printf("dict :%s=%s\n",m,v);
	}
}
void printList(PyObject* l){
	if(!PyList_Check(l)){
		cout<<"Not a list "<<endl;
		return;
	}
	for(int i=0;i<PyList_GET_SIZE(l);i++){
		cout<<"List "<<i<<" :"<<PyString_AsString(PyList_GetItem(l,i))<<endl;;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{

	Py_Initialize();
	if(!Py_IsInitialized()){
		printf("python初始化失败");
		return -1;
	}

	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('z:\\')");
	PyRun_SimpleString("print sys.path");

	PyObject* pModule=PyImport_ImportModule("manhua178");
	PyObject* pDict=PyModule_GetDict(pModule);
	printDict(pDict);
	PyObject* test=PyObject_GetAttrString(pModule,"test");
	
	ifstream f("z:\\178.txt");
	string s(
		istreambuf_iterator<char>(f.rdbuf()), 
		istreambuf_iterator<char>()
		);
	
	PyObject* resp=PyObject_CallFunction(test,"ss",s.c_str(),"pages");
	if(PyErr_Occurred()){
		PyErr_Print();
		PyErr_Clear();
	}
	if(resp==NULL){
		cout<<"RESP Failed"<<endl;
	}else{
		if(PyString_Check(resp)){
			cout<<"RESP :"<<PyString_AsString(resp)<<endl;
		}else{
			cout<<"RESP Failed"<<endl;
		}
	}
	
	
	
	Py_DECREF(pModule);
	Py_Finalize();

	return 0;
}

