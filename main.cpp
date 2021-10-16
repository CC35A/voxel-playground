#include <windows.h>
#include <string>

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

LRESULT CALLBACK sWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    if (Msg == WM_DESTROY) 
    {
        PostQuitMessage(0);
    }
    if (Msg == WM_COMMAND && LOWORD(wParam) == 1)
    {
        MessageBox(hWnd, "eine wilde message box erscheint!", "gccs tolle message box", MB_ICONINFORMATION);
    }
    if (Msg == WM_COMMAND && LOWORD(wParam) == 2)
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
    std::string log_text;
    unsigned width = GetSystemMetrics(SM_CXSCREEN);
    unsigned height = GetSystemMetrics(SM_CYSCREEN);
    width = width / 2;
    height = height / 2;
    RECT r;
    SetRect(&r, 0, 0, width, height);
    AdjustWindowRect(&r, windowstyle, 0);
    unsigned child_stlye = WS_CHILD | WS_BORDER;
    unsigned edit_style = ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL | WS_VSCROLL;

    HWND hWnd1 = CreateWindow(wc.lpszClassName, "gccs tolle voxel engine", windowstyle, xpos, ypos, r.right - r.left, r.bottom - r.top, 0, 0, hInstance, 0);
    auto CWC = [hInstance](const char* classname, const char* title, DWORD style, int x, int y, int w, int h, HWND hWnd, int ID = 0)
    {
        return CreateWindow(classname, title, WS_CHILD | WS_VISIBLE | style, x, y, w, h, hWnd, reinterpret_cast<HMENU>(ID), hInstance, 0);
    };

    HWND hWnd2 = CWC(wc.lpszClassName, "windows left", child_stlye, 0, 0, width / 5, height, hWnd1);
    HWND hWnd3 = CWC(wc.lpszClassName, "windows right", child_stlye, width - width / 5, 0, width / 5, height, hWnd1);
    HWND hWnd4 = CWC("EDIT", log_text.c_str(), child_stlye | edit_style, width / 5, height - height / 3, width - width / 2.5, height / 3, hWnd1);
    HWND hWnd5 = CWC(wc.lpszClassName, "game", 0, width / 5, 0, width - width / 2.5, height / 1.5, hWnd1);
    HWND hWnd6 = CWC("BUTTON", "gccs toller knopf", 0, 10, 50, 150, 30, hWnd2, 1);
    HWND hWnd7 = CWC("BUTTON", "mach putt", 0, 10, 10, 150, 30, hWnd2, 2);

    log_text = "Hello, World!1\r\nHello, World!2\r\nHello, World!3\r\nHello, World!4\r\nHello, World!5\r\nHello, World!6\r\nHello, World!7\r\nHello, World!8\r\nHello, World!9\r\nHello, World!10\r\nHello, World!10\r\nHello, World!11\r\nHello, World!12\r\nHello, World!13";
    SetWindowText(hWnd4, log_text.c_str());
    //SetWindowLongPtr(hWnd4, GWL_STYLE, child_stlye | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL); //sets hWnd4 to read and write

    ShowWindow(hWnd1, SW_SHOWNORMAL);
    MSG Msg;
    while (GetMessage(&Msg, 0, 0, 0))
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return 0;
}
