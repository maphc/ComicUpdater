/*
 * MyQuery.h
 *
 *  Created on: 2009-2-26
 *      Author: Administrator
 */
#include "stdafx.h"

#ifndef MYQUERY_H_
#define MYQUERY_H_

class MyQuery {
public:
	MyQuery(string src_file_name);
	virtual ~MyQuery();
	void read_file(){
		this->open_file();
		this->store_file();
		this->build_map();
	}
	void getInfoByWord(const string& word);
	void getAllInfo();
private:
	ifstream fis;
	string src_file_name;
	vector<string> src_content;
	map<string ,set<int> > lines_of_text;
	ifstream& open_file();
	void build_map();
	void store_file();
	void print_info(set<int> num_info,const string& word);

};

#endif /* MYQUERY_H_ */
