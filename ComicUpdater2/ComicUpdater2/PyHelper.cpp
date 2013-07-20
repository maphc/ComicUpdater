#include "stdafx.h"
#include <compile.h>
#include <frameobject.h>
#include <traceback.h>

void PyHelper::initMulti()
{
	Py_Initialize();
	// 初始化线程支持
	PyEval_InitThreads();

	if(!Py_IsInitialized()){
		TRACE0("python初始化失败");
		if(PyErr_Occurred()){
			PyErr_Print();
			PyErr_Clear();
		}
		MessageBox(NULL,_T("Python INIT"),_T("Python init Error"),MB_OK);
		return;
	}

	PyRun_SimpleString("import os");
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('./scripts')");

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	hFind = FindFirstFile(_T(".\\scripts\\*.py"), &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) 
	{
		TRACE0("Can't find python scripts ! \n");

	} 
	else 
	{
		do 
		{
			TRACE1 ("Python script file found is %s\n", FindFileData.cFileName);
			CString pyFileName(FindFileData.cFileName);
			CString pyModuleName=pyFileName.Mid(0,pyFileName.FindOneOf(_T(".")));
			PyObject* pyModule=PyImport_ImportModule(pyModuleName);
			if(PyErr_Occurred()){
				PyErr_Clear();
				TRACE1 ("import module  %s fail \n", pyModuleName);
			}else{
				moduleMap.insert(make_pair(pyModuleName,pyModule));
				TRACE1 ("import module  %s success\n", pyModuleName);
			}

		} while (FindNextFile(hFind, &FindFileData) != 0);

		FindClose(hFind);
	}
	// 启动子线程前执行，为了释放PyEval_InitThreads获得的全局锁，否则子线程可能无法获取到全局锁。
	PyEval_ReleaseThread(PyThreadState_Get());
}
PyHelper::~PyHelper(){
	// 保证子线程调用都结束后
	//PyGILState_Ensure();
	//Py_Finalize();

}

PyHelper& PyHelper::getInstance()
{
	static PyHelper _inst;
	return _inst;
}

void PyHelper::printDict( PyObject* o )
{
	if(!PyDict_Check(o)){
		TRACE0("Not A Dict!");
		return;
	}

	PyObject *k,*keys;
	keys=PyDict_Keys(o);
	for(int i=0,n=PyList_GET_SIZE(keys);i<n;i++){
		k=PyList_GetItem(keys,i);
		char* m=PyString_AsString(k);
		char* v=PyString_AsString(PyDict_GetItem(o,k));
		TRACE2("dict :%s=%s\n",m,v);
	}
}

void PyHelper::printList( PyObject* l )
{
	if(l==NULL){
		TRACE0(_T("List is null\n"));
	}
	if(!PyList_Check(l)){
		TRACE0(_T("Not a list\n"));
		return;
	}
	for(int i=0;i<PyList_GET_SIZE(l);i++){

		TRACE2(_T("List %d :%s\n"),i,PyString_AsString(PyList_GetItem(l,i)));
	}
}

