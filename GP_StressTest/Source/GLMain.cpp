#include "pch.h"
#include "GLWindow.h"

//Todo: 나중에 분포도 위치 그린다면 그때 사용
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow)
{
    GLWindow window;
    if (!window.Create(L"TestClient", 800, 600, nCmdShow))
    {
        return -1;
    }

    window.Run();

    return 0;
}
