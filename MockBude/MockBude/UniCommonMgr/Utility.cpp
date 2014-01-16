#include "stdafx.h"
#include "Utility.h"
#include "../log4cpp/Logger.h"

using namespace log4cpp;
static Logger &logger = Logger::getLogger("gboss.crm.CustServ");


//UniTradeDao  &gDao = UniTradeDao::getInstance();
const int LENGTH = 3000;

const string Utility::formatDate(const string &date) {
    
    switch(date.length()) {
        case 14:
            return date.substr(0,4)+"-"+date.substr(4,2)+"-"+date.substr(6,2)+" "+date.substr(8,2)+":"+date.substr(10,2)+":"+date.substr(12,2);
        case 8:
            return date+"";
        case 19:
        	return date;
    }
    return "";
}

const int Utility::getCycleId(const string &date) {
    return 201101;
}
const string Utility::getSysdate() {
    return "2011-01-01";
}

const void Utility::GetRandPassword(string &password)
{
	time_t t1;
    unsigned int dp;
    int i = 0;
    int n = 0;
    char ch[] = "0123456789";
    char ch1;

    while(i<6)
    {
        dp = time(&t1);
        srand((unsigned int)t1);
        
        n = rand()%10;
        ch1=ch[n];
        password=password+ch1;
        i++;
    }
}

const string Utility::getSysTagInfo(DataBuf &tagInfo,const string sysCode,const string tagCode,const string tagType,const string def,const string strEparchyCode)
{
	/*gDao.getSysTagInfo(tagInfo,sysCode,tagCode,strEparchyCode);
	if (tagInfo.size()==0) 
        return def;
    else
        return tagInfo.GetString(tagType)!=""?tagInfo.GetString(tagType):def;*/

	return "";
}

const DataBuf Utility::FilterSameValue(DataBuf &Info,const string &key)
{
	DataBuf tempBuf;
	
	int k=0;
	
	for (int i=0;i<Info.size();i++)
	{
		if (Info.IsFldExist(key,i))
		{
			string value=Info.GetString(key,i);
		
			bool flag= true;
		
			for (int j=i+1;j<Info.size();j++)
			{
					
				if (Info.IsFldExist(key,j))
				{
					if (Info.GetString(key,j)==value)
					{
						flag=false;
						break;
					}
				}
			}
			
			if (flag)
			{
				tempBuf.CoverLine(k,Info,i);
				k++;
			}
		}
	}
	
	return tempBuf;
}

const DataBuf Utility::FilterKeyValue(DataBuf &Info,const string &key,const string &value)
{
	DataBuf tempBuf;
	
	int j=0;
	
	for (int i=0;i<Info.size();i++)
	{
		if (Info.IsFldExist(key,i))
		{
			if (Info.GetString(key,i)==value)
			{
				tempBuf.CoverLine(j,Info,i);
				j++;
			}
		}
	}
	
	return tempBuf;
}

const DataBuf Utility::UnFilterKeyValue(DataBuf &Info,const string &key,const string &value)
{
	DataBuf tempBuf;
	
	int j=0;
	
	for (int i=0;i<Info.size();i++)
	{
		if (Info.IsFldExist(key,i))
		{
			if (Info.GetString(key,i)!=value)
			{
				tempBuf.CoverLine(j,Info,i);
				j++;
			}
		}
	}
	
	return tempBuf;
}

/* LCU流程调用
* inBuf必输参数字段
* X_TRANS_CODE
* TRADE_EPARCHY_CODE
* TRADE_CITY_CODE
* TRADE_DEPART_ID
* TRADE_STAFF_ID
* ROUTE_EPARCHY_CODE
*/
const void Utility::TransLcuFlow(DataBuf &inBuf,DataBuf &outBuf)
{
	

}

///* LCU流程调用
//* inBuf必输参数字段
//* X_TRANS_CODE
//* TRADE_EPARCHY_CODE
//* TRADE_CITY_CODE
//* TRADE_DEPART_ID
//* TRADE_STAFF_ID
//* ROUTE_EPARCHY_CODE
//*/
//const void Utility::TransLcuFlow(CFmlBuf &inBuf,CFmlBuf &outBuf)
//{
//	
//}

