
#include "Canvas.h"

#include <string>
#include <sstream>

//The compiler doesn't implement it yet
#define constexpr

namespace CanvasPlus
{

    //
    constexpr int GetR(const char* p);
    constexpr int GetG(const char* p);
    constexpr int GetB(const char* p);
    //



    Color::Color(const char* psz)
        : r(GetR(psz)),
          g(GetG(psz)),
          b(GetB(psz))
    {
    }

    int ParsePositiveInt(const wchar_t* psz)
    {
        int result = 0;

        while ((*psz >= '0') && (*psz <= '9'))
        {
            result = result * 10 + (*psz - '0');
            psz++;
        }

        return result;
    }

    TextBaseline ParseTextBaseline(const char* psz)
    {
        if (strcmp(psz, "top") == 0)
        {
            return TextBaselineTop;
        }
        else if (strcmp(psz, "hanging") == 0)
        {
            return TextBaselineHanging;
        }
        else if (strcmp(psz, "middle") == 0)
        {
            return TextBaselineMiddle;
        }
        else if (strcmp(psz, "alphabetic") == 0)
        {
            return TextBaselineAlphabetic;
        }
        else if (strcmp(psz, "ideographic") == 0)
        {
            return TextBaselineIdeographic;
        }
        else if (strcmp(psz, "bottom") == 0)
        {
            return TextBaselineBottom;
        }

        //assert
        return TextBaselineTop;
    }


    TextAlign ParseTextAlign(const char* psz)
    {
        if (strcmp(psz, "start") == 0)
        {
            return TextAlignStart;
        }
        else if (strcmp(psz, "end") == 0)
        {
            return TextAlignEnd;
        }
        else if (strcmp(psz, "left") == 0)
        {
            return TextAlignLeft;
        }
        else if (strcmp(psz, "right") == 0)
        {
            return TextAlignRight;
        }
        else if (strcmp(psz, "center") == 0)
        {
            return TextAlignCenter;
        }

        //assert
        return TextAlignLeft;
    }

    FillStyle::FillStyle(const FillStyle& fs)
    {
        fillStyleEnum = fs.fillStyleEnum;
        m_Color = fs.m_Color;
        canvasGradient = fs.canvasGradient;
    }


    FillStyle::FillStyle()
    {
        fillStyleEnum =  FillStyleEnumSolid;
    }

    FillStyle& FillStyle::operator = (const char* psz)
    {
        fillStyleEnum = FillStyleEnumSolid;
        m_Color = psz;
        return *this;
    }

    FillStyle& FillStyle::operator = (const Color& color)
    {
        //canvasGradient reset?
        fillStyleEnum = FillStyleEnumSolid;
        m_Color = color;
        return *this;
    }


    FillStyle& FillStyle::operator = (const FillStyle& fs)
    {
        fillStyleEnum = fs.fillStyleEnum;
        m_Color = fs.m_Color;
        canvasGradient = fs.canvasGradient;
        return *this;
    }


    FillStyle& FillStyle::operator = (const CanvasGradient& cg)
    {
        fillStyleEnum =     FillStyleEnumGradient;
        canvasGradient = cg;
        return *this;
    }

    FillStyle& FillStyle::operator = (const CanvasPattern&)
    {
        //TODO
        return *this;
    }



    //==================BEGIN COLOR ==================

    //#define ParserError() throw "error"
    //#define ParserError() 0

    constexpr int ParserError()
    {
        return 0;
    }

    constexpr int DECVAL(char c)
    {
        return c - '0';
    }

    constexpr int HEXVAL(char c)
    {
        return ((c >= '0' && c < '9') ? c - '0' : 10 + c - 'A');
    }

    constexpr int RState10(const char* p, int val)
    {
        return (*p >= '0' && *p <= '9') ? val * 10 + DECVAL(*p) : val;
    }

    constexpr int RState9(const char* p, int val)
    {
        return (*p >= '0' && *p <= '9') ? RState10(p + 1, val * 10 + DECVAL(*p)) : val;
    }

    constexpr int RState8(const char* p)
    {
        return (*p == ' ') ? RState8(p + 1) : (*p >= '0' && *p <= '9') ? RState9(p + 1, DECVAL(*p)) : ParserError();
    }

