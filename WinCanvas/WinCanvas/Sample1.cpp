#include "StdAfx.h"
#include "Sample1.h"


//http://dev.w3.org/html5/2dcontext/

using namespace CanvasPlus;

void Sample1(Canvas& canvas)
{
  auto ctx = canvas.getContext("2d");
  ctx.fillStyle = "rgb(255, 200, 200)";
  ctx.fillRect(10, 10, 50, 50);
  
  ctx.textBaseline = "bottom";
  ctx.fillStyle = "rgb(0, 0, 0)";
  ctx.fillText(L"teste", 10,10);


  ctx.textAlign = "center";  
  ctx.fillText(L"teste", 10+50, 10+50);

  ctx.textAlign = "left";  
  ctx.fillText(L"teste", 10+50, 10);
}

void Sample2(Canvas& canvas)
{
  auto ctx = canvas.getContext("2d");
  //ctx.setFillStyle("rgb(255, 200, 200)");
  ctx.fillRect(10, 10, 50, 50);
}