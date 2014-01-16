/** Copyright (c) 2011, 联创亚信科技
 *  All rights reserved.
 * 
 *  文件名称：DataBuf.cpp
 *  文件标识：
 *  摘    要：支持数据存取及其序列化和反序列化
 * 
 *  当前版本：1.0
 *  作    者：Tangz
 *  完成日期：2011-2-18 12:40:32
 */

#include "stdafx.h"
#include "DataBuf.h"

namespace databuf {
template <typename T>
inline const string to_string1(T value, bool sign = false) {
        ostringstream buffer;
        if(sign)buffer<<showpos;
        buffer<<fixed<<value;
        return buffer.str();
}

void DataBuf::ClearFmlValue()
{
	mDatas.clear();
}

void DataBuf::printFmlBuffer()
{
	cout<<"printFmlBuffer :"<<ToString()<<endl;
}

}

AnyData::~AnyData(){
}

class Long: public AnyData {
    private:
        long mData;
        
    public:
        Long(const long&l) {
            mData = l;
        }
        
        const int GetDataType() const {
            return 'l';
        }
        
        const long GetValue() {
            return mData;
        }
        
        const string ToString() {
            return databuf::to_string1(mData);
        }
};

class Int : public AnyData {
    private:
        int mData;
    
    public:
        Int(const int &i) {
            mData = i;
        }
        
        const int GetDataType() const {
            return 'i';
        }
        
        const int GetValue() {
            return mData;
        }
        
        const string ToString() {
            return databuf::to_string1(mData);
        }
};

class Double :public AnyData {
    private:
        double mData;
        
    public:
        Double(const double &d){
            mData = d;
        }   
        
        const int GetDataType() const {
            return 'd';
        }
        
        const double GetValue() {
            return mData;
        }
        
        const string ToString() {
            return databuf::to_string1(mData);
        }
};

class Float :public AnyData {
    private:
        float mData;
        
    public:
        Float(const float &f) {
            mData = f;
        }   
        
        const int GetDataType() const {
            return 'f';
        }
        
        const float GetValue() {
            return mData;
        }
        
        const string ToString() {
            return databuf::to_string1(mData);
        }
};

class String :public AnyData {
    private:
        string mData;
        
    public:
        String () {}
        
        String(const string &s){
            mData = s;
        }
        
        const int GetDataType() const {
            return 's';
        }
        
        const string GetValue() {
            return mData;
        }
        
        const string ToString() {
            return mData;
        }
};

void DataBuf::fill(const int &size) {
    if (mDatas.size()<size) {
        for(int i=mDatas.size(); i<size; ++i) {
            map_Data data;
            mDatas.push_back(data);
        }
    }
}

struct databuf::pair_to_string : public binary_function<string, map_Data::value_type, string>
{
    string operator()(string &result, map_Data::value_type& any) const {
        string content = any.second->ToString();
        result.append(databuf::to_string1(any.first.length()));
        result.append("n"+any.first);
        result.append(databuf::to_string1(content.length()));
        result.append(1,(char)any.second->GetDataType());
        result.append(content);
        return result;
    }
};

struct databuf::map_to_string : public binary_function<string, map_Data, string>
{
    string operator()(string &result, map_Data &data) const {
        string str;
        
        str = accumulate(data.begin(), data.end(), str, databuf::pair_to_string());
        result.append(databuf::to_string1(str.length()));
        result.append("m"+str);
        
        return result;
    }
};

void DataBuf::SetValue(const string &name, PAnyData any, const int &pos) {
    
    if (!(pos<mDatas.size())) fill(pos+1);
    
    map_Data &data = mDatas.at(pos);
    
    data[name] = any;
    
}

PAnyData DataBuf::GetValue(const string &name, const int &pos) const {
    const map_Data& data = mDatas.at(pos);
    map_Data::const_iterator it = data.find(name);
    if (it != data.end())
        return it->second;
    else
        throw std::runtime_error(name + " field not exits!");
        
}

DataBuf::DataBuf() {
}

DataBuf::DataBuf(const DataBuf &buf) {
    mDatas = buf.mDatas;
}

