#ifndef _FileDeal_H_
#define _FileDeal_H_


#include "InfoPublic.h"


/**
* FileDeal:输入文件类,根据输入文件读取文件记录。
* 	FileDeal:		输入文件类构造函数
*	~FileDeal:		输入文件类析构函数
*	setfilepath:	设置输入文件目录
*	setbakfilepath:	设置输入文件备份目录
*	setfilename:	设置输入文件文件名称
*	setinfolen:		设置系统级资料信息记录列数
*	setloadtables:	设置系统级资料信息记录列数
*	setseperator:	设置输入文件文件分隔符
*	getsrcdbnum:	获取文件来源数据库编码
*	open:			打开输入文件
*	close:			关闭输入文件
*	remove:			移除输入文件
*	getInfo:		获取文件内的记录信息
*   getRefreshTag:	文件处理是否导致参数重新刷新
*/
class FileDeal
{
	public:
		
		/**
		* FileDeal:输入文件类构造函数
		* @param	无
		* @return
		*/
		FileDeal();

		/**
		* ~FileDeal:输入文件类析构函数
		* @param	无
		* @return
		*/
		~FileDeal();
		
		/**
		* setfilepath:设置输入文件目录
		* @param	filepath	输入文件目录
		* @return	无
		*/
		void	setfilepath(const char* filepath);

		/**
		* setbakfilepath:设置输入文件备份目录
		* @param	bakfilepath	输入文件备份目录
		* @return	无
		*/
		void	setbakfilepath(const char* bakfilepath);

		/**
		* setfilename:设置输入文件文件名称
		* @param	filename	输入文件文件名称
		* @return	无
		*/				
		void	setfilename(const char* filename);

		/**
		* setinfolen:设置系统级资料信息记录列数
		* @param	infolens	指向系统级资料信息记录列数的指针
		* @return	无
		*/
		void	setinfolen(int* infolens);
		
		/**
		* setloadtables:设置系统级资料装载信息
		* @param	loadtables	指向资料装载信息数组的指针
		* @return	无
		*/	
		void	setloadtables(LoadTable* loadtables);
		
		/**
		* setseperator:设置输入文件文件分隔符
		* @param	seperator	指向输入文件文件分隔符的指针
		* @return	无
		*/		
		void	setseperator(const char* seperator);

		/**
		* getsrcdbnum:获取文件来源数据库编码
		* @param	无
		* @return	文件类型 0:增量的增量 1:增量的全量 2:全量的全量
		*/		
		int		getsrcdbnum();
		
		/**
		* open:打开输入文件(抛异常)
		* @param	无
		* @return	无
		*/				
		void	open();

		/**
		* close:关闭输入文件(抛异常)
		* @param	无
		* @return	无
		*/		
		void	close();

		/**
		* unlink:移除输入文件(抛异常)
		* @param	log			日志信息
		* @return	无
		*/
		void	remove();

		/**
		* getIIInfo:获取增量的增量信息(抛异常)
		* @param	chgid		变更流水号			
		* @param	info		资料信息
		* @param	log			日志信息
		* @return	执行结果 true:成功 false:失败,无记录
		*/
		int		getInfo(string &chgid, int &infotype, Info &info, char* errrecord);
		
		/**
		* getRefreshTag:获取该文件处理过程中是否要修改参数重刷标志
		* @param	无		
		* @return	返回是否要修改参数重刷标志 0:无需刷新 1:需要刷新
		*/
		int		getRefreshTag();
		
	private:

		/**
		* parse:将一行记录从字符串转换成Record型
		* @param	filetype	文件类型
		* @param	line		一行记录(字符串型)
		* @param	record		一行记录(Record型)
		* @return	执行结果 true:成功 false:失败,记录存在文件属于错误记录
		*/
		bool	parse(char* line, Record &record);

		/**
		* getfilefullname:根据输入的路径和文件名获取文件全名
		* @param	filepath		路径
		* @param	filename		文件名
		* @param	filefullname	文件全名
		* @return	无
		*/		
		void	getfilefullname(const char* filepath, const char* filename, char* filefullname);
	
	private:
	
		//资料文件输入路径、备份路径、文件名称、文件分割符号
		char			m_filepath[MAX_FILEPATH_LEN];
		char			m_bakfilepath[MAX_FILEPATH_LEN];
		char			m_filename[MAX_FILENAME_LEN];
		char			m_seperator[MAX_SEPERATOR_LEN];
		
		//资料文件对应的文件流
		FILE			*m_fp;

		//资料文件当前读取到的最后一条记录的信息
		string			m_lastchgid;
		int				m_lastinfotype;
		Record			*m_lastrecord;
		
		//资料文件长度信息
		int				*m_infolens;
		//资料文件装载信息
		LoadTable		*m_loadtables;
		
		//资料文件是否导致参数重新刷新 0:无需刷新 1:需要刷新
		int				m_refreshTag;
};


#endif

