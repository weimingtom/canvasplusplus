#include "StdAfx.h"
#include "CanvasRenderingContext2D.h"
#include <Windows.h>

//THIS FILE IS WINDOWS ONLY AND GDI ONLY
//GDI+ at the same file?

//TODO Direct2d


static void FillSolidRect(HDC hDC, LPCRECT lpRect, COLORREF clr)
{
    COLORREF clrOld = ::SetBkColor(hDC, clr);

    if (clrOld != CLR_INVALID)
    {
        ::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL);
        ::SetBkColor(hDC, clrOld);
    }
}


CanvasPlus::Canvas::Canvas(void* p) : m_CanvasRenderingContext2D(p)
{
}

CanvasPlus::Canvas::~Canvas()
{
}

CanvasPlus::CanvasRenderingContext2D& CanvasPlus::Canvas::getContext(const char*)
{
    return m_CanvasRenderingContext2D;
}

CanvasPlus::CanvasRenderingContext2D::CanvasRenderingContext2D(void* p)
    : m_pNativeHandle(p)
{
}

CanvasPlus::CanvasRenderingContext2D::~CanvasRenderingContext2D()
{
}

///////////////////////////////////////////////////////////////////////////////
// The fillRect(x, y, w, h) method must paint the specified rectangular area 
//  using the fillStyle. If either height or width are zero, this method 
//  has no effect.
void CanvasPlus::CanvasRenderingContext2D::fillRect(double x, double y, double w, double h)
{
    //------------------------------    
    HDC hdc = (HDC) m_pNativeHandle;
    //-------------------------------
    
    //TODO waiting for transformations..
    
    RECT rect = {(LONG)x, (LONG)y, (LONG)(x + w), (LONG)(y + h)};
    COLORREF color = RGB(fillStyle.m_Color.r, fillStyle.m_Color.g, fillStyle.m_Color.b);
    FillSolidRect(hdc, &rect, color);
}


