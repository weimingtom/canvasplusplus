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
        //fillColor
        float redValue = fillStyle.m_Color.r / 255.0;
        float greenValue = fillStyle.m_Color.g / 255.0;
        float blueValue = fillStyle.m_Color.b/ 255.0;
        
        
        NSColor *nscolor = [NSColor colorWithCalibratedRed:redValue green:greenValue blue:blueValue alpha:1.0f];
        
        NSRect nsrect;
        nsrect.origin.x = left;
        nsrect.origin.y = top;
        nsrect.size.width = w;
        nsrect.size.height = h;
        
        
        [nscolor setFill];
        NSBezierPath* drawingPath = [NSBezierPath bezierPath];
        
        [drawingPath appendBezierPathWithRect: nsrect];
        
            
        // and fill it
        [drawingPath fill];

    }
    
    void Context2D::strokeRect(double left, double top, double w, double h)
    {
        //fillColor
        float redValue = strokeStyle.m_Color.r / 255.0;
        float greenValue = strokeStyle.m_Color.g / 255.0;
        float blueValue = strokeStyle.m_Color.b/ 255.0;
        
        NSColor *nscolor = [NSColor colorWithCalibratedRed:redValue green:greenValue blue:blueValue alpha:1.0f];
        
        NSRect nsrect;
        nsrect.origin.x = left;
        nsrect.origin.y = top;
        nsrect.size.width = w;
        nsrect.size.height = h;
        
        
        [nscolor setStroke];
        NSBezierPath* drawingPath = [NSBezierPath bezierPath];
        
        [drawingPath appendBezierPathWithRect: nsrect];
        
        // and fill it
        [drawingPath stroke];
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