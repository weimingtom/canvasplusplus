

#include <Windows.h>
#include <cassert>
#include "..\Canvas.h"


//THIS FILE IS WINDOWS ONLY AND GDI ONLY
//GDI+ at the same file?

//TODO Direct2d


//Gradient
#pragma comment( lib, "msimg32.lib" )
///

class SaveClipRgn
{
    HDC  m_hDC;
    HRGN m_hOldRgn;

public:
    SaveClipRgn()
    {
        m_hDC = NULL;
        m_hOldRgn = NULL;
    }

    ~SaveClipRgn()
    {
        Restore();
        assert(m_hDC == NULL);
        assert(m_hOldRgn == NULL);
    }

    SaveClipRgn(HDC hdc)
    {
        assert(hdc != NULL);
        m_hDC = NULL;
        m_hOldRgn = NULL;
        Save(hdc);
    }

    HRGN GetOldRgn()
    {
        if (m_hDC == NULL)
        {
            assert(FALSE); //lint !e527
            return NULL;
        }

        assert(m_hOldRgn != NULL);
        return m_hOldRgn;
    }

    BOOL Save(HDC hdc)
    {
        assert(hdc != NULL);
        assert(m_hOldRgn == NULL);
        assert(m_hDC == NULL);
        m_hOldRgn = ::CreateRectRgn(0, 0, 0, 0);

        if (m_hOldRgn != NULL)
        {
            switch (GetClipRgn(hdc, m_hOldRgn))
            {
            case 1:
            {
                // success, m_hOldRgn contains the current clipping region for the DC
                m_hDC = hdc;
            }
            break;

            case 0:
            {
                // success, but the DC has no clipping region
                m_hDC = hdc;
                DeleteObject(m_hOldRgn);
                m_hOldRgn = NULL;
            }
            break;

            default:
            case -1:
            {
                // erro
                DeleteObject(m_hOldRgn);
                m_hOldRgn = NULL;
            }
            break;
            }
        }

        return (m_hDC != NULL);
    }

    BOOL Restore()
    {
        if (m_hDC == NULL)
        {
            return FALSE;
        }

        // m_hDC only gets stored if the clipping region was saved successfully,
        // but m_hOldRgn may be NULL to indicate that no clipping region was defined
        ::SelectClipRgn(m_hDC, m_hOldRgn);

        if (m_hOldRgn != NULL)
        {
            // free the HRGN object (if any)
            DeleteObject(m_hOldRgn);
            m_hOldRgn = NULL;
        }

        m_hDC = NULL;
        return TRUE;
    }

    HRGN Detach()
    {
        HRGN hRgn = m_hOldRgn;
        m_hOldRgn = NULL;
        m_hDC = NULL;
        return hRgn;
    }
};

class ClipRect
{
    SaveClipRgn m_old;
    HRGN m_hNewRegionClip;
public:

    ClipRect(HDC hdc, RECT& newClipRect) : m_old(hdc)
    {
        RECT rcClip = newClipRect;
        ::LPtoDP(hdc, (LPPOINT)&rcClip, 2);
        m_hNewRegionClip = CreateRectRgnIndirect(&rcClip);
        ///SelectClipRgn(hdc, m_hNewRegionClip);
        ExtSelectClipRgn(hdc, m_hNewRegionClip, RGN_AND);
    }

    ~ClipRect()
    {
        DeleteObject(m_hNewRegionClip);
        EndClip();
    }

    void EndClip()
    {
        m_old.Restore();
    }
};

inline void DrawBitmap(HDC hdc, HBITMAP hbm, int Left, int Top)
{
    BOOL f;
    HDC hdcBits;
    BITMAP bm;
    hdcBits = CreateCompatibleDC(hdc);
    GetObject(hbm, sizeof(BITMAP), &bm);
    SelectObject(hdcBits, hbm);
    f = BitBlt(hdc, Left, Top, bm.bmWidth, bm.bmHeight, hdcBits, 0, 0, SRCCOPY);
    DeleteDC(hdcBits);
}