    constexpr int RState6(const char* p)
    {
        return (*p == '(') ? RState8(p + 1) : ParserError();
    }

    constexpr int RState4(const char* p)
    {
        return (*p == 'b') ? RState6(p + 1) : ParserError();
    }

    constexpr int RState3(const char* p, int val)
    {
        return ((*p >= '0' && *p <= '9') || (*p >= 'A' && *p <= 'F')) ? val * 16 + HEXVAL(*p) : ParserError();
    }

    constexpr int RState2(const char* p)
    {
        return (*p == 'g') ? RState4(p + 1) : ParserError();
    }

    constexpr int RState1(const char* p)
    {
        return ((*p >= '0' && *p <= '9') || (*p >= 'A' && *p <= 'F')) ? RState3(p + 1, HEXVAL(*p)) : ParserError();
    }

    constexpr int GetR(const char* p)
    {
        return (*p == '#') ? RState1(p + 1) : (*p == 'r') ? RState2(p + 1) : ParserError();
    }

    constexpr int GState12(const char* p);
    constexpr int GState13(const char* p);

    constexpr int GState16(const char* p, int val)
    {
        //TK_RGB2
        return (*p >= '0' && *p <= '9') ? val * 10 + DECVAL(*p) : val;
    }

    constexpr int GState15(const char* p, int val)
    {
        return (*p >= '0' && *p <= '9') ? GState16(p + 1, val * 10 + DECVAL(*p)) : val;
    }

    constexpr int GState14(const char* p)
    {
        return (*p == ' ') ? GState12(p + 1) : (*p == ',')  ? GState13(p + 1) : (*p >= '0' && *p <= '9') ? GState12(p + 1) : ParserError();
    }

    constexpr int GState13(const char* p)
    {
        return (*p == ' ') ? GState13(p + 1) : (*p >= '0' && *p <= '9') ? GState15(p + 1, DECVAL(*p)) : ParserError();
    }

    constexpr int GState12(const char* p)
    {
        return (*p == ' ') ? GState12(p + 1) : (*p == ',') ? GState13(p + 1) : ParserError();
    }

    constexpr int GState10(const char* p)
    {
        return (*p == ' ') ? GState12(p + 1) : (*p == ',') ? GState13(p + 1) : (*p >= '0' && *p <= '9') ? GState14(p + 1) : ParserError();
    }

    constexpr int GState8(const char* p)
    {
        return (*p == ' ') ? GState8(p + 1) : (*p >= '0' && *p <= '9') ? GState10(p + 1) : ParserError();
    }

    constexpr int GState7(const char* p, int val)
    {
        return ((*p >= '0' && *p <= '9') || (*p >= 'A' && *p <= 'F')) ? val * 16 + HEXVAL(*p) : ParserError();
    }

    constexpr int GState6(const char* p)
    {
        return (*p == '(') ? GState8(p + 1) : ParserError();
    }

    constexpr int GState5(const char* p)
    {
        return ((*p >= '0' && *p <= '9') || (*p >= 'A' && *p <= 'F')) ? GState7(p + 1, HEXVAL(*p)) : ParserError();
    }

    constexpr int GState4(const char* p)
    {
        return (*p == 'b') ? GState6(p + 1) : ParserError();
    }

    constexpr int GState3(const char* p)
    {
        return ((*p >= '0' && *p <= '9') || (*p >= 'A' && *p <= 'F')) ? GState5(p + 1) : ParserError();
    }

    constexpr int GState2(const char* p)
    {
        return (*p == 'g') ? GState4(p + 1) : ParserError();
    }

    constexpr int GState1(const char* p)
    {
        return ((*p >= '0' && *p <= '9') || (*p >= 'A' && *p <= 'F')) ? GState3(p + 1) : ParserError();
    }

    constexpr int GetG(const char* p)
    {
        return (*p == '#') ? GState1(p + 1) : (*p == 'r') ? GState2(p + 1) :  ParserError();
    }

    constexpr int BState22(const char* p, int val)
    {
        //TK_RGB3
        return (*p >= '0' && *p <= '9') ? val * 10 + DECVAL(*p) : val;
    }