/* LCU流程调用
* inBuf必输参数字段
* X_TRANS_CODE
* TRADE_EPARCHY_CODE
* TRADE_CITY_CODE
* TRADE_DEPART_ID
* TRADE_STAFF_ID
* ROUTE_EPARCHY_CODE
*/
const void Utility::TransLcuFlow(PCFmlBuf &inBuf,PCFmlBuf &outBuf)
{	
	
  
}


/* 
* inBuf必输参数字段：
* X_TRANS_CODE
* TRADE_EPARCHY_CODE
* TRADE_CITY_CODE
* TRADE_DEPART_ID
* TRADE_STAFF_ID
* ROUTE_EPARCHY_CODE
* X_TAG:0-本机，1-跨主机
*/
const int Utility::callLCU(CFmlBuf &inBuf,CFmlBuf &outBuf)
{

	
	return 1;
}
//-----------------------------------------------------------------------
//-- Function:          TransXmlToDataBuf
//-- Description:       转换xml生成DataBuf
//-- Author:            lvjf
//-- Date:              2011-08-08 20:39
//-- Version:           1.0
//-----------------------------------------------------------------------
//inline const bool hasChildElementNode(DOMNode* xmlNode)
//{
//    bool hasElementNode=false;
//    if (xmlNode->hasChildNodes())
//    {
//        DOMNodeList* childNodeList = xmlNode->getChildNodes();  
//        int length=childNodeList->getLength();      
//        for(int y = 0; y < length; y++) 
//        {
//            if (childNodeList->item(y)->getNodeType() == DOMNode::ELEMENT_NODE)
//                {
//                     hasElementNode=true;
//                     break;
//                }
//        }
//    }
//    return hasElementNode;
//}
//-----------------------------------------------------------------------
//-- Function:          TransXmlToDataBuf
//-- Description:       转换xml生成DataBuf
//-- Author:            lvjf
//-- Date:              2011-08-08 20:39
//-- Version:           1.0
//-----------------------------------------------------------------------
//static inline const int subTransXmlToDataBuf(DOMNode* xmlNode,DataBuf &xmlDbuf)
//{
//    int iAddRoot=0;
//    if (xmlNode->getNodeType() == DOMNode::ELEMENT_NODE)
//    {
//        auto_ptr<XmlHelper> xmlHelper(new XmlHelper());
//    	int iErrorCode = xmlHelper->Initialize();
//    	if(iErrorCode != 0)
//        	THROW_C_P1(CRMException, 8888,  "构造XML使用环境异常！错误编码：", to_string(iErrorCode).c_str());
//        		
//        if (hasChildElementNode(xmlNode))       
//        {
//            DOMNodeList* nodeList = xmlNode->getChildNodes();
//            int length=nodeList->getLength();
//            for(int z = 0; z <length ; z++)           
//            {
//                if (nodeList->item(z)->getNodeType() == DOMNode::ELEMENT_NODE)
//                {
//                    char* nodeName=XMLString::transcode(nodeList->item(z)->getNodeName()); 
//                    if (hasChildElementNode(nodeList->item(z))) 
//                    {
//                        DataBuf nodeBuf;              
//                        subTransXmlToDataBuf(nodeList->item(z),nodeBuf);                        
//                        xmlDbuf.SetBuf(nodeName,nodeBuf,xmlDbuf.GetCount(nodeName)); 
//                    }
//                    else
//                    {
//                        XMLCh* xcValue = (XMLCh*) nodeList->item(z)->getTextContent();
//                        string nodeValue=xmlHelper->Transcode(xcValue);
//                        xmlDbuf.SetString(nodeName,nodeValue);
//                    }
//                }
//            }
//            iAddRoot=1;
//        }
//        else
//        {
//            char* nodeName = XMLString::transcode(xmlNode->getNodeName());
//            XMLCh* xcValue = (XMLCh*) xmlNode->getTextContent();
//           	string nodeValue=xmlHelper->Transcode(xcValue); 
//            xmlDbuf.SetString(nodeName,nodeValue);               
//        }
//    }
//    return iAddRoot;
//}
//-----------------------------------------------------------------------
//-- Function:          TransXmlToDataBuf
//-- Description:       转换xml生成DataBuf
//-- Author:            lvjf
//-- Date:              2011-08-08 20:39
//-- Version:           1.0
//-----------------------------------------------------------------------
//const int Utility::TransXmlToDataBuf(DOMDocument *document,DataBuf &xmlDbuf)
//{
//    DOMNode *xmlNode = (DOMNode*)document->getDocumentElement();
//    if (xmlNode)
//    {
//        DataBuf subxmlDbuf;
//        if (subTransXmlToDataBuf(xmlNode,subxmlDbuf)==1)
//        {
//            char* nodeName = XMLString::transcode(xmlNode->getNodeName());
//            xmlDbuf.SetBuf(nodeName,subxmlDbuf);           
//        }
//        else
//        {
//            xmlDbuf=subxmlDbuf;
//        }
//    }
//    return 0;
//}
//-----------------------------------------------------------------------
//-- Function:          TransXmlToDataBuf
//-- Description:       转换xml生成DataBuf
//-- Author:            lvjf 摘自 lirui 函数
//-- Date:              2011-08-08 20:39
//-- Version:           1.0
//-----------------------------------------------------------------------
const string Utility::TransDataBufToXmlString(DataBuf &xmlDbuf)
{
	string strXcodeing="";
	for(int i=0,j=xmlDbuf.GetData().size(); i<j; ++i) 
	{
        const map_Data& data = xmlDbuf.GetData().at(i);
        for(map_Data::const_iterator it = data.begin();it!=data.end();++it) 
        {
            if (it->second->GetDataType() == 'b') 
            {
                strXcodeing  = strXcodeing + "<" +  it->first + ">";
                DataBuf &mExtData = *dynamic_pointer_cast<DataBuf>(it->second);
                strXcodeing  = strXcodeing + TransDataBufToXmlString(mExtData);
                strXcodeing  = strXcodeing + "</" +  it->first + ">";
            }
            else
            {
           		strXcodeing  =  strXcodeing +"<" + it->first + ">" + xmlDbuf.GetString(it->first) +  "</" + it->first + ">" ;
           	}
        }
    }
    return strXcodeing;    
}

