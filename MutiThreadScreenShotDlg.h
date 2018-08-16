
// MutiThreadScreenShotDlg.h : 头文件
//

#pragma once

// CMutiThreadScreenShotDlg 对话框
class CMutiThreadScreenShotDlg : public CDialogEx
{
// 构造
public:
	CMutiThreadScreenShotDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MUTITHREADSCREENSHOT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
    void OnBnClickedMonitor();
    void OnBnClickedScreenshot();
    void OnBnClickedStopmonitor();
	DECLARE_MESSAGE_MAP()

public:
    BOOL m_IsMonitor;
    //创建的两个线程
    HANDLE m_hTheds[2];
    DWORD m_dwthreadid[2];
};
