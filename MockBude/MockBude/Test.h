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
		THROW_AGC_P1(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ��Ʒ���ɰ���Ϣ����[%s]",ex.getCause()->what());
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

	//��ȡ��Ʒ���ɰ���Ϣ
	CFmlBuf bufPackageByProd;
	int iPackageCount = GetPackageByProduct(iProductId, strEparchyCode, strTradeStaffId, bufPackageByProd, xTag);

	int iMaxElement = 0;
	int iMinElement = 0;
	int iUserElement = 0;
	string strErrorInfo = "";

	int iUserPackageCount = bufUserPackageByProd.GetInt("X_RECORDNUM");
	for(int i = 0; i < iUserPackageCount; i++)
	{
		//��ǰҵ���ޱ仯�İ������ж�
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

		//��ǰҵ����ȫɾ���İ������ж�
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

		//��Ʒ����Ԫ����С�����ѡ�����ж�
		if(iMinElement >= 0 || iMaxElement >= 0)
		{
			//��ȡ��Ʒ�����û�Ԫ��ѡ����
			for(int z = 0; z < iUserAllElementCount; z++)
			{
				if(bufUserAllElements.GetInt("PRODUCT_ID", z) == iProductId
					&& bufUserAllElements.GetInt("PACKAGE_ID", z) == bufUserPackageByProd.GetInt("PACKAGE_ID", i))
				{
					iUserElement++;
				}
			}

			//��ȥ��������ʱ��Ԫ�ظ���
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

			//��Ʒ����Ԫ����Сѡ�����ж�
			if(iMinElement >= 0 && iUserElement < iMinElement)
			{
				if(strErrorInfo == "")
				{
					strErrorInfo = "ҵ�����\""
						+ bufUserPackageByProd.GetString("PACKAGE_NAME", i)
						+ "\"����ѡ��" + to_string(iMinElement) + "��Ԫ��";
				}
				else
				{
					strErrorInfo += "��ҵ�����\""
						+ bufUserPackageByProd.GetString("PACKAGE_NAME", i)
						+ "\"����ѡ��" + to_string(iMinElement) + "��Ԫ��";
				}
			}

			//��Ʒ����Ԫ�����ѡ�����ж�
			if(iMaxElement >= 0 && iUserElement > iMaxElement)
			{
				if(strErrorInfo == "")
				{
					strErrorInfo = "ҵ�����\""
						+ bufUserPackageByProd.GetString("PACKAGE_NAME", i)
						+ "\"���ѡ��" + to_string(iMaxElement) + "��Ԫ��";
				}
				else
				{
					strErrorInfo += "��ҵ�����\""
						+ bufUserPackageByProd.GetString("PACKAGE_NAME", i)
						+ "\"���ѡ��" + to_string(iMaxElement) + "��Ԫ��";
				}
			}
		}
	}

	if(strErrorInfo != "")
	{
		THROW_GC(CRMException, Grade::WARNING, CHECKAFTERTRADE_ERR, "��Ʒ��\"" +
			strProductName + "\"" + strErrorInfo + "��ҵ���޷�������");
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
		THROW_AGC(CRMException, ex, Grade::WARNING, CHECKAFTERTRADE_ERR, "ҵ��ǼǺ������ж�:��ȡ��ǰʡ�������");
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