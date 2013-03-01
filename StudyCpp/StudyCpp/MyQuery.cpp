/*
 * MyQuery.cpp
 *
 *  Created on: 2009-2-26
 *      Author: Administrator
 */

#include "MyQuery.h"

MyQuery::MyQuery(string src_file_name) {
	this->src_file_name = src_file_name;

}

MyQuery::~MyQuery() {
	fis.close();
	fis.clear();
}

ifstream & MyQuery::open_file() {
	fis.close();
	fis.clear();
	fis.open(this->src_file_name.c_str());
	if(!fis){
		cout<<"Can't open file"<<src_file_name<<endl;
	}else{
		cout<<"File "<<src_file_name<<" is ready "<<endl;
	}
	return fis;
}

void MyQuery::build_map() {
	string word;
	for(vector<string>::size_type i=0;i<this->src_content.size();i++){
		istringstream iss(src_content[i]);
		while(iss>>word){
			this->lines_of_text[word].insert(i);
		}
		
	}

	cout<<"total :"<<lines_of_text.size()<<" words"<<endl;

}

void MyQuery::getInfoByWord(const string & word)
{
	map<string,set<int> >::const_iterator iter=this->lines_of_text.find(word);
	if( iter!=lines_of_text.end() ){
		cout<<"Here's some infomation"<<endl;
		print_info(iter->second,word);
	}else{
		cout<<"No infomation for "<<word<<endl;
	}

}

void MyQuery::store_file()
{
	string line;
	while(getline(fis,line)){
		this->src_content.push_back(line);
	}
	cout<<"read "<<src_content.size()<<" lines"<<endl;
}

void MyQuery::getAllInfo()
{
	cout<<"print All word's infomation :"<<endl;
	//map<string,set<int> >::const_iterator iter=lines_of_text.begin();
	
	for(map<string,set<int> >::const_iterator iter=lines_of_text.begin();iter!=lines_of_text.end();iter++){

		print_info(iter->second,iter->first);
	}
	

}

void MyQuery::print_info(set<int> num_info,const string& word)
{
	set<int>::size_type num=num_info.size();
	cout<<word<<" occurs "<<num<<" time(s)"<<endl;
	for(set<int>::const_iterator iter=num_info.begin();iter!=num_info.end();iter++){
		cout<<"\tin line "<<(*iter)+1<<" :"<<src_content[*iter]<<endl;
	}

}





