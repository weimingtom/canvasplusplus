
#include "WinCanvasDemo.h"
#include <Canvas.h>
#include "Samples.h"



// Provides the application entry point.
int WINAPI WinMain(
    HINSTANCE /* hInstance */,
    HINSTANCE /* hPrevInstance */,
    LPSTR /* lpCmdLine */,
    int /* nCmdShow */
)
{
    // Use HeapSetInformation to specify that the process should
    // terminate if the heap manager detects an error in any heap used
    // by the process.
    // The return value is ignored, because we want to continue running in the
    // unlikely event that HeapSetInformation fails.
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
    DemoApp app;

    if (SUCCEEDED(app.Initialize()))
    {
        app.RunMessageLoop();
    }

    return 0;
}

// DemoApp constructor
DemoApp::DemoApp() :
    m_hwnd(NULL)
{
    m_pCanvas = CanvasPlus::CreateCanvas(0);
    m_CurrentSampleIndex = 1;
}

// DemoApp destructor
// Releases the application's resources.
DemoApp::~DemoApp()
{
    delete m_pCanvas;
}

// Creates the application window and device-independent
// resources.
HRESULT DemoApp::Initialize()
{
    // Register the window class.
    WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
    wcex.style         = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc   = DemoApp::WndProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = sizeof(LONG_PTR);
    wcex.hInstance     = HINST_THISCOMPONENT;
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);;
    wcex.lpszMenuName  = NULL;
    wcex.hCursor       = LoadCursor(NULL, IDI_APPLICATION);
    wcex.lpszClassName = L"CanvasPlusPlus";
    RegisterClassEx(&wcex);
    // Create the window.
    m_hwnd = CreateWindow(
                 L"CanvasPlusPlus",
                 L"Canvas++ demo app",
                 WS_OVERLAPPEDWINDOW,
                 CW_USEDEFAULT,
                 CW_USEDEFAULT,
                 500,
                 500,
                 NULL,
                 NULL,
                 HINST_THISCOMPONENT,
                 this
             );
    HRESULT hr = m_hwnd ? S_OK : E_FAIL;

    if (SUCCEEDED(hr))
    {
        ShowWindow(m_hwnd, SW_SHOWNORMAL);
        UpdateWindow(m_hwnd);
    }

    return S_OK;
}



// Runs the main window message loop.
void DemoApp::RunMessageLoop()
{
    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}


//  If the application receives a WM_SIZE message, this method
//  resizes the render target appropriately.
void DemoApp::OnResize(UINT /*width*/, UINT /*height*/)
{
}

// Handles window messages.
LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        DemoApp* pDemoApp = (DemoApp*)pcs->lpCreateParams;
        ::SetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA,
            PtrToUlong(pDemoApp)
        );
        result = 1;
    }
    else
    {
        DemoApp* pDemoApp = reinterpret_cast<DemoApp*>(static_cast<LONG_PTR>(
                                ::GetWindowLongPtrW(
                                    hwnd,
                                    GWLP_USERDATA
                                )));
        bool wasHandled = false;

        if (pDemoApp)
        {
            switch (message)
            {
                case WM_SIZE:
                {
                    UINT width = LOWORD(lParam);
                    UINT height = HIWORD(lParam);
                    pDemoApp->OnResize(width, height);
                }

                result = 0;
                wasHandled = true;
                break;

                case WM_DISPLAYCHANGE:
                {
                    InvalidateRect(hwnd, NULL, FALSE);
                }

                result = 0;
                wasHandled = true;
                break;
                
                case WM_ERASEBKGND:
                 result = 0;
                 wasHandled = true;
                 break;

                case WM_LBUTTONDOWN:
                    if (pDemoApp->m_CurrentSampleIndex < TOTALSAMPLES)
                    {
                        pDemoApp->m_CurrentSampleIndex++;
                    }
                    else
                    {
                        pDemoApp->m_CurrentSampleIndex = 1;
                    }

                    InvalidateRect(pDemoApp->m_hwnd, NULL, TRUE);
                    break;

                case WM_RBUTTONDOWN:
                    if (pDemoApp->m_CurrentSampleIndex > 1)
                    {
                        pDemoApp->m_CurrentSampleIndex--;
                    }
                    else
                    {
                        pDemoApp->m_CurrentSampleIndex = TOTALSAMPLES;
                    }

                    InvalidateRect(pDemoApp->m_hwnd, NULL, TRUE);
                    break;

                case WM_PAINT:
                {
                    RECT clientRect;
                    GetClientRect(hwnd, &clientRect);
                    PAINTSTRUCT ps;
                    HDC hdc = BeginPaint(pDemoApp->m_hwnd, &ps);
                    {
                        //D2D
#ifdef USING_WIN_D2D
                        pDemoApp->m_pCanvas->BeginDraw(pDemoApp->m_hwnd, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);
#endif
#ifdef USING_WIN_GDI
                        //GDI
                        pDemoApp->m_pCanvas->BeginDraw(hdc, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);
#endif
                        DrawSample(pDemoApp->m_CurrentSampleIndex, *pDemoApp->m_pCanvas);
                        pDemoApp->m_pCanvas->EndDraw();
                    } //need to call destructor before end
                    // TODO: Add any drawing code here...
                    EndPaint(pDemoApp->m_hwnd, &ps);
                }

                result = 0;
                wasHandled = true;
                break;

                case WM_DESTROY:
                {
                    PostQuitMessage(0);
                }

                result = 1;
                wasHandled = true;
                break;
            }
        }

        if (!wasHandled)
        {
            result = DefWindowProc(hwnd, message, wParam, lParam);
        }
    }

    return result;
}

