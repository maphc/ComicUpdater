#include "stdafx.h"


FileDeal::FileDeal()
{
	m_fp = NULL;

	m_lastchgid = "";	
	m_lastinfotype = -1;
	m_lastrecord = NULL;
	
	m_infolens = NULL;
	m_loadtables = NULL;
}


FileDeal::~FileDeal()
{
	if (m_lastrecord != NULL)
	{
		delete m_lastrecord;
		m_lastrecord = NULL;
	}
	
	if (m_fp != NULL)
	{
		close();
	}
}


void FileDeal::setfilepath(const char* filepath)
{
	strcpy(m_filepath, filepath);
}


void FileDeal::setbakfilepath(const char* bakfilepath)
{
	strcpy(m_bakfilepath, bakfilepath);
}


void FileDeal::setfilename(const char* filename)
{
	strcpy(m_filename, filename);
	m_refreshTag = 0;
}


void FileDeal::setinfolen(int* infolens)
{
	m_infolens = infolens;
}


void FileDeal::setloadtables(LoadTable* loadtables)
{
	m_loadtables = loadtables;
}


void FileDeal::setseperator(const char* seperator)
{
	strcpy(m_seperator, seperator);
}


int	FileDeal::getsrcdbnum()
{
	 return (m_filename[0]-'0');
}


void FileDeal::open()
{
	m_lastchgid = "";	
	m_lastinfotype = -1;
	m_lastrecord = NULL;

	//获取资料文件全名
	char filefullname[MAX_FILENAME_LEN];
	getfilefullname(m_filepath, m_filename, filefullname); 
	
	//打开资料文件
	int i = 0;
	label1:
	if ((m_fp=fopen(filefullname, "rb")) == NULL)
	{
		++i;
		//sleep(5);
		if (i == 5)
		{
			throw CException(__FILE__, __LINE__, "打开资料文件出错,文件名称:=%s", filefullname);
		}
		else
		{
			goto label1;
		}
	}
}


void FileDeal::close()
{
	//关闭资料文件流
	if(m_fp != NULL)
	{
		if (fclose(m_fp) != 0)
		{
			throw CException(__FILE__, __LINE__, "关闭资料文件流出错,文件名称:=%s", m_filename);
		}
		m_fp = NULL;
	}
}


void FileDeal::remove()
{
	//原始文件移动到备份目录下面
	char filefullname[MAX_FILENAME_LEN];
	char bakfilefullname[MAX_FILENAME_LEN];
	getfilefullname(m_filepath, m_filename, filefullname);
	getfilefullname(m_bakfilepath, m_filename, bakfilefullname);
	if (rename(filefullname, bakfilefullname) != 0)
	{
		throw CException(__FILE__, __LINE__, "移除文件的时候出错,原文件名称:=%s,目标文件名称为:=%s", filefullname, bakfilefullname);
	}
}


int	FileDeal::getInfo(string &chgid, int &infotype, Info &info, char* errrecord)
{
	int iRecNum = 0;
	
	//值初始化
	chgid = "";
	infotype = -1;
//	for (int i=0; i<info.size(); ++i)
//	{
//		delete info[i];
//	}
	info.clear();
	
	//如果本次获取记录不是这个文件的第一次
	if (m_lastrecord != NULL)
	{
		//记录数自动计数
		++iRecNum;
		
		chgid = m_lastchgid;
		infotype = m_lastinfotype;
		info.push_back(m_lastrecord);

		m_lastrecord = NULL;
	}
	
	//获取记录
	string	tmpchgid = "";
	int		tmpinfotype = -1;
	char line[MAX_FILERECORD_LEN] = "\0";
	while(fgets(line, MAX_FILERECORD_LEN, m_fp) != NULL)
	{	
		//去除最后一个换行符合
		if (line[strlen(line)-1] == '\n')
		{
				line[strlen(line)-1] = '\0';
		}
			
		//分析获取到的记录数据，如果错误，直接跳出
		m_lastrecord = new Record();
		if (!parse(line, *m_lastrecord))
		{
			delete m_lastrecord;
			m_lastrecord = NULL;
			strcpy(errrecord, line);
			iRecNum = -1;
			break;
		}
		
		//如果数据正常,继续下面处理
		string tmpchgid = (*m_lastrecord)[0];
		int    tmpinfotype = atoi((*m_lastrecord)[1]);
		//该资料信息会导值参数重刷
		if (m_loadtables[tmpinfotype].m_triggerflag == '1')
		{
			m_refreshTag = 1;
		}
		
		//如果读取的是文件首记录		
		if (m_lastchgid == "")
		{
			//记录数自动计数
			++iRecNum;
		
			chgid = tmpchgid;
			infotype = tmpinfotype;
			info.push_back(m_lastrecord);
			
			m_lastchgid = tmpchgid;
			m_lastinfotype = tmpinfotype;
			m_lastrecord = NULL;
		}
		//如果读取的记录和上条记录同变更流水+同信息类型
		else if (m_lastchgid==tmpchgid && m_lastinfotype==tmpinfotype)
		{
			//记录数自动计数
			++iRecNum;
		
			info.push_back(m_lastrecord);
			
			m_lastrecord = NULL;
		}
		//如果读取的记录和上条记录变更流水+信息类型不同
		else
		{
			m_lastchgid = tmpchgid;
			m_lastinfotype = tmpinfotype;
			
			break;
		}
	}
	
	return iRecNum;
}


int	FileDeal::getRefreshTag()
{
	return m_refreshTag;
}


bool FileDeal::parse(char* line, Record &record)
{
	//将字符串解析成Record
	string strline = line;
	
	string strsep = m_seperator;
	vector<string> sVector;
	StringUtil::split(strline, strsep, sVector, false);
	
	record.clear();
	for (int i=0; i<sVector.size(); ++i)
	{
		record.push_back(sVector[i].c_str());
	}

	//对Record字段进行检查
	int infotype = atoi(record[1]);
	if (infotype<MIN_INFOTYPE_VALUE || infotype>MAX_INFOTYPE_VALUE)
	{
		return false;
	}

	if (record.size() != INFO_ADDLEN+m_infolens[infotype])
	{
		return false;
	}

	return true;
}


void FileDeal::getfilefullname(const char* filepath, const char* filename, char* filefullname)
{
	if (filepath[strlen(filepath)-1] != '\\')
	{
		sprintf(filefullname, "%s\\%s",filepath, filename);
	}
	else
	{
		sprintf(filefullname, "%s%s",filepath, filename);
	}
}

