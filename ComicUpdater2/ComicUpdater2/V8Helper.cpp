#include "stdafx.h"
#include "V8Helper.h"
using namespace std;
using namespace v8;

namespace V8Helper{
	CString get178VolPages(CString h){
		//v8::Locker locker;
		// Get the default Isolate created at startup.
		string pyResp=PyHelper::getSimpleResp(h,"manhua178_s","getPages");
		if(pyResp.c_str()==""){
			return "";
		}
		Isolate* isolate = Isolate::New();
		
		Isolate::Scope iscope(isolate);
		
		//TryCatch trycatch;
		// Create a stack-allocated handle scope.
		HandleScope handle_scope(isolate);
		
		// Create a new context.
		Persistent<Context> context = Context::New();

		if(context.IsEmpty()){
			TRACE0("context is empty");
		}else{
			TRACE0("context is not empty");
		}

		// Enter the created context for compiling and
		// running the hello world script. 
		Context::Scope context_scope(context);

		//execute the encrypt code
		Script::Compile(String::New(pyResp.c_str()))->Run();
		/*if (trycatch.HasCaught())
		{
			Handle<Value> exception = trycatch.Exception();    
			String::AsciiValue exception_str(exception);    
			TRACE1("Exception: %s\n", *exception_str);
			return "";
		}*/
		//get the result
		Handle<Value> v8Resp = Script::Compile(String::New("pages"))->Run();
		/*if (trycatch.HasCaught())
		{
			Handle<Value> exception = trycatch.Exception();    
			String::AsciiValue exception_str(exception);    
			TRACE1("Exception: %s\n", *exception_str);
			return "";
		}*/

		
		// Dispose the persistent context.
		context.Dispose(isolate);

		// Convert the result to an ASCII string and print it.
		CString result(*String::AsciiValue(v8Resp));

		return result;
	}
	CString getIManhuaVolPages(CString h){
		string pyResp=PyHelper::getSimpleResp(h,"imanhua_s","getPages");
		if(pyResp.c_str()==""){
			return "";
		}
		Isolate* isolate = Isolate::New();
		Isolate::Scope iscope(isolate);

		// Create a stack-allocated handle scope.
		HandleScope handle_scope(isolate);
		// Create a new context.
		Persistent<Context> context = Context::New();

		// Enter the created context for compiling and
		// running the hello world script. 
		Context::Scope context_scope(context);

		//execute the encrypt code
		Script::Compile(String::New(pyResp.c_str()))->Run();

		//get the result
		Handle<Value> v8bid = Script::Compile(String::New("cInfo.bid"))->Run();
		Handle<Value> v8cid = Script::Compile(String::New("cInfo.cid"))->Run();
		Handle<Value> v8files = Script::Compile(String::New("cInfo.files.toString()"))->Run();

		int bid=v8bid->Int32Value();
		int cid=v8cid->Int32Value();
		CString result;
		vector<CString> filesList=Downloader::Split(CString(*String::AsciiValue(v8files)),",");
		
		for (UINT i=0;i<filesList.size();i++)
		{
			CString file(filesList[i]);
			result.AppendFormat("http://c4.mangafiles.com/Files/Images/%d/%d/%s",bid,cid,file);
			if(i!=filesList.size()-1){
				result.Append(",");
			}
		}
		// Dispose the persistent context.
		context.Dispose(isolate);

		// Convert the result to an ASCII string and print it.


		return result;
	}
}