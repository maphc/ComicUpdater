
#include "stdafx.h"
#include "TinyReplacer.h"

void TinyReplacer::init(){
	char curr_dir[4096]={0};
	GetCurrentDirectory(4096,curr_dir);
	config_file_name=curr_dir;
	config_file_name+="\\PlugIns\\TinyReplacer\\config.ini";

	ifstream f(config_file_name);

	string section_name;
	while(!f.eof()){
		string line;
		line.reserve(256);
		getline(f,line);
		transform(line.begin(), line.end(), line.begin(), ::toupper);
		if(line.empty()||line[0]=='#'){
			continue;
		}

		if(line[0]=='['){
			section_name=line.substr(1,line.size()-2);
		}else{
			int sep=line.find("=");

			if(sep==string::npos){
				tplt[section_name].insert(make_pair(line,string("")));

			}else{
				tplt[section_name].insert(make_pair(line.substr(0,sep),line.substr(sep+1)));

			}

		}

	}

}

TinyReplacer::TinyReplacer(){


}

string TinyReplacer::replace(string s ){
	s= replaceStatic(s,tplt.at("STATIC"));

	if(tplt.find("DYNAMIC")!=tplt.end()){
		s=replaceDynamic(s,tplt.at("DYNAMIC"));
	}

	return s;
}

string TinyReplacer::replaceDynamic(string s, map<string,string>& t ){
	string result(s);
	string schema;
	if (!user.empty() && tplt.find("USER")!=tplt.end())
	{
		for (map<string,string>::iterator iter=tplt["USER"].begin();iter!=tplt["USER"].end();iter++)
		{
			if (user.find(iter->first)!=string::npos)
			{
				schema=iter->second;
				break;
			}
		}
	}
	if(!conn.empty()&&schema.empty()&& tplt.find("CONN")!=tplt.end()){
		for (map<string,string>::iterator iter=tplt["CONN"].begin();iter!=tplt["CONN"].end();iter++)
		{
			if (user.find(iter->first)!=string::npos)
			{
				schema=iter->second;
				break;;
			}
		}
	}

	if(!schema.empty()){
		for (map<string,string>::iterator iter=t.begin();iter!=t.end();iter++)
		{
			result=replaceStaticItem(result,iter->first,schema);
		}
	}
	
	return result;

}

string TinyReplacer::replaceStatic(string s, map<string,string>& t ){
	string result(s);
	for (map<string,string>::iterator iter=t.begin();iter!=t.end();iter++)
	{
		result=replaceStaticItem(result,iter->first,iter->second);
	}

	return result;
}
string TinyReplacer::replaceStaticItem(string s, string from, string to ){
	int pos=0, last_pos=0;
	string s_upper(s);
	transform(s_upper.begin(), s_upper.end(), s_upper.begin(), ::toupper);
	string result;
	result.reserve(s.size()*4/3);
	pos=s_upper.find(from,last_pos);
	string mark="\n, ;";
	while((pos=s_upper.find(from,last_pos))!=string::npos  ){
		
		if(pos+from.size()<s.size()&& mark.find(s_upper[pos+from.size()])!=string::npos){
			bool is_already_has_owner=false;
			for(int i=pos-1;i>=0;i--){
				if(s[i]==' '||s[i]=='\n'){
					continue;
				}
				if(s[i]=='.'){
					is_already_has_owner=true;
				}
				break;
			}
			if(!is_already_has_owner&&!to.empty()){
				result+=s.substr(last_pos,(pos-last_pos));
				result.append(to+'.');
				result.append(from);
			}else{
				result.append(s.substr(pos,from.size()));
			}
			
		}else{
			result.append(s.substr(last_pos,pos-last_pos));
			result.append(s.substr(pos,from.size()));
		}
		
		

		last_pos=pos+from.size();
	}

	result.append(s.substr(last_pos));
	return result;
}


TinyReplacer::~TinyReplacer(void)
{
}
void TinyReplacer::setUser(string user)
{
	transform(user.begin(),user.end(),user.begin(),::toupper);
	this->user=user;
}
void TinyReplacer::setConn(string conn)
{
	transform(conn.begin(),conn.end(),conn.begin(),::toupper);
	this->conn=conn;
}