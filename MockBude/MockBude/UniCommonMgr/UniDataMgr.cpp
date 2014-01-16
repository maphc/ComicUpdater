#include "stdafx.h"
#include "UniDataMgr.h"
#include "../log4cpp/Logger.h"

using namespace log4cpp;
static Logger& logUniTradeMgr = Logger::getLogger("gboss.crm.CustServ.UniTradeMgr");

UniDataMgr * UniDataMgr::instance;
DataBuf UniDataMgr::mUniData;
vector<DataBuf> UniDataMgr::mMultiUniData;
int UniDataMgr::_mIdx;

UniDataMgr & UniDataMgr::getDataMgrImpl() {

	if (instance == 0) {
		instance = new UniDataMgr();
	}

	return *instance;
}


UniDataMgr::UniDataMgr() {
	init();
}

void UniDataMgr::clear() {

	mUniData.Clear();
	mMultiUniData.clear();

	init();
}

void UniDataMgr::init() {
	DataBuf base,ext,bak,ret,com;
	mUniData.SetBuf("base", base);
	mUniData.SetBuf("ext", ext);
	mUniData.SetBuf("bak", bak);
	mUniData.SetBuf("ret", ret);
	mUniData.SetBuf("com", com);

	_mIdx = 0;
}


DataBuf & UniDataMgr::getBase() {
	return mUniData.GetBuf("base");
}

bool UniDataMgr::existBaseInfo(const string &name) {
	return mUniData.GetBuf("base").IsFldExist(name);
}

const long UniDataMgr::getBaseInt(const string &name) {
	//assert(mUniData.GetBuf("base").IsFldExist(name));
	return mUniData.GetBuf("base").GetLong(name);
}

const string UniDataMgr::getBaseString(const string &name) {
	//assert(mUniData.GetBuf("base").IsFldExist(name));
	return mUniData.GetBuf("base").GetString(name);
}

DataBuf & UniDataMgr::getExts() {
	return mUniData.GetBuf("ext");
}

bool UniDataMgr::existExtInfo(const string &name) {
	return mUniData.GetBuf("ext").IsFldExist(name);
}

DataBuf & UniDataMgr::getExtBuf(const string &name) {
	//assert(mUniData.GetBuf("ext").IsFldExist(name));
	return mUniData.GetBuf("ext").GetBuf(name);
}

DataBuf & UniDataMgr::getCom() {
	return mUniData.GetBuf("com");
}

bool UniDataMgr::existComInfo(const string &name) {
	return mUniData.GetBuf("com").IsFldExist(name);
}

const long UniDataMgr::getComInt(const string &name) {
	return mUniData.GetBuf("com").GetLong(name);
}

const string UniDataMgr::getComString(const string &name) {
	return mUniData.GetBuf("com").GetString(name);
}

void UniDataMgr::setBaseInt(const string &name, const long &value) {
	mUniData.GetBuf("base").SetLong(name, value);
}

void UniDataMgr::setBaseString(const string &name, const string &value) {
	mUniData.GetBuf("base").SetString(name, value);
}

void UniDataMgr::setBase(const DataBuf &buf) {
	mUniData.SetBuf("base", buf);
}

void UniDataMgr::setBaseBuf(const string &name, const DataBuf &buf) {
	mUniData.GetBuf("base").SetBuf(name, buf);
}


void UniDataMgr::appendBaseBuf(const string &name, const DataBuf &buf) {
	if (mUniData.GetBuf("base").IsFldExist(name))
		mUniData.GetBuf("base").GetBuf(name).Append(buf);
	else
		setBaseBuf(name, buf);
}

void UniDataMgr::setExt(const DataBuf &buf) {
	mUniData.SetBuf("ext", buf);
}

void UniDataMgr::setExtBuf(const string &name, const DataBuf &buf) {
	mUniData.GetBuf("ext").SetBuf(name, buf);
}

void UniDataMgr::appendExtBuf(const string &name, const DataBuf &buf) {
	if (mUniData.GetBuf("ext").IsFldExist(name))
		mUniData.GetBuf("ext").GetBuf(name).Append(buf);
	else
		setExtBuf(name, buf);
}

void UniDataMgr::coverExtBuf(const string &name, const DataBuf &buf) {
	if (mUniData.GetBuf("ext").IsFldExist(name))
		mUniData.GetBuf("ext").GetBuf(name).Cover(buf);
	else
		setExtBuf(name, buf);
}


void UniDataMgr::setComInt(const string &name, const long &value) {
	mUniData.GetBuf("com").SetLong(name, value);
}