    constexpr int BState21(const char* p, int val)
    {
        //TK_RGB3
        return (*p >= '0' && *p <= '9') ? BState22(p + 1, val * 10 + DECVAL(*p)) : val;
    }

    constexpr int BState18(const char* p);
    constexpr int BState19(const char* p);

    constexpr int BState20(const char* p)
    {
        return (*p == ' ') ? BState18(p + 1) : (*p == ',') ? BState19(p + 1) : (*p >= '0' && *p <= '9') ? BState18(p + 1) : ParserError();
    }

    constexpr int BState19(const char* p)
    {
        return (*p == ' ') ? BState19(p + 1) : (*p >= '0' && *p <= '9') ? BState21(p + 1, DECVAL(*p)) : ParserError();
    }

    constexpr int BState18(const char* p)
    {
        return (*p == ' ') ? BState18(p + 1) : (*p == ',') ? BState19(p + 1) : ParserError();
    }

    constexpr int BState16(const char* p)
    {
        return (*p == ' ') ? BState18(p + 1) : (*p == ',') ? BState19(p + 1) : (*p >= '0' && *p <= '9') ? BState20(p + 1) : ParserError();
    }

    constexpr int BState12(const char* p);
    constexpr int BState13(const char* p);

    constexpr int BState14(const char* p)
    {
        return (*p == ' ') ? BState12(p + 1) : (*p == ',') ? BState13(p + 1) : (*p >= '0' && *p <= '9') ? BState12(p + 1) : ParserError();
    }

    constexpr int BState13(const char* p)
    {
        return (*p == ' ') ? BState13(p + 1) : (*p >= '0' && *p <= '9') ? BState16(p + 1) : ParserError();
    }

    constexpr int BState12(const char* p)
    {
        return (*p == ' ') ? BState12(p + 1) : (*p == ',') ? BState13(p + 1) : ParserError();
    }

    constexpr int BState11(const char* p, int val)
    {
        return ((*p >= '0' && *p <= '9') || (*p >= 'A' && *p <= 'F')) ? val * 16 + HEXVAL(*p) : ParserError();
    }

    constexpr int BState10(const char* p)
    {
        return (*p == ' ') ? BState12(p + 1) : (*p == ',') ? BState13(p + 1) : (*p >= '0' && *p <= '9') ? BState14(p + 1) : ParserError();
    }

    constexpr int BState9(const char* p)
    {
        return ((*p >= '0' && *p <= '9') || (*p >= 'A' && *p <= 'F')) ? BState11(p + 1, HEXVAL(*p)) : ParserError();
    }

    constexpr int BState8(const char* p)
    {
        return (*p == ' ') ? BState8(p + 1) : (*p >= '0' && *p <= '9') ? BState10(p + 1) : ParserError();
    }

    constexpr int BState7(const char* p)
    {
        return ((*p >= '0' && *p <= '9') || (*p >= 'A' && *p <= 'F')) ? BState9(p + 1) : ParserError();
    }

    constexpr int BState6(const char* p)
    {
        return (*p == '(') ? BState8(p + 1) : ParserError();
    }

    constexpr int BState5(const char* p)
    {
        return ((*p >= '0' && *p <= '9') || (*p >= 'A' && *p <= 'F')) ?  BState7(p + 1) : ParserError();
    }

    constexpr int BState4(const char* p)
    {
        return (*p == 'b') ? BState6(p + 1) : ParserError();
    }

    constexpr int BState3(const char* p)
    {
        return ((*p >= '0' && *p <= '9') || (*p >= 'A' && *p <= 'F')) ? BState5(p + 1) : ParserError();
    }


    constexpr int BState2(const char* p)
    {
        return (*p == 'g') ? BState4(p + 1) : ParserError();
    }


    constexpr int BState1(const char* p)
    {
        return ((*p >= '0' && *p <= '9') || (*p >= 'A' && *p <= 'F')) ? BState3(p + 1) : ParserError();
    }

    constexpr int GetB(const char* p)
    {
        return (*p == '#') ? BState1(p + 1) : (*p == 'r') ? BState2(p + 1) : ParserError();
    }

    //==================END COLOR ==================

} //CanvasPlus


