#ifndef _UTILITY_H_20110220
#define _UTILITY_H_20110220


class Utility {
    
    public:
        
        static const string formatDate(const string &date);
        static const int getCycleId(const string &date);
        static const string getSysdate();
        static const string getSysTagInfo(DataBuf &tagInfo,const string sysCode,const string tagCode,const string tagType,const string def,const string strEparchyCode);//获取系统TAG
        static const void GetRandPassword(string &password);//生成随机密码
        static const DataBuf FilterSameValue(DataBuf &Info,const string &key);//过滤重复关键字信息
        static const DataBuf FilterKeyValue(DataBuf &Info,const string &key,const string &value);//过滤除关键字值后的数据
        static const DataBuf UnFilterKeyValue(DataBuf &Info,const string &key,const string &value);//过滤关键字值后的数据
    	static const void TransLcuFlow(DataBuf &inBuf,DataBuf &outBuf);//调用LCU流程
    	
    	static const void TransLcuFlow(PCFmlBuf &inBuf,PCFmlBuf &outBuf);//调用LCU流程
    	static const int callLCU(CFmlBuf &inBuf,CFmlBuf &outBuf); // 动态tuxedo调用
    	//static const int TransXmlToDataBuf(DOMDocument *document,DataBuf &xmlDbuf);
    	static const string TransDataBufToXmlString(DataBuf &xmlDbuf);
    	/**
      * 对O侧返回的时间数据进行预处理,去掉毫秒部分
      * CRM-IOM接口文档7.3.1数据类型描述:
      * 日期时间类型数据统一用DT表示，长度为23，精度到毫秒，毫秒保留3位，
      * 年月日用“-”分隔,时分秒毫秒之间统一用“:”分隔，年月日与时分秒之间空格分隔，
      * 如：2008-09-02 10:20:30:000
      */
    	static const string pretreatIOMDate(string strInDate);

		static bool isGBK(unsigned char head, unsigned char tail);

		static void geneEncodeStr(string &strRet,CFmlBuf &outBuf);

		static const string getEncodeStr(CFmlBuf &buf) ;
};

#endif //