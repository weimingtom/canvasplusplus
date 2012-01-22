//
//  Mac_Canvas.m
//  MacCanvasDemo
//
//  Created by Thiago Adams on 12/24/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//
#include "Canvas.h"
#include <string>
#include <cassert>
#include <sstream>

inline std::wstring widen( const std::string& str )
{
    std::wostringstream wstm ;
    const std::ctype<wchar_t>& ctfacet = 
    std::use_facet< std::ctype<wchar_t> >( wstm.getloc() ) ;
    for( size_t i=0 ; i<str.size() ; ++i ) 
        wstm << ctfacet.widen( str[i] ) ;
    return wstm.str() ;
}

inline std::string narrow( const std::wstring& str )
{
    std::ostringstream stm ;
    const std::ctype<char>& ctfacet = 
    std::use_facet< std::ctype<char> >( stm.getloc() ) ;
    for( size_t i=0 ; i<str.size() ; ++i ) 
        stm << ctfacet.narrow( str[i], 0 ) ;
    return stm.str() ;
}

namespace CanvasPlus
{
    //
    //
    class CGContext2D : public Context2D
    {
        CGContextRef m_Context;
        
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
        
        void UpdateShadow();
        
    public:
        CGContext2D(CGContextRef ctx)
        {
           m_Context = ctx;    
        }
        
        void BeginDraw(int width, int height)
        {
            CGContextSelectFont(m_Context, "Arial", 14.0, kCGEncodingMacRoman);
            set_font("10px sans-serif");
            set_textAlign("start");
            set_textBaseline("alphabetic");
            set_lineWidth(1);           // (default 1)
            set_shadowColor("#000000");          // (default transparent black)
            set_shadowOffsetX(0);       // (default 0)
            set_shadowOffsetY(0);       // (default 0)
            set_shadowBlur(0);          // (default 0)
        }
                
        void EndDraw()
        {        
        }
        
        ~CGContext2D()
        {
            EndDraw();        
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
        CGGradientRef m_CGGradientRef;
        CGPoint m_startPoint;
        CGPoint m_endPoint;
        CGGradientDrawingOptions m_options;
        std::vector<double> m_locations;
        std::vector<double> m_colorcomponents;
        bool m_dirty;
        unsigned int m_refcount;
        
        void AddRef()
        {
            m_refcount++;
        }
        
        void Release()
        {
          m_refcount--;
          if (m_refcount == 0)
          {
              delete this;
          }
        }
        
        CanvasGradientImp(double x0, double y0, double x1, double y1)
        {
            m_refcount  =1;
            m_dirty = true;
            
            m_startPoint.x = x0;
            m_startPoint.y = y0;
            m_endPoint.x = x1;
            m_endPoint.y = y1;
        }
        
        void addColorStop(double offset, const Color& color)
        {
            m_dirty  =true;
            
            m_locations.push_back(offset);
            
            float redValue = color.r / 255.0;
            float greenValue = color.g / 255.0;
            float blueValue = color.b/ 255.0;        
            
            m_colorcomponents.push_back(redValue);
            m_colorcomponents.push_back(greenValue);
            m_colorcomponents.push_back(blueValue);
            m_colorcomponents.push_back(1.0);
        }

        
        void Update()
        {
            if (m_dirty)
            {
            CGColorSpaceRef myColorspace=CGColorSpaceCreateDeviceRGB();
            m_CGGradientRef = 0;
            if (!m_locations.empty() && !m_colorcomponents.empty())
            {
              m_CGGradientRef = CGGradientCreateWithColorComponents(myColorspace,
                                                                    &m_colorcomponents[0],
                                                                    &m_locations[0],
                                                                    m_locations.size());
                m_dirty = false;
            }
            }
        }
    };
    
    CanvasGradient::CanvasGradient(double x0, double y0, double x1, double y1)
    {
        m_pCanvasGradientImp = new CanvasGradientImp(x0, y0, x1, y1); //get the ref 
    }
    
