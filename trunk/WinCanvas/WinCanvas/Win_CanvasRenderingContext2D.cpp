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
  SelectObject(hdc, oldBrush );
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

UINT TextAlignToWindowsFlags(CanvasPlus::TextAlign textAlign,
                             CanvasPlus::TextBaseline textBaseline)
{
    UINT flags = 0;

    switch (textAlign)
    {
        case CanvasPlus::TextAlignStart:
            flags |= TA_LEFT;
            break;//?

        case CanvasPlus::TextAlignEnd:
            flags |= TA_LEFT;
            break;//?

        case CanvasPlus::TextAlignLeft:
            flags |= TA_LEFT;
            break;//?

        case CanvasPlus::TextAlignRight:
            flags |= TA_RIGHT;
            break;//?

        case CanvasPlus::TextAlignCenter:
            flags |= TA_CENTER;
            break;//?
    }

    switch (textBaseline)
    {
        case CanvasPlus::TextBaselineTop :
            flags |= TA_TOP;
            break;//?  //The top of the em square

        case CanvasPlus::TextBaselineHanging:
            break; //The hanging baseline

        case CanvasPlus::TextBaselineMiddle :
            break; //The middle of the em square

        case CanvasPlus::TextBaselineAlphabetic :
            flags |= TA_BASELINE ;
            break;//The alphabetic baseline

        case CanvasPlus::TextBaselineIdeographic :
            break; //The ideographic baseline

        case CanvasPlus::TextBaselineBottom :
            flags |= TA_BOTTOM;
            break;//? //The bottom of the em square

        default:
            break;
    }

    return flags;
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

void CanvasPlus::CanvasRenderingContext2D::fillText(const wchar_t* psz, double x, double y)
{
    //------------------------------
    HDC hdc = (HDC) m_pNativeHandle;
    //-------------------------------
    const int ix = ToPixelX(x);
    const int iy = ToPixelY(y);
    //
    UINT flags = TextAlignToWindowsFlags(this->textAlign, this->textBaseline);
    UINT old = SetTextAlign(hdc, flags);
    COLORREF color = RGB(fillStyle.m_Color.r, fillStyle.m_Color.g, fillStyle.m_Color.b);
    COLORREF oldcolor = SetTextColor(hdc, color);
    int oldbkmode = SetBkMode(hdc, TRANSPARENT);
    HFONT oldfont = (HFONT) SelectObject(hdc, (HFONT)font.m_pNativeObject);
    //
    TextOut(hdc, ix, iy, psz, wcslen(psz));
    //
    SetTextAlign(hdc, old);
    SetTextColor(hdc, oldcolor);
    SetBkMode(hdc, oldbkmode);
    SelectObject(hdc, oldfont);
}

