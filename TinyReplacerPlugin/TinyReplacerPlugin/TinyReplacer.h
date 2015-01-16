#pragma once
class TinyReplacer
{
private:
	
	string config_file_name;
	
	map<string,map<string,string> > tplt;
	string user;
	string conn;
public:
	TinyReplacer(void);
	~TinyReplacer(void);
	void init();
	string replace(string s);
	string removeSchema(string s);
	string replaceStatic(string s, map<string,string>& t);
	string replaceDynamic(string s,map<string,string>& t);
	string replaceStaticItem(string s, string from, string to );
	string replaceString(string s, string from, string to );
	string getPathByFullName(string fullName);
	void setUser(string);
	void setConn(string);
	static string toUpper(string);
	
};