//////////////////////////////////////////////////////////////
inline HBITMAP CreateBitmap_32(const SIZE& size, void** pBits)
{
    assert(0 < size.cx);
    assert(0 != size.cy);

    if (pBits != NULL)
    {
        *pBits = NULL;
    }

    if (size.cx <= 0 || size.cy == 0)
    {
        return NULL;
    }

    BITMAPINFO bi = {0};
    // Fill in the BITMAPINFOHEADER
    bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth       = size.cx;
    bi.bmiHeader.biHeight      = size.cy;
    bi.bmiHeader.biSizeImage   = size.cx * abs(size.cy);
    bi.bmiHeader.biPlanes      = 1;
    bi.bmiHeader.biBitCount    = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    LPVOID pData = NULL;
    HBITMAP hbmp = ::CreateDIBSection(NULL, &bi, DIB_RGB_COLORS, &pData, NULL, 0);

    if (pData != NULL && hbmp != NULL && pBits != NULL)
    {
        *pBits = pData;
    }

    return hbmp;
}

inline COLORREF  __stdcall PixelAlpha(COLORREF srcPixel, int percent)
{
    // My formula for calculating the transpareny is as
    // follows(for each single color):
    //
    //                             percent
    // destPixel = sourcePixel *( ------- )
    //                               100
    //
    // This is not real alpha blending, as it only modifies the brightness,
    // but not the color(a real alpha blending had to mix the source and
    // destination pixels, e.g. mixing green and red makes yellow).
    // For our nice "menu" shadows its good enough.
    COLORREF clrFinal = RGB(min(255, (GetRValue(srcPixel) * percent) / 100), min(255, (GetGValue(srcPixel) * percent) / 100), min(255, (GetBValue(srcPixel) * percent) / 100));
    // TRACE("%d %d %d\n", GetRValue(clrFinal), GetGValue(clrFinal), GetBValue(clrFinal));
    return(clrFinal);
}

inline int GetHeight(const RECT& rect)
{
    return rect.bottom - rect.top;
}

inline int GetWidth(const RECT& rect)
{
    return rect.right - rect.left;
}


inline void __stdcall SetAlphaPixel(COLORREF* pBits,
                                    int rect_Width,
                                    int rect_Height,
                                    int x,
                                    int y,
                                    int percent,
                                    int iShadowSize,
                                    COLORREF clrBase,
                                    BOOL bIsRight)
{
    // Our direct bitmap access swapped the y coordinate...
    y = (rect_Height + iShadowSize) - y;
    COLORREF* pColor = (COLORREF*)(bIsRight ? (pBits + (rect_Width + iShadowSize) * (y + 1) - x) : (pBits + (rect_Width + iShadowSize) * y + x));
    *pColor = PixelAlpha(*pColor, percent);

    if (clrBase == (COLORREF) - 1)
    {
        return;
    }

    *pColor = RGB(min(255, (3 * GetRValue(*pColor) + GetBValue(clrBase)) / 4), min(255, (3 * GetGValue(*pColor) + GetGValue(clrBase)) / 4), min(255, (3 * GetBValue(*pColor) + GetRValue(clrBase)) / 4));
}