    void CanvasGradient::addColorStop(double offset, const Color& color)
    {
        if (m_pCanvasGradientImp != nullptr)
          m_pCanvasGradientImp->addColorStop(offset, color);
    }
    
    CanvasGradient& CanvasGradient::operator=(const CanvasGradient& other)
    {
        m_pCanvasGradientImp = other.m_pCanvasGradientImp;
        if (m_pCanvasGradientImp)
          m_pCanvasGradientImp->AddRef();
        return *this;
    }
    
    CanvasGradient::CanvasGradient(const CanvasGradient& other)
    {
        m_pCanvasGradientImp = other.m_pCanvasGradientImp;
        if (m_pCanvasGradientImp)
        m_pCanvasGradientImp->AddRef();
    }
    
    CanvasGradient::~CanvasGradient()
    {
        if (m_pCanvasGradientImp)
          m_pCanvasGradientImp->Release();
    }
    
    
    //font/////////////////////////////////////////////////////////////////////
    
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
    //font/////////////////////////////////////////////////////////////////////
    
    CanvasGradient CGContext2D::createLinearGradient(double x0,
                                                     double y0,
                                                     double x1,
                                                     double y1)
    {
        
        return CanvasGradient(x0, y0, x1, y1);
    }

    
    // state
    void CGContext2D::save()
    {
        CGContextSaveGState(m_Context);
    }
    
    void CGContext2D::restore()
    {
        CGContextRestoreGState(m_Context);
    }
    //
    ///////////////////////////////////////////////////////////////////////////
    // The clearRect(x, y, w, h) method must clear the pixels in the specified
    // rectangle that also intersect the current clipping region to a fully transparent
    // black, erasing any previous image. If either height or width are zero,
    // this method has no effect.
    void CGContext2D::clearRect(double x, double y, double w, double h)
    {
        CGRect rect;
        rect.origin.x = x;
        rect.origin.y = y;
        rect.size.width = w;
        rect.size.height = h;

        CGContextClearRect(m_Context, rect);
    }
    ///////////////////////////////////////////////////////////////////////////////
    // The fillRect(x, y, w, h) method must paint the specified rectangular area
    //  using the fillStyle. If either height or width are zero, this method
    //  has no effect.
    void CGContext2D::fillRect(double x, double y, double w, double h)
    {
        CGRect rect;
        rect.origin.x = x;
        rect.origin.y = y;
        rect.size.width = w;
        rect.size.height = h;

        
        if (fillStyle.IsUsingGradient())
        {
            CGContextSaveGState(m_Context);
            CGContextAddRect(m_Context, rect);
            CGContextClip(m_Context);
            fillStyle.canvasGradient.m_pCanvasGradientImp->Update();
            CGContextDrawLinearGradient (m_Context,
                                         fillStyle.canvasGradient.m_pCanvasGradientImp->m_CGGradientRef,
                                         fillStyle.canvasGradient.m_pCanvasGradientImp->m_startPoint,
                                         fillStyle.canvasGradient.m_pCanvasGradientImp->m_endPoint,
                                         0);
            CGContextRestoreGState(m_Context);
            return;
        }
        

        CGContextFillRect(m_Context, rect);
    }
    
    void CGContext2D::strokeRect(double x, double y, double w, double h)
    {
        CGRect rect;
        rect.origin.x = x;
        rect.origin.y = y;
        rect.size.width = w;
        rect.size.height = h;
        CGContextStrokeRect(m_Context, rect);
    }
    
    TextMetrics CGContext2D::measureText(const wchar_t* psz)
    {
        //    CGContextSelectFont(context, "Arial", 14.0, kCGEncodingMacRoman);
        
        std::wstring ws(psz);
        std::string s = narrow(ws);
        //assert(s.size() >= slen);
        
        CGPoint startpt = CGContextGetTextPosition (m_Context);
        CGContextSetTextDrawingMode(m_Context, kCGTextInvisible); 
        CGContextShowText (m_Context, s.c_str(), s.size());
        CGPoint endpt = CGContextGetTextPosition (m_Context);
        CGContextSetTextDrawingMode(m_Context, kCGTextFill); 
        return TextMetrics(endpt.x - startpt.x);

    }
    
