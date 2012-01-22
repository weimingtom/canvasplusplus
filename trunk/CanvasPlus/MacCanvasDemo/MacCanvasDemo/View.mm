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
    if (currentaSample < TOTALSAMPLES)
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
     currentaSample = TOTALSAMPLES-1;
     [self setNeedsDisplay:YES];
}


- (void)drawRect:(NSRect)dirtyRect
{
    CGContextRef context = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    

    CanvasPlus::CanvasImp* pCanvas =  CanvasPlus::CreateCanvas(0);
    pCanvas->BeginDraw((void*)context, 100, 100);
    
    DrawSample(currentaSample, *pCanvas);
 
    pCanvas->EndDraw();
    delete pCanvas;
    // Drawing code here.
}

@end
