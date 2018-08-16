// BitmapMemDC.h: interface for the CBitmapMemDC class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <atlstr.h>

#pragma comment( lib, "Msimg32.lib" )

#pragma once
class CBitmapMemDC  
{
public:	
	CBitmapMemDC();
	virtual ~CBitmapMemDC();

	//定义拷贝构造和赋值构造
	CBitmapMemDC(const CBitmapMemDC& other)
	{
		if (this != &other && other.IsDCCreated())
		{
			CreateBitmapFromDC(0, 0, other.GetWidth(), other.GetHeight(), other);
		}
	}
	const CBitmapMemDC& operator =(const CBitmapMemDC& other)
	{
		if (this != &other && other.IsDCCreated())
		{
			_Clear();
			CreateBitmapFromDC(0, 0, other.GetWidth(), other.GetHeight(), other);
		}
		
		return *this;
	}

    enum ErrCode
    {
        ERR_BITBLT = -5,
        ERR_CREATEBITMAP = -4,
        ERR_CREATEDC = -3,
        ERR_WINSIZE = -2,
        ERR_GETDC = -1,
        ERR_SUCCESS = 0,
    };

	/**
	*	@brief 从指定DC的指定矩形区域创建一个位图
	*	@param	[in] (x, y) 左上的坐标，如果hDC不是桌面DC，那么始终为0;如果是桌面DC，那么采用的是需要截取窗口句柄的坐标(CRect rc; ::GetWindowRect(hwnd, &rc);) (rc.left, rc.top)
	*	@param	[in] cx	宽度，-1时使用整个虚拟屏幕的宽度，如果不为-1，那么需要将这个大小与屏幕大小进行对比，因为窗口可能有左右拉动条，导致截图为黑色
	*	@param	[in] cy 高度，-1时使用整个虚拟屏幕的高度，如果不为-1，那么需要将这个大小与屏幕大小进行对比
	*	@param	[in] hDC 如果为空，使用桌面DC
	*/
	BOOL CreateBitmapFromDC(int x, int y, int cx, int cy, HDC hDC, BOOL bBitblt = TRUE);


    /**
    *	@brief 从指定窗口hwnd的创建一个位图，如果创建失败就返回ErrCode
    *	@param	[in] hwnd为空时，使用的是桌面窗口
    */
    int CreateBitmapFromHWND(HWND hwnd, BOOL bBitblt = TRUE);


    /**
    *	@brief 从指定窗口hwnd的创建一个上下文无关位图
    *	@param	[in] hwnd为空时，使用的是桌面窗口
    */
    BOOL CreateDIBBitmapFromHWND(HWND hwnd, BOOL bBitblt = TRUE);

	/**
	*	@brief 给位图叠加颜色
	*	@param	[in] clr 要叠加的颜色
	*	@param	[in] byTransparent 透明度
	*/
	BOOL AddColorMaskToBitmap(COLORREF clr, BYTE byTransparent);
	

    /**
    *    @brief 保存图片到当前文件目录下
    *    @param [in] hBitmap 需要保存的位图
    *    @param [in] fileName 保存的图片名称
    */
    BOOL SaveBmp(HBITMAP hBitmap, CString fileName);

	BOOL IsDCCreated() const;	///<由于要被赋值构造中的常量引用调用，因此也定义成常量		
	operator HDC() const{return m_hMemDC;}
	operator HBITMAP() const{return m_hBitmap;}
	int GetWidth() const{return m_cx;}
	int GetHeight() const{return m_cy;}

protected:
	void _Clear();
	

//protected:
public:
    HDC		m_hMemDC;
	HBITMAP	m_hBitmap;
	HBITMAP	m_hOldBitmap;
	int m_cx;
	int m_cy;
};
