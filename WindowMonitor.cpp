#include "stdafx.h"
#include "WindowMonitor.h"
#include "BitmapMemDC.h"
#include "MutiThreadScreenShot.h"
#include "WndInfo.h"
#include "TaskPool.h"
#include <oleacc.h>
#include <vector>

//全局变量。
HWINEVENTHOOK g_hook;
BOOL g_Continue;
CTaskPool<EVENTINFO>* g_taskpool;
HANDLE hTheds[2];
DWORD threadid[2];

void CALLBACK HandleWindEvent(HWINEVENTHOOK hook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
    CLogFile::WriteLog(_T("enter"));

    do
    {
        if (!g_Continue)
        {
            if (!PostThreadMessage(threadid[0], WM_QUIT, 0, 0))
            {
                GetLastError();
            }
            break;
        }

        
        EVENTINFO eventinfo(event, hwnd, idObject, idChild, dwEventThread, dwmsEventTime);
        //判断窗口是否有效
        if (eventinfo.IsInValid())
            break;

        //去重
        if (g_taskpool->IsRepeat(eventinfo))
        {
            CString strLog;
            strLog.Format(_T("[0] front(%d) rear(%d)  event(0x%x) hwnd(0x%x) idObject(0x%x) idChild(0x%x) dwEventThread(%d) dwEventTime(%d)\n"), g_taskpool->m_front, g_taskpool->m_rear, eventinfo.m_event, eventinfo.m_hwnd, eventinfo.m_idObject, eventinfo.m_idChild, eventinfo.m_dwEventThread, eventinfo.m_dwmsEventTime);
            //CLogFile::WriteLog(strLog);
            break;
        }

        CString strLog;
        strLog.Format(_T("[1] front(%d) rear(%d)  event(0x%x) hwnd(0x%x) idObject(0x%x) idChild(0x%x) dwEventThread(%d) dwEventTime(%d)\n"), g_taskpool->m_front, g_taskpool->m_rear, eventinfo.m_event, eventinfo.m_hwnd, eventinfo.m_idObject, eventinfo.m_idChild, eventinfo.m_dwEventThread, eventinfo.m_dwmsEventTime);
        //CLogFile::WriteLog(strLog);

        g_taskpool->AddTask(eventinfo);

    } while (FALSE);

    CLogFile::WriteLog(_T("leave"));
}

DWORD WINAPI Thread1(LPVOID pParam)
{
    CoInitialize(NULL);
    g_hook = SetWinEventHook(EVENT_MIN, EVENT_MAX, NULL, HandleWindEvent, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    
    MSG msg;
    // 消息循环: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    //MessageBox((HWND)pParam, _T("线程1已结束"), NULL, 0);
    return 0;
}

DWORD WINAPI Thread2(LPVOID pParam)
{
    EVENTINFO eventinfo;
    static int i = 0;

    while (g_Continue)
    {
        g_taskpool->GetTask(&eventinfo);

        do
        {
            //先判断是窗口是否有效
            if (eventinfo.IsInValid())
            {
                /*CString strLog;
                strLog.Format(_T("[%d] InValid event(0x%x) hwnd(0x%x) idObject(0x%x) idChild(0x%x) dwEventThread(%d) dwEventTime(%d)\n"), i, eventinfo.m_event, eventinfo.m_hwnd, eventinfo.m_idObject, eventinfo.m_idChild, eventinfo.m_dwEventThread, eventinfo.m_dwmsEventTime);
                CLogFile::WriteLog(strLog);*/
                break;
            }

            //有效的情况下在延时截图
            Sleep(50);

            CBitmapMemDC mapDc;
            int errcode = 0;
            if ((errcode = mapDc.CreateBitmapFromHWND(eventinfo.m_hwnd, TRUE)) < 0) {
                //CString strLog;
                //strLog.Format(_T("[%d] %d event(0x%x) hwnd(0x%x) idObject(0x%x) idChild(0x%x) dwEventThread(%d) dwEventTime(%d)\n"), i, errcode, eventinfo.m_event, eventinfo.m_hwnd, eventinfo.m_idObject, eventinfo.m_idChild, eventinfo.m_dwEventThread, eventinfo.m_dwmsEventTime);
                //CLogFile::WriteLog(strLog);
                break;
            }

            //CString strLog;
            //strLog.Format(_T("[%d]    event(0x%x) hwnd(0x%x) idObject(0x%x) idChild(0x%x) dwEventThread(%d) dwEventTime(%d)\n"), i, eventinfo.m_event, eventinfo.m_hwnd, eventinfo.m_idObject, eventinfo.m_idChild, eventinfo.m_dwEventThread, eventinfo.m_dwmsEventTime);
            //CLogFile::WriteLog(strLog);

            //CString filename;
            //filename.Format(_T("../Output/Test/BMP/%d.bmp"), i);
            //mapDc.SaveBmp(mapDc.m_hBitmap, filename);

            i++;
        } while (FALSE);
    }

    //MessageBox((HWND)pParam, _T("线程2已结束"), NULL, 0);
    return 0;
}
void InitializeMSAA(HWND hwnd)
{
    if (g_hook == NULL)
    {
        g_Continue = TRUE;
        g_taskpool = new CTaskPool<EVENTINFO>(10000);
        if (g_taskpool == NULL)
            return;
        hTheds[0] = CreateThread(NULL, 0, Thread1, NULL, 0, &(threadid[0]));
        hTheds[1] = CreateThread(NULL, 0, Thread2, NULL, 0, &(threadid[1]));
    }
}

void ShutdownMSAA()
{
    if (g_hook != NULL)
    {
        g_Continue = FALSE;
        WaitForMultipleObjects(2, hTheds, TRUE, INFINITE);
        CloseHandle(hTheds[0]);
        CloseHandle(hTheds[1]);

        if (g_taskpool)
        {   
            delete(g_taskpool);
            g_taskpool = NULL;
        }

        UnhookWinEvent(g_hook);
        CoUninitialize();
    }
}
