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

	//���忽������͸�ֵ����
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
	*	@brief ��ָ��DC��ָ���������򴴽�һ��λͼ
	*	@param	[in] (x, y) ���ϵ����꣬���hDC��������DC����ôʼ��Ϊ0;���������DC����ô���õ�����Ҫ��ȡ���ھ��������(CRect rc; ::GetWindowRect(hwnd, &rc);) (rc.left, rc.top)
	*	@param	[in] cx	��ȣ�-1ʱʹ������������Ļ�Ŀ�ȣ������Ϊ-1����ô��Ҫ�������С����Ļ��С���жԱȣ���Ϊ���ڿ��������������������½�ͼΪ��ɫ
	*	@param	[in] cy �߶ȣ�-1ʱʹ������������Ļ�ĸ߶ȣ������Ϊ-1����ô��Ҫ�������С����Ļ��С���жԱ�
	*	@param	[in] hDC ���Ϊ�գ�ʹ������DC
	*/
	BOOL CreateBitmapFromDC(int x, int y, int cx, int cy, HDC hDC, BOOL bBitblt = TRUE);


    /**
    *	@brief ��ָ������hwnd�Ĵ���һ��λͼ���������ʧ�ܾͷ���ErrCode
    *	@param	[in] hwndΪ��ʱ��ʹ�õ������洰��
    */
    int CreateBitmapFromHWND(HWND hwnd, BOOL bBitblt = TRUE);


    /**
    *	@brief ��ָ������hwnd�Ĵ���һ���������޹�λͼ
    *	@param	[in] hwndΪ��ʱ��ʹ�õ������洰��
    */
    BOOL CreateDIBBitmapFromHWND(HWND hwnd, BOOL bBitblt = TRUE);

	/**
	*	@brief ��λͼ������ɫ
	*	@param	[in] clr Ҫ���ӵ���ɫ
	*	@param	[in] byTransparent ͸����
	*/
	BOOL AddColorMaskToBitmap(COLORREF clr, BYTE byTransparent);
	

    /**
    *    @brief ����ͼƬ����ǰ�ļ�Ŀ¼��
    *    @param [in] hBitmap ��Ҫ�����λͼ
    *    @param [in] fileName �����ͼƬ����
    */
    BOOL SaveBmp(HBITMAP hBitmap, CString fileName);

	BOOL IsDCCreated() const;	///<����Ҫ����ֵ�����еĳ������õ��ã����Ҳ����ɳ���		
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