static BOOL DrawShadow(HDC hdc,
                       RECT& rect,                 // Shadow will be draw around this rect
                       int nDepth,                     // Shadow depth (pixels)
                       int iMinBrightness = 100,       // Min. brighttness
                       int iMaxBrightness = 50,        // Max. beightness
                       COLORREF clrBase = (COLORREF) - 1, // Base color
                       BOOL bRightShadow = TRUE)
{
    assert(nDepth >= 0);

    if (nDepth == 0 || IsRectEmpty(&rect))
    {
        return TRUE;
    }

    const int cx = GetWidth(rect);
    const int cy = GetHeight(rect);
    const BOOL bIsLeft = !bRightShadow;
    // Copy screen content into the memory bitmap:
    HDC hdcMem = ::CreateCompatibleDC(hdc);

    if (hdcMem == NULL)
    {
        assert(FALSE);
        return FALSE;
    }

    // Gets the whole menu and changes the shadow.
    HBITMAP hbmpMem = ::CreateCompatibleBitmap(hdc, cx + nDepth, cy + nDepth);

    if (hbmpMem  == NULL)
    {
        ::DeleteDC(hdcMem);
        assert(FALSE);
        return FALSE;
    }

    HBITMAP hOldBmp = (HBITMAP)SelectObject(hdcMem, hbmpMem);
    COLORREF* pBits;
    SIZE sz = {cx + nDepth, cy + nDepth};
    HBITMAP hmbpDib = CreateBitmap_32(sz, (LPVOID*)&pBits);

    if (hmbpDib == NULL || pBits == NULL)
    {
        ::DeleteDC(hdcMem);
        ::DeleteObject(hbmpMem);
        assert(FALSE);
        return FALSE;
    }

    ::SelectObject(hdcMem, hmbpDib);
    ::BitBlt(hdcMem, 0, 0, cx + nDepth, cy + nDepth, hdc, bIsLeft ? rect.left - nDepth : rect.left, rect.top, SRCCOPY);
    // Process shadowing:
    // For having a very nice shadow effect, its actually hard work. Currently,
    // I'm using a more or less "hardcoded" way to set the shadows(by using a
    // hardcoded algorythm):
    //
    // This algorithm works as follows:
    //
    // It always draws a few lines, from left to bottom, from bottom to right,
    // from right to up, and from up to left). It does this for the specified
    // shadow width and the color settings.
    // For speeding up things, iShadowOffset is the
    // value which is needed to multiply for each shadow step
    const int iShadowOffset = (iMaxBrightness - iMinBrightness) / nDepth;
    const int rect_Width = GetWidth(rect);
    const int rect_Height = GetHeight(rect);

    // Loop for drawing the shadow
    for (int c = 0; c < nDepth; c++)
    {
        // Draw the shadow from left to bottom
        for (int y = cy; y < cy + (nDepth - c); y++)
        {
            SetAlphaPixel(pBits, rect_Width, rect_Height, c + nDepth, y, iMaxBrightness - ((nDepth  - c) * (iShadowOffset)), nDepth, clrBase, bIsLeft);
        }

        // Draw the shadow from left to right
        for (int x = nDepth + (nDepth - c); x < cx + c; x++)
        {
            SetAlphaPixel(pBits, rect_Width, rect_Height, x, cy + c, iMaxBrightness - ((c) * (iShadowOffset)), nDepth, clrBase, bIsLeft);
        }

        // Draw the shadow from top to bottom
        for (int y1 = nDepth + (nDepth - c); y1 < cy + c + 1; y1++)
        {
            SetAlphaPixel(pBits,  rect_Width, rect_Height, cx + c, y1, iMaxBrightness - ((c) * (iShadowOffset)), nDepth, clrBase, bIsLeft);
        }

        // Draw the shadow from top to left
        for (int x1 = cx; x1 < cx + (nDepth - c); x1++)
        {
            SetAlphaPixel(pBits,  rect_Width, rect_Height, x1, c + nDepth, iMaxBrightness - ((nDepth - c) * (iShadowOffset)), nDepth, clrBase, bIsLeft);
        }
    }

    // Copy shadowed bitmap back to the screen:
    ::BitBlt(hdc, bIsLeft ? rect.left - nDepth : rect.left, rect.top, cx + nDepth, cy + nDepth, hdcMem, 0, 0, SRCCOPY);
    ::SelectObject(hdcMem, hOldBmp);
    ::DeleteObject(hmbpDib);
    ::DeleteDC(hdcMem);
    ::DeleteObject(hbmpMem);
    return TRUE;
}



static void FillGradientRect(HDC hDC, RECT& rect, COLORREF bk, COLORREF fore,  ULONG dwMode)
{
    TRIVERTEX v[2];
    GRADIENT_RECT r[1];
    v[0].x     = (rect.left);
    v[0].y     = (rect.top);
    v[0].Red   = GetRValue(bk) << 8;
    v[0].Green = GetGValue(bk) << 8;
    v[0].Blue  = GetBValue(bk) << 8;
    v[0].Alpha = 0x0000;
    v[1].x     = (rect.right);
    v[1].y     = (rect.bottom);
    v[1].Red   = GetRValue(fore) << 8;
    v[1].Green = GetGValue(fore) << 8;
    v[1].Blue  = GetBValue(fore) << 8;
    v[1].Alpha = 0x0000;
    r[0].UpperLeft = 0;
    r[0].LowerRight = 1;
    GradientFill(hDC, v, (ULONG)2, r, (ULONG)1, dwMode);
}



