
// ScreenShotDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ScreenShot.h"
#include "ScreenShotDlg.h"
#include "afxdialogex.h"
#include "BitmapMemDC.h"
#include "WindowMonitor.h"

#include <WinUser.h>
#include <iostream>
#include <vector>

using namespace std;

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
    ON_BN_CLICKED(IDOK, &CScreenShotDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CScreenShotDlg::OnBnClickedCancel)
    ON_BN_CLICKED(IDC_BUTTON2, &CScreenShotDlg::OnBnClickedButton2)
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

vector<HWND> hwnd_list;

BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam)
{
    cout << "[Child window] window handle :" << hwnd << endl;
    return TRUE;
}


BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{

    /*
    * Remarks
    The EnumWindows function does not enumerate child windows,
    with the exception of a few top-level windows owned by the
    system that have the WS_CHILD style.
    */
    cout << "[Parent window] window handle: " << hwnd << endl;
    CBitmapMemDC *mapDc = new CBitmapMemDC();

    if (!mapDc->CreateBitmapFromHWND(hwnd, TRUE)) {
        exit(-1);
    }

    delete(mapDc);

    //hwnd_list.push_back(hwnd);
    //EnumChildWindows(hwnd, EnumChildProc, lParam);

    return TRUE;
}

void CScreenShotDlg::OnBnClickedOk()
{
    InitializeMSAA();
}


void CScreenShotDlg::OnBnClickedCancel()
{
    ShutdownMSAA();
    CDialogEx::OnCancel();
}


void CScreenShotDlg::OnBnClickedButton2()
{
    //HWND hwnd = ::FindWindow(NULL, _T("��ҵ΢��"));
    //HWND hwnd = ::FindWindow(_T("WeChatMainWndForPC"), _T("΢��"));
    //HWND hwnd = (HWND)(0x00380A8E); //Foxmail

    CBitmapMemDC *mapDc = new CBitmapMemDC();
    HWND hwnd = (HWND)(0x00560356);

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

    mapDc->SaveBmp(mapDc->m_hBitmap, _T("΢��.bmp"));
    delete(mapDc);
}
