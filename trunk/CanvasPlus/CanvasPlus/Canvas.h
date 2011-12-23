#pragma once

//Copyright (C) <2011> <copyright holders>
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of
//this software and associated documentation files (the "Software"), to deal in
//the Software without restriction, including without limitation the rights to
//use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
//of the Software, and to permit persons to whom the Software is furnished to do
//so, subject to the following conditions:

//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#include <string>


namespace CanvasPlus //Better name?
{
    //The possible values are start, end, left, right, and center.
    enum TextAlignEnum
    {
        TextAlignStart,
        TextAlignEnd,
        TextAlignLeft,
        TextAlignRight,
        TextAlignCenter,
    };

    TextAlignEnum ParseTextAlign(const char* psz);

    
    struct TextAlign
    {
      TextAlignEnum m_Value;

      TextAlign()
      {
         //"When the context is created, the textAlign attribute must
        //initially have the value start."
         m_Value = CanvasPlus::TextAlignStart;
      }

      TextAlign & operator = (const char* psz)
      {
        m_Value = ParseTextAlign(psz);
        return *this;
      }

      /*TextAlign & operator = (TextAlignEnum e)
      {
        m_Value = e;
        return *this;
      }*/

      operator TextAlignEnum() const
      {
        return m_Value;
      }
    };

    enum TextBaselineEnum
    {
        TextBaselineTop , //The top of the em square
        TextBaselineHanging, //The hanging baseline
        TextBaselineMiddle , //The middle of the em square
        TextBaselineAlphabetic ,//The alphabetic baseline
        TextBaselineIdeographic , //The ideographic baseline
        TextBaselineBottom  //The bottom of the em square
    };

    TextBaselineEnum ParseTextBaseline(const char* psz);
  

    struct TextBaseline
    {
      TextBaselineEnum m_Value;

      TextBaseline()
      {
        //"the textBaseline attribute must initially have the value alphabetic."
        m_Value = CanvasPlus::TextBaselineAlphabetic;
      }

      TextBaseline & operator = (const char* psz)
      {
        m_Value = ParseTextBaseline(psz);
        
        
        return *this;
      }

      /*TextBaseline & operator = (TextBaselineEnum e)
      {
        m_Value = e;
        return *this;
      }*/

      operator TextBaselineEnum() const
      {
        return m_Value;
      }
    };

    //TODO
    struct CanvasGradient
    {
        // opaque object
        //  void addColorStop(double offset, DOMString color);
    };

    //TODO
    struct CanvasPattern
    {
        // opaque object
    };

    struct Font
    {
      friend class CanvasRenderingContext2D;
      void* m_pNativeObject;

    public:
        Font();
        ~Font();

        bool italic;
        bool bold;
        std::wstring name;
    };

    //Local representation of color
    struct Color
    {
        int r;
        int g;
        int b;
        int a;
        Color()
        {
          r = 0;
          g = 0;
          b = 0;
          a = 0;
        }
        Color(const char*);
    };

    void ParserColor(const char* psz, Color&);

    struct TextMetrics
    {
        const double width;
        TextMetrics(double w) : width(w)
        {
        }
    };


    class FillStyle
    {

    public:
        //TODO variant type?
        Color m_Color;

        FillStyle()
        {
          
        }

        FillStyle& operator = (const char* color)
        {
            ParserColor(color, m_Color);
            return *this;
        }

        FillStyle& operator = (const CanvasGradient&)
        {
            //TODO
            return *this;
        }

        FillStyle& operator = (const CanvasPattern&)
        {
            //TODO
            return *this;
        }
    };

    //http://dev.w3.org/html5/2dcontext/
    class CanvasRenderingContext2D
    {
        friend class Canvas;
        void* m_pNativeHandle;
        CanvasRenderingContext2D(void*);

        int ToPixelX(double);
        int ToPixelY(double);

    public:
        ~CanvasRenderingContext2D();

        // rects
        void fillRect(double x, double y, double w, double h);
        void strokeRect(double x, double y, double w, double h);

        //fillStyle
        FillStyle fillStyle;
        FillStyle strokeStyle;

        void fillText(const wchar_t*, double x, double y);
        

        TextAlign textAlign;
        TextBaseline textBaseline;
        Font font;
        TextMetrics measureText(const wchar_t*);
        double lineWidth;

        void moveTo(double x, double y);
        void lineTo(double x, double y);

    };

    class Canvas
    {
        CanvasRenderingContext2D m_CanvasRenderingContext2D;
    public:
        Canvas(void*);
        ~Canvas();
        CanvasRenderingContext2D& getContext(const char*);
    };
}
