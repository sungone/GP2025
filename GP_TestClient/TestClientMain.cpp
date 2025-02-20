#include "GLWindow.h"

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
