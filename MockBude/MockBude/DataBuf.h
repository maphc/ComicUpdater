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
        
        
        //! 默认构造函数
        DataBuf();
        
        //! 复制构造
        DataBuf(const DataBuf &buf);
        
        //! 解析构造.  (对序列化后的字符传进行解析)
        DataBuf(const string &str);
        
        const int GetDataType() const;
        
        //保存字符串型数据
        //param name  数据的名称
        //param value 数据值
        //param pos   在buffer中的位置 默认在第一行
        void SetString(const string &name, const string &value, const int &pos=0);
        
        //保存int型数据
        //param name  数据的名称
        //param value 数据值
        //param pos   在buffer中的位置 默认在第一行
        void SetInt(const string &name, const int &value, const int &pos=0);
        
        //保存long型数据
        //param name  数据的名称
        //param value 数据值
        //param pos   在buffer中的位置 默认在第一行
        void SetLong(const string &name, const long &value, const int &pos=0);
        
        //保存float型数据
        //param name  数据的名称
        //param value 数据值
        //param pos   在buffer中的位置 默认在第一行
        void SetFloat(const string &name, const float &value, const int &pos=0);
        
        //保存double型数据
        //param name  数据的名称
        //param value 数据值
        //param pos   在buffer中的位置 默认在第一行
        void SetDouble(const string &name, const double &value, const int &pos=0);
        
        //保存 DataBuf 型数据
        //param name  数据的名称
        //param value 数据值
        //param pos   在buffer中的位置 默认在第一行
        void SetBuf(const string &name, const DataBuf &value, const int &pos=0);
        
        //获取 string 型数据
        //param name  数据的名称
        //param pos   在buffer中的位置 默认取第一行
        const string GetString(const string &name, const int &pos=0) const;
        
        //获取 int 型数据
        //param name  数据的名称
        //param pos   在buffer中的位置 默认取第一行
        const int GetInt(const string &name, const int &pos=0) const;
        
        //获取 long 型数据
        //param name  数据的名称
        //param pos   在buffer中的位置 默认取第一行
        const long GetLong(const string &name, const int &pos=0) const;
        
        //获取 double 型数据
        //param name  数据的名称
        //param pos   在buffer中的位置 默认取第一行
        const double GetDouble(const string &name, const int &pos=0) const;
        
        //获取 float 型数据
        //param name  数据的名称
        //param pos   在buffer中的位置 默认取第一行
        const float GetFloat(const string &name, const int &pos=0) const;
        
        //获取 DataBuf 型数据
        //param name  数据的名称
        //param pos   在buffer中的位置 默认取第一行
        DataBuf& GetBuf(const string &name, const int &pos=0) const;
        
        //将DataBuf的内容编码成字符串输出
        const string ToString() ;
        
        //DataBuf中的记录数
        int size() const ;
        
        //判断某个名称的数据是否存在
        bool IsFldExist(const string &name, const int &pos = 0) const ;
        
        //删除指定行某个名称的数据  默认删除第一行  .返回值为删除的个数
        int DelFld(const string &name, const int &pos = 0) ;
        
        //删除所有行中某个名称的数据.不递归 .返回值为删除的个数
        int DelAllFld(const string &name) ;
        
        //判断 DataBuf 是否没有数据
        bool IsEmpty() const ;
        
        //返回某个名称的数据在 buff 中存在的个数
        int GetCount(const string &name) const ;
        
        //把另一个 DataBuf 中的数据,添加到自己的数据中
        DataBuf & Append(const DataBuf & right) ;
        
        //把另一个 DataBuf 中的一行数据,添加到自己的一行数据中
        DataBuf & CoverLine(const int &targetLine, const DataBuf &right, const int &sourceLine) ;
        
        //用给定的 DataBuf 覆盖自己
        DataBuf & Cover(const DataBuf &right) ;
        
        //! 解析构造.  (将CFmlBuf格式进行解析转换)
        //DataBuf(CFmlBuf & fml);
        
        //转换为 CFmlBuf 格式
        //int ConvertToFml(CFmlBuf &fml) const;
        
        //清除数据
        void Clear();

		void ClearFmlValue();
        
        //删除指定行  line:指定行  size:同时删除的行数，默认是1行
        DataBuf & RemoveLine(const int & line, const int &size = 1);
        
        //获取属性类型 add by zhangyangshuo
        const int  GetFldType(const string &name, const int &pos=0) const;
		
		// 适配cfmlbuf
		void printFmlBuffer();
};



};  //namespace databuf

using namespace databuf;
typedef DataBuf CFmlBuf;

#endif //_DATA_BUF_H_20110106
