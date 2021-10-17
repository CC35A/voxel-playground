#include <windows.h>
#include <string>
#include <d3d11.h>

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")

IDXGISwapChain* swapchain;             // the pointer to the swap chain interface
ID3D11Device* dev;                     // the pointer to our Direct3D device interface
ID3D11DeviceContext* devcon;           // the pointer to our Direct3D device context
ID3D11RenderTargetView* backbuffer;    // global declaration


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



void InitD3D(HWND hWnd) // sets up and initializes Direct3D
{
    // create a struct to hold information about the swap chain
    DXGI_SWAP_CHAIN_DESC scd;
    // clear out the struct for use
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    // fill the swap chain description struct
    scd.BufferCount = 1;                                    // two back buffers (edited: original was only one back buffer)
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
    scd.OutputWindow = hWnd;                                // the window to be used
    scd.SampleDesc.Count = 4;                               // how many multisamples
    scd.Windowed = TRUE;                                    // windowed/full-screen mode

    // create a device, device context and swap chain using the information in the scd struct
    D3D11CreateDeviceAndSwapChain(NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        NULL,
        NULL,
        NULL,
        D3D11_SDK_VERSION,
        &scd,
        &swapchain,
        &dev,
        NULL,
        &devcon);

    // get the address of the back buffer
    ID3D11Texture2D* pBackBuffer;
    swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    // use the back buffer address to create the render target
    dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
    pBackBuffer->Release();

    // set the render target as the back buffer
    devcon->OMSetRenderTargets(1, &backbuffer, NULL);

    // Set the viewport
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = 800;
    viewport.Height = 600;

    devcon->RSSetViewports(1, &viewport);
}

// this is the function used to render a single frame
void RenderFrame(void)
{
    // clear the back buffer to a deep blue
    float color[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
    devcon->ClearRenderTargetView(backbuffer, color);

    // do 3D rendering on the back buffer here

    // switch the back buffer and the front buffer
    swapchain->Present(0, 0);
}

// this is the function that cleans up Direct3D and COM
void CleanD3D()
{
    // close and release all existing COM objects
    swapchain->Release();
    backbuffer->Release();
    dev->Release();
    devcon->Release();
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

    // set up and initialize Direct3D
    InitD3D(hWnd5);

    MSG Msg = { 0 };
    while (TRUE)
    {
        // Check to see if any messages are waiting in the queue
        if (PeekMessage(&Msg, 0, 0, 0, PM_REMOVE))
        {
            // translate keystroke messages into the right format
            TranslateMessage(&Msg);

            // send the message to the WindowProc function
            DispatchMessageA(&Msg);

            // check to see if it's time to quit
            if (Msg.message == WM_QUIT)
            {
                break;
            }
        }
        else
        {
            RenderFrame();
        }
    }
    // clean up DirectX and COM
    CleanD3D();

    return Msg.wParam;
}