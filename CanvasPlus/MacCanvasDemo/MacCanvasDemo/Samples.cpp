//
//  Samples.cpp
//  MacCanvasDemo
//
//  Created by Thiago Adams on 12/24/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include "Canvas.h"

using namespace CanvasPlus;

void Sample1(Canvas& canvas)
{
    Context2D& ctx = canvas.getContext("2d");
    ctx.fillStyle = "rgb(255,0,0)";
    ctx.fillRect(10,10,50,50);
    
    ctx.strokeRect(100,10,50,50);
    ctx.fillText(L"testing", 100,100);
}