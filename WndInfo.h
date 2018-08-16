#pragma once

class CWndEventInfo
{
public:
	CWndEventInfo() 
		: m_hWinEventHook(NULL)
		, m_dwEvent(0)
		, m_hWnd(NULL)
		, m_idObject(0)
		, m_idChild(0)
		, m_dwEventThread(0)
		, m_dwmsEventTime(0)
	{}

	CWndEventInfo(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) 
		: m_hWinEventHook(hWinEventHook)
		, m_dwEvent(dwEvent)
		, m_hWnd(hwnd)
		, m_idObject(idObject)
		, m_idChild(idChild)
		, m_dwEventThread(dwEventThread)
		, m_dwmsEventTime(dwmsEventTime)
	{}

	BOOL IsRepeat(const CWndEventInfo& oInfo) const
	{
		if (m_dwEvent == oInfo.m_dwEvent &&
			m_hWnd == oInfo.m_hWnd &&
			m_idChild == oInfo.m_idChild &&
			m_idObject == oInfo.m_idObject)
		{
			return TRUE;
		}

		return FALSE;
	}

    void SetEventInfo(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime);
    void CopyEventInfo(CWndEventInfo& eventinfo);

	HWINEVENTHOOK m_hWinEventHook;
	DWORD m_dwEvent;
	HWND m_hWnd;
	LONG m_idObject;
	LONG m_idChild;
	DWORD m_dwEventThread;
	DWORD m_dwmsEventTime;
};

// 窗口基础信息
class CWndInfo
{
public:
	CWndInfo();

	enum EInfoType
	{
		WIT_TITLE = 0x01,
		WIT_PROCESS = 0x02,
		WIT_CLASS = 0x04,
		WIT_RECT = 0x08,
		WIT_STYLE = 0x10,
		WIT_MOUSE = 0x20,
		WIT_ALL_BUT_PROCESS = WIT_TITLE | WIT_CLASS | WIT_RECT | WIT_STYLE | WIT_MOUSE,
		WIT_ALL = WIT_TITLE | WIT_PROCESS | WIT_CLASS | WIT_RECT | WIT_STYLE | WIT_MOUSE
	};

	// 若采用驱动获取进程，需要先加载psapi.dll，hPsapi句柄就是用来拿到它的
	void SetWnd(HWND hWnd, HMODULE hPsapi = NULL, EInfoType type = WIT_ALL);
	void SetWndEvent(const CWndEventInfo& oWndEvent, HMODULE hPsapi = NULL, EInfoType type = WIT_ALL);
	void Print() const;
	CAtlString PrintWnd() const;
	BOOL IsInvalid();
	BOOL IsTop() const;

	// 判定两个窗口相等，实际上只关心进程、窗口标题、类名、位置
	friend bool operator==(const CWndInfo& left, const CWndInfo& right)
	{
		if (//(left.m_strTitle.CompareNoCase(right.m_strTitle) == 0) &&
			//(left.m_strProcessName.CompareNoCase(right.m_strProcessName) == 0) &&
			//(left.m_strWndClass.CompareNoCase(right.m_strWndClass) == 0) &&
			(left.m_rcWnd == right.m_rcWnd))
		{
			return true;
		}

		return false;
	}

	HWND m_hWnd;
	CWndEventInfo m_WndEvent;
	CAtlString m_strTitle;
	CAtlString m_strProcessName;
	CAtlString m_strProcessPath;
	CAtlString m_strParentProcess;
	CAtlString m_strParentPath;
	CAtlString m_strWndClass;
	CRect m_rcWnd;
	DWORD m_dwStyle;
	DWORD m_dwExStyle;
	CPoint	m_ptMouse;
	CAtlString	m_strSoftName;		// 老版本有，现在已经废弃掉了
	CAtlString	m_strCompany;
	DWORD m_dwPID;
	DWORD m_dwTID;

	// 提供一些工具
	static BOOL QueryTitle(HWND hWnd, CAtlString& strTitle);
	static BOOL QueryClass(HWND hWnd, CAtlString& strClass);
	static BOOL QueryRect(HWND hWnd, CRect& rcWnd);
	static BOOL QueryStyle(HWND hWnd, DWORD& dwStyle, DWORD& dwExStyle);
	static BOOL QueryMouse(CPoint& ptMouse);
};
