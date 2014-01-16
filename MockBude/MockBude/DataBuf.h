#ifndef _DATA_BUF_H_20110106
#define _DATA_BUF_H_20110106

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <iostream>
#include "base/SmartPtr.h"



using namespace std;


namespace databuf {
    

inline void CheckToken(const char *t, const char &c) {
    if (*t != c) throw std::runtime_error("decode Error! The token does not match. The token \""+string(1,c)+"\" is expected!");
}


class AnyData {
    
    public :
        
        virtual const int GetDataType() const = 0;
        
        virtual const string ToString() = 0;
        
        virtual ~AnyData();
};

struct pair_to_string;
struct map_to_string;
struct map_for_each;


typedef counted_ptr < AnyData > PAnyData;

typedef map<string, PAnyData>  map_Data; 

/**
 * 
 *
 */
class DataBuf:public AnyData {
    private:
        vector< map_Data > mDatas;
        
        void fill(const int &size) ;
        
    protected:
        
        
    public:
        
        void SetValue(const string &name, PAnyData any, const int &pos);
        PAnyData GetValue(const string &name, const int &pos) const ;
        const vector< map_Data > & GetData() const;  
        
        
        //! Ĭ�Ϲ��캯��
        DataBuf();
        
        //! ���ƹ���
        DataBuf(const DataBuf &buf);
        
        //! ��������.  (�����л�����ַ������н���)
        DataBuf(const string &str);
        
        const int GetDataType() const;
        
        //�����ַ���������
        //param name  ���ݵ�����
        //param value ����ֵ
        //param pos   ��buffer�е�λ�� Ĭ���ڵ�һ��
        void SetString(const string &name, const string &value, const int &pos=0);
        
        //����int������
        //param name  ���ݵ�����
        //param value ����ֵ
        //param pos   ��buffer�е�λ�� Ĭ���ڵ�һ��
        void SetInt(const string &name, const int &value, const int &pos=0);
        
        //����long������
        //param name  ���ݵ�����
        //param value ����ֵ
        //param pos   ��buffer�е�λ�� Ĭ���ڵ�һ��
        void SetLong(const string &name, const long &value, const int &pos=0);
        
        //����float������
        //param name  ���ݵ�����
        //param value ����ֵ
        //param pos   ��buffer�е�λ�� Ĭ���ڵ�һ��
        void SetFloat(const string &name, const float &value, const int &pos=0);
        
        //����double������
        //param name  ���ݵ�����
        //param value ����ֵ
        //param pos   ��buffer�е�λ�� Ĭ���ڵ�һ��
        void SetDouble(const string &name, const double &value, const int &pos=0);
        
        //���� DataBuf ������
        //param name  ���ݵ�����
        //param value ����ֵ
        //param pos   ��buffer�е�λ�� Ĭ���ڵ�һ��
        void SetBuf(const string &name, const DataBuf &value, const int &pos=0);
        
        //��ȡ string ������
        //param name  ���ݵ�����
        //param pos   ��buffer�е�λ�� Ĭ��ȡ��һ��
        const string GetString(const string &name, const int &pos=0) const;
        
        //��ȡ int ������
        //param name  ���ݵ�����
        //param pos   ��buffer�е�λ�� Ĭ��ȡ��һ��
        const int GetInt(const string &name, const int &pos=0) const;
        
        //��ȡ long ������
        //param name  ���ݵ�����
        //param pos   ��buffer�е�λ�� Ĭ��ȡ��һ��
        const long GetLong(const string &name, const int &pos=0) const;
        
        //��ȡ double ������
        //param name  ���ݵ�����
        //param pos   ��buffer�е�λ�� Ĭ��ȡ��һ��
        const double GetDouble(const string &name, const int &pos=0) const;
        
        //��ȡ float ������
        //param name  ���ݵ�����
        //param pos   ��buffer�е�λ�� Ĭ��ȡ��һ��
        const float GetFloat(const string &name, const int &pos=0) const;
        
        //��ȡ DataBuf ������
        //param name  ���ݵ�����
        //param pos   ��buffer�е�λ�� Ĭ��ȡ��һ��
        DataBuf& GetBuf(const string &name, const int &pos=0) const;
        
        //��DataBuf�����ݱ�����ַ������
        const string ToString() ;
        
        //DataBuf�еļ�¼��
        int size() const ;
        
        //�ж�ĳ�����Ƶ������Ƿ����
        bool IsFldExist(const string &name, const int &pos = 0) const ;
        
        //ɾ��ָ����ĳ�����Ƶ�����  Ĭ��ɾ����һ��  .����ֵΪɾ���ĸ���
        int DelFld(const string &name, const int &pos = 0) ;
        
        //ɾ����������ĳ�����Ƶ�����.���ݹ� .����ֵΪɾ���ĸ���
        int DelAllFld(const string &name) ;
        
        //�ж� DataBuf �Ƿ�û������
        bool IsEmpty() const ;
        
        //����ĳ�����Ƶ������� buff �д��ڵĸ���
        int GetCount(const string &name) const ;
        
        //����һ�� DataBuf �е�����,��ӵ��Լ���������
        DataBuf & Append(const DataBuf & right) ;
        
        //����һ�� DataBuf �е�һ������,��ӵ��Լ���һ��������
        DataBuf & CoverLine(const int &targetLine, const DataBuf &right, const int &sourceLine) ;
        
        //�ø����� DataBuf �����Լ�
        DataBuf & Cover(const DataBuf &right) ;
        
        //! ��������.  (��CFmlBuf��ʽ���н���ת��)
        //DataBuf(CFmlBuf & fml);
        
        //ת��Ϊ CFmlBuf ��ʽ
        //int ConvertToFml(CFmlBuf &fml) const;
        
        //�������
        void Clear();

		void ClearFmlValue();
        
        //ɾ��ָ����  line:ָ����  size:ͬʱɾ����������Ĭ����1��
        DataBuf & RemoveLine(const int & line, const int &size = 1);
        
        //��ȡ�������� add by zhangyangshuo
        const int  GetFldType(const string &name, const int &pos=0) const;
		
		// ����cfmlbuf
		void printFmlBuffer();
};



};  //namespace databuf

using namespace databuf;
typedef DataBuf CFmlBuf;

#endif //_DATA_BUF_H_20110106
