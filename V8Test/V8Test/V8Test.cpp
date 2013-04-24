// V8Test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <v8.h>
#include <string>
#include <iostream>
#include <fstream>

//#include <v8-debug.h>
#pragma comment(lib,"v8.lib")
using namespace v8;
using namespace std;

string pages;

Handle<Value> GetPages(Local<String> property, const AccessorInfo &info){
	return String::New(pages.c_str());
}
void SetPages(Local<String> property, Local<Value> value, const AccessorInfo& info){
	pages=*(String::Utf8Value(value));
}
int _tmain(int argc, _TCHAR* argv[])
{
	
	fstream fs("z:\\js.js");
	string input;
	input.assign(istreambuf_iterator<char>(fs.rdbuf()),istreambuf_iterator<char>());

	// Get the default Isolate created at startup.
	Isolate* isolate = Isolate::GetCurrent();

	// Create a stack-allocated handle scope.
	HandleScope handle_scope(isolate);

	
	Handle<ObjectTemplate> global_templ=ObjectTemplate::New();
	global_templ->SetAccessor(String::New("pages.a"),GetPages,SetPages);

	// Create a new context.
	Persistent<Context> context = Context::New(NULL,global_templ);

	// Enter the created context for compiling and
	// running the hello world script. 
	Context::Scope context_scope(context);

	// Create a string containing the JavaScript source code.
	Handle<String> source = String::New(input.c_str());

	// Compile the source code.
	Handle<Script> script = Script::Compile(source);
	script->Run();
	// Run the script to get the result.
	Handle<Value> result = Script::Compile(String::New("pages.a"))->Run();

	printf("pages is %s\n",pages.c_str());

	printf("result is %s\n",*(String::Utf8Value(result)));


	// Dispose the persistent context.
	context.Dispose(isolate);

	// Convert the result to an ASCII string and print it.
	//String::AsciiValue ascii(result);
	//printf("%s\n", *ascii);
	return 0;
}

