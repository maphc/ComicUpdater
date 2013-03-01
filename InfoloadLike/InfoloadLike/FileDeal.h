#ifndef _FileDeal_H_
#define _FileDeal_H_


#include "InfoPublic.h"


/**
* FileDeal:�����ļ���,���������ļ���ȡ�ļ���¼��
* 	FileDeal:		�����ļ��๹�캯��
*	~FileDeal:		�����ļ�����������
*	setfilepath:	���������ļ�Ŀ¼
*	setbakfilepath:	���������ļ�����Ŀ¼
*	setfilename:	���������ļ��ļ�����
*	setinfolen:		����ϵͳ��������Ϣ��¼����
*	setloadtables:	����ϵͳ��������Ϣ��¼����
*	setseperator:	���������ļ��ļ��ָ���
*	getsrcdbnum:	��ȡ�ļ���Դ���ݿ����
*	open:			�������ļ�
*	close:			�ر������ļ�
*	remove:			�Ƴ������ļ�
*	getInfo:		��ȡ�ļ��ڵļ�¼��Ϣ
*   getRefreshTag:	�ļ������Ƿ��²�������ˢ��
*/
class FileDeal
{
	public:
		
		/**
		* FileDeal:�����ļ��๹�캯��
		* @param	��
		* @return
		*/
		FileDeal();

		/**
		* ~FileDeal:�����ļ�����������
		* @param	��
		* @return
		*/
		~FileDeal();
		
		/**
		* setfilepath:���������ļ�Ŀ¼
		* @param	filepath	�����ļ�Ŀ¼
		* @return	��
		*/
		void	setfilepath(const char* filepath);

		/**
		* setbakfilepath:���������ļ�����Ŀ¼
		* @param	bakfilepath	�����ļ�����Ŀ¼
		* @return	��
		*/
		void	setbakfilepath(const char* bakfilepath);

		/**
		* setfilename:���������ļ��ļ�����
		* @param	filename	�����ļ��ļ�����
		* @return	��
		*/				
		void	setfilename(const char* filename);

		/**
		* setinfolen:����ϵͳ��������Ϣ��¼����
		* @param	infolens	ָ��ϵͳ��������Ϣ��¼������ָ��
		* @return	��
		*/
		void	setinfolen(int* infolens);
		
		/**
		* setloadtables:����ϵͳ������װ����Ϣ
		* @param	loadtables	ָ������װ����Ϣ�����ָ��
		* @return	��
		*/	
		void	setloadtables(LoadTable* loadtables);
		
		/**
		* setseperator:���������ļ��ļ��ָ���
		* @param	seperator	ָ�������ļ��ļ��ָ�����ָ��
		* @return	��
		*/		
		void	setseperator(const char* seperator);

		/**
		* getsrcdbnum:��ȡ�ļ���Դ���ݿ����
		* @param	��
		* @return	�ļ����� 0:���������� 1:������ȫ�� 2:ȫ����ȫ��
		*/		
		int		getsrcdbnum();
		
		/**
		* open:�������ļ�(���쳣)
		* @param	��
		* @return	��
		*/				
		void	open();

		/**
		* close:�ر������ļ�(���쳣)
		* @param	��
		* @return	��
		*/		
		void	close();

		/**
		* unlink:�Ƴ������ļ�(���쳣)
		* @param	log			��־��Ϣ
		* @return	��
		*/
		void	remove();

		/**
		* getIIInfo:��ȡ������������Ϣ(���쳣)
		* @param	chgid		�����ˮ��			
		* @param	info		������Ϣ
		* @param	log			��־��Ϣ
		* @return	ִ�н�� true:�ɹ� false:ʧ��,�޼�¼
		*/
		int		getInfo(string &chgid, int &infotype, Info &info, char* errrecord);
		
		/**
		* getRefreshTag:��ȡ���ļ�����������Ƿ�Ҫ�޸Ĳ�����ˢ��־
		* @param	��		
		* @return	�����Ƿ�Ҫ�޸Ĳ�����ˢ��־ 0:����ˢ�� 1:��Ҫˢ��
		*/
		int		getRefreshTag();
		
	private:

		/**
		* parse:��һ�м�¼���ַ���ת����Record��
		* @param	filetype	�ļ�����
		* @param	line		һ�м�¼(�ַ�����)
		* @param	record		һ�м�¼(Record��)
		* @return	ִ�н�� true:�ɹ� false:ʧ��,��¼�����ļ����ڴ����¼
		*/
		bool	parse(char* line, Record &record);

		/**
		* getfilefullname:���������·�����ļ�����ȡ�ļ�ȫ��
		* @param	filepath		·��
		* @param	filename		�ļ���
		* @param	filefullname	�ļ�ȫ��
		* @return	��
		*/		
		void	getfilefullname(const char* filepath, const char* filename, char* filefullname);
	
	private:
	
		//�����ļ�����·��������·�����ļ����ơ��ļ��ָ����
		char			m_filepath[MAX_FILEPATH_LEN];
		char			m_bakfilepath[MAX_FILEPATH_LEN];
		char			m_filename[MAX_FILENAME_LEN];
		char			m_seperator[MAX_SEPERATOR_LEN];
		
		//�����ļ���Ӧ���ļ���
		FILE			*m_fp;

		//�����ļ���ǰ��ȡ�������һ����¼����Ϣ
		string			m_lastchgid;
		int				m_lastinfotype;
		Record			*m_lastrecord;
		
		//�����ļ�������Ϣ
		int				*m_infolens;
		//�����ļ�װ����Ϣ
		LoadTable		*m_loadtables;
		
		//�����ļ��Ƿ��²�������ˢ�� 0:����ˢ�� 1:��Ҫˢ��
		int				m_refreshTag;
};


#endif

