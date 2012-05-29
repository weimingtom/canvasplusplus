
#include "Samples.h"


//http://dev.w3.org/html5/2dcontext/

using namespace CanvasPlus;

void DrawPos(CanvasPlus::Context2D& ctx, double x, double y)
{
    ctx.beginPath();
    ctx.strokeStyle = "rgb(0,0,255)";
    ctx.moveTo(x - 20, y);
    ctx.lineTo(x + 20, y);
    ctx.moveTo(x, y - 20);
    ctx.lineTo(x, y + 20);
    ctx.closePath();
    ctx.stroke();
}

void Sample1(Canvas& canvas)
{
    Context2D& ctx = canvas.getContext("2d");
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
    ctx.font = "bold 12px arial";

   
  
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
    Context2D& ctx = canvas.getContext("2d");
    ctx.beginPath();
    ctx.rect(50, 50, 30, 30);
    ctx.clip();
    ctx.fillStyle = "rgb(0,0,0)";
    ctx.fillRect(50, 50, 100, 100);
    //   ctx.strokeStyle = "rgb(0,0,0)";
    //  ctx.beginPath();
    ctx.fillStyle = "rgb(0,170,0)";
    // ctx.strokeStyle = "rgb(0,0,0)";
    ctx.fillRect(60, 60, 40, 40);
    ctx.textBaseline = "top";
    ctx.fillStyle = "rgb(250,250,250)";
    ctx.fillText(L"test clip", 50, 50);
    // ctx.closePath();
    // ctx.fill();
    // ctx.stroke();
}

void Sample3(Canvas& canvas)
{
    Context2D& ctx = canvas.getContext("2d");
    ctx.fillStyle = "rgb(255, 200, 200)";
    ctx.fillRect(10, 10, 50, 50);
}

void Sample4(CanvasPlus::Canvas& canvas)
{
    Context2D& ctx = canvas.getContext("2d");
    ctx.fillStyle = "rgb(170,170,170)";
    ctx.fillRect(10, 10, 50, 50);
    ctx.strokeRect(10, 10, 50, 50);
    ctx.fillStyle = "rgb(255,255,255)";
    ctx.textBaseline = "middle";
    ctx.textAlign = "center";
    ctx.fillText(L"Button", (10 + 10 + 50) / 2, (10 + 10 + 50) / 2);
}

void Sample5(CanvasPlus::Canvas& canvas)
{
    Context2D& ctx = canvas.getContext("2d");
    ctx.fillStyle = "rgb(220,220,220)";
    auto w = 200;
    auto h = 200;
    auto t = 100;
    auto l = 100;
    ctx.font = "14px arial";
    //shadow
    ctx.shadowOffsetX = 10;
    ctx.shadowOffsetY = 10;
    ctx.shadowBlur = 5;
    ctx.shadowColor = "rgba(0, 0, 0, 0.5)";
    //main
    ctx.fillRect(l, t, w, h);
    ctx.strokeStyle = "rgb(0,0,0)";
    ctx.strokeRect(l + 2, t + 2, w - 4, h - 4);
    ctx.strokeRect(l + 4, t + 4, w - 8, h - 8);
    auto text = L"Title";
    auto textw = ctx.measureText(text).width;
    ctx.shadowOffsetX = 0;
    ctx.shadowOffsetY = 0;
    ctx.fillRect(l + w / 2 - textw / 2,
                 t + 2,
                 textw,
                 14);
    ctx.fillStyle = "rgb(0,0,0)";
    ctx.textAlign = "center";
    ctx.textBaseline = "top";
    ctx.fillText(text, l + w / 2, t);
}
void Sample6(CanvasPlus::Canvas& canvas)
{
    Context2D& ctx = canvas.getContext("2d");
    auto lingrad = ctx.createLinearGradient(0, 10, 0, 10 + 130);
    lingrad.addColorStop(0, "rgb(255,0,0)");
    lingrad.addColorStop(1, "rgb(255,255,255)");
    ctx.fillStyle = lingrad;
    ctx.fillRect(10, 10, 130, 130);
}

void Sample7(CanvasPlus::Canvas& canvas)
{
    Context2D& ctx = canvas.getContext("2d");
    auto lingrad = ctx.createLinearGradient(10, 0, 130, 0);
    lingrad.addColorStop(0, "rgb(0,0,255)");// "#FF01F2");
    lingrad.addColorStop(1, "rgb(255,255,255)");
    ctx.fillStyle = lingrad;
    ctx.fillRect(10, 10, 130, 130);
}
void Sample8(CanvasPlus::Canvas& canvas)
{
    Context2D& ctx = canvas.getContext("2d");
    auto lingrad = ctx.createLinearGradient(130, 0, 10, 0);
    lingrad.addColorStop(0, "rgb(255,0,0)");
    lingrad.addColorStop(1, "rgb(255,255,255)");
    ctx.fillStyle = lingrad;
    ctx.fillRect(10, 10, 130, 130);
}
void Sample9(CanvasPlus::Canvas& canvas)
{
    Context2D& ctx = canvas.getContext("2d");
    auto lingrad = ctx.createLinearGradient(0, 10 + 130, 0, 10);
    lingrad.addColorStop(0, "rgb(255,0,0)");
    lingrad.addColorStop(1, "rgb(255,255,255)");
    ctx.fillStyle = lingrad;
    ctx.fillRect(10, 10, 130, 130);
}
void Sample10(CanvasPlus::Canvas& canvas)
{
    Context2D& ctx = canvas.getContext("2d");
    ctx.shadowOffsetX = 5;
    ctx.shadowOffsetY = 5;
    ctx.shadowBlur = 5;
    ctx.shadowColor = "rgba(0, 0, 0, 0.5)";
    ctx.fillStyle = "rgb(255,0,0)";
    ctx.fillRect(20, 20, 150, 100);
}

