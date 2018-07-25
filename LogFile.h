#pragma once
class CLogFile
{
public:
    CLogFile();
    ~CLogFile();

private:
    /**
    * @brief 根据当前日期获取日志文件名
    */
    static CString GetFileName();
    /**
    * @brief 获取文件路径，这里使用的是相对路径
    */
    static CString GetFilePath();
public:
    static BOOL WriteLog(CString LogText);
};

