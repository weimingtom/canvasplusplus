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
    
    Font::Font()
    {
    }
    
    Font& Font::operator=(const char * psz)
    {
        return *this;
    }
    
    Font::~Font()
    {
    }
    
    
    struct CanvasGradientImp
    {
      void AddRef()
        {
        }
        void Release()
        {
        
        }
    };
    
    CanvasGradient::CanvasGradient(const CanvasGradient& other)
    {
        pCanvasGradientImp = other.pCanvasGradientImp;
        pCanvasGradientImp->AddRef();
    }
    
    
    CanvasGradient::CanvasGradient(CanvasGradientImp* p)
    {
        pCanvasGradientImp = p;
    }
    
    void CanvasGradient::addColorStop(double offset, const char *color)
    {
    }
    
    CanvasGradient::~CanvasGradient()
    {
        if(pCanvasGradientImp)
            pCanvasGradientImp->Release();
    }
    
    Context2D::Context2D(void*p)
    {
        m_pNativeHandle = p;
    }
    
    Context2D::~Context2D()
    {
    
    }
    
    void Context2D::fillText(const wchar_t* psz, double x, double y)
    {
        /////
        CGContextRef context = (CGContextRef) m_pNativeHandle;
        /////
        
        CGContextSelectFont(context, "Arial", 14.0, kCGEncodingMacRoman);
        
        CGContextSetTextDrawingMode(context, kCGTextFill);
        
        float redValue = fillStyle.m_Color.r / 255.0;
        float greenValue = fillStyle.m_Color.g / 255.0;
        float blueValue = fillStyle.m_Color.b/ 255.0;
        
        CGContextSetRGBFillColor(context, redValue, greenValue, blueValue, 1);
        
        
        std::wstring ws(psz);
        std::string s = narrow(ws);
        
       // assert(s.size() >= slen);
        
        CGAffineTransform xform = CGAffineTransformMake(
                                                        1.0,  0.0,
                                                        0.0, -1.0,
                                                        0.0,  0.0);
        CGContextSetTextMatrix(context, xform);
        
        NSFont *theFont = [NSFont fontWithName:@"Arial" size:14.0];
        
        int h= [theFont ascender];
        
        CGContextShowTextAtPoint(context, x, y + h, s.c_str(), s.size());
    }
    
    void Context2D::fillRect(double left, double top, double w, double h)
    {
        /////
        CGContextRef context = (CGContextRef) m_pNativeHandle;
        /////
        
        float redValue = fillStyle.m_Color.r / 255.0;
        float greenValue = fillStyle.m_Color.g / 255.0;
        float blueValue = fillStyle.m_Color.b/ 255.0;
        
        float alphaValue = 1.0;
        
        CGContextSetRGBFillColor (context,
                                  redValue,
                                  greenValue,
                                  blueValue,
                                  alphaValue);
        
        CGRect rect;
        rect.origin.x = left;
        rect.origin.y = top;
        rect.size.width = w;
        rect.size.height = h;

        CGContextFillRect(context, rect);
        
    }
    
    void Context2D::strokeRect(double left, double top, double w, double h)
    {
        /////
        CGContextRef context = (CGContextRef) m_pNativeHandle;
        /////
        float redValue = strokeStyle.m_Color.r / 255.0;
        float greenValue = strokeStyle.m_Color.g / 255.0;
        float blueValue = strokeStyle.m_Color.b/ 255.0;
        float alphaValue = 1.0;
        
        CGContextSetRGBStrokeColor(context,
                                  redValue,
                                  greenValue,
                                  blueValue,
                                  alphaValue);
        
        CGRect rect;
        rect.origin.x = left;
        rect.origin.y = top;
        rect.size.width = w;
        rect.size.height = h;
        
        CGContextStrokeRect(context, rect);
     }

    TextMetrics Context2D::measureText(const wchar_t* psz)
    {
        /////
        CGContextRef context = (CGContextRef) m_pNativeHandle;
        /////
    
    //    CGContextSelectFont(context, "Arial", 14.0, kCGEncodingMacRoman);
        
        std::wstring ws(psz);
        std::string s = narrow(ws);
        //assert(s.size() >= slen);
        
        CGPoint startpt = CGContextGetTextPosition (context);
        CGContextSetTextDrawingMode(context, kCGTextInvisible); 
        CGContextShowText (context, s.c_str(), s.size());
        CGPoint endpt = CGContextGetTextPosition (context);
        CGContextSetTextDrawingMode(context, kCGTextFill); 
        return TextMetrics(endpt.x - startpt.x);

    }
    
    CanvasGradient Context2D::createLinearGradient(double x0, double y0, double x1, double y1)
    {
        return CanvasGradient();
    }

    void Context2D::beginPath()
    {
        /////
        CGContextRef context = (CGContextRef) m_pNativeHandle;
        /////
        CGContextBeginPath(context);
    }
    
    void Context2D::closePath()
    {
        /////
        CGContextRef context = (CGContextRef) m_pNativeHandle;
        /////
        CGContextClosePath(context);
    }
    
    void Context2D::stroke()
    {
        /////
        CGContextRef context = (CGContextRef) m_pNativeHandle;
        /////
        CGContextStrokePath(context);
    }
    
    void Context2D::moveTo(double x, double y)
    {
        /////
        CGContextRef context = (CGContextRef) m_pNativeHandle;
        /////

        CGContextMoveToPoint(context, x,y);
    }
    
    void Context2D::lineTo(double x, double y)
    {
        /////
        CGContextRef context = (CGContextRef) m_pNativeHandle;
        /////
        
        CGContextAddLineToPoint(context, x,y);
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

} //namespace canvasplus