static void FillSolidRect(HDC hDC, LPCRECT lpRect, COLORREF clr)
{
    COLORREF clrOld = ::SetBkColor(hDC, clr);

    if (clrOld != CLR_INVALID)
    {
        ::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL);
        ::SetBkColor(hDC, clrOld);
    }
}

namespace CanvasPlus
{
    struct CanvasStateInfo
    {
         // push state on state stack
        //The current transformation matrix.
        //The current clipping region.
        SaveClipRgn m_SaveClipRgn;
        //The current values of the following attributes: 
        FillStyle strokeStyle;
        FillStyle fillStyle;
        //globalAlpha, 
        double lineWidth;
        //lineCap, 
        //lineJoin, 
        //miterLimit, 
        double shadowOffsetX;
        double shadowOffsetY;
        double shadowBlur;
        Color shadowColor;
        //globalCompositeOperation, 
        Font font;
        TextAlign textAlign;
        TextBaseline textBaseline;
};

    
    
    


    inline COLORREF ColorToColorRef(const Color& color)
    {
        return RGB(color.r, color.g, color.b);
    }

    struct CanvasGradientImp
    {
        COLORREF color1;
        COLORREF color2;
        double x0, y0, x1, y1;

        unsigned int refcount;


        void Release()
        {
            refcount--;

            if (refcount == 0)
            {
                delete this;
            }
        }

        bool isVertical() const
        {
            return x0 == x1;
        }

        CanvasGradientImp(double x0, double y0, double x1, double y1)
        {
            this->x0 = x0;
            this->y0 = y0;
            this->x1 = x1;
            this->y1 = y1;
            refcount = 1;
        }

        void AddRef()
        {
            refcount++;
        }

        // opaque object
        void addColorStop(double offset, const Color& color)
        {
            if (offset == 0)
            {
                color1 = RGB(color.r, color.g, color.b);
            }
            else if (offset == 1)
            {
                color2 = RGB(color.r, color.g, color.b);
            }
            else
            {
                assert(false);
            }
        }
    };




void CanvasGradient::addColorStop(double offset, const Color& color)
    {
        if (pCanvasGradientImp)
        {
            pCanvasGradientImp->addColorStop(offset, color);
        }
    }

    CanvasGradient::CanvasGradient(CanvasGradientImp* p)
    {
        this->pCanvasGradientImp = p;

        if (this->pCanvasGradientImp)
        {
            this->pCanvasGradientImp->AddRef();
        }
    }

    CanvasGradient& CanvasGradient::operator=(const CanvasGradient& other)
    {
        this->pCanvasGradientImp = other.pCanvasGradientImp;

        if (this->pCanvasGradientImp)
        {
            this->pCanvasGradientImp->AddRef();
        }

        return *this;
    }

    CanvasGradient::CanvasGradient(const CanvasGradient& other)
    {
        this->pCanvasGradientImp = other.pCanvasGradientImp;
        this->pCanvasGradientImp->AddRef();
    }

    CanvasGradient::~CanvasGradient()
    {
        if (pCanvasGradientImp)
        {
            pCanvasGradientImp->Release();
        }
    }

    CanvasGradient Context2D::createLinearGradient(double x0,
            double y0,
            double x1,
            double y1)
    {
        if (x0 == x1)
        {
            return CanvasGradient(new CanvasGradientImp(x0, y0, x1, y1));
        }
        else if (y0 == y1)
        {
            return CanvasGradient(new CanvasGradientImp(x0, y0, x1, y1));
        }
        else
        {
        }

        return CanvasGradient();
    }



    Font::Font()
    {
        m_pNativeObject = nullptr;
        operator = ("10px sans-serif");
    }