CString PyHelper::get178VolPages( CString h )
{
	init();
	CString result;

	TRACE1(_T("Py_GetPath :%s\n"),Py_GetPath());
	TRACE1(_T("Py_GetProgramFullPath :%s\n"),Py_GetProgramFullPath());
	TRACE1(_T("Py_GetProgramName :%s\n"),Py_GetProgramName());
	TRACE1(_T("Py_GetPrefix :%s\n"),Py_GetPrefix());
	TRACE1(_T("Py_GetExecPrefix :%s\n"),Py_GetExecPrefix());

	PyObject* manhua178=PyImport_ImportModule("manhua178");
	PyObject* getPages= NULL;
	try
	{
		//PyObject* manhua178=moduleMap.find(CString(_T("manhua178")))->second;
		getPages=PyObject_GetAttrString(manhua178,"getPages");
		if(PyFunction_Check(getPages)){
			TRACE0(_T("PyFunction getPages is OK\n"));
		}
	}
	catch (exception &e)
	{
		TRACE1("Python exception %s\n",e.what());
		MessageBox(NULL,e.what(),_T("Python Error"),MB_OK);
	}

	if(PyErr_Occurred()){
		PyErr_Print();
		PyErr_Clear();
	}

	PyObject* resp=NULL;
	try
	{
		resp=PyObject_CallFunction(getPages,"s",(LPCTSTR)h);
		//resp=PyObject_CallFunction(getPages,"s","aaa");
		printExceptionMessage();
		if(PyErr_Occurred()){

			PyErr_Clear();
		}
	}catch (exception &e)
	{
		TRACE1("Python exception %s\n",e.what());
		MessageBox(NULL,e.what(),_T("Python Error"),MB_OK);
	}catch(...){
		MessageBox(NULL,_T("Python 初始化异常"),_T("Python Error"),MB_OK);
	}

	if(resp==NULL){
		Py_DECREF(resp);
		Py_DECREF(getPages);

	}else{
		result=PyString_AsString(resp);
		Py_DECREF(resp);
		Py_DECREF(getPages);

	}
	Py_Finalize();
	return result;
	
}
void PyHelper::printExceptionMessage(){
	char buf[512], *buf_p = buf;
	PyObject *type_obj, *value_obj, *traceback_obj;
	PyErr_Fetch(&type_obj, &value_obj, &traceback_obj);
	if (value_obj == NULL){
		TRACE0(_T("PyErr_Fetch value_obj is NULL1\n"));
		return;
	}
	if (!PyString_Check(value_obj)){
		TRACE0(_T("PyErr_Fetch value_obj is NULL2\n"));
		//return;
	}
	char *value = PyString_AsString(value_obj);
	size_t szbuf = sizeof(buf);
	int l;
	PyCodeObject *codeobj;

	l = snprintf(buf_p, szbuf, _T("Error Message:\n%s"), value);
	buf_p += l;
	szbuf -= l;

	if (traceback_obj != NULL) {
		l = snprintf(buf_p, szbuf, _T("\n\nTraceback:\n"));
		buf_p += l;
		szbuf -= l;

		PyTracebackObject *traceback = (PyTracebackObject *)traceback_obj;
		for (;traceback && szbuf > 0; traceback = traceback->tb_next) {
			codeobj = traceback->tb_frame->f_code;
			l = snprintf(buf_p, szbuf, "%s: %s(# %d)\n",
				PyString_AsString(codeobj->co_name),
				PyString_AsString(codeobj->co_filename),
				traceback->tb_lineno);
			buf_p += l;
			szbuf -= l;
		}
	}

	TRACE1("PyException :%s\n",buf);

	Py_XDECREF(type_obj);
	Py_XDECREF(value_obj);
	Py_XDECREF(traceback_obj);


}

void PyHelper::init()
{
	Py_Initialize();

	if(!Py_IsInitialized()){
		TRACE0("python初始化失败");
		if(PyErr_Occurred()){
			PyErr_Print();
			PyErr_Clear();
		}
		MessageBox(NULL,_T("Python INIT"),_T("Python init Error"),MB_OK);
		return;
	}

	PyRun_SimpleString("import os");
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('.\\scripts')");
	
	if(PyErr_Occurred()){
		printExceptionMessage();
		PyErr_Clear();
	}
	TRACE1(_T("Py_GetPath :%s\n"),Py_GetPath());
	TRACE1(_T("Py_GetProgramFullPath :%s\n"),Py_GetProgramFullPath());
	TRACE1(_T("Py_GetProgramName :%s\n"),Py_GetProgramName());
}

CString PyHelper::getIManhuaVolPages( CString h )
{
	init();
	CString result;

	PyObject* imanhua=PyImport_ImportModule("imanhua");
	if (imanhua==NULL||PyErr_Occurred())
	{
		TRACE0(_T("imanhua 加载失败"));
		return "";
	}
	PyObject* getPages= NULL;
	
	try
	{
		getPages=PyObject_GetAttrString(imanhua,"getPages");
		if(PyFunction_Check(getPages)){
			TRACE0(_T("PyFunction getPages is OK\n"));
		}
	}
	catch (exception &e)
	{
		TRACE1("Python exception %s\n",e.what());
		MessageBox(NULL,e.what(),_T("Python Error"),MB_OK);
	}

	if(PyErr_Occurred()){
		PyErr_Print();
		PyErr_Clear();
	}

	PyObject* resp=NULL;
	try
	{
		resp=PyObject_CallFunction(getPages,"s",(LPCTSTR)h);
		//resp=PyObject_CallFunction(getPages,"s","aaa");

		if(PyErr_Occurred()){

			printExceptionMessage();
			PyErr_Clear();
		}
	}catch (exception &e)
	{
		TRACE1("Python exception %s\n",e.what());
		MessageBox(NULL,e.what(),_T("Python Error"),MB_OK);

	}catch(...){
		MessageBox(NULL,_T("Python 初始化异常"),_T("Python Error"),MB_OK);
	}

	if(resp==NULL){
		Py_DECREF(resp);
		Py_DECREF(getPages);

	}else{
		result=PyString_AsString(resp);
		Py_DECREF(resp);
		Py_DECREF(getPages);

	}
	Py_Finalize();
	return result;
	
}

