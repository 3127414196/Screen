
// MutiThreadScreenShotDlg.h : ͷ�ļ�
//

#pragma once

// CMutiThreadScreenShotDlg �Ի���
class CMutiThreadScreenShotDlg : public CDialogEx
{
// ����
public:
	CMutiThreadScreenShotDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MUTITHREADSCREENSHOT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
    void OnBnClickedMonitor();
    void OnBnClickedScreenshot();
    void OnBnClickedStopmonitor();
	DECLARE_MESSAGE_MAP()

public:
    BOOL m_IsMonitor;
    //�����������߳�
    HANDLE m_hTheds[2];
    DWORD m_dwthreadid[2];
};
