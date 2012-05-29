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
#include <vector>
#define nullptr 0


namespace CanvasPlus //Better name?
{
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
        
        bool operator == (const Color& other) const
        {
            return other.r == r && other.g == g && other.b == b;
        }
    };


    //The possible values are start, end, left, right, and center.
    enum TextAlign
    {
        TextAlignStart,
        TextAlignEnd,
        TextAlignLeft,
        TextAlignRight,
        TextAlignCenter,
    };

    enum TextBaseline
    {
        TextBaselineTop , //The top of the em square
        TextBaselineHanging, //The hanging baseline
        TextBaselineMiddle , //The middle of the em square
        TextBaselineAlphabetic ,//The alphabetic baseline
        TextBaselineIdeographic , //The ideographic baseline
        TextBaselineBottom  //The bottom of the em square
    };
    
    
    //TODO
    enum CanvasGradientType
    {
        CanvasGradientTypeLinear,
    };

    struct CanvasGradientImp;
    struct CanvasGradient
    {
        CanvasGradientImp* pCanvasGradientImp;

        CanvasGradient()
        {
            pCanvasGradientImp = nullptr; //error
        }

        CanvasGradient(const CanvasGradient&);
        CanvasGradient& operator=(const CanvasGradient&);

        CanvasGradient(CanvasGradientImp*);
        ~CanvasGradient();

        virtual void addColorStop(double offset, const Color& color);
    };

    //TODO
    struct CanvasPattern
    {
        // opaque object
    };

    struct Font
    {
        friend struct Context2D;
        void* m_pNativeObject;
        std::string fontdesc;
        void setFont(const char* psz);

    public:
        Font();
        Font(const Font&);
        ~Font();

        Font& operator = (const Font&);
        Font& operator = (const char*);
    };


    struct TextMetrics
    {
        const double width;
        TextMetrics(double w) : width(w)
        {
        }
        TextMetrics& operator=(const TextMetrics& other); //not imp
    };

    enum FillStyleEnum
    {
        FillStyleEnumSolid,
        FillStyleEnumGradient,
    };

    class FillStyle
    {
    public:
        FillStyleEnum fillStyleEnum;
        Color m_Color;
        CanvasGradient canvasGradient;

        FillStyle();
        FillStyle(const FillStyle&);
        FillStyle& operator = (const char*);
        FillStyle& operator = (const Color&);
        FillStyle& operator = (const FillStyle&);
        FillStyle& operator = (const CanvasGradient&);
        FillStyle& operator = (const CanvasPattern&);
        
        bool operator == (const char* psz) const
        {
            Color c(psz);
            return c == m_Color;
        }
    };

    // Store information on the stack
    class Context2D;
    struct CanvasStateInfo;

