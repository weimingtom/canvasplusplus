

#ifdef USING_WIN_D2D

#include <Windows.h>
#include <cassert>
#include "..\Canvas.h"
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <vector>


#pragma comment( lib, "d2d1.lib" )
#pragma comment( lib, "WindowsCodecs.lib" )
#pragma comment( lib, "dwrite.lib" )

template<class Interface>
inline void SafeRelease(
    Interface** ppInterfaceToRelease
)
{
    if (*ppInterfaceToRelease != NULL)
    {
        (*ppInterfaceToRelease)->Release();
        (*ppInterfaceToRelease) = NULL;
    }
}
namespace CanvasPlus
{
    //
    //
    class D2DContext2D : public Context2D
    {
        ID2D1Factory* m_pDirect2dFactory;
        ID2D1HwndRenderTarget* m_pRenderTarget;
        ID2D1SolidColorBrush* m_pFillBrush;
        ID2D1SolidColorBrush* m_pStrokeBrush;

        int m_LayerCount;

        // DirectWrite
        IDWriteFactory* m_pDWriteFactory;
        //IDWriteTextFormat* m_pTextFormat;

        FLOAT m_strokeWidth;
        ID2D1StrokeStyle* m_pStrokeStyle;



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


        std::vector<ID2D1Geometry*> m_paths;
        ID2D1GeometrySink* m_pCurrentGeometrySink;

        void CloseCurrentSink()
        {
            if (m_pCurrentGeometrySink != NULL)
            {
                m_pCurrentGeometrySink->EndFigure(D2D1_FIGURE_END_CLOSED);
                HRESULT hr = m_pCurrentGeometrySink->Close();
                SafeRelease(&m_pCurrentGeometrySink);
            }
        }
    public:
        D2DContext2D(HWND hwnd) :
            m_pDirect2dFactory(NULL),
            m_pRenderTarget(NULL),
            m_pFillBrush(NULL),
            m_pStrokeBrush(NULL),
            //m_pTextFormat(NULL),
            m_pDWriteFactory(NULL),
            m_strokeWidth(1.1f),
            m_pStrokeStyle(NULL),
            m_pCurrentGeometrySink(NULL),
            m_LayerCount(0)

        {
            RECT rc;
            GetClientRect(hwnd, &rc);
            int w = rc.right - rc.left;
            int h = rc.bottom - rc.top;
            HRESULT hr = CreateDeviceIndependentResources();
            // Because the CreateWindow function takes its size in pixels,
            // obtain the system DPI and use it to scale the window size.
            //  FLOAT dpiX, dpiY;
            // The factory returns the current system DPI. This is also the value it will use
            // to create its own windows.
            //  m_pDirect2dFactory->GetDesktopDpi(&dpiX, &dpiY);
            hr = CreateDeviceResources(hwnd, w, h);
        }



        // Discards device-dependent resources. These resources must be
        // recreated when the Direct3D device is lost.
        void DiscardDeviceResources()
        {
            SafeRelease(&m_pRenderTarget);
            SafeRelease(&m_pFillBrush);
            SafeRelease(&m_pStrokeBrush);
        }

        // Creates resources that are bound to a particular
        // Direct3D device. These resources need to be recreated
        // if the Direct3D device dissapears, such as when the display
        // changes, the window is remoted, etc.
        HRESULT CreateDeviceResources(HWND hwnd, int w, int h)
        {
            HRESULT hr = S_OK;

            if (!m_pRenderTarget)
            {
                D2D1_SIZE_U size = D2D1::SizeU(w, h);
                // Create a Direct2D render target.
                hr = m_pDirect2dFactory->CreateHwndRenderTarget(
                         D2D1::RenderTargetProperties(),
                         D2D1::HwndRenderTargetProperties(hwnd, size),
                         &m_pRenderTarget
                     );
            }

            return hr;
        }

        // Creates resources that are not bound to a particular device.
        // Their lifetime effectively extends for the duration of the
        // application.
        HRESULT CreateDeviceIndependentResources()
        {
            HRESULT hr = S_OK;
            // Create a Direct2D factory.
            hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);