    void CGContext2D::moveTo(double x, double y)
    {
        CGContextMoveToPoint(m_Context,x,y);
    }
    
    void CGContext2D::lineTo(double x, double y)
    {
          CGContextAddLineToPoint(m_Context,x,y);
    }
    
    void CGContext2D::beginPath()
    {
        CGContextBeginPath(m_Context);
    }
    
    void CGContext2D::closePath()
    {
       CGContextClosePath(m_Context);
    }
    
    void CGContext2D::rect(double x, double y, double w, double h)
    {        
        CGRect rect;
        rect.origin.x = x;
        rect.origin.y = y;
        rect.size.width = w;
        rect.size.height = h;

        CGContextAddRect(m_Context, rect);
    }
    
    void CGContext2D::clip()
    {
        CGContextClip(m_Context);
    }
    
    void CGContext2D::fill()
    {
        CGContextFillPath(m_Context);
    }
    
    void CGContext2D::stroke()
    {
       CGContextStrokePath(m_Context);
    }
    
    
    static void TextOut(CGContextRef ctx,
                        TextAlign textAlign,
                        TextBaseline baseLine,
                        const std::string& s,
                        int ix,
                        int iy)
    {
     
        //text witdh?
        CGPoint startpt = CGContextGetTextPosition (ctx);
        CGContextSetTextDrawingMode(ctx, kCGTextInvisible); 
        CGContextShowText (ctx, s.c_str(), s.size());
        CGPoint endpt = CGContextGetTextPosition (ctx);
        CGContextSetTextDrawingMode(ctx, kCGTextFill); 
        int cx = (endpt.x - startpt.x);
        //
        
        int newx = ix;
        int newy = iy;
        
        switch (textAlign)
        {
            case TextAlignStart:
                newx = ix;
                break;
                
            case TextAlignEnd:
                newx = ix - cx;
                break;
                
            case TextAlignLeft:
                newx = ix;
                break;
                
            case TextAlignRight:
                newx = ix - cx;
                break;
                
            case TextAlignCenter:
                newx = ix - cx / 2;
                break;
                
            default:
                assert(false);
        }
        
        
        NSFont *theFont = [NSFont fontWithName:@"Arial" size:14.0];
        int h = [theFont ascender];
        int tmDescent = [theFont descender];
        int tmHeight = 0;
        int cy = 14;
        
        switch (baseLine)
        {
            case TextBaselineTop :
                newy = iy + h;
                break;//The top of the em square
                
            case TextBaselineHanging:
                newy = iy + h;
                break; //The hanging baseline
                
            case TextBaselineMiddle :
                newy = (iy + h) - cy / 2;
                break; //The middle of the em square
                
            case TextBaselineAlphabetic :
            {
                newy = (iy + h) - (tmHeight - tmDescent);
            }
                break;//The alphabetic baseline
                
            case TextBaselineIdeographic :
            {
                newy = (iy + h) - (tmHeight);
            }
                break; //The ideographic baseline
                
            case TextBaselineBottom :
                newy = (iy + h) - cy;
                break;//The bottom of the em square
                
            default:
                break;
        }
        
 
        
        CGContextShowTextAtPoint(ctx, newx, newy, s.c_str(), s.size());
    }
    
    void CGContext2D::fillText(const wchar_t* psz, double x, double y)
    {
        
        CGContextSetTextDrawingMode(m_Context, kCGTextFill);
        
        std::wstring ws(psz);
        std::string s = narrow(ws);
        
        // assert(s.size() >= slen);
        
        CGAffineTransform xform = CGAffineTransformMake(
                                                        1.0,  0.0,
                                                        0.0, -1.0,
                                                        0.0,  0.0);
        CGContextSetTextMatrix(m_Context, xform);
        
        TextOut(m_Context,textAlign, textBaseline,s,  x, y);
      

    }
    
