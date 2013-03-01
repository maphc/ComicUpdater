#ifndef __StrTable_H_
#define __StrTable_H_

#include "../stdafx.h"

using namespace std;

const int MAX_TABLES      = 20;     /**< �������� */
const int MAX_COLUMNS     = 40;     /**< ��������� */
const int TABLE_NAME_LEN  = 4;      /**< �������볤�� */
const int ROW_COUNT_LEN   = 3;      /**< �������볤�� */
const int COL_COUNT_LEN   = 2;      /**< �������볤�� */
const int COL_SIZE_LEN    = 4;      /**< �ֶ����ݳ��ȱ��볤�� */

/** @class CRecord
 *  @brief ���¼��
 *  ���嵥�����¼����
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
 * ���������
 */
typedef vector <CRecord> CTable;

string Trim(const string &inStr);       /**< ��������:ȥ���ַ��� inStr ���˵Ŀո� */
int StrToInt(const string inStr);     /**< ��������:���ַ��� inStr ת��Ϊ int �� */

/** @class StrTable
 *  @brief �ַ�����
 *  �ɽ�һ�������������ƴ���ַ�����Ҳ�ɽ�һ���ַ�������Ϊ������
 *  һ���ַ���������ӣ�
 *  "FeSt00302000110010��׼ȫ��ͨ000120006������000130008���еش�OpFc00203000110004����00011000120004����00010OpFe00303000100001C00025000011000100003100000120001Z00042000"
 *  ��������:
 *  ��һ�ű�:FeSt
 *  FeSt        ����
 *  003         ��¼��
 *  02          �ֶ���
 *  0001        ��һ���ֶ�ʵ�ʳ���
 *  1           ��һ���ֶ�����
 *  0010        �ڶ����ֶ�ʵ�ʳ���
 *  ��׼ȫ��ͨ    �ڶ����ֶ�����
 *  ...
 *  �ڶ��ű�OpFc
 *  OpFc        ����
 *  003         ��¼��
 *  02          �ֶ���
 *  0001        ��һ���ֶ�ʵ�ʳ���
 *  1           ��һ���ֶ�����
 *  0010        �ڶ����ֶ�ʵ�ʳ���
 *  ��׼ȫ��ͨ    �ڶ����ֶ�����
 *  ...
 */
class StrTable
{
    public:
        StrTable();                                             /**< ���캯�� */
        virtual ~StrTable();                                    /**< �������� */

        void Clear();                                           /**< ��չ����ṹ���� */
        int Count();                                            /**< ���ر����� */
        int AddTable(const string &tName, const int &cCount);   /**< �ڽṹ�����д�����Ϊ tName,�ֶ���Ϊ cCount �ı� */
        int AddField(const string &tName, const string &fldStr);/**< ����Ϊ tName �ı��в���һ������Ϊ fldStr ���ֶ� */
        int FindTable(const string &tName);                     /**< ͨ���������ұ����� */
        int DecodeString(const string &inStr);                  /**< �⴮����:�� inStr ���ݽ���Ϊ�ṹ�����еı����� */
        int ToString(string &outStr);                           /**< ���ṹ���������б������ת���ַ��� outStr */

    public:
        /**
         *  �ṹ����
         */
        struct
        {
            string tableName;                       /**< ���� */
            int rowCount;                           /**< ���� */
            int colCount;                           /**< ���� */
            int colIndex;                           /**< ������ */
            CTable table;                            /**< ���ֶ����� */
        } tables[MAX_TABLES];

        string messages;                            /**< ��һ�β����Ĵ�����Ϣ */
        int errCode;                                /**< ��һ�β����Ĵ������ */

    private:
        int tableCount;                             /**< ���б����� */
};




#endif  /* __StrTable_H_ */
