#pragma once
class CLogFile
{
public:
    CLogFile();
    ~CLogFile();

private:
    /**
    * @brief ���ݵ�ǰ���ڻ�ȡ��־�ļ���
    */
    static CString GetFileName();
    /**
    * @brief ��ȡ�ļ�·��������ʹ�õ������·��
    */
    static CString GetFilePath();
public:
    static BOOL WriteLog(CString LogText);
};