    void CGContext2D::set_fillStyle(const CanvasGradient& v)
    {
        fillStyle = v;
    }
    
    void CGContext2D::set_fillStyle(const Color& v)
    {
        fillStyle = v;     

        float redValue = fillStyle.m_Color.r / 255.0;
        float greenValue = fillStyle.m_Color.g / 255.0;
        float blueValue = fillStyle.m_Color.b/ 255.0;        
        CGContextSetRGBFillColor(m_Context, redValue, greenValue, blueValue, 1);
    }
    
    void CGContext2D::set_fillStyle(const char* v)
    {
       fillStyle = v;
        
        float redValue = fillStyle.m_Color.r / 255.0;
        float greenValue = fillStyle.m_Color.g / 255.0;
        float blueValue = fillStyle.m_Color.b/ 255.0;        
        CGContextSetRGBFillColor(m_Context, redValue, greenValue, blueValue, 1);
    }
    
    void CGContext2D::set_fillStyle(const FillStyle& v)
    {
        fillStyle = v;
        float redValue = fillStyle.m_Color.r / 255.0;
        float greenValue = fillStyle.m_Color.g / 255.0;
        float blueValue = fillStyle.m_Color.b/ 255.0;        
        CGContextSetRGBFillColor(m_Context, redValue, greenValue, blueValue, 1);
    }
    
    const FillStyle& CGContext2D::get_fillStyle()const
    {
        return fillStyle;
    }
    
    void CGContext2D::set_strokeStyle(const CanvasGradient& v)
    {
        strokeStyle = v;

        float redValue = strokeStyle.m_Color.r / 255.0;
        float greenValue = strokeStyle.m_Color.g / 255.0;
        float blueValue = strokeStyle.m_Color.b/ 255.0;        
        CGContextSetRGBStrokeColor(m_Context, redValue, greenValue, blueValue, 1);
    }
    
    void CGContext2D::set_strokeStyle(const Color& v)
    {
        strokeStyle = v;
        float redValue = strokeStyle.m_Color.r / 255.0;
        float greenValue = strokeStyle.m_Color.g / 255.0;
        float blueValue = strokeStyle.m_Color.b/ 255.0;        
        CGContextSetRGBStrokeColor(m_Context, redValue, greenValue, blueValue, 1);

    }
    
    void CGContext2D::set_strokeStyle(const char* v)
    {
        strokeStyle = v;
        float redValue = strokeStyle.m_Color.r / 255.0;
        float greenValue = strokeStyle.m_Color.g / 255.0;
        float blueValue = strokeStyle.m_Color.b/ 255.0;        
        CGContextSetRGBStrokeColor(m_Context, redValue, greenValue, blueValue, 1);

    }
    
    void CGContext2D::set_strokeStyle(const FillStyle& v)
    {
        strokeStyle = v;
        float redValue = strokeStyle.m_Color.r / 255.0;
        float greenValue = strokeStyle.m_Color.g / 255.0;
        float blueValue = strokeStyle.m_Color.b/ 255.0;        
        CGContextSetRGBStrokeColor(m_Context, redValue, greenValue, blueValue, 1);

    }
    
    const FillStyle& CGContext2D::get_strokeStyle()const
    {
        return strokeStyle;
    }
    
    void CGContext2D::set_textAlign(const char* v)
    {
        textAlign = ParseTextAlign(v);
        //CGTextDrawingMode
    }
    
    void CGContext2D::set_textAlign(const TextAlign& v)
    {
       textAlign = v;
    }
    
    const TextAlign& CGContext2D::get_textAlign()const
    {
        return textAlign;
    }
    
    void CGContext2D::set_textBaseline(const char* v)
    {
        textBaseline = ParseTextBaseline(v);
    }
    
    void CGContext2D::set_textBaseline(const TextBaseline& v)
    {
        textBaseline = v;
    }
    
    const TextBaseline& CGContext2D::get_textBaseline()const
    {
        return textBaseline;
    }
    
