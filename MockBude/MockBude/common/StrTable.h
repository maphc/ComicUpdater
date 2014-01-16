#ifndef __StrTable_H_
#define __StrTable_H_

#include "../stdafx.h"

using namespace std;

const int MAX_TABLES      = 20;     /**< 最大表数量 */
const int MAX_COLUMNS     = 40;     /**< 最大列数量 */
const int TABLE_NAME_LEN  = 4;      /**< 表名编码长度 */
const int ROW_COUNT_LEN   = 3;      /**< 行数编码长度 */
const int COL_COUNT_LEN   = 2;      /**< 列数编码长度 */
const int COL_SIZE_LEN    = 4;      /**< 字段内容长度编码长度 */

/** @class CRecord
 *  @brief 表记录类
 *  定义单条表记录类型
 */
class CRecord
{
    public:
        string value[MAX_COLUMNS];
        void Clear()
        {
            for (int i=0; i<MAX_COLUMNS; i++) value[i].clear();
        }
};

/**
 * 定义表类型
 */
typedef vector <CRecord> CTable;

string Trim(const string &inStr);       /**< 公共函数:去掉字符串 inStr 两端的空格 */
int StrToInt(const string inStr);     /**< 公共函数:将字符串 inStr 转换为 int 型 */

/** @class StrTable
 *  @brief 字符串表
 *  可将一个或多个表的内容拼成字符串，也可将一个字符串解析为表内容
 *  一个字符串表的例子：
 *  "FeSt00302000110010标准全球通000120006神州行000130008动感地带OpFc00203000110004语音00011000120004短信00010OpFe00303000100001C00025000011000100003100000120001Z00042000"
 *  解析如下:
 *  第一张表:FeSt
 *  FeSt        表名
 *  003         记录数
 *  02          字段数
 *  0001        第一个字段实际长度
 *  1           第一个字段内容
 *  0010        第二个字段实际长度
 *  标准全球通    第二个字段内容
 *  ...
 *  第二张表：OpFc
 *  OpFc        表名
 *  003         记录数
 *  02          字段数
 *  0001        第一个字段实际长度
 *  1           第一个字段内容
 *  0010        第二个字段实际长度
 *  标准全球通    第二个字段内容
 *  ...
 */
class StrTable
{
    public:
        StrTable();                                             /**< 构造函数 */
        virtual ~StrTable();                                    /**< 析构函数 */

        void Clear();                                           /**< 清空公共结构数组 */
        int Count();                                            /**< 返回表数量 */
        int AddTable(const string &tName, const int &cCount);   /**< 在结构数组中创建名为 tName,字段数为 cCount 的表 */
        int AddField(const string &tName, const string &fldStr);/**< 在名为 tName 的表中插入一条内容为 fldStr 的字段 */
        int FindTable(const string &tName);                     /**< 通过表名查找表索引 */
        int DecodeString(const string &inStr);                  /**< 解串方法:将 inStr 内容解析为结构数组中的表内容 */
        int ToString(string &outStr);                           /**< 将结构数组中所有表的内容转换字符串 outStr */

    public:
        /**
         *  结构数组
         */
        struct
        {
            string tableName;                       /**< 表名 */
            int rowCount;                           /**< 行数 */
            int colCount;                           /**< 列数 */
            int colIndex;                           /**< 列索引 */
            CTable table;                            /**< 表字段内容 */
        } tables[MAX_TABLES];

        string messages;                            /**< 上一次操作的错误消息 */
        int errCode;                                /**< 上一次操作的错误编码 */

    private:
        int tableCount;                             /**< 现有表数量 */
};




#endif  /* __StrTable_H_ */
