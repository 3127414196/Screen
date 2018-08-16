#include "stdafx.h"
#include "WinEventHook.h"
#include "SingletonEventHandle.h"
#include "AutoCriticalSection.h"
#include "TaskPool.h"

// Callback function that handles events.
//
void CALLBACK CWinEventHook::HandleWinEvent(HWINEVENTHOOK hook, DWORD event, HWND hwnd,
	LONG idObject, LONG idChild,
	DWORD dwEventThread, DWORD dwmsEventTime)
{
	CWinEventHook::Instance().OnWinEventCallback(event, hwnd,
		idObject, idChild,
		dwEventThread, dwmsEventTime);
}


CWinEventHook::CWinEventHook()
	: m_hWinEventHook(NULL)
	, m_pCallback(NULL)
	, m_bInitCOM(FALSE)
{
}


CWinEventHook::~CWinEventHook()
{
	UnHook();
}

CWinEventHook& CWinEventHook::Instance()
{
	static CWinEventHook theObj;
	return theObj;
}

HRESULT CWinEventHook::Hook(IWinEventHookCallback* pCallback)
{
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		return hr;
	}

	m_bInitCOM = TRUE;

	m_pCallback = pCallback;

	m_hWinEventHook = SetWinEventHook(
		EVENT_MIN, EVENT_MAX,  // Range of events (4 to 5).
		NULL,                                          // Handle to DLL.
		HandleWinEvent,                                // The callback.
		0, 0,              // Process and thread IDs of interest (0 = all)
		WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS); // Flags.

	return hr;
}

void CWinEventHook::UnHook()
{
	if (m_hWinEventHook)
	{
		UnhookWinEvent(m_hWinEventHook);
		m_hWinEventHook = NULL;
	}
	
	if (m_bInitCOM)
	{
		CoUninitialize();
		m_bInitCOM = FALSE;
	}
}

void CWinEventHook::OnWinEventCallback(DWORD event, HWND hwnd,
	LONG idObject, LONG idChild,
	DWORD dwEventThread, DWORD dwmsEventTime)
{
	if (m_pCallback)
	{
		m_pCallback->OnWinEventCallback(event, hwnd,
			idObject, idChild,
			dwEventThread, dwmsEventTime);
	}
}

void WinEventHookCallback::OnWinEventCallback(DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
    EventPool &eventpool = CSingletonEventHandle<EventPool>::getInstance();

    do
    {
        if (!eventpool.m_Continue)
            break;

        EVENTINFO eventinfo(event, hwnd, idObject, idChild, dwEventThread, dwmsEventTime);
        //判断窗口是否有效
        if (eventinfo.IsInValid())
            break;

        
        //加锁从taskpool取出待操作的event
        {
            CAutoCriticalSection lock(eventpool.m_cs);
            //去重
            if (eventpool.m_taskpool->IsRepeat(eventinfo))
            {
                CString strLog;
                strLog.Format(_T("%d [0] front(%d) rear(%d)  event(0x%x) hwnd(0x%x) idObject(0x%x) idChild(0x%x) dwEventThread(%d) dwEventTime(%d)\n"), eventpool.m_taskpool->m_nowsize, eventpool.m_taskpool->m_front, eventpool.m_taskpool->m_rear, eventinfo.m_event, eventinfo.m_hwnd, eventinfo.m_idObject, eventinfo.m_idChild, eventinfo.m_dwEventThread, eventinfo.m_dwmsEventTime);
                //CLogFile::WriteLog(strLog);
                break;
            }

            CString strLog;
            strLog.Format(_T("%d [1] front(%d) rear(%d)  event(0x%x) hwnd(0x%x) idObject(0x%x) idChild(0x%x) dwEventThread(%d) dwEventTime(%d)\n"), eventpool.m_taskpool->m_nowsize, eventpool.m_taskpool->m_front, eventpool.m_taskpool->m_rear, eventinfo.m_event, eventinfo.m_hwnd, eventinfo.m_idObject, eventinfo.m_idChild, eventinfo.m_dwEventThread, eventinfo.m_dwmsEventTime);
            //CLogFile::WriteLog(strLog);
            eventpool.m_taskpool->AddTask(eventinfo);
        }

    } while (FALSE);
}