//-----------------------------------------------------------------------
//-- Function:          pretreatIOMDate
//-- Description:       预处理O侧返回的时间数据
//-- Date:              2011-10-20 21:26
//-----------------------------------------------------------------------
const string Utility::pretreatIOMDate(string strInDate)
{
    if(strInDate!="" && strInDate.length() == 23)
    {
        strInDate = strInDate.substr(0, strInDate.length()-4);
    }
    
    return strInDate;
}

/*
*	判断是否是汉字,用于字符串截取,防止把一个汉字截取成一半
*	add by yyh 20110830
*/
bool Utility::isGBK(unsigned char head, unsigned char tail)
{
   if ((head>=0xb0 && head<=0xf7) && (tail >= 0xa1 && tail <= 0xfe))
   {
      	return true;
   }
   return false;
}

void Utility::geneEncodeStr(string &strRet,CFmlBuf &outBuf)
{
	
	string tempStr;
	int i=0;
	while(strRet!="")
	{
		if (strRet.length()>LENGTH)
		{
			int j;
			for(j=1;j<LENGTH;j++)
			{
				char  s=strRet[LENGTH-(j+1)];
				char  d=strRet[LENGTH-j];
				if (!isGBK(s,d))
			 		break;
			}
			tempStr=strRet.substr(0,  LENGTH-j);
			
		}
		else
			tempStr=strRet;
		
		LOG_TRACE_P1(logger, "tempStr----:%s",tempStr.c_str());
		outBuf.SetString("X_CODING_STR", tempStr,i);
		if (tempStr.length()==strRet.length())
			strRet="";
		else
			strRet=strRet.substr(tempStr.length(),strRet.length()-tempStr.length());
		i++;
	} 
}

const string Utility::getEncodeStr(CFmlBuf &buf) 
{
    int iCnt = buf.GetCount("X_CODING_STR");
    
    string strRet;
    for(int i=0;i<iCnt;++i) 
	{
        strRet += buf.GetString("X_CODING_STR", i);
		LOG_TRACE_P1(logger, "temp----:%s",buf.GetString("X_CODING_STR", i));
	}

    return strRet;
}