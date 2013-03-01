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

	//��ȡ�����ļ�ȫ��
	char filefullname[MAX_FILENAME_LEN];
	getfilefullname(m_filepath, m_filename, filefullname); 
	
	//�������ļ�
	int i = 0;
	label1:
	if ((m_fp=fopen(filefullname, "rb")) == NULL)
	{
		++i;
		//sleep(5);
		if (i == 5)
		{
			throw CException(__FILE__, __LINE__, "�������ļ�����,�ļ�����:=%s", filefullname);
		}
		else
		{
			goto label1;
		}
	}
}


void FileDeal::close()
{
	//�ر������ļ���
	if(m_fp != NULL)
	{
		if (fclose(m_fp) != 0)
		{
			throw CException(__FILE__, __LINE__, "�ر������ļ�������,�ļ�����:=%s", m_filename);
		}
		m_fp = NULL;
	}
}


void FileDeal::remove()
{
	//ԭʼ�ļ��ƶ�������Ŀ¼����
	char filefullname[MAX_FILENAME_LEN];
	char bakfilefullname[MAX_FILENAME_LEN];
	getfilefullname(m_filepath, m_filename, filefullname);
	getfilefullname(m_bakfilepath, m_filename, bakfilefullname);
	if (rename(filefullname, bakfilefullname) != 0)
	{
		throw CException(__FILE__, __LINE__, "�Ƴ��ļ���ʱ�����,ԭ�ļ�����:=%s,Ŀ���ļ�����Ϊ:=%s", filefullname, bakfilefullname);
	}
}


int	FileDeal::getInfo(string &chgid, int &infotype, Info &info, char* errrecord)
{
	int iRecNum = 0;
	
	//ֵ��ʼ��
	chgid = "";
	infotype = -1;
//	for (int i=0; i<info.size(); ++i)
//	{
//		delete info[i];
//	}
	info.clear();
	
	//������λ�ȡ��¼��������ļ��ĵ�һ��
	if (m_lastrecord != NULL)
	{
		//��¼���Զ�����
		++iRecNum;
		
		chgid = m_lastchgid;
		infotype = m_lastinfotype;
		info.push_back(m_lastrecord);

		m_lastrecord = NULL;
	}
	
	//��ȡ��¼
	string	tmpchgid = "";
	int		tmpinfotype = -1;
	char line[MAX_FILERECORD_LEN] = "\0";
	while(fgets(line, MAX_FILERECORD_LEN, m_fp) != NULL)
	{	
		//ȥ�����һ�����з���
		if (line[strlen(line)-1] == '\n')
		{
				line[strlen(line)-1] = '\0';
		}
			
		//������ȡ���ļ�¼���ݣ��������ֱ������
		m_lastrecord = new Record();
		if (!parse(line, *m_lastrecord))
		{
			delete m_lastrecord;
			m_lastrecord = NULL;
			strcpy(errrecord, line);
			iRecNum = -1;
			break;
		}
		
		//�����������,�������洦��
		string tmpchgid = (*m_lastrecord)[0];
		int    tmpinfotype = atoi((*m_lastrecord)[1]);
		//��������Ϣ�ᵼֵ������ˢ
		if (m_loadtables[tmpinfotype].m_triggerflag == '1')
		{
			m_refreshTag = 1;
		}
		
		//�����ȡ�����ļ��׼�¼		
		if (m_lastchgid == "")
		{
			//��¼���Զ�����
			++iRecNum;
		
			chgid = tmpchgid;
			infotype = tmpinfotype;
			info.push_back(m_lastrecord);
			
			m_lastchgid = tmpchgid;
			m_lastinfotype = tmpinfotype;
			m_lastrecord = NULL;
		}
		//�����ȡ�ļ�¼��������¼ͬ�����ˮ+ͬ��Ϣ����
		else if (m_lastchgid==tmpchgid && m_lastinfotype==tmpinfotype)
		{
			//��¼���Զ�����
			++iRecNum;
		
			info.push_back(m_lastrecord);
			
			m_lastrecord = NULL;
		}
		//�����ȡ�ļ�¼��������¼�����ˮ+��Ϣ���Ͳ�ͬ
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
	//���ַ���������Record
	string strline = line;
	
	string strsep = m_seperator;
	vector<string> sVector;
	StringUtil::split(strline, strsep, sVector, false);
	
	record.clear();
	for (int i=0; i<sVector.size(); ++i)
	{
		record.push_back(sVector[i].c_str());
	}

	//��Record�ֶν��м��
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

