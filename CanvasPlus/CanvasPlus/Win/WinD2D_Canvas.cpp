

#ifdef USING_WIN_D2D

#include <Windows.h>
#include <cassert>
#include "..\Canvas.h"
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

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
    class CanvasStateInfo;
    //
    //
    class CanvasPlus::Context2D::Imp
    {
        ID2D1Factory* m_pDirect2dFactory;
        ID2D1HwndRenderTarget* m_pRenderTarget;
        ID2D1SolidColorBrush* m_pFillBrush;
        ID2D1SolidColorBrush* m_pStrokeBrush;

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
        Imp(HWND hwnd) :
            m_pDirect2dFactory(NULL),
            m_pRenderTarget(NULL),
            m_pFillBrush(NULL),
            m_pStrokeBrush(NULL)
        {
            RECT rc;
            GetClientRect(hwnd, &rc);
            int w = rc.right - rc.left;
            int h = rc.bottom - rc.top;
            HRESULT hr = CreateDeviceIndependentResources();
            // Because the CreateWindow function takes its size in pixels,
            // obtain the system DPI and use it to scale the window size.
            FLOAT dpiX, dpiY;
            // The factory returns the current system DPI. This is also the value it will use
            // to create its own windows.
            m_pDirect2dFactory->GetDesktopDpi(&dpiX, &dpiY);
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

                if (SUCCEEDED(hr))
                {
                    // Create a gray brush.
                    hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pFillBrush);
                }

                if (SUCCEEDED(hr))
                {
                    // Create a blue brush.
                    hr = m_pRenderTarget->CreateSolidColorBrush(
                             D2D1::ColorF(D2D1::ColorF::CornflowerBlue),
                             &m_pStrokeBrush
                         );
                }
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
            return hr;
        }

        void BeginDraw(int width, int height)
        {
            m_pRenderTarget->Resize(D2D1::SizeU(width, height));

            m_pRenderTarget->BeginDraw();
            m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
            m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
            
            
        }


        void EndDraw()
        {
            if (m_pRenderTarget)
              m_pRenderTarget->EndDraw();
        }

        ~Imp()
        {
            EndDraw();
            SafeRelease(&m_pDirect2dFactory);
            SafeRelease(&m_pRenderTarget);
            SafeRelease(&m_pFillBrush);
            SafeRelease(&m_pStrokeBrush);
        }



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


    void CanvasGradient::addColorStop(double offset, const Color& color)
    {
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

    CanvasGradient Context2D::Imp::createLinearGradient(double x0,
            double y0,
            double x1,
            double y1)
    {
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

    Canvas::Canvas()
    {
    }

    Canvas::~Canvas()
    {
    }

    Context2D& Canvas::getContext(const char*)
    {
        return m_CanvasRenderingContext2D;
    }

    void Canvas::EndDraw()
    {
        m_CanvasRenderingContext2D.EndDraw();
    }

    void Canvas::BeginDraw(void* p, int w, int h)
    {
        RECT rc;
        GetClientRect((HWND) p, &rc);
        width = (rc.right - rc.left);
        height = (rc.bottom - rc.top);
        m_CanvasRenderingContext2D.BeginDraw(p);
    }


    // state
    void Context2D::Imp::save()
    {
    }

    void Context2D::Imp::restore()
    {
    }
    //
    ///////////////////////////////////////////////////////////////////////////
    // The clearRect(x, y, w, h) method must clear the pixels in the specified
    // rectangle that also intersect the current clipping region to a fully transparent
    // black, erasing any previous image. If either height or width are zero,
    // this method has no effect.
    void Context2D::Imp::clearRect(double x, double y, double w, double h)
    {
    }
    ///////////////////////////////////////////////////////////////////////////////
    // The fillRect(x, y, w, h) method must paint the specified rectangular area
    //  using the fillStyle. If either height or width are zero, this method
    //  has no effect.
    void Context2D::Imp::fillRect(double x, double y, double w, double h)
    {
        D2D1_RECT_F rc = D2D1::RectF(x, y, x + w, y + h);
        m_pRenderTarget->FillRectangle(&rc, m_pFillBrush);
    }

    void Context2D::Imp::strokeRect(double x, double y, double w, double h)
    {
    }


    TextMetrics Context2D::Imp::measureText(const wchar_t* psz)
    {
        return TextMetrics(1);
    }

    void Context2D::Imp::moveTo(double x, double y)
    {
    }

    void Context2D::Imp::lineTo(double x, double y)
    {
        //
    }

    void Context2D::Imp::beginPath()
    {
    }
    void Context2D::Imp::closePath()
    {
    }

    void Context2D::Imp::rect(double x, double y, double w, double h)
    {
    }
    void Context2D::Imp::clip()
    {
        //TODO
    }

    void Context2D::Imp::fill()
    {
    }

    void Context2D::Imp::stroke()
    {
    }
    void Context2D::Imp::fillText(const wchar_t* psz, double x, double y)
    {
    }

    void Context2D::Imp::set_fillStyle(const CanvasGradient& v)
    {
    }

    void Context2D::Imp::set_fillStyle(const Color& v)
    {
    }

    void Context2D::Imp::set_fillStyle(const char* v)
    {
        Color c(v);
        D2D1::ColorF d2dcolor(RGB(c.g, c.g, c.r));
        SafeRelease(&m_pFillBrush);
        HRESULT hr = m_pRenderTarget->CreateSolidColorBrush(d2dcolor, &m_pFillBrush);
    }

    void Context2D::Imp::set_fillStyle(const FillStyle& v)
    {
    }

    const FillStyle& Context2D::Imp::get_fillStyle() const
    {
        return FillStyle();
    }

    void Context2D::Imp::set_strokeStyle(const CanvasGradient& v)
    {
    }

    void Context2D::Imp::set_strokeStyle(const Color& v)
    {
    }

    void Context2D::Imp::set_strokeStyle(const char* v)
    {
    }

    void Context2D::Imp::set_strokeStyle(const FillStyle& v)
    {
    }

    const FillStyle& Context2D::Imp::get_strokeStyle() const
    {
        return FillStyle();
    }

    void Context2D::Imp::set_textAlign(const char* v)
    {
    }

    void Context2D::Imp::set_textAlign(const TextAlign& v)
    {
    }

    const TextAlign& Context2D::Imp::get_textAlign()const
    {
        return TextAlign();
    }

    void Context2D::Imp::set_textBaseline(const char* v)
    {
    }

    void Context2D::Imp::set_textBaseline(const TextBaseline& v)
    {
    }

    const TextBaseline& Context2D::Imp::get_textBaseline() const
    {
        return TextBaseline();
    }

    void Context2D::Imp::set_font(const char* v)
    {
    }

    void Context2D::Imp::set_font(const Font& v)
    {
    }

    const Font& Context2D::Imp::get_font() const
    {
        return Font();
    }

    void Context2D::Imp::set_lineWidth(double v)
    {
    }

    double Context2D::Imp::get_lineWidth() const
    {
        return 0;
    }

    void Context2D::Imp::set_shadowColor(const Color& v)
    {
    }

    const Color&  Context2D::Imp::get_shadowColor()const
    {
        return Color();
    }

    void Context2D::Imp::set_shadowOffsetX(double v)
    {
    }

    double Context2D::Imp::get_shadowOffsetX() const
    {
        return 0;
    }

    void Context2D::Imp::set_shadowOffsetY(double v)
    {
    }

    double Context2D::Imp::get_shadowOffsetY() const
    {
        return 0;
    }

    void Context2D::Imp::set_shadowBlur(double v)
    {
    }

    double Context2D::Imp::get_shadowBlur() const
    {
        return 0;
    }

    ////////
    ////////
    Context2D::Context2D()
    {
        m_pContext2DImp = nullptr;        
    }

    Context2D::~Context2D()
    {
        delete m_pContext2DImp;
    }

    // state
    void Context2D::save()
    {
        m_pContext2DImp->save();
    }

    void Context2D::restore()
    {
        m_pContext2DImp->restore();
    }

    void Context2D::clearRect(double x, double y, double w, double h)
    {
        m_pContext2DImp->clearRect(x, y, w, h);
    }

    void Context2D::fillRect(double x, double y, double w, double h)
    {
        m_pContext2DImp->fillRect(x, y, w, h);
    }

    void Context2D::strokeRect(double x, double y, double w, double h)
    {
        m_pContext2DImp->strokeRect(x, y, w, h);
    }

    void Context2D::fillText(const wchar_t* psz, double x, double y)
    {
        m_pContext2DImp->fillText(psz, x, y);
    }

    CanvasGradient Context2D::createLinearGradient(double x0, double y0, double x1, double y1)
    {
        return m_pContext2DImp->createLinearGradient(x0, y0, x1, y1);
    }

    TextMetrics Context2D::measureText(const wchar_t* psz)
    {
        return m_pContext2DImp->measureText(psz);
    }

    void Context2D::moveTo(double x, double y)
    {
        m_pContext2DImp->moveTo(x, y);
    }

    void Context2D::lineTo(double x, double y)
    {
        m_pContext2DImp->lineTo(x, y);
    }

    void Context2D::beginPath()
    {
        m_pContext2DImp->beginPath();
    }

    void Context2D::closePath()
    {
        m_pContext2DImp->closePath();
    }

    void Context2D::stroke()
    {
        m_pContext2DImp->stroke();
    }

    void Context2D::fill()
    {
        m_pContext2DImp->fill();
    }

    void Context2D::clip()
    {
        m_pContext2DImp->clip();
    }

    void Context2D::rect(double x, double y, double w, double h)
    {
        m_pContext2DImp->rect(x, y, w, h);
    }

    ///

    void Context2D::set_fillStyle(const CanvasGradient& v)
    {
        m_pContext2DImp->set_fillStyle(v);
    }

    void Context2D::set_fillStyle(const Color& v)
    {
        m_pContext2DImp->set_fillStyle(v);
    }

    void Context2D::set_fillStyle(const char* v)
    {
        m_pContext2DImp->set_fillStyle(v);
    }

    void Context2D::set_fillStyle(const FillStyle& v)
    {
        m_pContext2DImp->set_fillStyle(v);
    }

    const FillStyle& Context2D::get_fillStyle() const
    {
        return m_pContext2DImp->get_fillStyle();
    }

    void Context2D::set_strokeStyle(const CanvasGradient& v)
    {
        m_pContext2DImp->set_strokeStyle(v);
    }

    void Context2D::set_strokeStyle(const Color& v)
    {
        m_pContext2DImp->set_strokeStyle(v);
    }

    void Context2D::set_strokeStyle(const char* v)
    {
        m_pContext2DImp->set_strokeStyle(v);
    }

    void Context2D::set_strokeStyle(const FillStyle& v)
    {
        m_pContext2DImp->set_strokeStyle(v);
    }

    const FillStyle& Context2D::get_strokeStyle() const
    {
        return m_pContext2DImp->get_strokeStyle();
    }

    void Context2D::set_textAlign(const char* v)
    {
        m_pContext2DImp->set_textAlign(v);
    }

    void Context2D::set_textAlign(const TextAlign& v)
    {
        m_pContext2DImp->set_textAlign(v);
    }

    const TextAlign& Context2D::get_textAlign()const
    {
        return m_pContext2DImp->get_textAlign();
    }

    void Context2D::set_textBaseline(const char* v)
    {
        m_pContext2DImp->set_textBaseline(v);
    }

    void Context2D::set_textBaseline(const TextBaseline& v)
    {
        m_pContext2DImp->set_textBaseline(v);
    }

    const TextBaseline& Context2D::get_textBaseline() const
    {
        return m_pContext2DImp->get_textBaseline();
    }

    void Context2D::set_font(const char* v)
    {
        m_pContext2DImp->set_font(v);
    }

    void Context2D::set_font(const Font& v)
    {
        m_pContext2DImp->set_font(v);
    }

    const Font& Context2D::get_font() const
    {
        return m_pContext2DImp->get_font();
    }

    void Context2D::set_lineWidth(double v)
    {
        m_pContext2DImp->set_lineWidth(v);
    }

    double Context2D::get_lineWidth() const
    {
        return m_pContext2DImp->get_lineWidth();
    }

    void Context2D::set_shadowColor(const Color& v)
    {
        m_pContext2DImp->set_shadowColor(v);
    }

    const Color&  Context2D::get_shadowColor()const
    {
        return m_pContext2DImp->get_shadowColor();
    }

    void Context2D::set_shadowOffsetX(double v)
    {
        m_pContext2DImp->set_shadowOffsetX(v);
    }

    double Context2D::get_shadowOffsetX() const
    {
        return m_pContext2DImp->get_shadowOffsetX();
    }

    void Context2D::set_shadowOffsetY(double v)
    {
        m_pContext2DImp->set_shadowOffsetY(v);
    }

    double Context2D::get_shadowOffsetY() const
    {
        return m_pContext2DImp->get_shadowOffsetY();
    }

    void Context2D::set_shadowBlur(double v)
    {
        m_pContext2DImp->set_shadowBlur(v);
    }

    double Context2D::get_shadowBlur() const
    {
        return m_pContext2DImp->get_shadowBlur();
    }

    void Context2D::BeginDraw(void* p)
    {
        RECT rc;
        GetClientRect((HWND) p, &rc);
        int width = (rc.right - rc.left);
        int height = (rc.bottom - rc.top);

        if (m_pContext2DImp  == nullptr)
        {
            m_pContext2DImp = new CanvasPlus::Context2D::Imp((HWND)p);
        }

        
        m_pContext2DImp->BeginDraw(width, height);
    }
    void Context2D::EndDraw()
    {
        m_pContext2DImp->EndDraw();
    }


    ////////
} //namespace CanvasPlus

#endif //USING_WIN_D2D
