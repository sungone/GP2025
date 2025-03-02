#include "pch.h"
#include "GLWindow.h"

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")

GLWindow::GLWindow()
    : hWnd(NULL), hDC(NULL), hRC(NULL), hInstance(NULL)
{
}

GLWindow::~GLWindow()
{
    DisableOpenGL();
    if (hWnd)
    {
        DestroyWindow(hWnd);
    }
}

bool GLWindow::Create(const wchar_t* title, int width, int height, int nCmdShow)
{
    hInstance = GetModuleHandle(NULL);

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"GLWindow";
    wc.style = CS_OWNDC;
    RegisterClass(&wc);

    hWnd = CreateWindowEx(0, L"GLWindow", title, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        NULL, NULL, hInstance, this);

    if (!hWnd)
    {
        return false;
    }

    ShowWindow(hWnd, nCmdShow);
    EnableOpenGL();
    return true;
}

void GLWindow::Run()
{
    MSG msg = {};
    BOOL running = TRUE;

    while (running)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                running = FALSE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            Render();
            SwapBuffers(hDC);
        }
    }
}

// OpenGL 활성화
void GLWindow::EnableOpenGL()
{
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR), 1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0,
        PFD_MAIN_PLANE, 0, 0, 0, 0
    };

    hDC = GetDC(hWnd);
    int format = ChoosePixelFormat(hDC, &pfd);
    SetPixelFormat(hDC, format, &pfd);
    hRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hRC);

    glViewport(0, 0, 800, 600);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// OpenGL 종료
void GLWindow::DisableOpenGL()
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hWnd, hDC);
}

void GLWindow::Render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();


    glEnd();
}

LRESULT CALLBACK GLWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    case WM_DESTROY:
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
