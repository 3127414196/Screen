
// MutiThreadScreenShotDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MutiThreadScreenShot.h"
#include "MutiThreadScreenShotDlg.h"
#include "afxdialogex.h"
#include "BitmapMemDC.h"
#include "WinEventHook.h"
#include "SingletonEventHandle.h"
#include "AutoCriticalSection.h"
#include "TaskPool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMutiThreadScreenShotDlg 对话框



CMutiThreadScreenShotDlg::CMutiThreadScreenShotDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SCREENSHOT_DIALOG, pParent)
    , m_IsMonitor(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMutiThreadScreenShotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMutiThreadScreenShotDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(Monitor, &CMutiThreadScreenShotDlg::OnBnClickedMonitor)
    ON_BN_CLICKED(ScreenShot, &CMutiThreadScreenShotDlg::OnBnClickedScreenshot)
    ON_BN_CLICKED(StopMonitor, &CMutiThreadScreenShotDlg::OnBnClickedStopmonitor)
END_MESSAGE_MAP()


// CMutiThreadScreenShotDlg 消息处理程序

BOOL CMutiThreadScreenShotDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMutiThreadScreenShotDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMutiThreadScreenShotDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

DWORD WINAPI EventHook(LPVOID pParam)
{
    CWinEventHook& eventhook = CWinEventHook::Instance();
    WinEventHookCallback callbackproc;

    eventhook.Hook(&callbackproc);

    //需要一个消息循环，在收到WM_QUIT退出
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

DWORD WINAPI EventHandle(LPVOID pParam)
{
    EVENTINFO eventinfo;
    EventPool &eventpool = CSingletonEventHandle<EventPool>::getInstance();
    static int i = 0;

    while (TRUE)
    {
        if (!eventpool.m_Continue)
            break;
        
        //加锁从taskpool取出待操作的event
        {
            CAutoCriticalSection lock(eventpool.m_cs);
            if (!eventpool.m_taskpool->GetTask(&eventinfo))
                continue;   //如果队列为空，不进行截图
        }

        do
        {
            //先判断是窗口是否有效
            if (eventinfo.IsInValid())
            {
                CString strLog;
                strLog.Format(_T("[InValid] event(0x%x) hwnd(0x%x) idObject(0x%x) idChild(0x%x) dwEventThread(%d) dwEventTime(%d)\n"), eventinfo.m_event, eventinfo.m_hwnd, eventinfo.m_idObject, eventinfo.m_idChild, eventinfo.m_dwEventThread, eventinfo.m_dwmsEventTime);
                CLogFile::WriteLog(strLog);
                break;
            }

            //有效的情况下在延时截图
            Sleep(50);

            CBitmapMemDC mapDc;
            int errcode = 0;
            if ((errcode = mapDc.CreateBitmapFromHWND(eventinfo.m_hwnd, TRUE)) < 0) {
                CString strLog;
                strLog.Format(_T("[mapDC] %d event(0x%x) hwnd(0x%x) idObject(0x%x) idChild(0x%x) dwEventThread(%d) dwEventTime(%d)\n"), errcode, eventinfo.m_event, eventinfo.m_hwnd, eventinfo.m_idObject, eventinfo.m_idChild, eventinfo.m_dwEventThread, eventinfo.m_dwmsEventTime);
                CLogFile::WriteLog(strLog);
                break;
            }

            CString strLog;
            strLog.Format(_T("[%d]    event(0x%x) hwnd(0x%x) idObject(0x%x) idChild(0x%x) dwEventThread(%d) dwEventTime(%d)\n"), i, eventinfo.m_event, eventinfo.m_hwnd, eventinfo.m_idObject, eventinfo.m_idChild, eventinfo.m_dwEventThread, eventinfo.m_dwmsEventTime);
            CLogFile::WriteLog(strLog);

            CString filename;
            filename.Format(_T("../Output/Test/BMP/%d.bmp"), i);
            mapDc.SaveBmp(mapDc.m_hBitmap, filename);
            i++;
        } while (FALSE);
    }

    return 0;
}

void CMutiThreadScreenShotDlg::OnBnClickedMonitor()
{
    if (!m_IsMonitor)
    {
        m_hTheds[0] = CreateThread(NULL, 0, EventHook, NULL, 0, &(m_dwthreadid[0]));
        m_hTheds[1] = CreateThread(NULL, 0, EventHandle, NULL, 0, &(m_dwthreadid[1]));
        m_IsMonitor = TRUE;
    }
}


void CMutiThreadScreenShotDlg::OnBnClickedScreenshot()
{
    CBitmapMemDC *mapDc = new CBitmapMemDC();
    HWND hwnd = (HWND)(0x00d30e94);

    CRect rc;
    ::GetWindowRect(hwnd, &rc);

    CString str;
    str.Format(_T("%d %d"), rc.Width(), rc.Height());
    MessageBox(str);

    if (!mapDc->CreateBitmapFromHWND(hwnd, TRUE)) {
        MessageBox(_T("截图失败"));
        CDialogEx::OnCancel();
        return;
    }

    MessageBox(_T("ok"));
    OpenClipboard(); 	//打开剪贴板，并将位图拷到剪贴板上 
    EmptyClipboard();
    SetClipboardData(CF_BITMAP, mapDc->m_hBitmap);
    CloseClipboard(); 	//关闭剪贴板  

    mapDc->SaveBmp(mapDc->m_hBitmap, _T("../Output/Test/BMP/微信.bmp"));
    delete(mapDc);
}


void CMutiThreadScreenShotDlg::OnBnClickedStopmonitor()
{
    if (m_IsMonitor)
    {
        CWinEventHook& eventhook = CWinEventHook::Instance();
        eventhook.UnHook();

        //发送WM_QUIT结束hook线程
        if (!PostThreadMessage(m_dwthreadid[0], WM_QUIT, 0, 0))
        {
            GetLastError();
        }
        //通过全局变量结束event处理线程
        EventPool &eventpool = CSingletonEventHandle<EventPool>::getInstance();
        {
            CAutoCriticalSection lock(eventpool.m_cs);
            eventpool.m_Continue = FALSE;
        }

        //等待两个线程结束
        WaitForMultipleObjects(2, m_hTheds, TRUE, INFINITE);
        CloseHandle(m_hTheds[0]);
        CloseHandle(m_hTheds[1]);

        m_IsMonitor = FALSE;
    }

    CDialog::OnCancel();
}
