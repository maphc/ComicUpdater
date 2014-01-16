#include "stdafx.h"
#include "StrTable.h"


using namespace std;

/**
 *  \fn StrTable()
 *  StrTable 类的构造函数
 */
StrTable::StrTable()
{
    Clear();
}


/**
 *  \fn ~StrTable()
 *  StrTable 类的析构函数
 */
StrTable::~StrTable()
{
}

/**
 *  \fn void Clear()
 *  StrTable 类的 Clear 方法
 *  将 tables 缓存清空
 */
void StrTable::Clear()
{
    tableCount = 0;
    for(int i=0; i<MAX_TABLES; i++)
    {
        tables[i].tableName = "";
        tables[i].rowCount = 0;
        tables[i].colCount = 0;
        tables[i].colIndex = 0;

        for(int j=0; j < (int) tables[i].table.size(); j++)
            tables[i].table[j].Clear();
        tables[i].table.clear();
    }
}

/**
 *  \fn void Count()
 *  StrTable 类的 Count 方法
 *  @return 返回字符串表中的表数量
 */
int StrTable::Count()
{
    return tableCount;
}

/**
 *  \fn int FindTable(const string &tName)
 *  StrTable 类的 FindTable 方法
 *  @param tName 表名
 *  @return 成功时返回表索引，失败时返回-1
 */
int StrTable::FindTable(const string &tName)
{
    for(int i=0; i<MAX_TABLES; i++)
    {
        if(tables[i].tableName == tName)
            return i;
    }
    return -1;
}

/**
 *  \fn int AddTable(const string &tName, const int &cCount)
 *  StrTable 类的 AddTable 方法
 *  在结构数组中创建名为 tName,字段数为 cCount 的表
 *  @param tName 表名
 *  @param cCount 列数
 *  @return 成功时返回表索引，失败时返回-1
 */
int StrTable::AddTable(const string &tName, const int &cCount)
{
    //初始化
    errCode =   0;
    messages = "创建成功";

    //检查合法性
    if(tName.length() != TABLE_NAME_LEN)
    {
        errCode = 2;
        messages = "表名称编码长度出错";
        return -1;
    }
    if(cCount > MAX_COLUMNS)
    {
        errCode = 2;
        messages = "字段数超出限度";
        return -1;
    }
    if (cCount < 1)
    {
        errCode = 1;
        messages = "不能创建无字段记录！";
        return -1;
    }
    //检查该名称的记录是否已经存在
    int i = FindTable(tName);
    //不存在则新建
    if (i == -1)
    {
        //如果还有空位置则插入新记录
        if (tableCount < MAX_TABLES)
        {
            tables[tableCount].tableName = tName;
            tables[tableCount].colCount = cCount;
            tables[tableCount].rowCount = 0;
            tables[tableCount].colIndex = 0;
            i = tableCount;
            tableCount++;
            return i;
        }
        //没有则报错返回
        else
        {
            errCode = 3;
            messages = "没有更多记录空间！";
            return -1;
        }
    }
    //存在同名记录则检查是否完全相同
    else
    {
        //完全相同
        if (tables[i].colCount == cCount)
        {
            errCode = 4;
            messages = "相同记录已存在！";
            return i;
        }
        //不完全相同则报错返回
        else
        {
            errCode = 4;
            messages = "同名记录已存在且字段数不符";
            return -1;
        }
    }
}

/**
 *  \fn int AddField(const string &tName, const string &fldStr)
 *  StrTable 类的 AddTable 方法
 *  在名为 tName 的表中插入一条内容为 fldStr 的字段
 *  @param tName 表名
 *  @param fldStr 字段内容
 *  @return 成功时返回0，失败时返回-1
 */
int StrTable::AddField(const string &tName, const string &fldStr)
{
    int iTidx, iCidx, iRidx, iSize;
    string tmpStr;
    CRecord record;

    errCode = 0;
    messages = "字段插入成功";

    iTidx = FindTable(tName);
    if (iTidx == -1)
    {
        errCode =   1;
        messages = "该表不存在！";
        return -1;
    }
    tmpStr = Trim(fldStr);
    iSize = tmpStr.length();
    if (iSize >= (int) pow(double(10), COL_SIZE_LEN)) //检查字段内容长度是否超出限制
    {
        errCode =   1;
        messages = "字段内容过长！";
        return -1;
    }
    if (tables[iTidx].colIndex == 0)    //新增一条记录
    {
        if (tables[iTidx].rowCount >= (int) pow(double(10), ROW_COUNT_LEN))  //检查行数是否超出限制
        {
            errCode =   2;
            messages = "记录数超出限制！";
            return -1;
        }
        tables[iTidx].rowCount++;
        record.Clear();
        tables[iTidx].table.push_back(record);
    }

    iCidx = tables[iTidx].colIndex;
    iRidx = tables[iTidx].rowCount - 1;
    tables[iTidx].table[iRidx].value[iCidx] = tmpStr;
    tables[iTidx].colIndex++;

    if (tables[iTidx].colIndex >= tables[iTidx].colCount)
    {
        tables[iTidx].colIndex = 0;
    }

    return 0;
}

/**
 *  \fn int ToString(string &outStr)
 *  StrTable 类的 ToString 方法
 *  将结构数组中所有表的内容转换字符串 outStr
 *  @param outStr 输出字符串
 *  @return 成功时返回0，失败时返回-1
 */
