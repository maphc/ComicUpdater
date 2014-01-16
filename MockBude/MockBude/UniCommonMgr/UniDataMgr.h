/** Copyright (c) 2011, 联创亚信科技
 *  All rights reserved.
 * 
 *  文件名称：UniDataMgr.h
 *  文件标识：
 *  摘    要：统一订单数据管理器.支持三种数据的存取 :base、ext、bak
 *            base和ext是为了插入订单准备的
 *            bak是操作过程中的临时数据备份
 *            
 *            新增ret数据，作为操作返回结果的存放结构
 *            
 *            @2011-3-8 13:22:50  增加 getUniDatas 方法，支持获取多条记录
 *                                增加 push  front  at 方法
 *            
 *  当前版本：1.0
 *  作    者：Tangz
 *  完成日期：2011-2-17 16:57:18
 */ 
#ifndef _UNI_DATA_MGR_H_20110217
#define _UNI_DATA_MGR_H_20110217

#include "../DataBuf.h"

class UniDataMgr {
    
    public:
        static UniDataMgr & getDataMgrImpl();
        
        void clear();
        
        /////////////////////////////////////////////////////////
        
        DataBuf & getBase();
        
        bool existBaseInfo(const string &name);
        
        const long getBaseInt(const string &name);
        
        const string getBaseString(const string &name);
        
        void setBaseInt(const string &name, const long &value);
        
        void setBaseString(const string &name, const string &value);
        
        void setBase(const DataBuf &buf);
        
        void setBaseBuf(const string &name, const DataBuf &buf) ;
        
        void appendBaseBuf(const string &name, const DataBuf &buf);
        
        
        /////////////////////////////////////////////////////////
        
        
        DataBuf & getExts();
        
        void setExt(const DataBuf &buf);
        
        bool existExtInfo(const string &name);
        
        DataBuf & getExtBuf(const string &name);
        
        void setExtBuf(const string &name, const DataBuf &buf);
        
        void appendExtBuf(const string &name, const DataBuf &buf);
        
        void coverExtBuf(const string &name, const DataBuf &buf);
        
        /////////////////////////////////////////////////////////
        
        DataBuf & getBaks();
        
        bool existBakInfo(const string &name);
        
        const long getBakInt(const string &name);
        
        const string getBakString(const string &name);
        
        DataBuf & getUserBak();
        
        DataBuf & getCustBak();
        
        DataBuf & getBakBuf(const string &name);
        
        void setBakInt(const string &name, const long &value);
        
        void setBakString(const string &name, const string &value);
        
        void setUserBak(const DataBuf &user);
        
        void setCustBak(const DataBuf &cust);
        
        void setBakBuf(const string &name, const DataBuf &buf);
        
        void appendBakBuf(const string &name, const DataBuf &buf);
        
        /////////////////////////////////////////////////////////
        
        DataBuf & getRetBuf();
        
        void setRetBuf(const DataBuf &buf);
        
        ////////////////////////////////////////////////////////
        
        DataBuf &getCom();
		
		bool existComInfo(const string &name);

		const long getComInt(const string &name) ;

		const string getComString(const string &name);
         
        void setComInt(const string &name, const long &value);

		void setComString(const string &name, const string &value);

		void setCom(const DataBuf &buf);

		void setComBuf(const string &name, const DataBuf &buf);

		void appendComBuf(const string &name, const DataBuf &buf);
		
		void refreshComBuf(const DataBuf &buf) ;
		
        ////////////////////////////////////////////////////////
        
        void push();
        
        void front();
        
        vector<DataBuf> & getUniDatas();
        
    protected:
        
        UniDataMgr();
        
    private:
        
        void init();
        
        void save();
        
        void at(const int &idx);
        
        static UniDataMgr * instance;
        
        static int _mIdx;
        
        static DataBuf mUniData;
        
        static vector<DataBuf> mMultiUniData;
        
};



#endif //_UNI_DATA_MGR_H_20110217
