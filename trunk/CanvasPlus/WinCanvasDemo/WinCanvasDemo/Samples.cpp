
#include "Samples.h"


//http://dev.w3.org/html5/2dcontext/

using namespace CanvasPlus;



void DrawPos(CanvasPlus::CanvasRenderingContext2D& ctx, double x, double y)
{
    ctx.moveTo(x - 20, y);
    ctx.lineTo(x + 20, y);
    ctx.moveTo(x, y - 20);
    ctx.lineTo(x, y + 20);
}

void Sample1(Canvas& canvas)
{
    auto ctx = canvas.getContext("2d");
    const char* baselines[] = {"top", "hanging",  "middle", "alphabetic", "ideographic", "bottom" };
    const char* align[] = { "start" , "end" , "left" , "right", "center" };
    const wchar_t* text[] =
    {
        L"top_start" , L"top_end" , L"top_left" , L"top_right", L"top_center",
        L"hanging_start" , L"hanging_end" , L"hanging_left" , L"hanging_right", L"hanging_center",
        L"middle_start" , L"middle_end" , L"middle_left" , L"middle_right", L"middle_center",
        L"alphabetic_start" , L"alphabetic_end" , L"alphabetic_left" , L"alphabetic_right", L"alphabetic_center",
        L"ideographic_start" , L"ideographic_end" , L"ideographic_left" , L"ideographic_right", L"ideographic_center",
        L"bottom_start" , L"bottom_end" , L"bottom_left" , L"bottom_right", L"bottom_center"
    };
    int j = 0;

    for (int i = 0; i < 6; i++)
    {
        for (int k = 0; k < 5; k++)
        {
            double x = 50 + i * 150;
            double y = 50 + k * 100;
            DrawPos(ctx, x, y);
            ctx.textBaseline = baselines[i];
            ctx.textAlign = align[k];
            ctx.fillText(text[j], x, y);
            j++;
        }
    }
}

void Sample2(Canvas& canvas)
{
    auto ctx = canvas.getContext("2d");
    ctx.fillRect(10, 10, 50, 50);
}

void Sample3(Canvas& canvas)
{
    auto ctx = canvas.getContext("2d");
    ctx.fillStyle = "rgb(255, 200, 200)";
    ctx.fillRect(10, 10, 50, 50);
}

void Sample4(CanvasPlus::Canvas&)
{
}
void Sample5(CanvasPlus::Canvas&)
{
}
void Sample6(CanvasPlus::Canvas&)
{
}
void Sample7(CanvasPlus::Canvas&)
{
}
void Sample8(CanvasPlus::Canvas&)
{
}
void Sample9(CanvasPlus::Canvas&)
{
}
void Sample10(CanvasPlus::Canvas&)
{
}