DataBuf::DataBuf(const string &str){
    
    if (str=="") return;            
    
    char *token = 0;  
    const char *p = str.c_str();
    
    long lCount = strtol( p, &token, 10);
    
    CheckToken(token,'a');
    
    p=token+1;
    
    long lenMap,lenName,lenContent;
    char *map=0;
    char *content=0;
    
    for(int i=0; i<lCount; ++i) {
        
        lenMap = strtol(p, &token, 10);
        CheckToken(token,'m');
        
        p=token+1;
        
        if (lenMap==0) continue;
        
        map = (char *)calloc(lenMap+1,sizeof(char));
        char *map_bak=map;
        memcpy(map,p,lenMap);
        
        while(*map!='\0') {
        
            lenName = strtol(map, &token, 10);
            CheckToken(token,'n');
            map=token+1;
            string name(map,lenName);
            map+=lenName;
            
            lenContent = strtol(map, &token, 10);
            map = token + 1;
            
            content = (char *)calloc(lenContent+1,sizeof(char));
            memcpy(content,map,lenContent);
            map += lenContent;
            
            switch(*token){
                case 'i':
                    SetValue(name, PAnyData(new Int(atoi(content))), i);
                    break;
                case 'l':
                    SetValue(name, PAnyData(new Long(atol(content))), i);
                    break;
                case 'f':
                    SetValue(name, PAnyData(new Float((float)atof(content))), i);
                    break;
                case 'd':
                    SetValue(name, PAnyData(new Double((float)atof(content))), i);
                    break;
                case 's':
                    SetValue(name, PAnyData(new String(string(content))), i);
                    break;
                case 'b':
                    SetValue(name, PAnyData(new DataBuf(string(content))), i);
                    break;
                default:
                    free(content);
                    free(map_bak);
                    throw std::runtime_error("decode Error!token:"+*token);
            }
            
            free(content);
        }
        
        free(map_bak);
        //map=0;
        p+=lenMap;
    }
    
}

//DataBuf::DataBuf(CFmlBuf & theRight) {
//    
//    FLDID iFieldId = FIRSTFLDID;
//	FLDOCC occ = 0;
//	int iTypeId ;
//	char charValue ;
//
//	long longValue;
//
//	double doubleValue;
//	string strValue;
//
//	while (theRight.GetNext(&iFieldId, &occ, NULL, NULL)>0)
//	{
//		iTypeId=theRight.GetFldType(iFieldId);
//		switch(iTypeId)
//		{
//			case  FLD_CHAR	:
//				charValue = theRight.GetChar(iFieldId,occ);
//				if(charValue != 0x00)
//					SetString(databuf::to_string1(theRight.GetFldName(iFieldId)),databuf::to_string1(charValue),occ);
//				break;
//			case  FLD_SHORT	:
// 			case  FLD_LONG	:
//				longValue = theRight.GetInt(iFieldId,occ);
//				SetLong(databuf::to_string1(theRight.GetFldName(iFieldId)),longValue,occ);
//	  			break;
//			case  FLD_FLOAT	:
//			case  FLD_DOUBLE:
//				doubleValue = theRight.GetFloat(iFieldId,occ);
//				SetDouble(databuf::to_string1(theRight.GetFldName(iFieldId)),doubleValue,occ);
//				break;
//			case  FLD_STRING:
//				strValue = theRight.GetStr(iFieldId,occ);
//				//if(strValue.length() != 0)
//			    	SetString(databuf::to_string1(theRight.GetFldName(iFieldId)),strValue,occ);
//				break;
//			default:
//				throw std::runtime_error("Fieldtype Not Support!type:"+iTypeId);
//    	}
//	}
//    
//}

const int DataBuf::GetDataType() const {
    return 'b';
}

void DataBuf::SetString(const string &name, const string &str, const int &pos){
    SetValue(name,PAnyData(new String(str)), pos);
}

void DataBuf::SetInt(const string &name, const int &i, const int &pos) {
    SetValue(name, PAnyData(new Int(i)), pos);
}

void DataBuf::SetLong(const string &name, const long &l, const int &pos) {
    SetValue(name, PAnyData(new Long(l)), pos);
}

void DataBuf::SetFloat(const string &name, const float &f, const int &pos) {
    SetValue(name, PAnyData(new Float(f)), pos);
}

void DataBuf::SetDouble(const string &name, const double &d, const int &pos) {
    SetValue(name, PAnyData(new Double(d)), pos);
}

void DataBuf::SetBuf(const string &name, const DataBuf &buf, const int &pos) {
    SetValue(name, PAnyData(new DataBuf(buf)), pos);
}

