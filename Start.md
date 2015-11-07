# Introduction #

What is Canvas++?

Canvas++ is a project to create a class to abstract drawing in different devices.
The class interface is based on the standard html5 canvas element.


# Details #

Why to use html canvas interface?

The Html 5 is a very strong standard. There are many devices like tablets smart phones and different OS implementing Html 5 compatible browsers. It means that every device is going to support the drawing as specified in the canvas element. For this reason, canvas interface it’s a safe choice very documented with a lot of samples available in the web.

One adicional reason is for windows users. Today we have 3 interfaces to draw. GDI, GDI+  and Direct2D. If you have to suport old versions of windows you have to create duplicate code to draw. Canvas will provide a common subset for it.

Why C++?

C++ is an ISO standard language with good abstractions and very fast. It is used by a large number of platforms including Windows, Mac, Linux, IOs etc.
These characteristics make C++ a perfect choice  to create applications that reuse code between different platforms and at the same time have access to native features and the best performance.

Why Canvas++?


Although many devices have a screen in common (most of them have a web browser as well) there is no standard native way to draw on the screen.
The canvas++ will allow C++ programmers to collaborate and have the same standard interface used on the web to drawing on the screen.



### HTML 5 Reference ###
This specification defines the 2D Context for the HTML canvas element.

http://dev.w3.org/html5/2dcontext/#dom-context-2d


### Plataforms ###

The first plataforms suported will be:

1) Windows

1.1 GDI (started)
GDI is very old, the drawing may not be pixel to pixel compatible   with the drawing on the web page, however is is very usefull to run in old windows plataforms.

http://msdn.microsoft.com/en-us/library/dd145203(v=vs.85).aspx

1.2 Direct2d (started)
Direct 2d

http://msdn.microsoft.com/en-us/library/windows/desktop/dd370990(v=vs.85).aspx


2) Mac (need update!)
> Can run in MacOS (CGContext)
> http://developer.apple.com/library/ios/#documentation/GraphicsImaging/Reference/CGContext/Reference/reference.html

3) IOs
> Can run un iPads and IOs (CGContext)
http://developer.apple.com/library/ios/#documentation/GraphicsImaging/Reference/CGContext/Reference/reference.html


## Learning HTML5 Canvas ##
http://www.html5canvastutorials.com/