
// MutiThreadScreenShotDlg.cpp : ʵ���ļ�
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


// CMutiThreadScreenShotDlg �Ի���



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


// CMutiThreadScreenShotDlg ��Ϣ�������

BOOL CMutiThreadScreenShotDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMutiThreadScreenShotDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMutiThreadScreenShotDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

DWORD WINAPI EventHook(LPVOID pParam)
{
    CWinEventHook& eventhook = CWinEventHook::Instance();
    WinEventHookCallback callbackproc;

    eventhook.Hook(&callbackproc);

    //��Ҫһ����Ϣѭ�������յ�WM_QUIT�˳�
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
        
        //������taskpoolȡ����������event
        {
            CAutoCriticalSection lock(eventpool.m_cs);
            if (!eventpool.m_taskpool->GetTask(&eventinfo))
                continue;   //�������Ϊ�գ������н�ͼ
        }

        do
        {
            //���ж��Ǵ����Ƿ���Ч
            if (eventinfo.IsInValid())
            {
                CString strLog;
                strLog.Format(_T("[InValid] event(0x%x) hwnd(0x%x) idObject(0x%x) idChild(0x%x) dwEventThread(%d) dwEventTime(%d)\n"), eventinfo.m_event, eventinfo.m_hwnd, eventinfo.m_idObject, eventinfo.m_idChild, eventinfo.m_dwEventThread, eventinfo.m_dwmsEventTime);
                CLogFile::WriteLog(strLog);
                break;
            }

            //��Ч�����������ʱ��ͼ
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
        MessageBox(_T("��ͼʧ��"));
        CDialogEx::OnCancel();
        return;
    }

    MessageBox(_T("ok"));
    OpenClipboard(); 	//�򿪼����壬����λͼ������������ 
    EmptyClipboard();
    SetClipboardData(CF_BITMAP, mapDc->m_hBitmap);
    CloseClipboard(); 	//�رռ�����  

    mapDc->SaveBmp(mapDc->m_hBitmap, _T("../Output/Test/BMP/΢��.bmp"));
    delete(mapDc);
}


void CMutiThreadScreenShotDlg::OnBnClickedStopmonitor()
{
    if (m_IsMonitor)
    {
        CWinEventHook& eventhook = CWinEventHook::Instance();
        eventhook.UnHook();

        //����WM_QUIT����hook�߳�
        if (!PostThreadMessage(m_dwthreadid[0], WM_QUIT, 0, 0))
        {
            GetLastError();
        }
        //ͨ��ȫ�ֱ�������event�����߳�
        EventPool &eventpool = CSingletonEventHandle<EventPool>::getInstance();
        {
            CAutoCriticalSection lock(eventpool.m_cs);
            eventpool.m_Continue = FALSE;
        }

        //�ȴ������߳̽���
        WaitForMultipleObjects(2, m_hTheds, TRUE, INFINITE);
        CloseHandle(m_hTheds[0]);
        CloseHandle(m_hTheds[1]);

        m_IsMonitor = FALSE;
    }

    CDialog::OnCancel();
}
