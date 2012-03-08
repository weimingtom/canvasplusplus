


#ifdef USING_WIN_GDI

#include <Windows.h>
#include <cassert>
#include "..\Canvas.h"
#include "..\FontParser.h"



struct LogfontActions : public Actions
{
    LOGFONTA m_LogFont;
    LogfontActions()
    {
        m_LogFont.lfHeight = -10; //-17;
        ////////////////////////////////////////////////
        m_LogFont.lfWidth = 0;
        m_LogFont.lfEscapement = 0;
        m_LogFont.lfOrientation = 0;
        m_LogFont.lfWeight = 400;
        ////
        m_LogFont.lfItalic = 0;
        m_LogFont.lfUnderline = 0;
        m_LogFont.lfStrikeOut = 0;
        m_LogFont.lfCharSet = 0;
        m_LogFont.lfOutPrecision = 3;
        m_LogFont.lfClipPrecision = 2;
        m_LogFont.lfQuality = 1;
        m_LogFont.lfPitchAndFamily = 49;
        strcpy(m_LogFont.lfFaceName, "sans-serif");
    }

    //Actions
    virtual void SetFontName(const std::string& s)
    {
        if (s.size() > 2 && s[0] == '"')
        {
            strncpy(m_LogFont.lfFaceName , s.c_str() + 1, s.size() - 2);
            m_LogFont.lfFaceName[s.size() - 2] = '\0';
        }
        else
        {
            strncpy(m_LogFont.lfFaceName , s.c_str(), s.size());
            m_LogFont.lfFaceName[s.size()] = '\0';
        }
    }
    virtual void SetNormalStyle()
    {
        //m_LogFont.lfItalic = TRUE;
    }
    virtual void SetItalic()
    {
        m_LogFont.lfItalic = TRUE;
    }
    virtual void SetOblique()
    {
    }
    virtual void SetNormalWeight()
    {
        m_LogFont.lfWeight = 400;
    }
    virtual void SetWeightBold()
    {
        m_LogFont.lfWeight = 700;
    }
    virtual void SetWeight100()
    {
        m_LogFont.lfWeight = 100;
    }
    virtual void SetWeight200()
    {
        m_LogFont.lfWeight = 200;
    }

    virtual void SetWeight300()
    {
        m_LogFont.lfWeight = 300;
    }
    virtual void SetWeight400()
    {
        m_LogFont.lfWeight = 400;
    }
    virtual void SetWeight500()
    {
        m_LogFont.lfWeight = 500;
    }
    virtual void SetWeight600()
    {
        m_LogFont.lfWeight = 600;
    }
    virtual void SetWeight700()
    {
        m_LogFont.lfWeight = 700;
    }
    virtual void SetWeight800()
    {
        m_LogFont.lfWeight = 800;
    }
    virtual void SetWeight900()
    {
        m_LogFont.lfWeight = 900;
    }
    virtual void SetSizePercent()
    {
    }
    virtual void SetSizePoints(int pt)
    {
        m_LogFont.lfHeight = pt; //-17;
    }
    virtual void SetSizePixels(int px)
    {
        m_LogFont.lfHeight = -px;
    }
    virtual void SetSizeLarger()
    {
    }
    virtual void SetSizeSmaller()
    {
    }
    virtual void SetSizexxsmall()
    {
    }
    virtual void SetSizexsmall()
    {
    }
    virtual void SetSizesmall()
    {
    }
    virtual void SetSizemedium()
    {
    }
    virtual void SetSizelarge()
    {
    }
    virtual void SetSizexlarge()
    {
    }
    virtual void SetSizexxlarge()
    {
    }
};