    Font::Font(const Font&)
    {
        assert(false);
    }

    Font::~Font()
    {
        if (m_pNativeObject != nullptr)
        {
          ::DeleteObject((HDC)m_pNativeObject);
        }
    }

    Font& Font::operator = (const Font& font)
    {
        setFont(font.fontdesc.c_str());        
        return *this;
    }

    
    Font& Font::operator = (const char* psz)
    {
        setFont(psz);        
        return *this;
    }

    void Font::setFont(const char* psz)
    {
        if (m_pNativeObject)
        {
            ::DeleteObject((HDC)m_pNativeObject);
            m_pNativeObject = nullptr;
        }

        // TODO THIS IS NOT A PARSER
        // PROVISORY

        //See 15.8 Shorthand font property
        //http://www.w3.org/TR/CSS2/fonts.html#font-shorthand

        std::string str(psz);
        //"When the context is created, the font of the
        //context must be set to 10px sans-serif. "
        LOGFONT logFont;

        /////////////////////////////////////////////////
        if (str.find("12pt") != std::string::npos)
        {
            logFont.lfHeight = 12;    //-17;
        }
        else if (str.find("14px") != std::string::npos)
        {
            logFont.lfHeight = -14;    //-17;
        }
        else
        {
            logFont.lfHeight = -10; //-17;
        }

        ////////////////////////////////////////////////
        logFont.lfWidth = 0;
        logFont.lfEscapement = 0;
        logFont.lfOrientation = 0;

        ////
        if (str.find("bold") != std::string::npos)
        {
            logFont.lfWeight = 700;
        }
        else
        {
            logFont.lfWeight = 400;
        }

        ////
        logFont.lfItalic = 0;
        logFont.lfUnderline = 0;
        logFont.lfStrikeOut = 0;
        logFont.lfCharSet = 0;
        logFont.lfOutPrecision = 3;
        logFont.lfClipPrecision = 2;
        logFont.lfQuality = 1;
        logFont.lfPitchAndFamily = 49;

        if (str.find("arial") != std::string::npos)
        {
            wcscpy(logFont.lfFaceName , L"arial");
        }
        else
        {
            wcscpy(logFont.lfFaceName , L"sans-serif");
        }

        assert(m_pNativeObject == nullptr);
        fontdesc = psz;
        m_pNativeObject = (void*)CreateFontIndirect(&logFont);
    }

    Canvas::Canvas(void* p, double w, double h) : m_CanvasRenderingContext2D(p), width(w), height(h)
    {
    }

    Canvas::~Canvas()
    {
    }

    Context2D& Canvas::getContext(const char*)
    {
        return m_CanvasRenderingContext2D;
    }


    void Context2D::Check()
    {
        //because GDI closes the path after Fill or stroke
        //we need a state to do both at the same time
        if (this->flags == 0)
        {
            //not using path
            return;
        }

        //------------------------------
        HDC hdc = (HDC) m_pNativeHandle;
        //-------------------------------

        if (this->flags == 1)
        {
            COLORREF color = ColorToColorRef(strokeStyle.m_Color);
            HPEN hpen = CreatePen(PS_SOLID, (int)(lineWidth), color); //TODO
            HPEN oldPen = (HPEN) SelectObject(hdc, hpen);
            StrokePath(hdc);
            SelectObject(hdc, oldPen);
            //
            DeleteObject(hpen);
        }
        else if (this->flags == 2)
        {
            COLORREF color = ColorToColorRef(fillStyle.m_Color);
            LOGBRUSH logbrush;
            logbrush.lbColor = color;
            logbrush.lbStyle = BS_SOLID;
            logbrush.lbHatch = 0;
            HBRUSH hBrush = CreateBrushIndirect(&logbrush);
            HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
            //
            FillPath(hdc);
            //
            SelectObject(hdc, oldBrush);
            DeleteObject(hBrush);
        }
        else if (this->flags == 3)
        {
            COLORREF color2 = ColorToColorRef(strokeStyle.m_Color);
            HPEN hpen = CreatePen(PS_SOLID, (int) lineWidth, color2); //TODO
            HPEN oldPen = (HPEN) SelectObject(hdc, hpen);
            COLORREF color = ColorToColorRef(fillStyle.m_Color);
            LOGBRUSH logbrush;
            logbrush.lbColor = color;
            logbrush.lbStyle = BS_SOLID;
            logbrush.lbHatch = 0;
            HBRUSH hBrush = CreateBrushIndirect(&logbrush);
            HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
            //
            StrokeAndFillPath(hdc);
            //
            SelectObject(hdc, oldPen);
            SelectObject(hdc, oldBrush);
            //
            DeleteObject(hBrush);
            DeleteObject(hpen);
        }

        this->flags = 0;
    }

