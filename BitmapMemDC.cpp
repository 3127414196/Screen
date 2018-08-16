// BitmapMemDC.cpp: implementation of the CBitmapMemDC class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "BitmapMemDC.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBitmapMemDC::CBitmapMemDC()
{
	m_hMemDC = NULL;
	m_hBitmap = NULL;
	m_hOldBitmap = NULL;
	m_cx = 0;
	m_cy = 0;
}

CBitmapMemDC::~CBitmapMemDC()
{
	_Clear();
}

void CBitmapMemDC::_Clear()
{
	if (NULL != m_hMemDC)
	{
		if (NULL != m_hOldBitmap)
		{
			SelectObject(m_hMemDC, m_hOldBitmap);
			m_hOldBitmap = NULL;
		}
        // -1 GDI Object
        if (!DeleteDC(m_hMemDC))
        {
            int error = GetLastError();
            assert(error == 0);
        }
		m_hMemDC = NULL;
	}
	if (NULL != m_hBitmap)
	{
        // -1 GDI Object
		DeleteObject(m_hBitmap);
		m_hBitmap = NULL;
	}	
}

BOOL CBitmapMemDC::IsDCCreated() const
{
	if (NULL != m_hMemDC && NULL != m_hBitmap)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CBitmapMemDC::CreateBitmapFromDC(int x, int y, int cx, int cy, HDC hDC, BOOL bBitblt/* = TRUE*/)
{
	_Clear();

	m_cx = (-1 == cx) ? GetSystemMetrics(SM_CXVIRTUALSCREEN) : cx;
	m_cy = (-1 == cy) ? GetSystemMetrics(SM_CYVIRTUALSCREEN) : cy;

    HDC hDCtmp = hDC;
	BOOL bGetDC = FALSE;
	if (NULL == hDCtmp)
	{
		hDCtmp = GetDC(::GetDesktopWindow());
		bGetDC = TRUE;
	}
	if (NULL == hDCtmp)
	{
		return FALSE;		
	}

	BOOL bOK = FALSE;	
	do 
	{
		m_hMemDC = CreateCompatibleDC(hDCtmp);
		if (NULL == m_hMemDC)
		{
			break;
		}
		m_hBitmap = CreateCompatibleBitmap(hDCtmp, m_cx, m_cy);
		if (NULL == m_hBitmap)
		{
			break;
		}
		m_hOldBitmap = (HBITMAP)SelectObject(m_hMemDC, m_hBitmap);
		
		if (bBitblt && !BitBlt(m_hMemDC, 0, 0, m_cx, m_cy, hDCtmp, x, y, SRCCOPY | CAPTUREBLT))
		{
			break;
		}

		bOK = TRUE;
	} while (FALSE);
	
	if (bGetDC)
	{
		ReleaseDC(::GetDesktopWindow(), hDCtmp);
	}

	if (!bOK)
	{
		_Clear();
	}

	return bOK;
}

BOOL CBitmapMemDC::AddColorMaskToBitmap(COLORREF clr, BYTE byTransparent)
{
	if (!IsDCCreated())
	{
		return FALSE;
	}

	BOOL bOK = FALSE;
	HDC hMemDC = NULL;
	HBITMAP hBitmap = NULL;
	
	do 
	{
		hMemDC = CreateCompatibleDC(m_hMemDC);
		if (NULL == hMemDC)
		{
			break;
		}
		hBitmap = CreateCompatibleBitmap(m_hMemDC, m_cx, m_cy);
		if (NULL == hBitmap)
		{
			break;
		}
		HBITMAP hBitmapOld = (HBITMAP)SelectObject(hMemDC, hBitmap);
		HBRUSH hbrush = CreateSolidBrush(clr);		
		RECT rc = {0};
		rc.right = m_cx;
		rc.bottom = m_cy;
		if (hbrush)
		{
			FillRect(hMemDC, &rc, hbrush);
			DeleteObject(hbrush);
		}
		
		BLENDFUNCTION   blendFunction;   
		blendFunction.BlendFlags=0;   
		blendFunction.BlendOp=AC_SRC_OVER;   
		blendFunction.SourceConstantAlpha = byTransparent;   
		blendFunction.AlphaFormat=0;			  			
		AlphaBlend(m_hMemDC, 0, 0, m_cx, m_cy, hMemDC, 0, 0, m_cx, m_cy, blendFunction);
		SelectObject(hMemDC, hBitmapOld);		
		
		bOK = TRUE;
	} while (FALSE);

	if (NULL != hBitmap)
	{
		DeleteObject(hBitmap);
		hBitmap = NULL;
	}
	if (NULL != hMemDC)
	{
		DeleteDC(hMemDC);
		hMemDC = NULL;
	}

	return bOK;
}

int CBitmapMemDC::CreateBitmapFromHWND(HWND hwnd, BOOL bBitblt)
{
    _Clear();

    HDC hDCtmp = NULL;
    BOOL bGetDC = FALSE;

    //��Ļ�Ŀ�͸�
    int Screen_X_length = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int Screen_Y_length = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    if (hwnd == NULL)
    {
        m_cx = Screen_X_length;
        m_cy = Screen_Y_length;
        hDCtmp = GetDC(::GetDesktopWindow());
        bGetDC = TRUE;
    }
    else
    {
        // +1 GDI Object
        //����Ҫ�ѳ����ĸ����򻺳����Ĳ��ֶ�Ҫ��һ�飬����
        hDCtmp = ::GetWindowDC((HWND)(hwnd));
        if (hDCtmp != NULL) {
            CRect rc;
            ::GetWindowRect(hwnd, &rc);
            m_cx = ((rc.left + rc.Width()) > Screen_X_length) ? (Screen_X_length - rc.left) : rc.Width();
            m_cy = ((rc.top + rc.Height()) > Screen_Y_length) ? (Screen_Y_length - rc.top) : rc.Height();
            
            bGetDC = TRUE;
        }
    }

    //ע�� ԭ��������Դй¶�Ĳ���,���������ֱ�ӷ����ˣ��ͻ�hDCtmp���ڴ�û�б��ͷ�
    /*if ((NULL == hDCtmp) || (m_cx == 0) || (m_cy == 0))
    {
        return FALSE;
    }*/

    if ((NULL == hDCtmp))
    {
        return ERR_GETDC;
    }

    int bOK = ERR_SUCCESS;
    do
    {
        if (m_cx <= 0 || m_cy <= 0)
        {
            bOK = ERR_WINSIZE;
            break;
        }

        // +1 GDI Object
        m_hMemDC = CreateCompatibleDC(hDCtmp);
        if (NULL == m_hMemDC)
        {
            bOK = ERR_CREATEDC;
            break;
        }
        // +1 GDI Object
        m_hBitmap = CreateCompatibleBitmap(hDCtmp, m_cx, m_cy);
        if (NULL == m_hBitmap)
        {
            bOK = ERR_CREATEBITMAP;
            break;
        }
        m_hOldBitmap = (HBITMAP)SelectObject(m_hMemDC, m_hBitmap);

        if (bBitblt && !BitBlt(m_hMemDC, 0, 0, m_cx, m_cy, hDCtmp, 0, 0, SRCCOPY | CAPTUREBLT))
        {
            bOK = ERR_BITBLT;
            break;
        }
    } while (FALSE);

    if (bGetDC)
    {
        if (hwnd == NULL)
        {
            ReleaseDC(::GetDesktopWindow(), hDCtmp);
        }
        else
        {
            ReleaseDC(hwnd, hDCtmp);    //-1 GDI Object
        }
    }

    if (bOK != ERR_SUCCESS)
    {
        _Clear();
    }

    return bOK;
}

BOOL CBitmapMemDC::CreateDIBBitmapFromHWND(HWND hwnd, BOOL bBitblt)
{
    _Clear();

    HDC hDCtmp = NULL;
    BOOL bGetDC = FALSE;

    //��Ļ�Ŀ�͸�
    int Screen_X_length = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int Screen_Y_length = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    if (hwnd == NULL)
    {
        m_cx = Screen_X_length;
        m_cy = Screen_Y_length;
        hDCtmp = GetDC(::GetDesktopWindow());
        bGetDC = TRUE;
    }
    else
    {
        // +1��GDI OBJ
        hDCtmp = ::GetWindowDC((HWND)(hwnd));
        if (hDCtmp != NULL) {
            CRect rc;
            ::GetWindowRect(hwnd, &rc);
            m_cx = ((rc.left + rc.Width()) > Screen_X_length) ? (Screen_X_length - rc.left) : rc.Width();
            m_cy = ((rc.top + rc.Height()) > Screen_Y_length) ? (Screen_Y_length - rc.top) : rc.Height();

            bGetDC = TRUE;
        }
    }

    //ע�� ԭ��������Դй¶�Ĳ���,���������ֱ�ӷ����ˣ��ͻ�hDCtmp���ڴ�û�б��ͷ�
    //if ((NULL == hDCtmp)|| (m_cx == 0) || (m_cy == 0))
    //{
    //    return FALSE;
    //}
    
    if (NULL == hDCtmp)
    {
        return FALSE;
    }

    BOOL bOK = FALSE;
    do
    {
        if (m_cx <= 0 || m_cy <= 0)
            break;

        // +1��GDI Object
        m_hMemDC = CreateCompatibleDC(hDCtmp);
        if (NULL == m_hMemDC)
        {
            break;
        }

        BITMAPINFOHEADER bmih;
        memset(&bmih, 0, sizeof(BITMAPINFOHEADER));
        bmih.biSize = sizeof(BITMAPINFOHEADER);
        bmih.biBitCount = 24;
        bmih.biCompression = BI_RGB;
        bmih.biPlanes = 1;
        bmih.biWidth = m_cx;
        bmih.biHeight = m_cy;

        BITMAPINFO bmi;
        memset(&bmi, 0, sizeof(BITMAPINFO));
        bmi.bmiHeader = bmih;

        void* p;    //ָ��λͼ�е����ݣ����Զ����䣬����Ҳ���Լ��ͷ�

        // +1 ��GDI Object
        m_hBitmap = ::CreateDIBSection(m_hMemDC, &bmi, DIB_RGB_COLORS, &p, NULL, 0);
        if (NULL == m_hBitmap)
        {
            break;
        }
        m_hOldBitmap = (HBITMAP)SelectObject(m_hMemDC, m_hBitmap);

        while (TRUE)
        {
            if (bBitblt && !BitBlt(m_hMemDC, 0, 0, m_cx, m_cy, hDCtmp, 0, 0, SRCCOPY | CAPTUREBLT))
            {
                break;
            }
        }
        //if (bBitblt && !BitBlt(m_hMemDC, 0, 0, m_cx, m_cy, hDCtmp, 0, 0, SRCCOPY | CAPTUREBLT))
        //{
        //    break;
        //}

        bOK = TRUE;
    } while (FALSE);

    if (bGetDC)
    {
        if (hwnd == NULL)
            bOK &= ReleaseDC(::GetDesktopWindow(), hDCtmp);
        else
            bOK &= ReleaseDC(hwnd, hDCtmp);     //-1 GDI Object
        hDCtmp = NULL;
    }

    if (!bOK)
    {
        _Clear();
    }

    return bOK;
}

BOOL CBitmapMemDC::SaveBmp(HBITMAP hBitmap, CString fileName)
{
    HDC hDC;						// �豸������

    int iBits;						// ��ǰ��ʾ�ֱ�����ÿ��������ռ�ֽ���
    WORD wBitCount;					// λͼ��ÿ��������ռ�ֽ���
    DWORD dwPaletteSize = 0, dwBmBitsSize, dwDIBSize, dwWritten;	// ��ɫ���С��λͼ���ݴ�С��λͼ�ļ���С��д���ļ��ֽ���
    BITMAP Bitmap;					//λͼ���Խṹ
    BITMAPFILEHEADER bmfHdr;		// λͼ�ļ�ͷ
    BITMAPINFOHEADER bi;			// λͼ��Ϣͷ
    LPBITMAPINFOHEADER lpbi;		// ָ��λͼ��Ϣͷ�ṹ

    HANDLE fh, hDib;				// �����ļ��������ڴ���
    HPALETTE hPal, hOldPal = NULL;	// ��ɫ����

                                    // ����λͼ�ļ�ÿ��������ռ�ֽ���
    hDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
    iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
    DeleteDC(hDC);
    if (iBits <= 1)
        wBitCount = 1;
    else if (iBits <= 4)
        wBitCount = 4;
    else if (iBits <= 8)
        wBitCount = 8;
    else if (iBits <= 24)
        wBitCount = 24;
    else
        wBitCount = 32;
    if (wBitCount <= 8)
        dwPaletteSize = (1 << wBitCount) * sizeof(RGBQUAD);		// �����ɫ���С

                                                                // ����λͼ��Ϣͷ�ṹ
    GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = Bitmap.bmWidth;
    bi.biHeight = Bitmap.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = wBitCount;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;
    dwBmBitsSize = ((Bitmap.bmWidth * wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;

    hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));	// Ϊλͼ���ݷ����ڴ�
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
    *lpbi = bi;
    // �����ɫ��
    hPal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);
    if (hPal)
    {
        hDC = GetDC(NULL);
        hOldPal = SelectPalette(hDC, hPal, FALSE);
        RealizePalette(hDC);
    }
    // ��ȡ�õ�ɫ�����µ�����ֵ
    GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize, (BITMAPINFO*)lpbi, DIB_RGB_COLORS);

    if (hOldPal)				// �ָ���ɫ��
    {
        SelectPalette(hDC, hOldPal, TRUE);
        RealizePalette(hDC);
        ReleaseDC(NULL, hDC);
    }
    // ����λͼ�ļ� 
    fh = CreateFile(fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (fh == INVALID_HANDLE_VALUE)
        return FALSE;

    // ����λͼ�ļ�ͷ
    bmfHdr.bfType = 0x4D42;		// �ļ�����: "BM"
    dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
    bmfHdr.bfSize = dwDIBSize;	// λͼ�ļ���С
    bmfHdr.bfReserved1 = 0;
    bmfHdr.bfReserved2 = 0;
    bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;

    WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);	// д��λͼ�ļ�ͷ
    WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);					// д��λͼ�ļ���������

    GlobalUnlock(hDib);
    GlobalFree(hDib);
    CloseHandle(fh);

    return TRUE;
}