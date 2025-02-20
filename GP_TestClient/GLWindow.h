#pragma once
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>

class GLWindow
{
public:
    GLWindow();
    ~GLWindow();

    bool Create(const wchar_t* title, int width, int height, int nCmdShow);
    void Run();

private:
    HWND hWnd;
    HDC hDC;
    HGLRC hRC;
    HINSTANCE hInstance;

    static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
    void EnableOpenGL();
    void DisableOpenGL();
    void RenderScene();
};