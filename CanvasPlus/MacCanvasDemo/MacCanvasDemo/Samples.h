//
//  Samples.h
//  MacCanvasDemo
//
//  Created by Thiago Adams on 12/24/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef MacCanvasDemo_Samples_h
#define MacCanvasDemo_Samples_h

#include "Canvas.h"


void Sample1(CanvasPlus::Canvas&);
void Sample2(CanvasPlus::Canvas&);
void Sample3(CanvasPlus::Canvas&);
void Sample4(CanvasPlus::Canvas&);
void Sample5(CanvasPlus::Canvas&);
void Sample6(CanvasPlus::Canvas&);
void Sample7(CanvasPlus::Canvas&);
void Sample8(CanvasPlus::Canvas&);
void Sample9(CanvasPlus::Canvas&);
void Sample10(CanvasPlus::Canvas&);
void Sample11(CanvasPlus::Canvas&);
void Sample12(CanvasPlus::Canvas&);
void Sample13(CanvasPlus::Canvas&);
void Sample14(CanvasPlus::Canvas&);
void Sample15(CanvasPlus::Canvas&);

#define TOTALSAMPLES 15

inline void DrawSample(int i, CanvasPlus::Canvas& canvas)
{
    switch (i)
    {
        case 1:
            Sample1(canvas);
            break;
            
        case 2:
            Sample2(canvas);
            break;
            
        case 3:
            Sample3(canvas);
            break;
            
        case 4:
            Sample4(canvas);
            break;
            
        case 5:
            Sample5(canvas);
            break;
            
        case 6:
            Sample6(canvas);
            break;
            
        case 7:
            Sample7(canvas);
            break;
            
        case 8:
            Sample8(canvas);
            break;
            
        case  9:
            Sample9(canvas);
            break;
            
        case  10:
            Sample10(canvas);
            break;
            
        case  11:
            Sample11(canvas);
            break;
            
        case  12:
            Sample12(canvas);
            break;
            
        case  13:
            Sample13(canvas);
            break;
            
        case  14:
            Sample14(canvas);
            break;
            
        case  15:
            Sample15(canvas);
            break;
    }
}


#endif
