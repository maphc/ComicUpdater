#include "stdafx.h"
#include "StrTable.h"


using namespace std;

/**
 *  \fn StrTable()
 *  StrTable ��Ĺ��캯��
 */
StrTable::StrTable()
{
    Clear();
}


/**
 *  \fn ~StrTable()
 *  StrTable �����������
 */
StrTable::~StrTable()
{
}

/**
 *  \fn void Clear()
 *  StrTable ��� Clear ����
 *  �� tables �������
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
 *  StrTable ��� Count ����
 *  @return �����ַ������еı�����
 */
int StrTable::Count()
{
    return tableCount;
}

/**
 *  \fn int FindTable(const string &tName)
 *  StrTable ��� FindTable ����
 *  @param tName ����
 *  @return �ɹ�ʱ���ر�������ʧ��ʱ����-1
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
 *  StrTable ��� AddTable ����
 *  �ڽṹ�����д�����Ϊ tName,�ֶ���Ϊ cCount �ı�
 *  @param tName ����
 *  @param cCount ����
 *  @return �ɹ�ʱ���ر�������ʧ��ʱ����-1
 */
int StrTable::AddTable(const string &tName, const int &cCount)
{
    //��ʼ��
    errCode =   0;
    messages = "�����ɹ�";

    //���Ϸ���
    if(tName.length() != TABLE_NAME_LEN)
    {
        errCode = 2;
        messages = "�����Ʊ��볤�ȳ���";
        return -1;
    }
    if(cCount > MAX_COLUMNS)
    {
        errCode = 2;
        messages = "�ֶ��������޶�";
        return -1;
    }
    if (cCount < 1)
    {
        errCode = 1;
        messages = "���ܴ������ֶμ�¼��";
        return -1;
    }
    //�������Ƶļ�¼�Ƿ��Ѿ�����
    int i = FindTable(tName);
    //���������½�
    if (i == -1)
    {
        //������п�λ��������¼�¼
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
        //û���򱨴���
        else
        {
            errCode = 3;
            messages = "û�и����¼�ռ䣡";
            return -1;
        }
    }
    //����ͬ����¼�����Ƿ���ȫ��ͬ
    else
    {
        //��ȫ��ͬ
        if (tables[i].colCount == cCount)
        {
            errCode = 4;
            messages = "��ͬ��¼�Ѵ��ڣ�";
            return i;
        }
        //����ȫ��ͬ�򱨴���
        else
        {
            errCode = 4;
            messages = "ͬ����¼�Ѵ������ֶ�������";
            return -1;
        }
    }
}

/**
 *  \fn int AddField(const string &tName, const string &fldStr)
 *  StrTable ��� AddTable ����
 *  ����Ϊ tName �ı��в���һ������Ϊ fldStr ���ֶ�
 *  @param tName ����
 *  @param fldStr �ֶ�����
 *  @return �ɹ�ʱ����0��ʧ��ʱ����-1
 */