const string DataBuf::GetString(const string &name, const int &pos) const {
    PAnyData data = GetValue(name, pos);
    
    switch(data->GetDataType()) {
        case 'i':
        case 'l':
        case 'f':
        case 'd':
            return data->ToString();
        case 's':
            return (dynamic_pointer_cast<String>(data))->GetValue();
            
        default:
            throw std::runtime_error("GetString Error!"+(char)data->GetDataType());
    }
}

const int DataBuf::GetInt(const string &name, const int &pos) const {
    PAnyData data = GetValue(name, pos);
    switch(data->GetDataType()){
        case 'i':
            return (dynamic_pointer_cast<Int>(data))->GetValue();
        case 'l':
            return (int) (dynamic_pointer_cast<Long>(data))->GetValue();
        case 'd':
            return (int) (dynamic_pointer_cast<Double>(data))->GetValue();
        case 'f':
            return (int) (dynamic_pointer_cast<Float>(data))->GetValue();
        case 's':
            {
                char *p=0;
                (void *)strtol((dynamic_pointer_cast<String>(data))->GetValue().c_str(), &p, 10);
                if (*p != '\0') {
                    throw std::runtime_error("GetInt Error!"+(dynamic_pointer_cast<String>(data))->GetValue());
                }
                return (int)atol((dynamic_pointer_cast<String>(data))->GetValue().c_str());
            }
        default:
            throw std::runtime_error("GetInt Error!"+(char)data->GetDataType());
    }
}

const long DataBuf::GetLong(const string &name, const int &pos) const {
    PAnyData data = GetValue(name, pos);
    switch(data->GetDataType()){
        case 'i':
            return (long) (dynamic_pointer_cast<Int>(data))->GetValue();
        case 'l':
            return (dynamic_pointer_cast<Long>(data))->GetValue();
        case 'd':
            return (long) (dynamic_pointer_cast<Double>(data))->GetValue();
        case 'f':
            return (long) (dynamic_pointer_cast<Float>(data))->GetValue();
        case 's':
            {
                char *p=0;
                (void *)strtol((dynamic_pointer_cast<String>(data))->GetValue().c_str(), &p, 10);
                if (*p != '\0') {
                    throw std::runtime_error("GetLong Error!"+(dynamic_pointer_cast<String>(data))->GetValue());
                }
                return (int)atol((dynamic_pointer_cast<String>(data))->GetValue().c_str());
            }
        default:
            throw std::runtime_error("GetLong Error!"+(char)data->GetDataType());
    }
}

const double DataBuf::GetDouble(const string &name, const int &pos) const {
    PAnyData data = GetValue(name, pos);
    switch(data->GetDataType()){
        case 'i':
            return (double)(dynamic_pointer_cast<Int>(data))->GetValue();
        case 'l':
            return (double)(dynamic_pointer_cast<Long>(data))->GetValue();
        case 'd':
            return (dynamic_pointer_cast<Double>(data))->GetValue();
        case 'f':
            return (double) (dynamic_pointer_cast<Float>(data))->GetValue();
        case 's':
            {
                char *p=0;
                strtod((dynamic_pointer_cast<String>(data))->GetValue().c_str(), &p);
                if (*p != '\0') {
                    throw std::runtime_error("GetDouble Error!"+(dynamic_pointer_cast<String>(data))->GetValue());
                }
                return (double)atof((dynamic_pointer_cast<String>(data))->GetValue().c_str());
            }
        default:
            throw std::runtime_error("GetDouble Error!"+(char)data->GetDataType());
    }
}

const float DataBuf::GetFloat(const string &name, const int &pos) const {
    PAnyData data = GetValue(name, pos);
    switch(data->GetDataType()){
        case 'i':
            return (float)(dynamic_pointer_cast<Int>(data))->GetValue();
        case 'l':
            return (float)(dynamic_pointer_cast<Long>(data))->GetValue();
        case 'd':
            return (float) (dynamic_pointer_cast<Double>(data))->GetValue();
        case 'f':
            return (dynamic_pointer_cast<Float>(data))->GetValue();
        case 's':
            {
                char *p=0;
                strtod((dynamic_pointer_cast<String>(data))->GetValue().c_str(), &p);
                if (*p != '\0') {
                    throw std::runtime_error("GetFloat Error!"+(dynamic_pointer_cast<String>(data))->GetValue());
                }
                return atof((dynamic_pointer_cast<String>(data))->GetValue().c_str());
            }
        default:
            throw std::runtime_error("GetFloat Error!"+(char)data->GetDataType());
    }
}

