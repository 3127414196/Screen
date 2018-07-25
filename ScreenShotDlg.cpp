
// ScreenShotDlg.cpp : 实现文件
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


// CScreenShotDlg 对话框



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


// CScreenShotDlg 消息处理程序

BOOL CScreenShotDlg::OnInitDialog()
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

void CScreenShotDlg::OnPaint()
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
    //HWND hwnd = ::FindWindow(NULL, _T("企业微信"));
    //HWND hwnd = ::FindWindow(_T("WeChatMainWndForPC"), _T("微信"));
    //HWND hwnd = (HWND)(0x00380A8E); //Foxmail

    CBitmapMemDC *mapDc = new CBitmapMemDC();
    HWND hwnd = (HWND)(0x00560356);

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

    mapDc->SaveBmp(mapDc->m_hBitmap, _T("微信.bmp"));
    delete(mapDc);
}
