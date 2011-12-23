

#include <Windows.h>
#include <cassert>
#include "..\Canvas.h"

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

CanvasPlus::Font::Font()
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

CanvasPlus::Font::~Font()
{
    ::DeleteObject((HDC)m_pNativeObject);
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
    //"When the context is created, the lineWidth attribute must initially have the value 1.0."
    lineWidth = 1.0;
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

void CanvasPlus::CanvasRenderingContext2D::strokeRect(double x, double y, double w, double h)
{
    //------------------------------
    HDC hdc = (HDC) m_pNativeHandle;
    //-------------------------------
    //TODO waiting for transformations..
    RECT rect = {(LONG)x, (LONG)y, (LONG)(x + w), (LONG)(y + h)};
    COLORREF color = RGB(fillStyle.m_Color.r, fillStyle.m_Color.g, fillStyle.m_Color.b);
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



int CanvasPlus::CanvasRenderingContext2D::ToPixelX(double x)
{
    //TODO
    return (int) x;
}

int CanvasPlus::CanvasRenderingContext2D::ToPixelY(double y)
{
    //TODO
    return (int) y;
}


CanvasPlus::TextMetrics CanvasPlus::CanvasRenderingContext2D::measureText(const wchar_t* psz)
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

void CanvasPlus::CanvasRenderingContext2D::moveTo(double x, double y)
{
    //------------------------------
    HDC hdc = (HDC) m_pNativeHandle;
    //-------------------------------
    const int ix = ToPixelX(x);
    const int iy = ToPixelY(y);
    ::MoveToEx(hdc, ix, iy, NULL);
}

void CanvasPlus::CanvasRenderingContext2D::lineTo(double x, double y)
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
                       CanvasPlus::TextAlign textAlign,
                       CanvasPlus::TextBaselineEnum baseLine,
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
    case CanvasPlus::TextAlignStart:  newx = ix;          break;
    case CanvasPlus::TextAlignEnd:    newx = ix - sz.cx;  break;
    case CanvasPlus::TextAlignLeft:   newx = ix;          break;
    case CanvasPlus::TextAlignRight:  newx = ix - sz.cx;  break;
    case CanvasPlus::TextAlignCenter: newx = ix - sz.cx / 2; break;
    default:
        assert(false);
    }

    switch (baseLine)
    {
    case CanvasPlus::TextBaselineTop :
        newy = iy;
        break;//The top of the em square

    case CanvasPlus::TextBaselineHanging:
        newy = iy;
        break; //The hanging baseline

    case CanvasPlus::TextBaselineMiddle :
        newy = iy - sz.cy / 2;
        break; //The middle of the em square

    case CanvasPlus::TextBaselineAlphabetic :
        {
            TEXTMETRIC tm;
            GetTextMetrics(hdc, &tm);
            newy = iy - (tm.tmHeight - tm.tmDescent);
        }
        break;//The alphabetic baseline

    case CanvasPlus::TextBaselineIdeographic :
        {
            TEXTMETRIC tm;
            GetTextMetrics(hdc, &tm);
            newy = iy - (tm.tmHeight);
        }
        break; //The ideographic baseline

    case CanvasPlus::TextBaselineBottom :
        newy = iy - sz.cy;
        break;//The bottom of the em square

    default:
        break;
    }

    TextOut(hdc, newx, newy, psz, wcslen(psz));
}

void CanvasPlus::CanvasRenderingContext2D::fillText(const wchar_t* psz, double x, double y)
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

