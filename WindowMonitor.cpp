#include "stdafx.h"
#include "WindowMonitor.h"
#include "BitmapMemDC.h"
#include "ScreenShot.h"
#include <stdio.h>
#include <time.h>

//全局变量。
HWINEVENTHOOK g_hook;

void CALLBACK HandleWindEvent(HWINEVENTHOOK hook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
    //IAccessible * pAcc = NULL;
    //VARIANT varChild;
    //HRESULT hr = AccessibleObjectFromEvent(hwnd, idObject, idChild, &pAcc, &varChild);

    static int i = 0;

    //if ((hr == S_OK) && (pAcc != NULL))
    {
       // if (event == EVENT_OBJECT_SHOW)
        {
            char buf[100];
            WCHAR ClassNameW[100];
            if (!GetClassName(hwnd, ClassNameW, 100))
            {
                return;
            }

            char ClassNameA[50];

            wcharTochar(ClassNameW, ClassNameA, sizeof(ClassNameA));

            if (theApp.readxml->Isignore(ClassNameA))
            {
                sprintf(buf, "%08x %08x %s ignore", event, hwnd, ClassNameA);
                LogDebug(buf, sizeof(buf));

            }

            CBitmapMemDC *mapDc = new CBitmapMemDC();

            clock_t t1 = clock();
            if (!mapDc->CreateBitmapFromHWND(hwnd, TRUE)) {
                return;
            }

            int time = (int)((clock() - t1) * 1.0 / CLOCKS_PER_SEC * 1000);

            CString filename;
            filename.Format(_T("./测试/temp/%d.bmp"), i);

            mapDc->SaveBmp(mapDc->m_hBitmap, filename);

            sprintf(buf, "%2d %2d %08x %08x %s", i, time, event, hwnd, ClassNameA);
            LogDebug(buf, sizeof(buf));

            delete(mapDc);
            i++;
        }

        //pAcc->Release();
    }
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
