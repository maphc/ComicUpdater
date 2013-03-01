#include "stdafx.h"
#pragma once
using namespace log4cpp;
static Logger& logUserQueryer = Logger::getLogger("gboss.crm.CustServ");

int GetPackageByProduct(const int &iProductId, const string &strEparchyCode, const string &strTradeStaffId, CFmlBuf &bufPackageByProd, const int xTag)
{
	bufPackageByProd.ClearFmlValue();
	StaticDAO& dao = StaticDAO::getDAO();
	int iCount = 0;

	try
	{
		dao.Clear();
		dao.SetParam(":VPRODUCT_ID", iProductId);
		dao.SetParam(":VEPARCHY_CODE", strEparchyCode);
		//dao.SetParam(":VTRADE_STAFF_ID", strTradeStaffId);
		dao.SetParam(":VX_TAG", xTag);
		iCount = dao.jselect(bufPackageByProd, "SEL_PACKAGES_BY_PID_CHEK", "TD_B_PRODUCT_PACKAGE");
	}
	catch (Exception &ex)
	{
		THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取产品构成包信息出错！[%s]",ex.getCause()->what());
	}

	
	bufPackageByProd.SetInt("X_RECORDNUM", iCount);
	return iCount;
}
void CheckPackageElementChoiceByProduct(const string &strEparchyCode, const string &strTradeStaffId, CFmlBuf &bufUserPackageByProd,
															  CFmlBuf &bufUserAllElements, CFmlBuf &bufNoChangePackage, CFmlBuf &bufDelPackage,
															  CFmlBuf &bufElementTimeSeries,
															  const int xTag)
{
	if(bufUserPackageByProd.GetInt("X_RECORDNUM") == 0)
		return;

	int iProductId = bufUserPackageByProd.GetInt("PRODUCT_ID");
	string strProductName = bufUserPackageByProd.GetString("PRODUCT_NAME");
	int iUserAllElementCount = bufUserAllElements.GetInt("X_RECORDNUM");

	//获取产品构成包信息
	CFmlBuf bufPackageByProd;
	int iPackageCount = GetPackageByProduct(iProductId, strEparchyCode, strTradeStaffId, bufPackageByProd, xTag);

	int iMaxElement = 0;
	int iMinElement = 0;
	int iUserElement = 0;
	string strErrorInfo = "";

	int iUserPackageCount = bufUserPackageByProd.GetInt("X_RECORDNUM");
	for(int i = 0; i < iUserPackageCount; i++)
	{
		//当前业务无变化的包不作判断
		bool bExist = false;
		int iNoChange = bufNoChangePackage.GetInt("X_RECORDNUM");
		for(int g = 0; g < iNoChange; g++)
		{
			if(bufNoChangePackage.GetInt("PRODUCT_ID", g) == bufUserPackageByProd.GetInt("PRODUCT_ID", i)
				&& bufNoChangePackage.GetInt("PACKAGE_ID", g) == bufUserPackageByProd.GetInt("PACKAGE_ID", i))
			{
				bExist = true;
				break;
			}
		}
		if(bExist) continue;

		//当前业务完全删除的包不作判断
		bExist = false;
		int iDelPack = bufDelPackage.GetInt("X_RECORDNUM");
		for(int g = 0; g < iDelPack; g++)
		{
			if(bufDelPackage.GetString("RSRV_STR1", g) == to_string(bufUserPackageByProd.GetInt("PRODUCT_ID", i))
				&& bufDelPackage.GetString("RSRV_STR3", g) == to_string(bufUserPackageByProd.GetInt("PACKAGE_ID", i)))
			{
				bExist = true;
				break;
			}
		}
		if(bExist) continue;


		iMaxElement = -1;
		iMinElement = -1;
		iUserElement = 0;
		for(int j = 0; j < iPackageCount; j++)
		{
			if(bufUserPackageByProd.GetInt("PACKAGE_ID", i) ==
				bufPackageByProd.GetInt("PACKAGE_ID", j))
			{
				iMaxElement = bufPackageByProd.GetInt("MAX_NUMBER", j);
				iMinElement = bufPackageByProd.GetInt("MIN_NUMBER", j);
				break;
			}
		}

		//产品包内元素最小、最大选择数判断
		if(iMinElement >= 0 || iMaxElement >= 0)
		{
			//获取产品包内用户元素选择数
			for(int z = 0; z < iUserAllElementCount; z++)
			{
				if(bufUserAllElements.GetInt("PRODUCT_ID", z) == iProductId
					&& bufUserAllElements.GetInt("PACKAGE_ID", z) == bufUserPackageByProd.GetInt("PACKAGE_ID", i))
				{
					iUserElement++;
				}
			}

			//减去包内连续时间元素个数
			int iRecCount = bufElementTimeSeries.GetInt("X_RECORDNUM");
			int iSeriesCount = 0;
			for(int g = 0; g < iRecCount; g++)
			{
				if(bufElementTimeSeries.GetInt("PRODUCT_ID", g) == iProductId
					&& bufElementTimeSeries.GetInt("PACKAGE_ID", g) == bufUserPackageByProd.GetInt("PACKAGE_ID", i))
				{
					iSeriesCount = bufElementTimeSeries.GetInt("X_TAG");
					break;
				}
			}
			iUserElement -= iSeriesCount;

			//产品包内元素最小选择数判断
			if(iMinElement >= 0 && iUserElement < iMinElement)
			{
				if(strErrorInfo == "")
				{
					strErrorInfo = "业务包：\""
						+ bufUserPackageByProd.GetString("PACKAGE_NAME", i)
						+ "\"最少选择" + to_string(iMinElement) + "个元素";
				}
				else
				{
					strErrorInfo += "、业务包：\""
						+ bufUserPackageByProd.GetString("PACKAGE_NAME", i)
						+ "\"最少选择" + to_string(iMinElement) + "个元素";
				}
			}

			//产品包内元素最大选择数判断
			if(iMaxElement >= 0 && iUserElement > iMaxElement)
			{
				if(strErrorInfo == "")
				{
					strErrorInfo = "业务包：\""
						+ bufUserPackageByProd.GetString("PACKAGE_NAME", i)
						+ "\"最多选择" + to_string(iMaxElement) + "个元素";
				}
				else
				{
					strErrorInfo += "、业务包：\""
						+ bufUserPackageByProd.GetString("PACKAGE_NAME", i)
						+ "\"最多选择" + to_string(iMaxElement) + "个元素";
				}
			}
		}
	}

	if(strErrorInfo != "")
	{
		THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR, "产品：\"" +
			strProductName + "\"" + strErrorInfo + "，业务无法继续！");
	}
}
void test(DataBuf& inBuf ,DataBuf& outBuf){

	int iMainCount = 0;
	string hbtag = "0";
	CFmlBuf hbTagBuf;
	StaticDAO& hbdao = StaticDAO::getDAO();
	try
	{
		hbdao.Clear();
		hbdao.SetParam(":VEPARCHY_CODE", "0010");
		hbdao.SetParam(":VTAG_CODE", "CSM_CHR_PRODUCTMODELLIMIT");
		hbdao.SetParam(":VSUBSYS_CODE", "CSM");
		hbdao.SetParam(":VUSE_TAG", "0");
		iMainCount = hbdao.jselect(hbTagBuf, "SEL_ALL_BY_TAGCODE","TD_S_TAG");
	}
	catch(Exception &ex)
	{
		THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "业务登记后条件判断:获取当前省代码出错！");
	}
	if( iMainCount > 0 &&  hbTagBuf.GetString("TAG_CHAR")=="1" )
	{
		hbtag = "1"  ;
	}



	DataBuf bufUserPackageByProd,bufUserAllElements,bufNoChangePackage,bufDelPackage,bufElementTimeSeries,bufTradeElement_UserAllElement,bufTradeSp_UserAllSp;
	string strEparchyCode="0010";
	string strTradeStaffId="v-yuyan35-x";
	FileInput(bufUserPackageByProd,"z:\\bufUserPackageByProd");
	FileInput(bufTradeElement_UserAllElement,"z:\\bufTradeElement_UserAllElement");
	FileInput(bufTradeSp_UserAllSp,"z:\\bufTradeSp_UserAllSp");
	FileInput(bufNoChangePackage,"z:\\bufNoChangePackage");
	FileInput(bufDelPackage,"z:\\bufDelPackage");
	FileInput(bufElementTimeSeries,"z:\\bufElementTimeSeries");

	CheckPackageElementChoiceByProduct(strEparchyCode, strTradeStaffId, bufUserPackageByProd, bufUserAllElements, bufNoChangePackage, bufDelPackage, bufElementTimeSeries, 1);


}