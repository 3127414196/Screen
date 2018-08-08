#include "stdafx.h"
#include "LogFile.h"

CLogFile::CLogFile()
{
}


CLogFile::~CLogFile()
{
}


CString CLogFile::GetFileName()
{
    CString m_FileName;

    m_FileName = CTime::GetCurrentTime().Format("%Y-%m-%d") + TEXT(".log");
    
    return m_FileName;
}

CString CLogFile::GetFilePath()
{
    return _T("../Output/Test/");
}

BOOL CLogFile::WriteLog(CString LogText)
{
    try
    {
        CFile m_File;

        CString m_FileName = GetFileName();
        CString m_FilePath = GetFilePath() + m_FileName;
        DWORD err= 0;
        if (!m_File.Open(m_FilePath,
            CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyWrite))
        {
            err = GetLastError();
            return FALSE;
        }

        m_File.SeekToEnd();

        if (sizeof(TCHAR) == sizeof(WCHAR))
        {
            WORD wSignature = 0XFEFF;
            m_File.Write(&wSignature, 2);
        }

        CString m_Message;

        LogText += TEXT("\r\n");
        m_File.Write(LogText, LogText.GetLength() * sizeof(TCHAR));
        m_File.Close();
    }
    catch (const CFileException& fileException)
    {
        return FALSE;
    }

    return TRUE;
}