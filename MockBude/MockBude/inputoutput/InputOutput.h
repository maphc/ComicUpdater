#ifndef _FILE_INPUT_OUTPUT__
#define _FILE_INPUT_OUTPUT__
#include "stdafx.h"
#include "../common/StrTable.h"
#pragma once
using namespace std;


bool startswith(string&s,string& head){
	return s.compare(0,head.size(),head)==0;
}
bool endswith(string& s,string& tail){
	return s.compare(s.size()-tail.size(),s.size(),tail)==0;
}
void simplesplit(vector<string>& l,string& s){
	l.assign(istream_iterator<string>(stringstream(s)),istream_iterator<string>(stringstream()));
}

void FileInput(DataBuf& in ,string fileName){
	//fileName="z:\\input";
	ifstream ifs(fileName.c_str());
	if(!ifs.good()){
		throw new string("文件"+fileName+"不存在 ");
	}
	while(!ifs.eof()){
		string line;
		line.reserve(4000);
		getline(ifs,line);
		Trim(line);
		if(!startswith(line,string("@!"))){
			continue;
		}
		
		vector<string> l;
		simplesplit(l,line);
		if(l.size()<5){
			cout<<"line ["<<line<<"]"<<" split error , skip"<<endl;
		}
		string key(l[1]);
		int row=atoi(l[3].c_str());
		string type(l[4]);
		if(type=="long"){
			in.SetLong(key,atol(l[5].c_str()),row);
		}else if(type=="string"){
			if(l.size()==6){
				in.SetString(key,l[5],row);
			}else if(l.size()==5){
				in.SetString(key,"",row);
			}else{
				in.SetString(key,accumulate(l.begin()+5,l.end(),string()),row);
			}
			
		}else{
			throw new string("Not Supportted buf type :"+type);
		}

		
	}
	
}

void FileOutput(DataBuf& out,string fileName){
	ofstream f(fileName.c_str());
	f<<out.ToString();
	f.close();
}

#endif