void UniDataMgr::setComString(const string &name, const string &value) {
	mUniData.GetBuf("com").SetString(name, value);
}

void UniDataMgr::setCom(const DataBuf &buf) {
	mUniData.SetBuf("com", buf);
}

void UniDataMgr::setComBuf(const string &name, const DataBuf &buf) {
	mUniData.GetBuf("com").SetBuf(name, buf);
}


void UniDataMgr::appendComBuf(const string &name, const DataBuf &buf) {
	if (mUniData.GetBuf("com").IsFldExist(name))
		mUniData.GetBuf("com").GetBuf(name).Append(buf);
	else
		setBaseBuf(name, buf);
}

void UniDataMgr::refreshComBuf(const DataBuf &buf) {

	int iExistsBuf = 0;

	const map_Data& data = mUniData.GetBuf("com").GetData().at(0);
	map_Data::const_iterator it = data.begin();
	for(;it!=data.end();++it) 
	{
		if (it->second->GetDataType() != 'b') 
		{
			string name=it->first;
			if (buf.IsFldExist(name))
			{
				switch(it->second->GetDataType()) 
				{
				case 'i':
				case 'l':
					mUniData.GetBuf("com").SetInt(name,mUniData.GetBuf("com").GetLong(name));
					break;
				case 'd':
				case 'f':
					mUniData.GetBuf("com").SetFloat(name,mUniData.GetBuf("com").GetFloat(name));
					break;
				case 's':
					mUniData.GetBuf("com").SetString(name, mUniData.GetBuf("com").GetString(name));
					break;
				case 'b':
					++iExistsBuf;
				}
			}
		}
	}
}

DataBuf & UniDataMgr::getBaks() {
	return mUniData.GetBuf("bak");
}

bool UniDataMgr::existBakInfo(const string &name) {
	return mUniData.GetBuf("bak").IsFldExist(name);
}

DataBuf & UniDataMgr::getUserBak() {
	//assert(mUniData.GetBuf("bak").IsFldExist("user"));
	return mUniData.GetBuf("bak").GetBuf("user");
}

DataBuf & UniDataMgr::getCustBak() {
	//assert(mUniData.GetBuf("bak").IsFldExist("cust"));
	return mUniData.GetBuf("bak").GetBuf("cust");
}

DataBuf & UniDataMgr::getBakBuf(const string &name) {
	//assert(mUniData.GetBuf("bak").IsFldExist(name));
	return mUniData.GetBuf("bak").GetBuf(name);
}

void UniDataMgr::setUserBak(const DataBuf &user) {
	mUniData.GetBuf("bak").SetBuf("user", user);
}

void UniDataMgr::setCustBak(const DataBuf &cust) {
	mUniData.GetBuf("bak").SetBuf("cust", cust);
}

void UniDataMgr::setBakBuf(const string &name, const DataBuf &buf) {
	mUniData.GetBuf("bak").SetBuf(name, buf);
}

void UniDataMgr::appendBakBuf(const string &name, const DataBuf &buf) {
	if (mUniData.GetBuf("bak").IsFldExist(name))
		mUniData.GetBuf("bak").GetBuf(name).Append(buf);
	else
		setBakBuf(name, buf);
}

const long UniDataMgr::getBakInt(const string &name) {
	return mUniData.GetBuf("bak").GetLong(name);
}

const string UniDataMgr::getBakString(const string &name) {
	return mUniData.GetBuf("bak").GetString(name);
}

void UniDataMgr::setBakInt(const string &name, const long &value) {
	mUniData.GetBuf("bak").SetLong(name, value);
}

void UniDataMgr::setBakString(const string &name, const string &value) {
	mUniData.GetBuf("base").SetString(name, value);
}

DataBuf & UniDataMgr::getRetBuf() {
	return mUniData.GetBuf("ret");
}

void UniDataMgr::setRetBuf(const DataBuf &buf) {
	mUniData.SetBuf("ret", buf);
}




void UniDataMgr::push() {

	save();

	_mIdx = mMultiUniData.size();
}

void UniDataMgr::front() {
	at(0);
}

void UniDataMgr::save() {

	if (_mIdx>mMultiUniData.size())
		throw std::runtime_error("save unidata error!");

	if (_mIdx==mMultiUniData.size())
		mMultiUniData.push_back(mUniData);
	else
		mMultiUniData[_mIdx] = mUniData;
}

vector<DataBuf> & UniDataMgr::getUniDatas() {

	return mMultiUniData;
}

void UniDataMgr::at(const int &idx) {

	_mIdx = idx;

	mUniData = mMultiUniData.at(idx);
}
