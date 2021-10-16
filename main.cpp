#include <windows.h>

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

LRESULT CALLBACK sWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    if (Msg == WM_DESTROY) 
    {
        PostQuitMessage(0);
    }
    if (Msg == WM_COMMAND && wParam == 0)
    {
        MessageBox(hWnd, "eine wilde message box erscheint!", "gccs tolle message box", MB_ICONINFORMATION);
    }
    if (Msg == WM_COMMAND && LOWORD(wParam) == 1)
    {
        PostQuitMessage(0);
    }
    return DefWindowProc(hWnd, Msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, char*, int)
{
    
    WNDCLASS wc = { 0 };
    //wc.cbWndExtra = sizeof(this);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
    wc.hInstance = hInstance;
    wc.lpszClassName = "gccs tolle fensterklasse";
    wc.lpfnWndProc = sWndProc;

    RegisterClass(&wc);
    
    unsigned windowstyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    int xpos = CW_USEDEFAULT;
    int ypos = CW_USEDEFAULT;
    unsigned width = GetSystemMetrics(SM_CXSCREEN);
    unsigned height = GetSystemMetrics(SM_CYSCREEN);
    width = width / 2;
    height = height / 2;
    RECT r;
    SetRect(&r, 0, 0, width, height);
    AdjustWindowRect(&r, windowstyle, 0);
    unsigned child_stlye = WS_CHILD | WS_BORDER;
    HWND hWnd1 = CreateWindow(wc.lpszClassName, "gccs tolle voxel engine", windowstyle, xpos, ypos, r.right - r.left, r.bottom - r.top, 0, 0, hInstance, 0);
    HWND hWnd2 = CreateWindow(wc.lpszClassName, "windows left", child_stlye, 0, 0, width / 5, height, hWnd1, 0, hInstance, 0);
    HWND hWnd3 = CreateWindow(wc.lpszClassName, "windows right", child_stlye, width - width / 5, 0, width / 5, height, hWnd1, 0, hInstance, 0);
    HWND hWnd4 = CreateWindow(wc.lpszClassName, "window bottom", child_stlye, width / 5, height - height / 3, width - width / 2.5, height - height / 3, hWnd1, 0, hInstance, 0);
    HWND hWnd5 = CreateWindow(wc.lpszClassName, "game", child_stlye, width / 5, 0, width - width / 2.5, height / 1.5, hWnd1, 0, hInstance, 0);
    HWND hWnd6 = CreateWindow("BUTTON", "gccs toller knopf", WS_CHILD | WS_VISIBLE, 10, 50, 150, 30, hWnd2, 0, hInstance, 0);
    HWND hWnd7 = CreateWindow("BUTTON", "mach putt", WS_CHILD | WS_VISIBLE, 10, 10, 150, 30, hWnd2, reinterpret_cast<HMENU>(1), hInstance, 0);

    ShowWindow(hWnd1, SW_SHOWNORMAL);
    ShowWindow(hWnd2, SW_SHOWNORMAL);
    ShowWindow(hWnd3, SW_SHOWNORMAL);
    ShowWindow(hWnd4, SW_SHOWNORMAL);
    ShowWindow(hWnd5, SW_SHOWNORMAL);
    MSG Msg;
    while (GetMessage(&Msg, 0, 0, 0))
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return 0;
}
