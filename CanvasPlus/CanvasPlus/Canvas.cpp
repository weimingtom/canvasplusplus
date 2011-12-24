
#include "Canvas.h"

#include <string>
#include <sstream>

namespace CanvasPlus
{
    // state
    void Context2D::save()
    {
        // push state on state stack
    }

    void Context2D::restore()
    {
        // pop state stack and restore state
    }


    Color::Color(const char* psz)
    {
        ParserColor(psz, *this);
    }

    static Color backcolor = "rgb(255,0,0)";


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


    int ParseAndMovePositiveInt(const char*& psz)
    {
        int result = 0;

        while ((*psz >= '0') && (*psz <= '9'))
        {
            result = result * 10 + (*psz - '0');
            psz++;
        }

        return result;
    }


    static bool Skip(const char*& psz)
    {
        //"rgb()"
        while ((*psz < '0') || (*psz > '9'))
        {
            if (*psz == 0)
            {
                return false;    //terminou
            }

            psz++;
        }

        return true;
    }
    void ParserColor(const char* psz, Color& color)
    {
        if (!Skip(psz))
        {
            return;
        }

        color.r = ParseAndMovePositiveInt(psz);

        if (!Skip(psz))
        {
            return;
        }

        color.g = ParseAndMovePositiveInt(psz);

        if (!Skip(psz))
        {
            return;
        }

        color.b = ParseAndMovePositiveInt(psz);

        if (!Skip(psz))
        {
            return;
        }

        color.a = ParseAndMovePositiveInt(psz);

        if (!Skip(psz))
        {
            return;
        }
    }



    TextBaselineEnum ParseTextBaseline(const char* psz)
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
        else if (psz == "bottom" || strcmp(psz, "bottom") == 0)
        {
            return TextBaselineBottom;
        }

        //assert
        return TextBaselineTop;
    }


    TextAlignEnum ParseTextAlign(const char* psz)
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

} //CanvasPlus

