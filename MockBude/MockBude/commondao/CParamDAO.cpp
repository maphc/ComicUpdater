#include "stdafx.h"
#include "CParamDAO.h"


using namespace std;

StaticDAO& CParamDAO::dao=StaticDAO::getDAO();
map<string, map<string,string> > CParamDAO::cache;
const bool CParamDAO::IS_SKIP=true;

string CParamDAO::getParam( const string& key, const string& value1 )
{
	return getParam(key,value1,"-1");
}


string CParamDAO::getParam( const string& key, const string& value1, const string& value2 )
{
	if(IS_SKIP){
		return key+"#"+value1+"#"+value2;
	}
	if(cache.find(key)!=cache.end()){
		return cache[key][value1+"#"+value2];
	}else{
		int n=0;
		dao.Clear();
		DataBuf out;
		dao.SetParam(":VKEY",key);
		n=dao.jselect(out,key,"TD_S_CPARAM");
		
		if(n>0){
			map<string,string> item;
			for(UINT i=0;i<out.GetCount("KEY");i++){
				string innerKey(out.GetString("VALUE1",i)+"#"+out.GetString("VALUE2",i));
				if(item.find(innerKey)==item.end()){
					item.insert(make_pair(innerKey,out.GetString("VRESULT",i)));
					cout<<"CParamDAO::getParam :"<<i<<"  "<<innerKey<<" "<<out.GetString("VRESULT",i)<<endl;
				}
				
			}
			cache.insert(make_pair(key,item));
			
			
		}
		return cache[key][value1+"#"+value2];
	}
	return "";

}

/**
 *  ��������-����ת������
 *  1. ���������fromFldName ��buf ���ҵ�����ֵ;
 *  2. ��key �ͱ���ֵΪ�����Ӳ��������в�ѯ����;
 *  3. �ڰ�������toFldName ����buf;
 *
 *	ConvertCodeToName(outBuf, "PRODUCT.PRODUCT_TYPE", "PRODUCT_TYPE", "PRODUCT_TYPE_NAME");
 *  outBuf.CodeToName("PRODUCT.PRODUCT_TYPE", "PRODUCT_TYPE", "PRODUCT_TYPE_NAME");
 */
int ConvertCodeToName(CFmlBuf& buf, const string& skey, const char* fromFldName, const char* toFldName)
{
	
	return 0;
}

/**
 *  ��������-����ת������
 *  1. ���������fromFldName1 �� fromFldName2 ��buf ���ҵ�����ֵ;
 *  2. ��key �ͱ���ֵΪ�����Ӳ��������в�ѯ����;
 *  3. �ڰ�������toFldName ����buf;
 *
 *	ConvertCodeToName(outBuf, "ItemName", "ITEM_CODE", "ITEM_TYPE", "ITEM_NAME");
 */
int ConvertCodeToName(CFmlBuf& buf, const string& skey, const char* fromFldName1, const char* fromFldName2, const char* toFldName)
{
	return 0;
}
