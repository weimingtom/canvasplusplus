

#ifdef USING_WIN_D2D

#include <Windows.h>
#include <cassert>
#include "..\Canvas.h"
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <vector>

enum Comands
{
    CMD_PATH_BEGIN,
    CMD_LINE_TO,
    CMD_RECT ,
};

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

        // DirectWrite
        IDWriteFactory* m_pDWriteFactory;
        IDWriteTextFormat* m_pTextFormat;

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

        std::vector<FLOAT> m_pathcmds;

    public:
        D2DContext2D(HWND hwnd) :
            m_pDirect2dFactory(NULL),
            m_pRenderTarget(NULL),
            m_pFillBrush(NULL),
            m_pStrokeBrush(NULL),
            m_pTextFormat(NULL),
            m_pDWriteFactory(NULL),
            m_strokeWidth(1.1f),
            m_pStrokeStyle(NULL)

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

            if (SUCCEEDED(hr))
            {
                hr = m_pDWriteFactory->CreateTextFormat(
                         L"Arial",                // Font family name.
                         NULL,                       // Font collection (NULL sets it to use the system font collection).
                         DWRITE_FONT_WEIGHT_REGULAR,
                         DWRITE_FONT_STYLE_NORMAL,
                         DWRITE_FONT_STRETCH_NORMAL,
                         14.0f,
                         L"en-us",
                         &m_pTextFormat
                     );
            }

            // Center align (horizontally) the text.
            if (SUCCEEDED(hr))
            {
                hr = m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            }

            if (SUCCEEDED(hr))
            {
                hr = m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
            }

            return hr;
        }

        void BeginDraw(int width, int height)
        {
            m_pRenderTarget->Resize(D2D1::SizeU(width, height));
            m_pRenderTarget->BeginDraw();
            m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
            m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
            m_strokeWidth = 1.1f;
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
        virtual const FillStyle& get_fillStyle()const ;

        virtual void set_strokeStyle(const CanvasGradient&);
        virtual void set_strokeStyle(const Color&);
        virtual void set_strokeStyle(const char*);
        virtual void set_strokeStyle(const FillStyle&);
        virtual const FillStyle& get_strokeStyle()const ;

        virtual void set_textAlign(const char*);
        virtual void set_textAlign(const TextAlign&);
        virtual const TextAlign& get_textAlign()const ;

        virtual void set_textBaseline(const char*);
        virtual void set_textBaseline(const TextBaseline&);
        virtual const TextBaseline& get_textBaseline()const ;

        virtual void set_font(const char*);
        virtual void set_font(const Font&);
        virtual const Font& get_font()const ;

        virtual void set_lineWidth(double);
        virtual double get_lineWidth()const ;

        virtual void set_shadowColor(const Color&);
        virtual const Color&  get_shadowColor()const ;

        virtual void set_shadowOffsetX(double);
        virtual double get_shadowOffsetX()const ;

        virtual void set_shadowOffsetY(double);
        virtual double get_shadowOffsetY()const ;

        virtual void set_shadowBlur(double);
        virtual double get_shadowBlur()const ;
        //
    };

    struct CanvasGradientImp
    {
      //D2D1_GRADIENT_STOP gradientStops
    };

    void CanvasGradient::addColorStop(double offset, const Color& color)
    {
      //D2D1_GRADIENT_STOP gradientStops
    }

    CanvasGradient::CanvasGradient(CanvasGradientImp* p)
    {
    }

    CanvasGradient& CanvasGradient::operator=(const CanvasGradient& other)
    {
        return *this;
    }

    CanvasGradient::CanvasGradient(const CanvasGradient& other)
    {
    }

    CanvasGradient::~CanvasGradient()
    {
    }

    CanvasGradient D2DContext2D::createLinearGradient(double x0,
            double y0,
            double x1,
            double y1)
    {
      /*
      if (SUCCEEDED(hr))
{
    hr = m_pRenderTarget->CreateLinearGradientBrush(
        D2D1::LinearGradientBrushProperties(
            D2D1::Point2F(0, 0),
            D2D1::Point2F(150, 150)),
        pGradientStops,
        &m_pLinearGradientBrush
        );
}
      */
        return CanvasGradient();
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
    }
    ///////////////////////////////////////////////////////////////////////////////
    // The fillRect(x, y, w, h) method must paint the specified rectangular area
    //  using the fillStyle. If either height or width are zero, this method
    //  has no effect.
    void D2DContext2D::fillRect(double x, double y, double w, double h)
    {
        D2D1_RECT_F rc = D2D1::RectF(x, y, x + w, y + h);
        m_pRenderTarget->FillRectangle(&rc, m_pFillBrush);
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
        m_pathcmds.push_back(CMD_PATH_BEGIN);
        m_pathcmds.push_back(x);
        m_pathcmds.push_back(y);
    }

    void D2DContext2D::lineTo(double x, double y)
    {
        m_pathcmds.push_back(CMD_LINE_TO);
        m_pathcmds.push_back(x);
        m_pathcmds.push_back(y);
    }

    void D2DContext2D::beginPath()
    {
        m_pathcmds.clear();
    }

    void D2DContext2D::closePath()
    {
        //m_pathcmds.push_back(CMD_PATH_BEGIN);
        //m_pathcmds.push_back(x);
        //m_pathcmds.push_back(y);
    }

    void D2DContext2D::rect(double x, double y, double w, double h)
    {
        m_pathcmds.push_back(CMD_RECT);
        m_pathcmds.push_back(x);
        m_pathcmds.push_back(y);
        m_pathcmds.push_back(w);
        m_pathcmds.push_back(h);
    }
    void D2DContext2D::clip()
    {
        //TODO
    }

    void D2DContext2D::fill()
    {
        if (m_pathcmds.size() < 2)
        {
            return;
        }

        ID2D1GeometrySink* pSink = NULL;
        std::vector<FLOAT>::iterator it = m_pathcmds.begin();
        HRESULT hr = E_FAIL;
        ID2D1PathGeometry* pPathGeometry = NULL;

        for (; it != m_pathcmds.end();)
        {
            Comands cmd = (Comands)(int)(*it);
            ++it;

            if (cmd == CMD_PATH_BEGIN)
            {
                if (pSink)
                {
                    pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
                    hr = pSink->Close();
                    m_pRenderTarget->FillGeometry(pPathGeometry, m_pFillBrush);
                    SafeRelease(&pSink);
                    SafeRelease(&pPathGeometry);
                    hr = m_pDirect2dFactory->CreatePathGeometry(&pPathGeometry);
                    hr = pPathGeometry->Open(&pSink);
                }
                else
                {
                    SafeRelease(&pPathGeometry);
                    hr = m_pDirect2dFactory->CreatePathGeometry(&pPathGeometry);
                    hr = pPathGeometry->Open(&pSink);
                }

                pSink->BeginFigure(D2D1::Point2F(*(it + 0), *(it + 1)),
                                   D2D1_FIGURE_BEGIN_FILLED);
                it += 2;
            }
            else if (cmd == CMD_LINE_TO)
            {
                pSink->AddLine(D2D1::Point2F(*(it + 0),  *(it + 1)));
                it += 2;
            }
        }

        pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
        pSink->Close();
        m_pRenderTarget->FillGeometry(pPathGeometry, m_pFillBrush, NULL);
        SafeRelease(&pSink);
        SafeRelease(&pPathGeometry);
    }

    void D2DContext2D::stroke()
    {
        if (m_pathcmds.size() < 2)
        {
            return;
        }

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
        ID2D1GeometrySink* pSink = NULL;
        std::vector<FLOAT>::iterator it = m_pathcmds.begin();
        ID2D1PathGeometry* pPathGeometry = NULL;

        for (; it != m_pathcmds.end();)
        {
            Comands cmd = (Comands)(int)(*it);
            ++it;

            if (cmd == CMD_PATH_BEGIN)
            {
                if (pSink)
                {
                    pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
                    hr = pSink->Close();
                    SafeRelease(&pSink);
                    m_pRenderTarget->DrawGeometry(pPathGeometry, m_pStrokeBrush, m_strokeWidth, strokeStyle);
                    SafeRelease(&pPathGeometry);
                    hr = m_pDirect2dFactory->CreatePathGeometry(&pPathGeometry);
                    hr = pPathGeometry->Open(&pSink);
                }
                else
                {
                    SafeRelease(&pPathGeometry);
                    hr = m_pDirect2dFactory->CreatePathGeometry(&pPathGeometry);
                    hr = pPathGeometry->Open(&pSink);
                }

                FLOAT x = *it++;
                FLOAT y = *it++;
                pSink->BeginFigure(D2D1::Point2F(x, y),
                                   D2D1_FIGURE_BEGIN_FILLED);
            }
            else if (cmd == CMD_LINE_TO)
            {
                FLOAT x = *it++;
                FLOAT y = *it++;
                pSink->AddLine(D2D1::Point2F(x, y));
            }
        }

        pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
        pSink->Close();
        m_pRenderTarget->DrawGeometry(pPathGeometry, m_pStrokeBrush, m_strokeWidth, strokeStyle);
        SafeRelease(&pSink);
        SafeRelease(&pPathGeometry);
    }

    void D2DContext2D::fillText(const wchar_t* psz, double x, double y)
    {
        // Create a D2D rect that is the same size as the window.
        D2D1_RECT_F layoutRect = D2D1::RectF(x, y, x + 10000, y + 10000);
        // Use the DrawText method of the D2D render target interface to draw.
        m_pRenderTarget->DrawText(
            psz,        // The string to render.
            wcslen(psz),    // The string's length.
            m_pTextFormat,    // The text format.
            layoutRect,       // The region of the window where the text will be rendered.
            m_pFillBrush// The brush used to draw the text.
        );
    }

    void D2DContext2D::set_fillStyle(const CanvasGradient& v)
    {
    }

    void D2DContext2D::set_fillStyle(const Color& v)
    {
        SafeRelease(&m_pStrokeBrush);
        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black),
                                               &m_pStrokeBrush);
    }

    void D2DContext2D::set_fillStyle(const char* v)
    {
        Color c(v);
        D2D1::ColorF d2dcolor(RGB(c.b, c.g, c.r));
        SafeRelease(&m_pFillBrush);
        HRESULT hr = m_pRenderTarget->CreateSolidColorBrush(d2dcolor, &m_pFillBrush);
    }

    void D2DContext2D::set_fillStyle(const FillStyle& v)
    {
    }

    const FillStyle& D2DContext2D::get_fillStyle()const
    {
        return FillStyle();
    }

    void D2DContext2D::set_strokeStyle(const CanvasGradient& v)
    {
    }

    void D2DContext2D::set_strokeStyle(const Color& v)
    {
    }

    void D2DContext2D::set_strokeStyle(const char* v)
    {
        Color c(v);
        D2D1::ColorF d2dcolor(RGB(c.b, c.g, c.r));
        SafeRelease(&m_pStrokeBrush);
        HRESULT hr = m_pRenderTarget->CreateSolidColorBrush(d2dcolor, &m_pStrokeBrush);
    }

    void D2DContext2D::set_strokeStyle(const FillStyle& v)
    {
    }

    const FillStyle& D2DContext2D::get_strokeStyle()const
    {
        return FillStyle();
    }

    void D2DContext2D::set_textAlign(const char* v)
    {
    }

    void D2DContext2D::set_textAlign(const TextAlign& v)
    {
    }

    const TextAlign& D2DContext2D::get_textAlign()const
    {
        return TextAlign();
    }

    void D2DContext2D::set_textBaseline(const char* v)
    {
    }

    void D2DContext2D::set_textBaseline(const TextBaseline& v)
    {
    }

    const TextBaseline& D2DContext2D::get_textBaseline()const
    {
        return TextBaseline();
    }

    void D2DContext2D::set_font(const char* v)
    {
    }

    void D2DContext2D::set_font(const Font& v)
    {
    }

    const Font& D2DContext2D::get_font()const
    {
        return Font();
    }

    void D2DContext2D::set_lineWidth(double v)
    {
        m_strokeWidth = v;
    }

    double D2DContext2D::get_lineWidth()const
    {
        return m_strokeWidth;
    }

    void D2DContext2D::set_shadowColor(const Color& v)
    {
    }

    const Color&  D2DContext2D::get_shadowColor()const
    {
        return Color();
    }

    void D2DContext2D::set_shadowOffsetX(double v)
    {
    }

    double D2DContext2D::get_shadowOffsetX()const
    {
        return 0;
    }

    void D2DContext2D::set_shadowOffsetY(double v)
    {
    }

    double D2DContext2D::get_shadowOffsetY()const
    {
        return 0;
    }

    void D2DContext2D::set_shadowBlur(double v)
    {
    }

    double D2DContext2D::get_shadowBlur()const
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