#define PROPERTY(CLASSTYPE, ATTR_TYPE, ATTR_NAME) \
   struct T##ATTR_NAME \
    { \
        private:\
        void operator = (const T##ATTR_NAME& ); \
        public:\
        template<class T> void operator=(T v) { \
        reinterpret_cast<CLASSTYPE*>(this - offsetof(CLASSTYPE, ATTR_NAME))->set_##ATTR_NAME(v);\
        }\
        operator ATTR_TYPE () {\
        return reinterpret_cast<CLASSTYPE*>(this - offsetof(CLASSTYPE, ATTR_NAME))->get_##ATTR_NAME();\
        }\
        template<class T>\
        bool operator ==(const T& v) {\
        return reinterpret_cast<CLASSTYPE*>(this - offsetof(CLASSTYPE, ATTR_NAME))->get_##ATTR_NAME() == v;\
        }\
    } ATTR_NAME


       
    //
    //
    //http://dev.w3.org/html5/2dcontext/
    //
    struct Context2D
    {
      virtual ~Context2D() {}

        virtual void set_fillStyle(const CanvasGradient&) = 0;
        virtual void set_fillStyle(const Color&) = 0;
        virtual void set_fillStyle(const char*) = 0;
        virtual void set_fillStyle(const FillStyle&) = 0;
        virtual const FillStyle& get_fillStyle() const = 0;

        virtual void set_strokeStyle(const CanvasGradient&) = 0;
        virtual void set_strokeStyle(const Color&) = 0;
        virtual void set_strokeStyle(const char*) = 0;
        virtual void set_strokeStyle(const FillStyle&) = 0;
        virtual const FillStyle& get_strokeStyle() const = 0;

        virtual void set_textAlign(const char*) = 0;
        virtual void set_textAlign(const TextAlign&) = 0;
        virtual const TextAlign& get_textAlign() const = 0;

        virtual void set_textBaseline(const char*) = 0;
        virtual void set_textBaseline(const TextBaseline&) = 0;
        virtual const TextBaseline& get_textBaseline() const = 0;

        virtual void set_font(const char*) = 0;
        virtual void set_font(const Font&) = 0;
        virtual const Font& get_font() const = 0;

        virtual void set_lineWidth(double) = 0;
        virtual double get_lineWidth() const = 0;

        virtual void set_shadowColor(const Color&) = 0;
        virtual const Color&  get_shadowColor() const = 0;

        virtual void set_shadowOffsetX(double) = 0;
        virtual double get_shadowOffsetX() const = 0;

        virtual void set_shadowOffsetY(double) = 0;
        virtual double get_shadowOffsetY() const = 0;

        virtual void set_shadowBlur(double) = 0;
        virtual double get_shadowBlur() const = 0;

        PROPERTY(Context2D, FillStyle, fillStyle);        // (default black)
        PROPERTY(Context2D, FillStyle, strokeStyle);      // (default black)

        PROPERTY(Context2D, TextAlign, textAlign);        // "start", "end", "left", "right", "center" (default: "start")
        PROPERTY(Context2D, TextBaseline, textBaseline);  // "top", "hanging", "middle", "alphabetic", "ideographic", "bottom" (default: "alphabetic")
        PROPERTY(Context2D, Font, font);                  // (default 10px sans-serif)

        PROPERTY(Context2D, double, lineWidth);           // (default 1)

        PROPERTY(Context2D, Color, shadowColor);          // (default transparent black)
        PROPERTY(Context2D, double, shadowOffsetX);       // (default 0)
        PROPERTY(Context2D, double, shadowOffsetY);       // (default 0)
        PROPERTY(Context2D, double,  shadowBlur);          // (default 0)

        // state
        virtual void save() = 0; // push state on state stack
        virtual void restore() = 0; // pop state stack and restore state

        virtual void clearRect(double x, double y, double w, double h) = 0;
        virtual void fillRect(double x, double y, double w, double h) = 0;
        virtual void strokeRect(double x, double y, double w, double h) = 0;
        virtual void fillText(const wchar_t*, double x, double y) = 0;        
        virtual CanvasGradient createLinearGradient(double x0, double y0, double x1, double y1) = 0;
        virtual TextMetrics measureText(const wchar_t*) = 0;
        virtual void moveTo(double x, double y) = 0;
        virtual void lineTo(double x, double y) = 0;
        virtual void beginPath() = 0;
        virtual void closePath() = 0;
        virtual void stroke() = 0;
        virtual void fill() = 0;
        virtual void clip() = 0;
        virtual void rect(double x, double y, double w, double h) = 0;
    };

    struct Canvas
    {
      virtual ~Canvas() {}
        virtual Context2D& getContext(const char*) =0;

        virtual double get_width() const = 0;
        virtual void set_width(double) = 0;

        virtual double get_height() const = 0;
        virtual void set_height(double) = 0;

        PROPERTY(Canvas, double, width);
        PROPERTY(Canvas, double, height);
    };

    struct CanvasImp : public Canvas
    {
       //
       virtual void BeginDraw(void*, int w, int h)=0;
       virtual void EndDraw()=0;
       //
    };

    CanvasImp* CreateCanvas(void*);    
    //
    TextAlign ParseTextAlign(const char* psz);
    TextBaseline ParseTextBaseline(const char* psz);
}
