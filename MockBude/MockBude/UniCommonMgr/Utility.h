#ifndef _UTILITY_H_20110220
#define _UTILITY_H_20110220


class Utility {
    
    public:
        
        static const string formatDate(const string &date);
        static const int getCycleId(const string &date);
        static const string getSysdate();
        static const string getSysTagInfo(DataBuf &tagInfo,const string sysCode,const string tagCode,const string tagType,const string def,const string strEparchyCode);//��ȡϵͳTAG
        static const void GetRandPassword(string &password);//�����������
        static const DataBuf FilterSameValue(DataBuf &Info,const string &key);//�����ظ��ؼ�����Ϣ
        static const DataBuf FilterKeyValue(DataBuf &Info,const string &key,const string &value);//���˳��ؼ���ֵ�������
        static const DataBuf UnFilterKeyValue(DataBuf &Info,const string &key,const string &value);//���˹ؼ���ֵ�������
    	static const void TransLcuFlow(DataBuf &inBuf,DataBuf &outBuf);//����LCU����
    	
    	static const void TransLcuFlow(PCFmlBuf &inBuf,PCFmlBuf &outBuf);//����LCU����
    	static const int callLCU(CFmlBuf &inBuf,CFmlBuf &outBuf); // ��̬tuxedo����
    	//static const int TransXmlToDataBuf(DOMDocument *document,DataBuf &xmlDbuf);
    	static const string TransDataBufToXmlString(DataBuf &xmlDbuf);
    	/**
      * ��O�෵�ص�ʱ�����ݽ���Ԥ����,ȥ�����벿��
      * CRM-IOM�ӿ��ĵ�7.3.1������������:
      * ����ʱ����������ͳһ��DT��ʾ������Ϊ23�����ȵ����룬���뱣��3λ��
      * �������á�-���ָ�,ʱ�������֮��ͳһ�á�:���ָ�����������ʱ����֮��ո�ָ���
      * �磺2008-09-02 10:20:30:000
      */
    	static const string pretreatIOMDate(string strInDate);

		static bool isGBK(unsigned char head, unsigned char tail);

		static void geneEncodeStr(string &strRet,CFmlBuf &outBuf);

		static const string getEncodeStr(CFmlBuf &buf) ;
};

#endif //