#include "stdafx.h"
#include "WindowMonitor.h"
#include "BitmapMemDC.h"
#include "ScreenShot.h"
#include "WndInfo.h"
#include <vector>

//全局变量。
HWINEVENTHOOK g_hook;

void CALLBACK HandleWindEvent(HWINEVENTHOOK hook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
    if (hwnd == 0)
        return;

    static int i = 0;

    do
    {
        CBitmapMemDC mapDc;
        if (!mapDc.CreateBitmapFromHWND(hwnd, TRUE)) {
            break;
        }
        i = i + 1;

    } while (FALSE);
}

void InitializeMSAA()
{
    if (g_hook == NULL)
    {
        CoInitialize(NULL);
        g_hook = SetWinEventHook(EVENT_MIN, EVENT_MAX, NULL, HandleWindEvent, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    }
}

void ShutdownMSAA()
{
    if (g_hook != NULL)
    {
        UnhookWinEvent(g_hook);
        CoUninitialize();
    }
}
