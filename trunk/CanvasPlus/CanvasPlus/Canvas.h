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

        TextAlign& operator = (const char* psz)
        {
            m_Value = ParseTextAlign(psz);
            return *this;
        }

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

        TextBaseline& operator = (const char* psz)
        {
            m_Value = ParseTextBaseline(psz);
            return *this;
        }

        operator TextBaselineEnum() const
        {
            return m_Value;
        }
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
        CanvasGradient(CanvasGradientImp*);
        ~CanvasGradient();

        void addColorStop(double offset, const Color& color);
    };

    //TODO
    struct CanvasPattern
    {
        // opaque object
    };

    struct Font
    {
        friend class Context2D;
        void* m_pNativeObject;

    public:
        Font();
        Font(const Font&);
        ~Font();

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

        FillStyle()
        {
            fillStyleEnum =  FillStyleEnumSolid;
        }

        FillStyle& operator = (const Color& color)
        {
            //canvasGradient reset?

            fillStyleEnum = FillStyleEnumSolid;
            m_Color = color;
            return *this;
        }

        FillStyle& operator = (const CanvasGradient& cg)
        {
            fillStyleEnum =     FillStyleEnumGradient;
            canvasGradient = cg;
            return *this;
        }

        FillStyle& operator = (const CanvasPattern&)
        {
            //TODO
            return *this;
        }
    };

    //http://dev.w3.org/html5/2dcontext/
    class Context2D
    {
        friend class Canvas;
        void* m_pNativeHandle;
        int flags;

        void Check();

        Context2D(void*);
        Context2D(const Context2D&);//

        int ToPixelX(double);
        int ToPixelY(double);
        int ToPixelSizeX(double);
        int ToPixelSizeY(double);

    public:

        //fillStyle
        FillStyle fillStyle;
        FillStyle strokeStyle;

        TextAlign textAlign;
        TextBaseline textBaseline;
        Font font;
        double lineWidth;
        //==Shadows==
        //
        Color shadowColor;
        double shadowOffsetX;
        double shadowOffsetY;
        double shadowBlur;

        ~Context2D();

        // state
        void save(); // push state on state stack
        void restore(); // pop state stack and restore state


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
    };

    class Canvas
    {
        Context2D m_CanvasRenderingContext2D;
    public:
        Canvas(void*);
        ~Canvas();
        Context2D& getContext(const char*);
    };
}