int StrTable::AddField(const string &tName, const string &fldStr)
{
    int iTidx, iCidx, iRidx, iSize;
    string tmpStr;
    CRecord record;

    errCode = 0;
    messages = "�ֶβ���ɹ�";

    iTidx = FindTable(tName);
    if (iTidx == -1)
    {
        errCode =   1;
        messages = "�ñ����ڣ�";
        return -1;
    }
    tmpStr = Trim(fldStr);
    iSize = tmpStr.length();
    if (iSize >= (int) pow(double(10), COL_SIZE_LEN)) //����ֶ����ݳ����Ƿ񳬳�����
    {
        errCode =   1;
        messages = "�ֶ����ݹ�����";
        return -1;
    }
    if (tables[iTidx].colIndex == 0)    //����һ����¼
    {
        if (tables[iTidx].rowCount >= (int) pow(double(10), ROW_COUNT_LEN))  //��������Ƿ񳬳�����
        {
            errCode =   2;
            messages = "��¼���������ƣ�";
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
 *  StrTable ��� ToString ����
 *  ���ṹ���������б������ת���ַ��� outStr
 *  @param outStr ����ַ���
 *  @return �ɹ�ʱ����0��ʧ��ʱ����-1
 */
int StrTable::ToString(string &outStr)
{
    errCode = 0;
    messages = "��ʾ��Ϣ:";
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
            messages += "\n��["+tables[i].tableName+"]���ֶβ�����";
            return -1;
        }

        //ƴ�����
        sprintf(formatstr, "%%%ds", TABLE_NAME_LEN);
        sprintf(buf, formatstr, tables[i].tableName.c_str());
        outStr += buf;
        //ƴ������
        sprintf(formatstr, "%%.%dd", ROW_COUNT_LEN);
        sprintf(buf, formatstr, tables[i].rowCount);
        outStr += buf;
        //ƴ������
        sprintf(formatstr, "%%.%dd", COL_COUNT_LEN);
        sprintf(buf, formatstr, tables[i].colCount);
        outStr += buf;

        //ƴ����ֶ�����
        for (int j=0; j < (int) tables[i].table.size(); j++)
        {
            for (int k=0; k < tables[i].colCount; k++)
            {
                //ƴ���ֶ����ݳ���
                iLen = tables[i].table[j].value[k].length();
                sprintf(formatstr, "%%.%dd", COL_SIZE_LEN);
                sprintf(buf, formatstr, iLen);
                outStr += buf;
                //ƴ���ֶ����ݳ���
                outStr += tables[i].table[j].value[k];
            }
        }

        //��ոñ��¼
        tables[i].rowCount=0;
        tables[i].colIndex=0;
        tables[i].table.clear();
    }
    return 0;
}

/**
 *  \fn int DecodeString(const string &inStr)
 *  StrTable ��� DecodeString ����
 *  �� inStr ���ݽ���Ϊ�ṹ�����еı�����
 *  @param inStr �����ַ���
 *  @return �ɹ�ʱ���ر�������ʧ��ʱ����-1
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
        // ��ȡ����
        strTemp = tmpStr.substr(0, TABLE_NAME_LEN); //TABLE_NAME_LEN Ϊ���ͱ��볤��(��:OpFc)
        tables[tableCount].tableName = strTemp;

        // ��ȡ����
        strTemp = tmpStr.substr(TABLE_NAME_LEN, ROW_COUNT_LEN);   //ROW_COUNT_LEN Ϊ�������볤��
        try
        {
            tables[tableCount].rowCount = StrToInt(strTemp);
        }
        catch(...)
        {
            errCode = 3;
            messages = "�ַ����������";
            return -1;
        }

        // ��ȡ����
        strTemp = tmpStr.substr(TABLE_NAME_LEN + ROW_COUNT_LEN, COL_COUNT_LEN); //COL_COUNT_LEN Ϊ�ֶθ������볤��
        try
        {
            tables[tableCount].colCount = StrToInt(strTemp);
        }
        catch(...)
        {
            errCode = 3;
            messages = "�ַ����������";
            return -1;
        }

        // ��ȡʣ�µ��ַ���
        tmpStr = tmpStr.substr(TABLE_NAME_LEN + ROW_COUNT_LEN + COL_COUNT_LEN);
        iLength = tmpStr.length();

        iRowIdx = 0;
        iColIdx = 0;

        // ��ȡ���ֶ�����
        while (iLength > 0 && tables[tableCount].colCount < MAX_COLUMNS)
        {
            if (iColIdx == 0)
            {
                record.Clear();
                tables[tableCount].table.push_back(record);
            }
            // ��ȡ�ֶ����ݳ���
            strTemp = tmpStr.substr(0, COL_SIZE_LEN); //COL_SIZE_LEN Ϊ�ֶ����ݳ��ȱ��볤��
            try
            {
                iLen = StrToInt(strTemp);
            }
            catch(...)
            {
                errCode = 3;
                messages = "�ַ����������";
                return -1;
            }
            // ��ȡ�ֶ�����
            strTemp = tmpStr.substr(COL_SIZE_LEN, iLen);
            tables[tableCount].table[iRowIdx].value[iColIdx] = tmpStr.substr(COL_SIZE_LEN, iLen);

            // ��ȡʣ�µ��ַ���
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
 *  ��������:Trim ����
 *  ȥ���ַ��� inStr ���˵Ŀո�
 *  @return ȥ���ո����ַ���
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
 *  ��������:StrToInt ����
 *  ���ַ��� inStr ת��Ϊ int ��
 *  @return ����ת���������ֵ
 */
int StrToInt(string inStr)
{
    return atoi(inStr.c_str());
}