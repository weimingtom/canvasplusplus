

#include <Windows.h>
#include <cassert>
#include "..\Canvas.h"

//THIS FILE IS WINDOWS ONLY AND GDI ONLY
//GDI+ at the same file?

//TODO Direct2d

//Gradient
#pragma comment( lib, "msimg32.lib" )
///

static void GradientRectH(HDC hDC, RECT& rect, COLORREF fore, COLORREF bk)
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
    GradientFill(hDC, v, (ULONG)2, r, (ULONG)1, GRADIENT_FILL_RECT_H);
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

//, GRADIENT_FILL_RECT_H
static void GradientRectV(HDC hdc, RECT& rect, COLORREF fore, COLORREF bk, ULONG dwMode = GRADIENT_FILL_RECT_V)
{
    FillGradientRect(hdc, rect, bk, fore, GRADIENT_FILL_RECT_V);
}

static void GradientRectH(HDC hdc, RECT& rect, COLORREF fore, COLORREF bk, ULONG dwMode = GRADIENT_FILL_RECT_V)
{
    FillGradientRect(hdc, rect, fore, bk, GRADIENT_FILL_RECT_H);
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

            refcount == 0;
        }

        void AddRef()
        {
            refcount++;
        }

        // opaque object
        void addColorStop(double offset, const char* colortxt)
        {
            CanvasPlus::Color color(colortxt);

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



    void CanvasGradient::addColorStop(double offset, const char* color)
    {
        if (pCanvasGradientImp)
        {
            pCanvasGradientImp->addColorStop(offset, color);
        }
    }

    CanvasGradient::CanvasGradient(CanvasGradientImp* p)
    {
        this->pCanvasGradientImp = p;
        this->pCanvasGradientImp->AddRef();
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
        //"When the context is created, the font of the
        //context must be set to 10px sans-serif. "
        LOGFONT logFont;
        logFont.lfHeight = -10; //-17;
        logFont.lfWidth = 0;
        logFont.lfEscapement = 0;
        logFont.lfOrientation = 0;
        logFont.lfWeight = 400;
        logFont.lfItalic = 0;
        logFont.lfUnderline = 0;
        logFont.lfStrikeOut = 0;
        logFont.lfCharSet = 0;
        logFont.lfOutPrecision = 3;
        logFont.lfClipPrecision = 2;
        logFont.lfQuality = 1;
        logFont.lfPitchAndFamily = 49;
        wcscpy(logFont.lfFaceName , L"sans-serif");
        m_pNativeObject = (void*)CreateFontIndirect(&logFont);
    }

    Font::~Font()
    {
        ::DeleteObject((HDC)m_pNativeObject);
    }

    Canvas::Canvas(void* p) : m_CanvasRenderingContext2D(p)
    {
    }

    Canvas::~Canvas()
    {
    }

    Context2D& Canvas::getContext(const char*)
    {
        return m_CanvasRenderingContext2D;
    }

    Context2D::Context2D(void* p)
        : m_pNativeHandle(p)
    {
        //"When the context is created, the lineWidth attribute must initially have the value 1.0."
        lineWidth = 1.0;
    }

    Context2D::~Context2D()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////
    // The fillRect(x, y, w, h) method must paint the specified rectangular area
    //  using the fillStyle. If either height or width are zero, this method
    //  has no effect.
    void Context2D::fillRect(double x, double y, double w, double h)
    {
        //------------------------------
        HDC hdc = (HDC) m_pNativeHandle;
        //-------------------------------
        //TODO waiting for transformations..
        RECT rect = {(LONG)x, (LONG)y, (LONG)(x + w), (LONG)(y + h)};

        if (fillStyle.fillStyleEnum == FillStyleEnumSolid)
        {
            COLORREF color = RGB(fillStyle.m_Color.r, fillStyle.m_Color.g, fillStyle.m_Color.b);
            FillSolidRect(hdc, &rect, color);
        }
        else if (fillStyle.fillStyleEnum == FillStyleEnumGradient)
        {
            UINT mode = fillStyle.canvasGradient.pCanvasGradientImp->isVertical() ?
                       GRADIENT_FILL_RECT_V: 
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


            FillGradientRect(hdc, rect, color1,color2, mode);
        }
    }

    void Context2D::strokeRect(double x, double y, double w, double h)
    {
        //------------------------------
        HDC hdc = (HDC) m_pNativeHandle;
        //-------------------------------
        //TODO waiting for transformations..
        RECT rect = {(LONG)x, (LONG)y, (LONG)(x + w), (LONG)(y + h)};
        COLORREF color = RGB(strokeStyle.m_Color.r, strokeStyle.m_Color.g, strokeStyle.m_Color.b);
        //DrawRect(hdc, &rect, color);
        HPEN hpen = CreatePen(PS_SOLID, lineWidth, color); //TODO
        HPEN oldPen = (HPEN) SelectObject(hdc, hpen);
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));
        //
        ::Rectangle(hdc, (LONG)x, (LONG)y, (LONG)(x + w), (LONG)(y + h));
        //
        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);
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


    TextMetrics Context2D::measureText(const wchar_t* psz)
    {
        //------------------------------
        HDC hdc = (HDC) m_pNativeHandle;
        //-------------------------------
        //  TEXTMETRIC tm;
        //  GetTextMetrics(hdc, &tm);
        SIZE sz;
        ::GetTextExtentPoint(hdc, psz, wcslen(psz), &sz);
        return TextMetrics(sz.cx);
    }

    void Context2D::moveTo(double x, double y)
    {
        //------------------------------
        HDC hdc = (HDC) m_pNativeHandle;
        //-------------------------------
        const int ix = ToPixelX(x);
        const int iy = ToPixelY(y);
        ::MoveToEx(hdc, ix, iy, NULL);
    }

    void Context2D::lineTo(double x, double y)
    {
        //------------------------------
        HDC hdc = (HDC) m_pNativeHandle;
        //-------------------------------
        const int ix = ToPixelX(x);
        const int iy = ToPixelY(y);
        COLORREF color = RGB(fillStyle.m_Color.r, fillStyle.m_Color.g, fillStyle.m_Color.b);
        HPEN hpen = CreatePen(PS_SOLID, lineWidth, color); //TODO
        HPEN oldPen = (HPEN) SelectObject(hdc, hpen);
        //
        ::LineTo(hdc, ix, iy);
        //
        SelectObject(hdc, oldPen);
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
        //------------------------------
        HDC hdc = (HDC) m_pNativeHandle;
        //-------------------------------
        const int ix = ToPixelX(x);
        const int iy = ToPixelY(y);
        //
        COLORREF color = RGB(fillStyle.m_Color.r, fillStyle.m_Color.g, fillStyle.m_Color.b);
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

