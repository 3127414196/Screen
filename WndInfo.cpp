#include "stdafx.h"
#include "WndInfo.h"
#include <WindowsX.h> 

void CWndEventInfo::SetEventInfo(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
    this->m_hWinEventHook = hWinEventHook;
    this->m_dwEvent = dwEvent;
    this->m_hWnd = hwnd;
    this->m_idObject = idObject;
    this->m_idChild = idChild;
    this->m_dwEventThread = dwEventThread;
    this->m_dwmsEventTime = dwmsEventTime;
}

void CWndEventInfo::CopyEventInfo(CWndEventInfo& eventinfo)
{
    this->m_hWinEventHook = eventinfo.m_hWinEventHook;
    this->m_dwEvent = eventinfo.m_dwEvent;
    this->m_hWnd = eventinfo.m_hWnd;
    this->m_idObject = eventinfo.m_idObject;
    this->m_idChild = eventinfo.m_idChild;
    this->m_dwEventThread = eventinfo.m_dwEventThread;
    this->m_dwmsEventTime = eventinfo.m_dwmsEventTime;
}

CWndInfo::CWndInfo()
: m_hWnd(NULL)
, m_rcWnd(0,0,0,0)
, m_dwStyle(0)
, m_dwExStyle(0)
, m_ptMouse(0,0)
, m_dwPID(0)
, m_dwTID(0)
{

}

void CWndInfo::SetWnd(HWND hWnd, HMODULE hPsapi/* = NULL*/, EInfoType type/* = WIT_ALL*/)
{ 
	m_hWnd = hWnd; 
	m_WndEvent.m_hWnd = hWnd;
	m_dwTID = m_WndEvent.m_dwEventThread;

	if (type & WIT_TITLE) QueryTitle(m_hWnd, m_strTitle);
	//if (type & WIT_PROCESS) QueryProcess(hPsapi);
	if (type & WIT_CLASS) QueryClass(m_hWnd, m_strWndClass);
	if (type & WIT_RECT) QueryRect(m_hWnd, m_rcWnd);
	if (type & WIT_STYLE) QueryStyle(m_hWnd, m_dwStyle, m_dwExStyle);
	if (type & WIT_MOUSE) QueryMouse(m_ptMouse);
}

void CWndInfo::SetWndEvent(const CWndEventInfo& oWndEvent, HMODULE hPsapi/* = NULL*/, EInfoType type/* = WIT_ALL*/)
{
	m_WndEvent = oWndEvent;
	SetWnd(oWndEvent.m_hWnd, hPsapi, type);
}

BOOL CWndInfo::QueryTitle(HWND hWnd, CAtlString& strTitle)
{
	BOOL bRet = FALSE;

	TCHAR szTitle[MAX_PATH + 1] = {0};
	bRet = (hWnd != NULL) && (GetWindowText(hWnd, szTitle, MAX_PATH) > 0);
	strTitle = szTitle;

	if (!bRet)
	{
		//LOG_WND_ERROR_PRINTF(L"%s, GetTitle Failed. ErrorCode(%d), Title(%s)", __FUNCTIONW__, GetLastError(), m_strTitle);
	}

	return bRet;
}

BOOL CWndInfo::QueryClass(HWND hWnd, CAtlString& strClass)
{
	TCHAR szWndClass[MAX_PATH + 1] = {0};
	BOOL bRet = (hWnd != NULL) && (GetClassName(hWnd, szWndClass, MAX_PATH) > 0);
	strClass = szWndClass;

	if (!bRet)
	{
		//LOG_WND_ERROR_PRINTF(L"%s, GetClassName Failed. ErrorCode(%d), Class(%s)", __FUNCTIONW__, GetLastError(), m_strWndClass);
	}

	return bRet;
}

BOOL CWndInfo::QueryRect(HWND hWnd, CRect& rcWnd)
{
	BOOL bRet = FALSE;

	__try
	{
		bRet = (hWnd != NULL) && GetWindowRect(hWnd, &rcWnd);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		bRet = FALSE;
	}

	if (!bRet)
	{
		//LOG_WND_ERROR_PRINTF(L"%s, GetWindowRect Failed. ErrorCode(%d)", __FUNCTIONW__, GetLastError());
	}

	return bRet;
}

