
// ScreenShotDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ScreenShot.h"
#include "ScreenShotDlg.h"
#include "afxdialogex.h"
#include "WindowMonitor.h"
#include "BitmapMemDC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CScreenShotDlg �Ի���



CScreenShotDlg::CScreenShotDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SCREENSHOT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CScreenShotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CScreenShotDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(Monitor, &CScreenShotDlg::OnBnClickedMonitor)
    ON_BN_CLICKED(ScreenShot, &CScreenShotDlg::OnBnClickedScreenshot)
    ON_BN_CLICKED(StopMonitor, &CScreenShotDlg::OnBnClickedStopmonitor)
END_MESSAGE_MAP()


// CScreenShotDlg ��Ϣ�������

BOOL CScreenShotDlg::OnInitDialog()
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

void CScreenShotDlg::OnPaint()
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
HCURSOR CScreenShotDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CScreenShotDlg::OnBnClickedMonitor()
{
    InitializeMSAA();
}


void CScreenShotDlg::OnBnClickedScreenshot()
{
    CBitmapMemDC *mapDc = new CBitmapMemDC();
    HWND hwnd = (HWND)(0x000205D2);

    CRect rc;
    ::GetWindowRect(hwnd, &rc);

    CString str;
    str.Format(_T("%d %d"), rc.Width(), rc.Height());
    MessageBox(str);

    if (!mapDc->CreateDIBBitmapFromHWND(hwnd, TRUE)) {
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


void CScreenShotDlg::OnBnClickedStopmonitor()
{
    ShutdownMSAA();
    CDialog::OnCancel();
}
