//
//  View.m
//  MacCanvasDemo
//
//  Created by Thiago Adams on 12/24/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#import "View.h"

#include "Canvas.h"
#include "Samples.h"

const int TotalSamples = 11;
int currentaSample = 0;

@implementation View

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (BOOL)isFlipped
{
    return TRUE;
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}


- (void)mouseDown:(NSEvent *)theEvent
{
    if (currentaSample < TotalSamples)
        currentaSample++;
    else
        currentaSample = 0;
     [self setNeedsDisplay:YES];
}

- (void)rightMouseDown:(NSEvent *)theEvent
{
   if (currentaSample > 0)
       currentaSample--;
    else
     currentaSample = TotalSamples-1;
     [self setNeedsDisplay:YES];
}


- (void)drawRect:(NSRect)dirtyRect
{
    CGContextRef context = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    
    CanvasPlus::Canvas canvas((void*)context);
    
    switch (currentaSample)
    {
        case 0:  Sample1(canvas); break;
        case 1:  Sample2(canvas); break;
               case 2:  Sample3(canvas); break;
               case 3:  Sample4(canvas); break;
               case 4:  Sample5(canvas); break;
               case 5:  Sample6(canvas); break;
               case 6:  Sample7(canvas); break;
               case 7:  Sample8(canvas); break;
        case 8:  Sample9(canvas); break;
               case 9:  Sample10(canvas); break;
               case 10:  Sample11(canvas); break;

    }
    // Drawing code here.
}

@end