    Context2D::Context2D(void* p)
        : m_pNativeHandle(p)
    {
        this->flags = 0;
        this->lineWidth = 1.0;
        this->shadowColor = "rgb(0,0,0)";
        this->shadowOffsetX = 0;
        this->shadowOffsetY = 0;
        this->shadowBlur = 0;
    }

    Context2D::~Context2D()
    {
        Check();
    }

     // state
    void Context2D::save()
    {
       Check();

       CanvasStateInfo * p = new CanvasStateInfo();
        
        HDC hdc = (HDC) m_pNativeHandle;
        
        p->m_SaveClipRgn.Save(hdc);

        // push state on state stack
        //The current transformation matrix.
        //The current clipping region.
        //The current values of the following attributes: 
        p->strokeStyle = strokeStyle;
        p->fillStyle = fillStyle; 
        //globalAlpha, 
        p->lineWidth = lineWidth; 
        //lineCap, 
        //lineJoin, 
        //miterLimit, 
        p->shadowOffsetX = shadowOffsetX; 
        p->shadowOffsetY = shadowOffsetY;
        p->shadowBlur = shadowBlur;
        p->shadowColor = shadowColor;
        //globalCompositeOperation, 
        p->font = font; 
        p->textAlign = textAlign; 
        p->textBaseline = textBaseline;

    
        m_stack.push_back(p);
    }

    void Context2D::restore()
    {
        Check();

        // pop state stack and restore state
         CanvasStateInfo * p = m_stack.back();
         m_stack.pop_back();
        
        
        p->m_SaveClipRgn.Restore();

        // push state on state stack
        //The current transformation matrix.
        //The current clipping region.
        //The current values of the following attributes: 
        strokeStyle = p->strokeStyle;
        fillStyle = p->fillStyle; 
        //globalAlpha, 
        lineWidth = p->lineWidth; 
        //lineCap, 
        //lineJoin, 
        //miterLimit, 
        shadowOffsetX = p->shadowOffsetX; 
        shadowOffsetY = p->shadowOffsetY;
        shadowBlur = p->shadowBlur;
        shadowColor = p->shadowColor;
        //globalCompositeOperation, 
        font = p->font; 
        textAlign = p->textAlign; 
        textBaseline = p->textBaseline;
    
        delete p;
    }
    //
    ///////////////////////////////////////////////////////////////////////////
    // The clearRect(x, y, w, h) method must clear the pixels in the specified 
    // rectangle that also intersect the current clipping region to a fully transparent 
    // black, erasing any previous image. If either height or width are zero, 
    // this method has no effect.
    void Context2D::clearRect(double x, double y, double w, double h)
    {
        FillStyle fs = fillStyle;
        fillStyle = Color("#FFFFFF"); //GDI background color white? black?
        //
        fillRect(x, y, w, h);
        //
        fillStyle = fs;
    }
    ///////////////////////////////////////////////////////////////////////////////
    // The fillRect(x, y, w, h) method must paint the specified rectangular area
    //  using the fillStyle. If either height or width are zero, this method
    //  has no effect.
    void Context2D::fillRect(double x, double y, double w, double h)
    {
        Check();
        //------------------------------
        HDC hdc = (HDC) m_pNativeHandle;
        //-------------------------------
        //TODO waiting for transformations..
        RECT rect = {(LONG)x, (LONG)y, (LONG)(x + w), (LONG)(y + h)};

        if (this->shadowOffsetX != 0 &&
                this->shadowOffsetY != 0)
        {
            DrawShadow(hdc, rect, (int)shadowOffsetX);
        }

        if (fillStyle.fillStyleEnum == FillStyleEnumSolid)
        {
            COLORREF color = ColorToColorRef(fillStyle.m_Color);
            FillSolidRect(hdc, &rect, color);
        }
        else if (fillStyle.fillStyleEnum == FillStyleEnumGradient)
        {
            UINT mode = fillStyle.canvasGradient.pCanvasGradientImp->isVertical() ?
                        GRADIENT_FILL_RECT_V :
                        GRADIENT_FILL_RECT_H;
            COLORREF color1 = fillStyle.canvasGradient.pCanvasGradientImp->color1;
            COLORREF color2 = fillStyle.canvasGradient.pCanvasGradientImp->color2;

            if (mode == GRADIENT_FILL_RECT_V &&
                    fillStyle.canvasGradient.pCanvasGradientImp->y0 >
                    fillStyle.canvasGradient.pCanvasGradientImp->y1)
            {
                std::swap(color1, color2);
            }

            if (mode == GRADIENT_FILL_RECT_H &&
                    fillStyle.canvasGradient.pCanvasGradientImp->x0 >
                    fillStyle.canvasGradient.pCanvasGradientImp->x1)
            {
                std::swap(color1, color2);
            }

            FillGradientRect(hdc, rect, color1, color2, mode);
        }
    }