BOOL CWndInfo::QueryStyle(HWND hWnd, DWORD& dwStyle, DWORD& dwExStyle)
{
	BOOL bRet = TRUE;
	dwStyle = GetWindowStyle(hWnd);
	dwExStyle = GetWindowExStyle(hWnd);

	return bRet;
}

BOOL CWndInfo::QueryMouse(CPoint& ptMouse)
{
	return GetCursorPos(&ptMouse);
}

CAtlString CWndInfo::PrintWnd() const
{
	CAtlString strLog;
	strLog.Format(L" [ hwnd(0x%0x), process(%s), title(%s), class(%s), rect(%d,%d,%d,%d), size(%d,%d), style(0x%0x), exstyle(0x%0x)) ]", 
		m_hWnd, m_strProcessName, m_strTitle, m_strWndClass, m_rcWnd.left, m_rcWnd.top, m_rcWnd.right, m_rcWnd.bottom,
		m_rcWnd.Width(), m_rcWnd.Height(), m_dwStyle, m_dwExStyle);

    CLogFile::WriteLog(strLog);
	return strLog;
}

void CWndInfo::Print() const
{
    CString strLog = _T("");

    strLog.Format(_T("%s ======================== 窗口信息 =======================\n"), __FUNCTIONW__);
    CLogFile::WriteLog(strLog);


	// 屏幕坐标
	CRect rcArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcArea, 0);


    strLog.Format(_T("%s, sysscreen(%d,%d), sysclient(%d,%d)"), __FUNCTIONW__,
		GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), rcArea.Width(), rcArea.Height());
    CLogFile::WriteLog(strLog);


    strLog.Format(_T("%s, hwnd(%0x), title(%s), process(%s), class(%s), Parent(%s)"), __FUNCTIONW__,
		m_hWnd, m_strTitle, m_strProcessName, m_strWndClass, m_strParentProcess);
    CLogFile::WriteLog(strLog);


    strLog.Format(_T("%s, hwnd(%0x), rect(%d,%d,%d,%d), size(%d,%d), style(%0x), exstyle(%0x)"), __FUNCTIONW__,
		m_hWnd, m_rcWnd.left, m_rcWnd.top, m_rcWnd.right, m_rcWnd.bottom, m_rcWnd.Width(), m_rcWnd.Height(), m_dwStyle, m_dwExStyle);
    CLogFile::WriteLog(strLog);

    strLog.Format(_T("%s, hwnd(%0x), procpath(%s), parentpath(%s)"), __FUNCTIONW__,
		m_hWnd, m_strProcessPath, m_strParentPath);
    CLogFile::WriteLog(strLog);
}

BOOL CWndInfo::IsInvalid()
{
    char buffer[200];

	if (m_strTitle.IsEmpty() && m_strProcessName.IsEmpty() && m_strProcessPath.IsEmpty() && m_strWndClass.IsEmpty())
	{
        memset(buffer, 0, 200);
		//snprintf(buffer, 200, "%s, 无效窗口(hwnd:0x%0x event:0x%0x)，标题、进程、类名，都为空", __FUNCTIONW__, m_hWnd, m_WndEvent);
        //LogDebug(buffer, sizeof(buffer) + 1);
		return TRUE;
	}

	// 窗口的left、top、right、bottom全部为负数或全为0，说明无效
	if (m_rcWnd.left <= 0 && m_rcWnd.top <= 0 && m_rcWnd.right <= 0 && m_rcWnd.bottom <= 0)
	{
		return TRUE;
	}

	if (m_rcWnd.Width() == 0 && m_rcWnd.Height() == 0)
	{
		//snprintf(buffer, 200, "%s, 无效窗口(hwnd:0x%0x event:0x%0x)，窗口大小为0*0", __FUNCTIONW__, m_hWnd, m_WndEvent);
		return TRUE;
	}

    if (m_strWndClass.Compare(_T("TXGuiFoundation")) == 0)
    {
        return TRUE;
    }

	return FALSE;
}

BOOL CWndInfo::IsTop() const
{
	if (m_hWnd && IsWindow(m_hWnd) && (NULL == GetParent(m_hWnd)))
	{
		return TRUE;
	}

	return FALSE;
}