DataBuf& DataBuf::GetBuf(const string &name, const int &pos) const {
    PAnyData data = GetValue(name, pos);
    if (data->GetDataType() == 'b')
        return *(dynamic_pointer_cast<DataBuf>(data));
    throw std::runtime_error("GetBuf Error!"+(char)data->GetDataType());
}

const string DataBuf::ToString() {
    string result="";
    
    result = accumulate(mDatas.begin(), mDatas.end(), result, databuf::map_to_string());
    
    result ="a" + result;
    result = databuf::to_string1(mDatas.size()) + result;
    
    return result;
}


int DataBuf::size() const {
    return mDatas.size();
}

bool DataBuf::IsFldExist(const string &name, const int &pos) const {
    if (pos >= mDatas.size()) return false;
        
    return mDatas.at(pos).count(name) > 0;
}

int DataBuf::DelFld(const string &name, const int &pos) {
    if (pos >= mDatas.size()) return 0;
        
    return mDatas.at(pos).erase(name);
}

int DataBuf::DelAllFld(const string &name) {
    int count = 0;
    for(int i=0,j=mDatas.size(); i<j; ++i)
        count += mDatas.at(i).erase(name);
    return count;
}

bool DataBuf::IsEmpty() const {
    return mDatas.size() == 0;
}

int DataBuf::GetCount(const string &name) const {
    int count = 0;
    for(int i=0,j=mDatas.size(); i<j; ++i)
        count += mDatas.at(i).count(name);
    return count;
}

DataBuf & DataBuf::Append(const DataBuf & right) {
    
    mDatas.insert(mDatas.end(), right.mDatas.begin(), right.mDatas.end());
    
    return *this;
}

DataBuf & DataBuf::CoverLine(const int &targetLine, const DataBuf &right, const int &sourceLine) {
    
    if (sourceLine>=right.mDatas.size()) return *this;
    if (targetLine >= mDatas.size()) fill(targetLine+1);
    
    const map_Data &source = right.mDatas.at(sourceLine);
    map_Data &target = mDatas.at(targetLine);
    
    map_Data::const_iterator it = source.begin();   
    for( ; it != source.end(); ++it ) {
        target[it->first] = it->second;
    }

    return *this;
}

DataBuf & DataBuf::Cover(const DataBuf &right) {
    
    for(int i=0,j=right.size(); i<j; ++i)
        CoverLine(i, right, i);
    
    return *this;
}
//
//int DataBuf::ConvertToFml(CFmlBuf &fml) const{
//    
//    int iExistsBuf = 0;
//    
//    for(int i=0,j=mDatas.size(); i<j; ++i) {
//        const map_Data &data = mDatas.at(i);
//        map_Data::const_iterator it = data.begin();   
//        for( ; it != data.end(); ++it ) {
//            
//            switch(it->second->GetDataType()) {
//                case 'i':
//                case 'l':
//                    fml.SetInt(it->first.c_str(), (dynamic_pointer_cast<Long>(it->second))->GetValue(), i);
//                    break;
//                case 'd':
//                case 'f':
//                    fml.SetFloat(it->first.c_str(), (dynamic_pointer_cast<Double>(it->second))->GetValue(), i);
//                    break;
//                case 's':
//                    fml.SetString(it->first.c_str(), (dynamic_pointer_cast<String>(it->second))->GetValue(), i);
//                    break;
//                case 'b':
//                    ++iExistsBuf;
//            }
//            
//        }
//    }
//    
//    return iExistsBuf;
//}

const vector<map_Data> & DataBuf::GetData() const{
    return mDatas;
}

void DataBuf::Clear() {
    mDatas.clear();
}

DataBuf & DataBuf::RemoveLine(const int & line, const int &size) {
    
    if (line < mDatas.size()) {
        mDatas.erase(mDatas.begin()+line, mDatas.begin()+line+1);
    }
    
    return *this;
}

//获取属性类型 add by zhangyangshuo
const int DataBuf::GetFldType(const string &name, const int &pos) const {
    PAnyData data = GetValue(name, pos);
    return  data->GetDataType();
}

