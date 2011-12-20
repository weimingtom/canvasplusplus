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




namespace CanvasPlus //Better name?
{
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

    //Local representation of color
    struct Color
    {
        int r;
        int g;
        int b;
        int a;
    };

    void ParserColor(const char* psz, Color& );

    class FillStyle
    {
      
    public:
        //TODO variant type?
        Color m_Color;

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

    public:
        ~CanvasRenderingContext2D();

        // rects
        void fillRect(double x, double y, double w, double h);
        
        //fillStyle
        FillStyle fillStyle;
        FillStyle strokeStyle;

        //TODO
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