int StrTable::ToString(string &outStr)
{
    errCode = 0;
    messages = "提示信息:";
    outStr = "";
    char buf[20], formatstr[20];
    int iLen;

    for (int i=0; i < tableCount; i++)
    {
        if (tables[i].rowCount == 0)
            continue;

        if (tables[i].colIndex != 0)
        {
            errCode = 2;
            messages += "\n表["+tables[i].tableName+"]中字段不完整";
            return -1;
        }

        //拼入表名
        sprintf(formatstr, "%%%ds", TABLE_NAME_LEN);
        sprintf(buf, formatstr, tables[i].tableName.c_str());
        outStr += buf;
        //拼入行数
        sprintf(formatstr, "%%.%dd", ROW_COUNT_LEN);
        sprintf(buf, formatstr, tables[i].rowCount);
        outStr += buf;
        //拼入列数
        sprintf(formatstr, "%%.%dd", COL_COUNT_LEN);
        sprintf(buf, formatstr, tables[i].colCount);
        outStr += buf;

        //拼入表字段内容
        for (int j=0; j < (int) tables[i].table.size(); j++)
        {
            for (int k=0; k < tables[i].colCount; k++)
            {
                //拼入字段内容长度
                iLen = tables[i].table[j].value[k].length();
                sprintf(formatstr, "%%.%dd", COL_SIZE_LEN);
                sprintf(buf, formatstr, iLen);
                outStr += buf;
                //拼入字段内容长度
                outStr += tables[i].table[j].value[k];
            }
        }

        //清空该表记录
        tables[i].rowCount=0;
        tables[i].colIndex=0;
        tables[i].table.clear();
    }
    return 0;
}

/**
 *  \fn int DecodeString(const string &inStr)
 *  StrTable 类的 DecodeString 方法
 *  将 inStr 内容解析为结构数组中的表内容
 *  @param inStr 输入字符串
 *  @return 成功时返回表数量，失败时返回-1
 */
int StrTable::DecodeString(const string &inStr)
{
    string strTemp, tmpStr;
    int iRowIdx, iColIdx, iLength, iLen;
    CRecord record;

    Clear();
    tmpStr = Trim(inStr);
    if(tmpStr == "") return 0;
    iLength = tmpStr.length();

    while (iLength > 0 && tableCount < MAX_TABLES)
    {
        // 获取表名
        strTemp = tmpStr.substr(0, TABLE_NAME_LEN); //TABLE_NAME_LEN 为类型编码长度(如:OpFc)
        tables[tableCount].tableName = strTemp;

        // 获取行数
        strTemp = tmpStr.substr(TABLE_NAME_LEN, ROW_COUNT_LEN);   //ROW_COUNT_LEN 为行数编码长度
        try
        {
            tables[tableCount].rowCount = StrToInt(strTemp);
        }
        catch(...)
        {
            errCode = 3;
            messages = "字符串编码错误！";
            return -1;
        }

        // 获取列数
        strTemp = tmpStr.substr(TABLE_NAME_LEN + ROW_COUNT_LEN, COL_COUNT_LEN); //COL_COUNT_LEN 为字段个数编码长度
        try
        {
            tables[tableCount].colCount = StrToInt(strTemp);
        }
        catch(...)
        {
            errCode = 3;
            messages = "字符串编码错误！";
            return -1;
        }

        // 截取剩下的字符串
        tmpStr = tmpStr.substr(TABLE_NAME_LEN + ROW_COUNT_LEN + COL_COUNT_LEN);
        iLength = tmpStr.length();

        iRowIdx = 0;
        iColIdx = 0;

        // 获取表字段内容
        while (iLength > 0 && tables[tableCount].colCount < MAX_COLUMNS)
        {
            if (iColIdx == 0)
            {
                record.Clear();
                tables[tableCount].table.push_back(record);
            }
            // 获取字段内容长度
            strTemp = tmpStr.substr(0, COL_SIZE_LEN); //COL_SIZE_LEN 为字段内容长度编码长度
            try
            {
                iLen = StrToInt(strTemp);
            }
            catch(...)
            {
                errCode = 3;
                messages = "字符串编码错误！";
                return -1;
            }
            // 获取字段内容
            strTemp = tmpStr.substr(COL_SIZE_LEN, iLen);
            tables[tableCount].table[iRowIdx].value[iColIdx] = tmpStr.substr(COL_SIZE_LEN, iLen);

            // 截取剩下的字符串
            tmpStr = tmpStr.substr(COL_SIZE_LEN + iLen);
            iLength = tmpStr.length();

            iColIdx++;
            tables[tableCount].colIndex = iColIdx;
            if(iColIdx == tables[tableCount].colCount)
            {
                iColIdx = 0;
                tables[tableCount].colIndex = iColIdx;
                iRowIdx++;

                if(iRowIdx == tables[tableCount].rowCount)
                    break;
            }
        }
        tableCount++;
    }
    return tableCount;
}

/**
 *  \fn string Trim(const string &inStr)
 *  公共函数:Trim 方法
 *  去掉字符串 inStr 两端的空格
 *  @return 去除空格后的字符串
 */
string Trim(const string &inStr)
{
    int lpos, rpos;
    string outStr = inStr;
    if(outStr == "") return outStr;

    lpos = outStr.find_first_not_of(' ');
    if (lpos > 0)
        outStr = outStr.substr(lpos);

    rpos = outStr.find_last_not_of(' ');
    if (rpos < (int) outStr.length())
        outStr = outStr.erase(rpos+1);

    return outStr;
}

/**
 *  \fn int StrToInt(string &inStr)
 *  公共函数:StrToInt 方法
 *  将字符串 inStr 转换为 int 型
 *  @return 返回转换后的整形值
 */
int StrToInt(string inStr)
{
    return atoi(inStr.c_str());
}