    void Context2D::strokeRect(double x, double y, double w, double h)
    {
        Check();
        //------------------------------
        HDC hdc = (HDC) m_pNativeHandle;
        //-------------------------------
        COLORREF color = ColorToColorRef(strokeStyle.m_Color);
        //
        HPEN hpen = CreatePen(PS_SOLID, (int)lineWidth, color); //TODO
        HPEN oldPen = (HPEN) SelectObject(hdc, hpen);
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));
        //
        ::Rectangle(hdc, (LONG)x, (LONG)y, (LONG)(x + w), (LONG)(y + h));
        //
        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);
        //
        DeleteObject(hpen);
    }



    int Context2D::ToPixelX(double x)
    {
        //TODO
        return (int) x;
    }

    int Context2D::ToPixelY(double y)
    {
        //TODO
        return (int) y;
    }

    int Context2D::ToPixelSizeX(double x)
    {
        //TODO
        return (int) x;
    }

    int Context2D::ToPixelSizeY(double y)
    {
        //TODO
        return (int) y;
    }



    TextMetrics Context2D::measureText(const wchar_t* psz)
    {
        Check();
        //------------------------------
        HDC hdc = (HDC) m_pNativeHandle;
        //-------------------------------
        //  TEXTMETRIC tm;
        //  GetTextMetrics(hdc, &tm);
        //
        HFONT oldfont = (HFONT) SelectObject(hdc, (HFONT)font.m_pNativeObject);
        SIZE sz;
        ::GetTextExtentPoint(hdc, psz, wcslen(psz), &sz);
        ::SelectObject(hdc, oldfont);
        return TextMetrics(sz.cx);
    }

    void Context2D::moveTo(double x, double y)
    {
        Check();
        //------------------------------
        HDC hdc = (HDC) m_pNativeHandle;
        //-------------------------------
        const int ix = ToPixelX(x);
        const int iy = ToPixelY(y);
        ::MoveToEx(hdc, ix, iy, NULL);
    }

    void Context2D::lineTo(double x, double y)
    {
        Check();
        //------------------------------
        HDC hdc = (HDC) m_pNativeHandle;
        //-------------------------------
        const int ix = ToPixelX(x);
        const int iy = ToPixelY(y);
        //
        ::LineTo(hdc, ix, iy);
        //
    }

    void Context2D::beginPath()
    {
        Check();
        //------------------------------
        HDC hdc = (HDC) m_pNativeHandle;
        //-------------------------------
        BeginPath(hdc);
    }

    void Context2D::closePath()
    {
        Check();
        //------------------------------
        HDC hdc = (HDC) m_pNativeHandle;
        //-------------------------------
        CloseFigure(hdc);
        EndPath(hdc);
    }

    void Context2D::rect(double x, double y, double w, double h)
    {
        //------------------------------
        HDC hdc = (HDC) m_pNativeHandle;
        //-------------------------------
        ::Rectangle(hdc, ToPixelX(x), ToPixelY(y), ToPixelX(x + w), ToPixelY(y + h));
    }
    void Context2D::clip()
    {
        // The clip() method must create a new clipping region by calculating the intersection
        //of the current clipping region and the area described by the current path, using the
        //non-zero winding number rule. Open subpaths must be implicitly closed when computing
        //the clipping region, without affecting the actual subpaths. The new clipping region
        //replaces the current clipping region.
        //When the context is initialized, the clipping region must be set to the rectangle with the top left corner at (0,0) and the width and height of the coordinate space.
        //------------------------------
        HDC hdc = (HDC) m_pNativeHandle;
        //-------------------------------
        EndPath(hdc);
        SelectClipPath(hdc, RGN_AND);
        //TODO
    }

    void Context2D::fill()
    {
        closePath();

        Check();

        if (flags == 0)
        {
            flags = 2;
        }
        else if (flags == 1)
        {
            flags = 3;
        }
    }

    void Context2D::stroke()
    {
        closePath();

        Check();

        if (flags == 0)
        {
            flags = 1;
        }
        else if (flags == 2)
        {
            flags = 3;
        }
    }

    static void GdiTextOut(HDC hdc,
                           TextAlign textAlign,
                           TextBaselineEnum baseLine,
                           const wchar_t* psz,
                           int textlen,
                           int ix,
                           int iy)
    {
        assert(GetTextAlign(hdc) == TA_LEFT);
        SIZE sz;
        ::GetTextExtentPointW(hdc, psz, textlen, &sz);
        int newx = ix;
        int newy = iy;

        switch (textAlign)
        {
        case TextAlignStart:
            newx = ix;
            break;

        case TextAlignEnd:
            newx = ix - sz.cx;
            break;

        case TextAlignLeft:
            newx = ix;
            break;

        case TextAlignRight:
            newx = ix - sz.cx;
            break;

        case TextAlignCenter:
            newx = ix - sz.cx / 2;
            break;

        default:
            assert(false);
        }

        switch (baseLine)
        {
        case TextBaselineTop :
            newy = iy;
            break;//The top of the em square

        case TextBaselineHanging:
            newy = iy;
            break; //The hanging baseline

        case TextBaselineMiddle :
            newy = iy - sz.cy / 2;
            break; //The middle of the em square

        case TextBaselineAlphabetic :
        {
            TEXTMETRIC tm;
            GetTextMetrics(hdc, &tm);
            newy = iy - (tm.tmHeight - tm.tmDescent);
        }
        break;//The alphabetic baseline

        case TextBaselineIdeographic :
        {
            TEXTMETRIC tm;
            GetTextMetrics(hdc, &tm);
            newy = iy - (tm.tmHeight);
        }
        break; //The ideographic baseline

        case TextBaselineBottom :
            newy = iy - sz.cy;
            break;//The bottom of the em square

        default:
            break;
        }

        TextOut(hdc, newx, newy, psz, wcslen(psz));
    }

    void Context2D::fillText(const wchar_t* psz, double x, double y)
    {
        Check();
        //------------------------------
        HDC hdc = (HDC) m_pNativeHandle;
        //-------------------------------
        const int ix = ToPixelX(x);
        const int iy = ToPixelY(y);
        //
        COLORREF color = ColorToColorRef(fillStyle.m_Color);
        COLORREF oldcolor = SetTextColor(hdc, color);
        int oldbkmode = SetBkMode(hdc, TRANSPARENT);
        HFONT oldfont = (HFONT) SelectObject(hdc, (HFONT)font.m_pNativeObject);
        //
        GdiTextOut(hdc, this->textAlign, this->textBaseline, psz, wcslen(psz), ix, iy);
        //
        SetTextColor(hdc, oldcolor);
        SetBkMode(hdc, oldbkmode);
        SelectObject(hdc, oldfont);
    }

} //namespace CanvasPlus