            if (SUCCEEDED(hr))
            {
                hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&m_pDWriteFactory));
            }

            // Create a text format using Gabriola with a font size of 72.
            // This sets the default font, weight, stretch, style, and locale.
            return hr;
        }

        void BeginDraw(int width, int height)
        {
            m_pRenderTarget->Resize(D2D1::SizeU(width, height));
            m_pRenderTarget->BeginDraw();
            m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
            m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
            m_strokeWidth = 1.1f;

            if (m_LayerCount > 0)
            {
                m_pRenderTarget->PopLayer();
                m_LayerCount--;
            }

            HRESULT hr = S_OK;

            if (SUCCEEDED(hr))
            {
                SafeRelease(&m_pFillBrush);
                hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pFillBrush);
            }

            if (SUCCEEDED(hr))
            {
                SafeRelease(&m_pStrokeBrush);
                hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black),
                        &m_pStrokeBrush);
            }
        }


        void EndDraw()
        {
            if (m_LayerCount > 0)
            {
                m_pRenderTarget->PopLayer();
                m_LayerCount--;
            }

            if (m_pRenderTarget)
            {
                m_pRenderTarget->EndDraw();
            }
        }

        ~D2DContext2D()
        {
            EndDraw();
            SafeRelease(&m_pDirect2dFactory);
            SafeRelease(&m_pRenderTarget);
            SafeRelease(&m_pFillBrush);
            SafeRelease(&m_pStrokeBrush);
        }



        // state
        virtual void save(); // push state on state stack
        virtual void restore(); // pop state stack and restore state

        virtual void clearRect(double x, double y, double w, double h);
        virtual void fillRect(double x, double y, double w, double h);
        virtual void strokeRect(double x, double y, double w, double h);
        virtual void fillText(const wchar_t*, double x, double y);
        virtual CanvasGradient createLinearGradient(double x0, double y0, double x1, double y1);
        virtual TextMetrics measureText(const wchar_t*);
        virtual void moveTo(double x, double y);
        virtual void lineTo(double x, double y);
        virtual void beginPath();
        virtual void closePath();
        virtual void stroke();
        virtual void fill();
        virtual void clip();
        virtual void rect(double x, double y, double w, double h);

        //

        virtual void set_fillStyle(const CanvasGradient&);
        virtual void set_fillStyle(const Color&);
        virtual void set_fillStyle(const char*);
        virtual void set_fillStyle(const FillStyle&);
        virtual const FillStyle& get_fillStyle() const;

        virtual void set_strokeStyle(const CanvasGradient&);
        virtual void set_strokeStyle(const Color&);
        virtual void set_strokeStyle(const char*);
        virtual void set_strokeStyle(const FillStyle&);
        virtual const FillStyle& get_strokeStyle() const;

        virtual void set_textAlign(const char*);
        virtual void set_textAlign(const TextAlign&);
        virtual const TextAlign& get_textAlign() const;

        virtual void set_textBaseline(const char*);
        virtual void set_textBaseline(const TextBaseline&);
        virtual const TextBaseline& get_textBaseline() const;

        virtual void set_font(const char*);
        virtual void set_font(const Font&);
        virtual const Font& get_font() const;

        virtual void set_lineWidth(double);
        virtual double get_lineWidth() const;

        virtual void set_shadowColor(const Color&);
        virtual const Color&  get_shadowColor() const;

        virtual void set_shadowOffsetX(double);
        virtual double get_shadowOffsetX() const;

        virtual void set_shadowOffsetY(double);
        virtual double get_shadowOffsetY() const;

        virtual void set_shadowBlur(double);
        virtual double get_shadowBlur() const;
        //
    };




    struct CanvasGradientImp
    {
        std::vector<D2D1_GRADIENT_STOP> m_gradientStops;
        FLOAT m_x0;
        FLOAT m_y0;
        FLOAT m_x1;
        FLOAT m_y1;
        unsigned int m_refCount;

        CanvasGradientImp(double x0,
                          double y0,
                          double x1,
                          double y1)
        {
            m_x0 = x0;
            m_y0 = y0;
            m_x1 = x1;
            m_y1 = y1;
            m_refCount = 1;
        }

        void Release()
        {
            m_refCount--;

            if (m_refCount == 0)
            {
                delete this;
            }
        }

        void AddRef()
        {
            m_refCount++;
        }

        virtual void addColorStop(double offset, const Color& color)
        {
            m_gradientStops.push_back(D2D1_GRADIENT_STOP());
            m_gradientStops.back().color = D2D1::ColorF(color.r, color.g, color.b);
            m_gradientStops.back().position = offset;
        }


    };

    CanvasGradient::CanvasGradient(CanvasGradientImp* p)
    {
        pCanvasGradientImp = p; //ja vem addref
        //if (pCanvasGradientImp)
        //{
        //pCanvasGradientImp->AddRef();
        //}
    }

    CanvasGradient& CanvasGradient::operator=(const CanvasGradient& other)
    {
        if (pCanvasGradientImp)
        {
            pCanvasGradientImp->Release();
        }

        pCanvasGradientImp = other.pCanvasGradientImp;

        if (pCanvasGradientImp)
        {
            pCanvasGradientImp->AddRef();
        }

        return *this;
    }

    CanvasGradient::CanvasGradient(const CanvasGradient& other)
    {
        pCanvasGradientImp = other.pCanvasGradientImp;

        if (pCanvasGradientImp)
        {
            pCanvasGradientImp->AddRef();
        }
    }

    void CanvasGradient::addColorStop(double offset, const Color& color)
    {
        if (pCanvasGradientImp)
        {
            pCanvasGradientImp->addColorStop(offset, color);
        }
    }
    CanvasGradient::~CanvasGradient()
    {
        if (pCanvasGradientImp != NULL)
        {
            pCanvasGradientImp->Release();
        }
    }

    CanvasGradient D2DContext2D::createLinearGradient(double x0,
            double y0,
            double x1,
            double y1)
    {
        CanvasGradientImp* p = new CanvasPlus::CanvasGradientImp(x0, y0, x1, y1);
        return CanvasGradient(p);
    }

    Font::Font()
    {
    }

    Font::Font(const Font&)
    {
    }

    Font::~Font()
    {
    }

    Font& Font::operator = (const Font& font)
    {
        return *this;
    }


    Font& Font::operator = (const char* psz)
    {
        return *this;
    }

    void Font::setFont(const char* psz)
    {
    }



    // state
    void D2DContext2D::save()
    {
    }

    void D2DContext2D::restore()
    {
    }
    //
    ///////////////////////////////////////////////////////////////////////////
    // The clearRect(x, y, w, h) method must clear the pixels in the specified
    // rectangle that also intersect the current clipping region to a fully transparent
    // black, erasing any previous image. If either height or width are zero,
    // this method has no effect.
    void D2DContext2D::clearRect(double x, double y, double w, double h)
    {
        // m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
    }
    ///////////////////////////////////////////////////////////////////////////////
    // The fillRect(x, y, w, h) method must paint the specified rectangular area
    //  using the fillStyle. If either height or width are zero, this method
    //  has no effect.
    void D2DContext2D::fillRect(double x, double y, double w, double h)
    {
        D2D1_RECT_F rc = D2D1::RectF(x, y, x + w, y + h);

        if (fillStyle.fillStyleEnum == FillStyleEnumSolid)
        {
            m_pRenderTarget->FillRectangle(&rc, m_pFillBrush);
        }
        else if (fillStyle.fillStyleEnum == FillStyleEnumGradient)
        {
            CanvasGradientImp* p = fillStyle.canvasGradient.pCanvasGradientImp;

            if (p == NULL || p->m_gradientStops.empty())
            {
                return;
            }

            ID2D1GradientStopCollection* pGradientStopCollection;
            // declared array of D2D1_GRADIENT_STOP structs.
            HRESULT hr = m_pRenderTarget->CreateGradientStopCollection(
                             &p->m_gradientStops[0],
                             p->m_gradientStops.size(),
                             D2D1_GAMMA_2_2,
                             D2D1_EXTEND_MODE_CLAMP,
                             &pGradientStopCollection);

            if (SUCCEEDED(hr))
            {
                ID2D1LinearGradientBrush* pLinearGradientBrush;
                hr = m_pRenderTarget->CreateLinearGradientBrush(
                         D2D1::LinearGradientBrushProperties(
                             D2D1::Point2F(p->m_x0, p->m_y0),
                             D2D1::Point2F(p->m_x1, p->m_y1)),
                         pGradientStopCollection,
                         &pLinearGradientBrush);

                if (SUCCEEDED(hr))
                {
                    m_pRenderTarget->FillRectangle(&rc, pLinearGradientBrush);
                }

                SafeRelease(&pLinearGradientBrush);
                SafeRelease(&pGradientStopCollection);
            }
        }
    }

    void D2DContext2D::strokeRect(double x, double y, double w, double h)
    {
        D2D1_RECT_F rc = D2D1::RectF(x, y, x + w, y + h);
        m_pRenderTarget->DrawRectangle(&rc, m_pStrokeBrush, m_strokeWidth, m_pStrokeStyle);
    }
    TextMetrics D2DContext2D::measureText(const wchar_t* psz)
    {
        return TextMetrics(1);
    }
    void D2DContext2D::moveTo(double x, double y)
    {
        CloseCurrentSink();
        ID2D1PathGeometry* pPathGeometry = NULL;
        HRESULT hr = m_pDirect2dFactory->CreatePathGeometry(&pPathGeometry);

        if (SUCCEEDED(hr))
        {
            m_paths.push_back(pPathGeometry);
        }

        hr = pPathGeometry->Open(&m_pCurrentGeometrySink);

        if (m_pCurrentGeometrySink)
        {
            m_pCurrentGeometrySink->BeginFigure(D2D1::Point2F(x, y), D2D1_FIGURE_BEGIN_FILLED);
        }
    }
    void D2DContext2D::lineTo(double x, double y)
    {
        if (m_pCurrentGeometrySink)
        {
            m_pCurrentGeometrySink->AddLine(D2D1::Point2F(x, y));
        }
    }
    void ClearPaths(std::vector<ID2D1Geometry*>& v)
    {
        for (int i = 0 ; i < v.size(); i++)
        {
            v[i]->Release();
        }

        v.clear();
    }
    void D2DContext2D::beginPath()
    {
        ClearPaths(m_paths);
    }
    void D2DContext2D::closePath()
    {
        CloseCurrentSink();
    }
    void D2DContext2D::rect(double x, double y, double w, double h)
    {
        CloseCurrentSink();
        D2D1_RECT_F layoutRect = D2D1::RectF(x, y, x + w, y + h);
        ID2D1RectangleGeometry* pRectangleGeometry = NULL;
        HRESULT hr = m_pDirect2dFactory->CreateRectangleGeometry(layoutRect, &pRectangleGeometry);

        if (SUCCEEDED(hr))
        {
            m_paths.push_back(pRectangleGeometry);
        }
    }
    void D2DContext2D::clip()
    {
        CloseCurrentSink();
        // Create a layer.
        ID2D1Layer* pLayer = NULL;
        HRESULT hr = m_pRenderTarget->CreateLayer(NULL, &pLayer);

        if (SUCCEEDED(hr))
        {
            ID2D1Geometry* pGeometry = m_paths.back();
            m_pRenderTarget->PushLayer(
                D2D1::LayerParameters(D2D1::InfiniteRect(), pGeometry),
                pLayer
            );
            m_LayerCount++;
            //m_pRenderTarget->PopLayer();
        }

        SafeRelease(&pLayer);
        //TODO
    }
    void D2DContext2D::fill()
    {
        closePath();

        for (int i = 0 ; i < m_paths.size(); i++)
        {
            ID2D1Geometry* pGeometry = m_paths[i];
            m_pRenderTarget->FillGeometry(pGeometry, m_pFillBrush, NULL);
        }
    }
    void D2DContext2D::stroke()
    {
        ID2D1StrokeStyle* strokeStyle = NULL;
        HRESULT hr = m_pDirect2dFactory->CreateStrokeStyle(
                         D2D1::StrokeStyleProperties(
                             D2D1_CAP_STYLE_FLAT,
                             D2D1_CAP_STYLE_FLAT,
                             D2D1_CAP_STYLE_FLAT ,
                             D2D1_LINE_JOIN_BEVEL,
                             0,
                             D2D1_DASH_STYLE_SOLID,
                             0.0f),
                         NULL,
                         0,
                         &strokeStyle);
        closePath();

        for (int i = 0 ; i < m_paths.size(); i++)
        {
            ID2D1Geometry* pGeometry = m_paths[i];
            m_pRenderTarget->DrawGeometry(pGeometry, m_pStrokeBrush, m_strokeWidth, strokeStyle);
        }
    }

    void D2DContext2D::fillText(const wchar_t* psz, double x, double y)
    {
      //// (default 10px sans-serif)
        //font.setFont();
        IDWriteTextFormat* pTextFormat;
        HRESULT hr = m_pDWriteFactory->CreateTextFormat(
                         L"Microsoft Sans Serif",                // Font family name.
                         NULL,                       // Font collection (NULL sets it to use the system font collection).
                         DWRITE_FONT_WEIGHT_REGULAR,
                         DWRITE_FONT_STYLE_NORMAL,
                         DWRITE_FONT_STRETCH_NORMAL,
                         10.0f,
                         L"en-us",
                         &pTextFormat
                     );
        //font metrics
        IDWriteFontCollection* collection;
        TCHAR name[64];
        UINT32 findex;
        BOOL exists;
        pTextFormat->GetFontFamilyName(name, 64);
        pTextFormat->GetFontCollection(&collection);
        collection->FindFamilyName(name, &findex, &exists);
        IDWriteFontFamily* ffamily;
        collection->GetFontFamily(findex, &ffamily);
        IDWriteFont* font;
        ffamily->GetFirstMatchingFont(pTextFormat->GetFontWeight(), pTextFormat->GetFontStretch(), pTextFormat->GetFontStyle(), &font);
        DWRITE_FONT_METRICS metrics;
        font->GetMetrics(&metrics);
        float ratio = pTextFormat->GetFontSize() / (float)metrics.designUnitsPerEm;
        float size = (metrics.ascent + metrics.descent + metrics.lineGap) * ratio;
        //  float height = GetHeight();
        //int retval = static_cast<int>(height/size);
        SafeRelease(&ffamily);
        SafeRelease(&collection);
        SafeRelease(&font);
        //
        double x0 = 0;
        double y0 = 0;
        double x1 = 0;
        double y1 = 0;

        if (textAlign == TextAlignCenter)
        {
            x0 = x - 10;
            x1 = x + 10;
            //centro
            pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        }
        else if (textAlign == TextAlignStart || textAlign == TextAlignLeft)
        {
            x0 = x;
            x1 = x + 10;
            //esquerda
            pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        }
        else if (textAlign == TextAlignRight ||
                 textAlign == TextAlignEnd)
        {
            x0 = x - 10;
            x1 = x ;
            //direita
            pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
        }
        else
        {
            x0 = x - 10;
            x1 = x + 10;
        }

        if (textBaseline == TextBaselineBottom || textBaseline == TextBaselineIdeographic)
        {
            y0 = y - 10;
            y1 = y;
            //BASE
            pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
        }
        else if (textBaseline == TextBaselineTop ||
                 textBaseline == TextBaselineHanging)
        {
            y0 = y;
            y1 = y + 10;
            //TOP
            pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
        }
        else  if (textBaseline == TextBaselineMiddle)
        {
            y0 = y - 10;
            y1 = y + 10;
            //CENTRO
            pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
        else  if (textBaseline == TextBaselineAlphabetic)
        {
            float size2 = (metrics.ascent) * ratio;
            y0 = y - size2;
            y1 = y + 10;
            //TOP
            pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
        }
        else
        {
            y0 = y - 10;
            y1 = y + 10;
        }

        D2D1_RECT_F layoutRect = D2D1::RectF(x0, y0, x1, y1);
        pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        // Create a D2D rect that is the same size as the window.
        //        D2D1_RECT_F layoutRect = D2D1::RectF(x-1, y, x + 10000, y + 10000);
        // Use the DrawText method of the D2D render target interface to draw.
        m_pRenderTarget->DrawText(
            psz,        // The string to render.
            wcslen(psz),    // The string's length.
            pTextFormat,    // The text format.
            layoutRect,       // The region of the window where the text will be rendered.
            m_pFillBrush// The brush used to draw the text.
        );
        SafeRelease(&pTextFormat);
        //set_strokeStyle("rgb(255,0,0)");
        //strokeRect(layoutRect.left, layoutRect.top, layoutRect.right - layoutRect.left, layoutRect.bottom - layoutRect.top);
        //set_strokeStyle("rgb(0,0,0)");
    }

    void D2DContext2D::set_fillStyle(const CanvasGradient& v)
    {
        fillStyle = v;
    }
    void D2DContext2D::set_fillStyle(const Color& v)
    {
        fillStyle = v;
        SafeRelease(&m_pStrokeBrush);
        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black),
                                               &m_pStrokeBrush);
    }
    void D2DContext2D::set_fillStyle(const char* v)
    {
        Color c(v);
        fillStyle = c;
        D2D1::ColorF d2dcolor(RGB(c.b, c.g, c.r));
        SafeRelease(&m_pFillBrush);
        HRESULT hr = m_pRenderTarget->CreateSolidColorBrush(d2dcolor, &m_pFillBrush);
    }
    void D2DContext2D::set_fillStyle(const FillStyle& v)
    {
        fillStyle = v;
    }
    const FillStyle& D2DContext2D::get_fillStyle() const
    {
        return fillStyle;
    }
    void D2DContext2D::set_strokeStyle(const CanvasGradient& v)
    {
        strokeStyle = v;
    }
    void D2DContext2D::set_strokeStyle(const Color& v)
    {
        strokeStyle = v;
    }
    void D2DContext2D::set_strokeStyle(const char* v)
    {
        Color c(v);
        strokeStyle = c;
        D2D1::ColorF d2dcolor(RGB(c.b, c.g, c.r));
        SafeRelease(&m_pStrokeBrush);
        HRESULT hr = m_pRenderTarget->CreateSolidColorBrush(d2dcolor, &m_pStrokeBrush);
    }
    void D2DContext2D::set_strokeStyle(const FillStyle& v)
    {
        strokeStyle = v;
    }
    const FillStyle& D2DContext2D::get_strokeStyle() const
    {
        return strokeStyle;
    }
    void D2DContext2D::set_textAlign(const char* v)
    {
        textAlign = ParseTextAlign(v);
    }
    void D2DContext2D::set_textAlign(const TextAlign& v)
    {
        textAlign = v;
    }
    const TextAlign& D2DContext2D::get_textAlign() const
    {
        return textAlign;
    }
    void D2DContext2D::set_textBaseline(const char* v)
    {
        textBaseline = ParseTextBaseline(v);
    }
    void D2DContext2D::set_textBaseline(const TextBaseline& v)
    {
        textBaseline = v;
    }
    const TextBaseline& D2DContext2D::get_textBaseline() const
    {
        return textBaseline;
    }
    void D2DContext2D::set_font(const char* v)
    {
        font = v;
    }
    void D2DContext2D::set_font(const Font& v)
    {
        font = v;
    }
    const Font& D2DContext2D::get_font() const
    {
        return font;
    }
    void D2DContext2D::set_lineWidth(double v)
    {
        m_strokeWidth = v;
    }
    double D2DContext2D::get_lineWidth() const
    {
        return m_strokeWidth;
    }
    void D2DContext2D::set_shadowColor(const Color& v)
    {
    }
    const Color&  D2DContext2D::get_shadowColor() const
    {
        return Color();
    }
    void D2DContext2D::set_shadowOffsetX(double v)
    {
    }
    double D2DContext2D::get_shadowOffsetX() const
    {
        return 0;
    }
    void D2DContext2D::set_shadowOffsetY(double v)
    {
    }
    double D2DContext2D::get_shadowOffsetY() const
    {
        return 0;
    }
    void D2DContext2D::set_shadowBlur(double v)
    {
    }
    double D2DContext2D::get_shadowBlur() const
    {
        return 0;
    }
    class D2DCanvas : public CanvasImp
    {
        CanvasPlus::D2DContext2D* m_pD2DContext2D;
        int m_width;
        int m_height;

        ~D2DCanvas();
        Context2D& getContext(const char*);
        void BeginDraw(void* p, int w, int h);
        void EndDraw();

        double D2DCanvas::get_width() const;
        void D2DCanvas::set_width(double v);

        double D2DCanvas::get_height() const;

        void D2DCanvas::set_height(double v);
    public:
        D2DCanvas();

    };
    D2DCanvas::D2DCanvas()
    {
        m_pD2DContext2D = nullptr;
    }
    D2DCanvas::~D2DCanvas()
    {
        delete m_pD2DContext2D;
    }
    Context2D& D2DCanvas::getContext(const char*)
    {
        return *m_pD2DContext2D;
    }
    void D2DCanvas::EndDraw()
    {
        m_pD2DContext2D->EndDraw();
    }
    void D2DCanvas::BeginDraw(void* p, int w, int h)
    {
        if (m_pD2DContext2D == nullptr)
        {
            m_pD2DContext2D = new D2DContext2D((HWND) p);
        }

        RECT rc;
        GetClientRect((HWND) p, &rc);
        set_width((rc.right - rc.left));
        set_height((rc.bottom - rc.top));
        m_pD2DContext2D->BeginDraw(width, height);
    }
    double D2DCanvas::get_width() const
    {
        return m_width;
    }
    void D2DCanvas::set_width(double v)
    {
        m_width = v;
    }
    double D2DCanvas::get_height() const
    {
        return m_height;
    }
    void D2DCanvas::set_height(double v)
    {
        m_height  = v;
    }
    CanvasImp* CreateCanvas(void*)
    {
        return new D2DCanvas();
    }
    ////////
} //namespace CanvasPlus

#endif //USING_WIN_D2D