//THIS FILE IS WINDOWS ONLY AND GDI ONLY

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

    SaveClipRgn(HDC m_hDC)
    {
        assert(m_hDC != NULL);
        m_hDC = NULL;
        m_hOldRgn = NULL;
        Save(m_hDC);
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

    BOOL Save(HDC hDC)
    {
        assert(hDC != NULL);
        assert(m_hOldRgn == NULL);
        assert(m_hDC == NULL);
        m_hOldRgn = ::CreateRectRgn(0, 0, 0, 0);

        if (m_hOldRgn != NULL)
        {
            switch (GetClipRgn(hDC, m_hOldRgn))
            {
            case 1:
            {
                // success, m_hOldRgn contains the current clipping region for the DC
                m_hDC = hDC;
            }
            break;

            case 0:
            {
                // success, but the DC has no clipping region
                m_hDC = hDC;
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

    ClipRect(HDC m_hDC, RECT& newClipRect) : m_old(m_hDC)
    {
        RECT rcClip = newClipRect;
        ::LPtoDP(m_hDC, (LPPOINT)&rcClip, 2);
        m_hNewRegionClip = CreateRectRgnIndirect(&rcClip);
        ///SelectClipRgn(m_hDC, m_hNewRegionClip);
        ExtSelectClipRgn(m_hDC, m_hNewRegionClip, RGN_AND);
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

inline void DrawBitmap(HDC m_hDC, HBITMAP hbm, int Left, int Top)
{
    BOOL f;
    HDC hdcBits;
    BITMAP bm;
    hdcBits = CreateCompatibleDC(m_hDC);
    GetObject(hbm, sizeof(BITMAP), &bm);
    SelectObject(hdcBits, hbm);
    f = BitBlt(m_hDC, Left, Top, bm.bmWidth, bm.bmHeight, hdcBits, 0, 0, SRCCOPY);
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




static BOOL DrawShadow(HDC m_hDC,
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
    HDC hdcMem = ::CreateCompatibleDC(m_hDC);

    if (hdcMem == NULL)
    {
        assert(FALSE);
        return FALSE;
    }

    // Gets the whole menu and changes the shadow.
    HBITMAP hbmpMem = ::CreateCompatibleBitmap(m_hDC, cx + nDepth, cy + nDepth);

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
    ::BitBlt(hdcMem, 0, 0, cx + nDepth, cy + nDepth, m_hDC, bIsLeft ? rect.left - nDepth : rect.left, rect.top, SRCCOPY);
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
    ::BitBlt(m_hDC, bIsLeft ? rect.left - nDepth : rect.left, rect.top, cx + nDepth, cy + nDepth, hdcMem, 0, 0, SRCCOPY);
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
    class CanvasStateInfo;
    //
    //
    class GdiContext2D : public Context2D
    {
        int flags;
        HDC m_hDC;
        //Context2D* m_pContext2D;
        std::vector<CanvasStateInfo*> m_stack;

        //
        FillStyle fillStyle;        // (default black)
        FillStyle strokeStyle;      // (default black)

        TextAlign textAlign;        // "start", "end", "left", "right", "center" (default: "start")
        TextBaseline textBaseline;  // "top", "hanging", "middle", "alphabetic", "ideographic", "bottom" (default: "alphabetic")
        Font font;                  // (default 10px sans-serif)

        double lineWidth;           // (default 1)

        Color shadowColor;          // (default transparent black)
        double shadowOffsetX;       // (default 0)
        double shadowOffsetY;       // (default 0)
        double  shadowBlur;          // (default 0)

    public:

        GdiContext2D(HDC hdc)
        {
            BeginDraw(hdc);
        }

        void BeginDraw(HDC hdc)
        {
            this->m_hDC = hdc;
            //defaults==
            this->textAlign = TextAlignStart;
            this->textBaseline = TextBaselineAlphabetic;
            this->lineWidth = 1.0;
            this->shadowColor = "rgb(0,0,0)";
            this->shadowOffsetX = 0;
            this->shadowOffsetY = 0;
            this->shadowBlur = 0;
            this->strokeStyle = "rgb(0,0,0)";
            this->fillStyle = "rgb(0,0,0)";
            this->font = "10px sans-serif";
            //TODO all init here!
        }

        void EndDraw()
        {
        }

        void Check();

        // state
        void save(); // push state on state stack
        void restore(); // pop state stack and restore state

        void clearRect(double x, double y, double w, double h);
        void fillRect(double x, double y, double w, double h);
        void strokeRect(double x, double y, double w, double h);
        void fillText(const wchar_t*, double x, double y);
        CanvasGradient createLinearGradient(double x0, double y0, double x1, double y1);
        TextMetrics measureText(const wchar_t*);
        void moveTo(double x, double y);
        void lineTo(double x, double y);
        void beginPath();
        void closePath();
        void stroke();
        void fill();
        void clip();
        void rect(double x, double y, double w, double h);

        //

        void set_fillStyle(const CanvasGradient&);
        void set_fillStyle(const Color&);
        void set_fillStyle(const char*);
        void set_fillStyle(const FillStyle&);
        const FillStyle& get_fillStyle() const;

        void set_strokeStyle(const CanvasGradient&);
        void set_strokeStyle(const Color&);
        void set_strokeStyle(const char*);
        void set_strokeStyle(const FillStyle&);
        const FillStyle& get_strokeStyle() const;

        void set_textAlign(const char*);
        void set_textAlign(const TextAlign&);
        const TextAlign& get_textAlign()const;

        void set_textBaseline(const char*);
        void set_textBaseline(const TextBaseline&);
        const TextBaseline& get_textBaseline() const;

        void set_font(const char*);
        void set_font(const Font&);
        const Font& get_font() const;

        void set_lineWidth(double);
        double get_lineWidth() const;

        void set_shadowColor(const Color&);
        const Color&  get_shadowColor()const;

        void set_shadowOffsetX(double);
        double get_shadowOffsetX() const;

        void set_shadowOffsetY(double);
        double get_shadowOffsetY() const;

        void set_shadowBlur(double);
        double get_shadowBlur() const;
        //
    };

    struct CanvasStateInfo
    {
        CanvasStateInfo()
        {
        }

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

    CanvasGradient GdiContext2D::createLinearGradient(double x0,
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

        LogfontActions actions;
        ParseFont(psz, actions);
        assert(m_pNativeObject == nullptr);
        fontdesc = psz;
        m_pNativeObject = (void*)CreateFontIndirectA(&actions.m_LogFont);
    }





    // == Context2D::Implementation ==

    void GdiContext2D::Check()
    {
        //because GDI closes the path after Fill or stroke
        //we need a state to do both at the same time
        if (this->flags == 0)
        {
            //not using path
            return;
        }

        if (this->flags == 1)
        {
            COLORREF color = ColorToColorRef(strokeStyle.m_Color);
            HPEN hpen = CreatePen(PS_SOLID, (int)lineWidth, color); //TODO
            HPEN oldPen = (HPEN) SelectObject(m_hDC, hpen);
            StrokePath(m_hDC);
            SelectObject(m_hDC, oldPen);
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
            HBRUSH oldBrush = (HBRUSH)SelectObject(m_hDC, hBrush);
            //
            FillPath(m_hDC);
            //
            SelectObject(m_hDC, oldBrush);
            DeleteObject(hBrush);
        }
        else if (this->flags == 3)
        {
            COLORREF color2 = ColorToColorRef(strokeStyle.m_Color);
            HPEN hpen = CreatePen(PS_SOLID, (int) lineWidth, color2); //TODO
            HPEN oldPen = (HPEN) SelectObject(m_hDC, hpen);
            COLORREF color = ColorToColorRef(fillStyle.m_Color);
            LOGBRUSH logbrush;
            logbrush.lbColor = color;
            logbrush.lbStyle = BS_SOLID;
            logbrush.lbHatch = 0;
            HBRUSH hBrush = CreateBrushIndirect(&logbrush);
            HBRUSH oldBrush = (HBRUSH)SelectObject(m_hDC, hBrush);
            //
            StrokeAndFillPath(m_hDC);
            //
            SelectObject(m_hDC, oldPen);
            SelectObject(m_hDC, oldBrush);
            //
            DeleteObject(hBrush);
            DeleteObject(hpen);
        }

        this->flags = 0;
    }



    // state
    void GdiContext2D::save()
    {
        Check();
        CanvasStateInfo* p = new CanvasStateInfo();
        p->m_SaveClipRgn.Save(m_hDC);
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

    void GdiContext2D::restore()
    {
        Check();
        // pop state stack and restore state
        CanvasStateInfo* p = m_stack.back();
        m_stack.pop_back();
        p->m_SaveClipRgn.Restore();
        // push state on state stack
        //The current transformation matrix.
        //The current clipping region.
        //The current values of the following attributes:
        set_strokeStyle(p->strokeStyle);
        set_fillStyle(p->fillStyle);
        //globalAlpha,
        lineWidth = p->lineWidth;
        //lineCap,
        //lineJoin,
        //miterLimit,
        set_shadowOffsetX(p->shadowOffsetX);
        set_shadowOffsetY(p->shadowOffsetY);
        set_shadowBlur(p->shadowBlur);
        set_shadowColor(p->shadowColor);
        //globalCompositeOperation,
        set_font(p->font);
        set_textAlign(p->textAlign);
        set_textBaseline(p->textBaseline);
        delete p;
    }
    //
    ///////////////////////////////////////////////////////////////////////////
    // The clearRect(x, y, w, h) method must clear the pixels in the specified
    // rectangle that also intersect the current clipping region to a fully transparent
    // black, erasing any previous image. If either height or width are zero,
    // this method has no effect.
    void GdiContext2D::clearRect(double x, double y, double w, double h)
    {
        FillStyle fs = fillStyle;
        fillStyle = Color("#FFFFFF"); //GDI background color white? black?
        //
        fillRect(x, y, w, h);
        //
        set_fillStyle(fs);
    }
    ///////////////////////////////////////////////////////////////////////////////
    // The fillRect(x, y, w, h) method must paint the specified rectangular area
    //  using the fillStyle. If either height or width are zero, this method
    //  has no effect.
    void GdiContext2D::fillRect(double x, double y, double w, double h)
    {
        Check();
        //TODO waiting for transformations..
        RECT rect = {(LONG)x, (LONG)y, (LONG)(x + w), (LONG)(y + h)};

        if (shadowOffsetX != 0 &&
                shadowOffsetY != 0)
        {
            DrawShadow(m_hDC, rect, (int)shadowOffsetX);
        }

        if (fillStyle.fillStyleEnum == FillStyleEnumSolid)
        {
            COLORREF color = ColorToColorRef(fillStyle.m_Color);
            FillSolidRect(m_hDC, &rect, color);
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

            FillGradientRect(m_hDC, rect, color1, color2, mode);
        }
    }

    void GdiContext2D::strokeRect(double x, double y, double w, double h)
    {
        Check();
        COLORREF color = ColorToColorRef(strokeStyle.m_Color);
        //
        HPEN hpen = CreatePen(PS_SOLID, (int)lineWidth, color); //TODO
        HPEN oldPen = (HPEN) SelectObject(m_hDC, hpen);
        HBRUSH oldBrush = (HBRUSH)SelectObject(m_hDC, GetStockObject(HOLLOW_BRUSH));
        //
        ::Rectangle(m_hDC, (LONG)x, (LONG)y, (LONG)(x + w), (LONG)(y + h));
        //
        SelectObject(m_hDC, oldPen);
        SelectObject(m_hDC, oldBrush);
        //
        DeleteObject(hpen);
    }


    TextMetrics GdiContext2D::measureText(const wchar_t* psz)
    {
        Check();
        //  TEXTMETRIC tm;
        //  GetTextMetrics(m_hDC, &tm);
        //
        HFONT oldfont = (HFONT) SelectObject(m_hDC, (HFONT) font.m_pNativeObject);
        SIZE sz;
        ::GetTextExtentPoint(m_hDC, psz, wcslen(psz), &sz);
        ::SelectObject(m_hDC, oldfont);
        return TextMetrics(sz.cx);
    }

    void GdiContext2D::moveTo(double x, double y)
    {
        Check();
        const int ix = x;
        const int iy = y;
        ::MoveToEx(m_hDC, ix, iy, NULL);
    }

    void GdiContext2D::lineTo(double x, double y)
    {
        Check();
        const int ix = (x);
        const int iy = (y);
        //
        ::LineTo(m_hDC, ix, iy);
        //
    }

    void GdiContext2D::beginPath()
    {
        Check();
        BeginPath(m_hDC);
    }

    void GdiContext2D::closePath()
    {
        Check();
        CloseFigure(m_hDC);
        EndPath(m_hDC);
    }

    void GdiContext2D::rect(double x, double y, double w, double h)
    {
        ::Rectangle(m_hDC, x, y, (x + w), (y + h));
    }
    void GdiContext2D::clip()
    {
        // The clip() method must create a new clipping region by calculating the intersection
        //of the current clipping region and the area described by the current path, using the
        //non-zero winding number rule. Open subpaths must be implicitly closed when computing
        //the clipping region, without affecting the actual subpaths. The new clipping region
        //replaces the current clipping region.
        //When the context is initialized, the clipping region must be set to the rectangle with the top left corner at (0,0) and the width and height of the coordinate space.
        EndPath(m_hDC);
        SelectClipPath(m_hDC, RGN_AND);
        //TODO
    }

    void GdiContext2D::fill()
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

    void GdiContext2D::stroke()
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

    static void GdiTextOut(HDC m_hDC,
                           TextAlign textAlign,
                           TextBaseline baseLine,
                           const wchar_t* psz,
                           int textlen,
                           int ix,
                           int iy)
    {
        assert(GetTextAlign(m_hDC) == TA_LEFT);
        SIZE sz;
        ::GetTextExtentPointW(m_hDC, psz, textlen, &sz);
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
            GetTextMetrics(m_hDC, &tm);
            newy = iy - (tm.tmHeight - tm.tmDescent);
        }
        break;//The alphabetic baseline

        case TextBaselineIdeographic :
        {
            TEXTMETRIC tm;
            GetTextMetrics(m_hDC, &tm);
            newy = iy - (tm.tmHeight);
        }
        break; //The ideographic baseline

        case TextBaselineBottom :
            newy = iy - sz.cy;
            break;//The bottom of the em square

        default:
            break;
        }

        TextOut(m_hDC, newx, newy, psz, wcslen(psz));
    }

    void GdiContext2D::fillText(const wchar_t* psz, double x, double y)
    {
        Check();
        const int ix = x;//ToPixelX(x);
        const int iy = y;//ToPixelY(y);
        //
        COLORREF color = ColorToColorRef(fillStyle.m_Color);
        COLORREF oldcolor = SetTextColor(m_hDC, color);
        int oldbkmode = SetBkMode(m_hDC, TRANSPARENT);
        HFONT oldfont = (HFONT) SelectObject(m_hDC, (HFONT)font.m_pNativeObject);
        //
        GdiTextOut(m_hDC, textAlign, textBaseline, psz, wcslen(psz), ix, iy);
        //
        SetTextColor(m_hDC, oldcolor);
        SetBkMode(m_hDC, oldbkmode);
        SelectObject(m_hDC, oldfont);
    }

    ///

    void GdiContext2D::set_fillStyle(const CanvasGradient& v)
    {
        Check();
        fillStyle = v;
    }

    void GdiContext2D::set_fillStyle(const Color& v)
    {
        Check();
        fillStyle = v;
    }

    void GdiContext2D::set_fillStyle(const char* v)
    {
        Check();
        fillStyle = v;
    }

    void GdiContext2D::set_fillStyle(const FillStyle& v)
    {
        fillStyle = v;
    }

    const FillStyle& GdiContext2D::get_fillStyle() const
    {
        return fillStyle;
    }

    void GdiContext2D::set_strokeStyle(const CanvasGradient& v)
    {
        strokeStyle = v;
    }

    void GdiContext2D::set_strokeStyle(const Color& v)
    {
        Check();
        strokeStyle = v;
    }

    void GdiContext2D::set_strokeStyle(const char* v)
    {
        Check();
        strokeStyle = v;
    }

    void GdiContext2D::set_strokeStyle(const FillStyle& v)
    {
        Check();
        strokeStyle = v;
    }

    const FillStyle& GdiContext2D::get_strokeStyle() const
    {
        return strokeStyle;
    }

    void GdiContext2D::set_textAlign(const char* v)
    {
        Check();
        textAlign = ParseTextAlign(v);
    }

    void GdiContext2D::set_textAlign(const TextAlign& v)
    {
        Check();
        textAlign = v;
    }

    const TextAlign& GdiContext2D::get_textAlign()const
    {
        return textAlign;
    }

    void GdiContext2D::set_textBaseline(const char* v)
    {
        Check();
        textBaseline = ParseTextBaseline(v);
    }

    void GdiContext2D::set_textBaseline(const TextBaseline& v)
    {
        Check();
        textBaseline = v;
    }

    const TextBaseline& GdiContext2D::get_textBaseline() const
    {
        return textBaseline;
    }

    void GdiContext2D::set_font(const char* v)
    {
        Check();
        font = v;
    }

    void GdiContext2D::set_font(const Font& v)
    {
        Check();
        font = v;
    }

    const Font& GdiContext2D::get_font() const
    {
        return font;
    }

    void GdiContext2D::set_lineWidth(double v)
    {
        Check();
        lineWidth  = v;
    }

    double GdiContext2D::get_lineWidth() const
    {
        return lineWidth;
    }

    void GdiContext2D::set_shadowColor(const Color& v)
    {
        Check();
        shadowColor = v;
    }

    const Color&  GdiContext2D::get_shadowColor()const
    {
        return shadowColor;
    }

    void GdiContext2D::set_shadowOffsetX(double v)
    {
        Check();
        shadowOffsetX = v;
    }

    double GdiContext2D::get_shadowOffsetX() const
    {
        return shadowOffsetX;
    }

    void GdiContext2D::set_shadowOffsetY(double v)
    {
        Check();
        shadowOffsetY = v;
    }

    double GdiContext2D::get_shadowOffsetY() const
    {
        return shadowOffsetY;
    }

    void GdiContext2D::set_shadowBlur(double v)
    {
        Check();
        shadowBlur = v;
    }

    double GdiContext2D::get_shadowBlur() const
    {
        return shadowBlur;
    }


    class GDICanvas : public CanvasImp
    {
        CanvasPlus::GdiContext2D* m_pD2DContext2D;
        int m_width;
        int m_height;

        ~GDICanvas();
        Context2D& getContext(const char*);
        void BeginDraw(void* p, int w, int h);
        void EndDraw();

        double get_width() const;
        void set_width(double v);

        double get_height() const;

        void set_height(double v);
    public:
        GDICanvas();

    };

    GDICanvas::GDICanvas()
    {
        m_pD2DContext2D = nullptr;
    }



    GDICanvas::~GDICanvas()
    {
        delete m_pD2DContext2D;
    }

    Context2D& GDICanvas::getContext(const char*)
    {
        return *m_pD2DContext2D;
    }

    void GDICanvas::EndDraw()
    {
        m_pD2DContext2D->EndDraw();
    }

    void GDICanvas::BeginDraw(void* p, int w, int h)
    {
        if (m_pD2DContext2D == nullptr)
        {
            m_pD2DContext2D = new GdiContext2D((HDC) p);
        }

        RECT rc;
        GetClientRect((HWND) p, &rc);
        set_width((rc.right - rc.left));
        set_height((rc.bottom - rc.top));
        m_pD2DContext2D->BeginDraw((HDC)p);
    }

    double GDICanvas::get_width() const
    {
        return m_width;
    }
    void GDICanvas::set_width(double v)
    {
        m_width = v;
    }

    double GDICanvas::get_height() const
    {
        return m_height;
    }

    void GDICanvas::set_height(double v)
    {
        m_height  = v;
    }

    CanvasImp* CreateCanvas(void*)
    {
        return new GDICanvas();
    }

    ////////
} //namespace CanvasPlus

#endif //USING_WIN_GDI