    void CGContext2D::set_font(const char* v)
    {
      //  Canvasplus::Font f;
//        f = v;
        
                CGContextSelectFont(m_Context, "Arial", 14.0, kCGEncodingMacRoman);
    }
    
    void CGContext2D::set_font(const Font& v)
    {
    }
    
    const Font& CGContext2D::get_font()const
    {
        return Font();
    }
    
    void CGContext2D::set_lineWidth(double v)
    {
        lineWidth = v;
        CGContextSetLineWidth(m_Context, lineWidth);
    }
    
    double CGContext2D::get_lineWidth()const
    {
        return lineWidth;
    }
    
    void CGContext2D::set_shadowColor(const Color& v)
    {
    }
    
    const Color&  CGContext2D::get_shadowColor()const
    {
        return Color();
    }
    
    void CGContext2D::UpdateShadow()
    {
        if (shadowOffsetX == 0 && shadowOffsetY == 0)
        {
            CGSize offset;

            CGContextSetShadowWithColor(m_Context, offset, 0, NULL);
            return;
        }
        
        CGSize offset;
        offset.width  = shadowOffsetX;
        offset.height = -shadowOffsetY;
        
        CGColorSpaceRef myColorspace=CGColorSpaceCreateDeviceRGB();
        
        CGFloat components[] = { 0, 0 , 0, 1};
        CGColorRef color = CGColorCreate(myColorspace, components);
        
        CGContextSetShadowWithColor(m_Context, offset, shadowBlur, color);
    
    }
    
    void CGContext2D::set_shadowOffsetX(double v)
    {
        shadowOffsetX = v;
        UpdateShadow();
     }
    
    double CGContext2D::get_shadowOffsetX()const
    {
        return shadowOffsetX;
    }
    
    void CGContext2D::set_shadowOffsetY(double v)
    {
        shadowOffsetY = v;
        UpdateShadow();
    }
    
    double CGContext2D::get_shadowOffsetY()const
    {
        return shadowOffsetY;
    }
    
    void CGContext2D::set_shadowBlur(double v)
    {
        shadowBlur = v;
                UpdateShadow();
    }
    
    double CGContext2D::get_shadowBlur()const
    {
        return shadowBlur;
    }
    
    
    class CGContextCanvas : public CanvasImp
    {
        CanvasPlus::CGContext2D* m_pCGContext2D;
        int m_width;
        int m_height;
        
        ~CGContextCanvas();
        Context2D& getContext(const char*);
        void BeginDraw(void* p, int w, int h);
        void EndDraw();
        
        double get_width() const;
        void set_width(double v);        
        double get_height() const;        
        void set_height(double v);
        
    public:
        CGContextCanvas();
        
    };
    
    CGContextCanvas::CGContextCanvas()
    {
        m_pCGContext2D = nullptr;
    }
    
    CGContextCanvas::~CGContextCanvas()
    {
        delete m_pCGContext2D;
    }
    
    Context2D& CGContextCanvas::getContext(const char*)
    {
        return *m_pCGContext2D;
    }
    
    void CGContextCanvas::EndDraw()
    {
        m_pCGContext2D->EndDraw();
    }
    
    void CGContextCanvas::BeginDraw(void* p, int w, int h)
    {
        if (m_pCGContext2D == nullptr)
        {
            m_pCGContext2D = new CGContext2D((CGContextRef) p);
        }
        
        //RECT rc;
        //G/etClientRect((HWND) p, &rc);
        //set_width((rc.right - rc.left));
        //set_height((rc.bottom - rc.top));
        m_pCGContext2D->BeginDraw(width, height);
    }
    
    double CGContextCanvas::get_width() const
    {
        return m_width;
    }
    
    void CGContextCanvas::set_width(double v)
    {
        m_width = v;
    }
    
    double CGContextCanvas::get_height() const
    {
        return m_height;
    }
    
    void CGContextCanvas::set_height(double v)
    {
        m_height  = v;
    }
    
    CanvasImp* CreateCanvas(void*)
    {
        return new CGContextCanvas();
    }
    
    ////////
} //namespace CanvasPlus