string PyHelper::getSimpleResp( CString input,CString moduleName,CString funcName )
{
	Py_Initialize();

	if(!Py_IsInitialized()){
		TRACE0("python初始化失败");
		if(PyErr_Occurred()){
			PyErr_Print();
			PyErr_Clear();
		}
		MessageBox(NULL,_T("Python INIT"),_T("Python init Error"),MB_OK);
		return "";
	}

	PyRun_SimpleString("import os");
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('.\\scripts')");

	if(PyErr_Occurred()){
		printExceptionMessage();
		PyErr_Clear();
	}
	TRACE1(_T("Py_GetPath :%s\n"),Py_GetPath());
	TRACE1(_T("Py_GetProgramFullPath :%s\n"),Py_GetProgramFullPath());
	TRACE1(_T("Py_GetProgramName :%s\n"),Py_GetProgramName());
	string result;

	PyObject* pyModule=PyImport_ImportModule(moduleName);
	if (pyModule==NULL||PyErr_Occurred())
	{
		TRACE0(_T("imanhua 加载失败"));
		return "";
	}
	PyObject* pyFunc= NULL;

	try
	{
		pyFunc=PyObject_GetAttrString(pyModule,funcName);
		if(PyFunction_Check(pyFunc)){
			TRACE1(_T("PyFunction %s is OK\n"),funcName);
		}
	}
	catch (exception &e)
	{
		TRACE1("Python exception %s\n",e.what());
		MessageBox(NULL,e.what(),_T("Python Error"),MB_OK);
	}

	if(PyErr_Occurred()){
		PyErr_Print();
		PyErr_Clear();
	}

	PyObject* resp=NULL;
	try
	{
		resp=PyObject_CallFunction(pyFunc,"s",(LPCTSTR)input);
		//resp=PyObject_CallFunction(getPages,"s","aaa");

		if(PyErr_Occurred()){

			printExceptionMessage();
			PyErr_Clear();
		}
	}catch (exception &e)
	{
		TRACE1("Python exception %s\n",e.what());
		MessageBox(NULL,e.what(),_T("Python Error"),MB_OK);

	}catch(...){
		MessageBox(NULL,_T("Python 初始化异常"),_T("Python Error"),MB_OK);
	}

	if(resp==NULL){
		Py_DECREF(resp);
		Py_DECREF(pyFunc);

	}else{
		result=PyString_AsString(resp);
		Py_DECREF(resp);
		Py_DECREF(pyFunc);

	}
	Py_Finalize();
	return result;
}

CString PyHelper::getXxbhSvcListJs()
{
	Py_Initialize();

	if(!Py_IsInitialized()){
		TRACE0("python初始化失败");
		if(PyErr_Occurred()){
			PyErr_Print();
			PyErr_Clear();
		}
		MessageBox(NULL,_T("Python INIT"),_T("Python init Error"),MB_OK);
		return "";
	}

	PyRun_SimpleString("import os");
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('.\\scripts')");

	if(PyErr_Occurred()){
		printExceptionMessage();
		PyErr_Clear();
	}
	TRACE1(_T("Py_GetPath :%s\n"),Py_GetPath());
	TRACE1(_T("Py_GetProgramFullPath :%s\n"),Py_GetProgramFullPath());
	TRACE1(_T("Py_GetProgramName :%s\n"),Py_GetProgramName());
	CString result;

	PyObject* pyModule=PyImport_ImportModule("xxbh_s");
	if (pyModule==NULL||PyErr_Occurred())
	{
		TRACE0(_T("imanhua 加载失败"));
		return "";
	}
	PyObject* pyAttr= NULL;

	try
	{
		pyAttr=PyObject_GetAttrString(pyModule,"svrListJs");
		
	}
	catch (exception &e)
	{
		TRACE1("Python exception %s\n",e.what());
		MessageBox(NULL,e.what(),_T("Python Error"),MB_OK);
	}

	if(PyErr_Occurred()){
		PyErr_Print();
		PyErr_Clear();
	}

	
	if(pyAttr==NULL){
		Py_DECREF(pyAttr);
	}else{
		result=PyString_AsString(pyAttr);
		Py_DECREF(pyAttr);
	}
	Py_Finalize();
	return result;
}