void Sample11(CanvasPlus::Canvas& canvas)
{
    Context2D& ctx = canvas.getContext("2d");
    bool ispressed =  false;
    bool isfocused = true;
    auto lingrad = ctx.createLinearGradient(10, 10 , 10 + 130, 10 + 130);

    if (ispressed)
    {
        lingrad.addColorStop(0, "rgb(245,245,245)");
        lingrad.addColorStop(1, "rgb(220,220,220)");
    }
    else
    {
        lingrad.addColorStop(0, "rgb(245,245,245)");
        lingrad.addColorStop(1, "rgb(220,220,220)");
    }

    ctx.fillStyle = lingrad;
    ctx.fillRect(10, 10, 130, 130);

    if (isfocused)
    {
        ctx.strokeStyle = "rgb(74,144,254)";
    }
    else
    {
        ctx.strokeStyle = "rgb(198,198,198)";
    }

    ctx.strokeRect(10, 10, 130, 130);
    ctx.fillStyle = "rgb(0,0,0)";
    ctx.textAlign = "center";
    ctx.textBaseline = "middle";
    ctx.fillText(L"Button", (10 + 10 + 130) / 2, (10 + 10 + 130) / 2);
}

void Sample12(CanvasPlus::Canvas& canvas)
{
    Context2D& ctx = canvas.getContext("2d");
    ctx.beginPath();
    ctx.moveTo(100, 100);
    ctx.lineTo(150, 200);
    ctx.lineTo(250, 20);
    ctx.closePath();
    ctx.stroke();
}

void Sample13(CanvasPlus::Canvas& canvas)
{
    Context2D& ctx = canvas.getContext("2d");

    for (int i = 0; i < 10; i++)
    {
        ctx.beginPath();
        ctx.strokeStyle = "#00F2A0";
        ctx.lineWidth = i;
        ctx.moveTo(i * 10 + 100, i * 10 + 100);
        ctx.lineTo(i * 10 + 150, i * 10 + 200);
        ctx.lineTo(i * 10 + 250, i * 10 + 20);
        ctx.lineTo(i * 10 + 50, i * 10 + 20);
        ctx.closePath();
        ctx.stroke();
    }
}
void Sample14(CanvasPlus::Canvas& canvas)
{
    // Get the canvas element and its drawing context
    Context2D& context = canvas.getContext("2d");
    /*
    |---------------------
    | Origin: Top Left
    | End: Bottom Right
    |---------------------
    */
    context.beginPath();
    context.moveTo(10, 10);
    context.lineTo(200, 200);
    context.stroke();
    /*
    |---------------------
    | Origin: Top Middle
    | End: Bottom Middle
    |---------------------
    */
    context.save();
    context.beginPath();
    context.moveTo(100, 0);
    context.lineTo(100, 200);
    context.stroke();
    context.restore();
    /*
    |---------------------
    | Origin: Top Right
    | End: Bottom Left
    |---------------------
    */
    context.save();
    context.beginPath();
    context.moveTo(200, 0);
    context.lineTo(0, 200);
    context.stroke();
    context.restore();
}

void Sample15(CanvasPlus::Canvas& canvas)
{
    Context2D& ctx = canvas.getContext("2d");
    //
    ctx.beginPath();
    ctx.moveTo(100 - 2, 100);
    ctx.lineTo(100 + 2, 100);
    ctx.moveTo(100, 100 - 2);
    ctx.lineTo(100, 100 + 2);
    ctx.closePath();
    ctx.stroke();
    // Filled triangle
    ctx.beginPath();
    ctx.moveTo(25, 25);
    ctx.lineTo(105, 25);
    ctx.lineTo(25, 105);
    ctx.fill();
    // Stroked triangle
    ctx.beginPath();
    ctx.moveTo(125, 125);
    ctx.lineTo(125, 45);
    ctx.lineTo(45, 125);
    ctx.closePath();
    ctx.stroke();

    //

    for (int i = 0; i < 10; i++)
    {
        ctx.lineWidth = 1 + i;
        ctx.beginPath();
        ctx.moveTo(5 + i * 14, 150 + 5);
        ctx.lineTo(5 + i * 14, 150 + 140);
        ctx.stroke();
    }
}

void Sample16(CanvasPlus::Canvas& canvas)
{
    Context2D& ctx = canvas.getContext("2d");
    ctx.lineWidth = 1;

    if (ctx.fillStyle == "#000000")
    {
    }

    for (int i = 0; i < 10; i++)
    {
        ctx.beginPath();
        ctx.strokeStyle = "#00F2A0";
        ctx.lineWidth = i;
        ctx.moveTo(i * 10 + 100, i * 10 + 100);
        ctx.lineTo(i * 10 + 150, i * 10 + 200);
        ctx.lineTo(i * 10 + 250, i * 10 + 20);
        ctx.lineTo(i * 10 + 50, i * 10 + 20);
        ctx.closePath();
        ctx.stroke();
    }
}


void Sample17(CanvasPlus::Canvas& canvas)
{
    Context2D& ctx = canvas.getContext("2d");
    ctx.font = "italic  12px arial";
    ctx.fillText(L"italic  12px arial", 10, 20);
    ctx.font = "normal 14px  arial";
    ctx.fillText(L"normal 14px normal arial", 10, 40);
    ctx.font = "normal 14px \"courier new\"";
    ctx.fillText(L"normal 14px normal courier new", 10, 60);
}