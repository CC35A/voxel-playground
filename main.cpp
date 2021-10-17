#include <windows.h>
#include <string>
#include <d3d11.h>
#include <d3dcompiler.h>

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

IDXGISwapChain* swapchain;             // the pointer to the swap chain interface
ID3D11Device* dev;                     // the pointer to our Direct3D device interface
ID3D11DeviceContext* devcon;           // the pointer to our Direct3D device context
ID3D11RenderTargetView* backbuffer;
ID3D11VertexShader* pVS;    // the vertex shader
ID3D11PixelShader* pPS;     // the pixel shader
ID3D11Buffer* pVBuffer;
ID3D11InputLayout* pLayout;

unsigned window_width = GetSystemMetrics(SM_CXSCREEN) / 2;
unsigned window_height = GetSystemMetrics(SM_CYSCREEN) / 2;

struct VERTEX_COLOR
{ 
    FLOAT R, G, B, A;
};
struct VERTEX
{ 
    FLOAT X, Y, Z;
    VERTEX_COLOR color; 
};

// function prototypes
void InitD3D(HWND hWnd);    // sets up and initializes Direct3D
void RenderFrame(void);     // renders a single frame
void CleanD3D(void);        // closes Direct3D and releases memory
void InitGraphics(void);    // creates the shape to render
void InitPipeline(void);    // loads and prepares the shaders


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

void ErrorBox(const std::string& msg)
{
    MessageBoxA(0, msg.c_str(), "Error", MB_ICONERROR);
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

    unsigned flags = 0;
#ifdef _DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D11CreateDeviceAndSwapChain(NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        flags,
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
    viewport.Width = window_width - window_width / 2.5;
    viewport.Height = window_height / 1.5;

    devcon->RSSetViewports(1, &viewport);
    InitPipeline();
    InitGraphics();
}

// this is the function used to render a single frame
void RenderFrame(void)
{
    // clear the back buffer to a deep blue
    float color[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
    devcon->ClearRenderTargetView(backbuffer, color);

        // select which vertex buffer to display
        UINT stride = sizeof(VERTEX);
        UINT offset = 0;
        devcon->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);

        // select which primtive type we are using
        devcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // draw the vertex buffer to the back buffer
        devcon->Draw(3, 0);

    // switch the back buffer and the front buffer
    swapchain->Present(0, 0);
}

// this is the function that cleans up Direct3D and COM
void CleanD3D()
{
    // close and release all existing COM objects
    pVS->Release();
    pPS->Release();
    swapchain->Release();
    backbuffer->Release();
    dev->Release();
    devcon->Release();
}

void InitPipeline()
{
    // load and compile the two shaders
    ID3D10Blob* VS, * PS;
    ID3DBlob* VS_error, * PS_error;
    D3DCompileFromFile(L"shaders.shader", 0, 0, "VShader", "vs_5_0", 0, 0, &VS, &VS_error);
    D3DCompileFromFile(L"shaders.shader", 0, 0, "PShader", "ps_5_0", 0, 0, &PS, 0);

    //ErrorBox(static_cast<const char*>(VS_error->GetBufferPointer()));

    // encapsulate both shaders into shader objects
    dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
    dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);

    // set the shader objects
    devcon->VSSetShader(pVS, 0, 0);
    devcon->PSSetShader(pPS, 0, 0);

    // create the input layout object
    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    dev->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
    devcon->IASetInputLayout(pLayout);
}

void InitGraphics()
{
    // create a triangle using the VERTEX struct
    VERTEX_COLOR  farbe1 = { 1.0f, 0.0f, 0.0f, 1.0f };
    VERTEX_COLOR  farbe2 = { 0.0f, 1.0f, 0.0f, 1.0f };
    VERTEX_COLOR  farbe3 = { 0.0f, 0.0f, 1.0f, 1.0f };

    VERTEX OurVertices[] =
    {
        //{0.0f, 0.5f, 0.0f, {1.0f, 0.0f, 0.0f, 1.0f}},
        //{0.45f, -0.5, 0.0f, {0.0f, 1.0f, 0.0f, 1.0f}},
        //{-0.45f, -0.5f, 0.0f, {0.0f, 0.0f, 1.0f, 1.0f}}
        {0.0f, 0.5f, 0.0f, farbe1},
        {0.45f, -0.5, 0.0f, farbe2},
        {-0.45f, -0.5f, 0.0f, farbe3}

    };


    // create the vertex buffer
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
    bd.ByteWidth = sizeof(VERTEX) * 3;             // size is the VERTEX struct * 3
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

    dev->CreateBuffer(&bd, NULL, &pVBuffer);       // create the buffer


    // copy the vertices into the buffer
    D3D11_MAPPED_SUBRESOURCE ms;
    devcon->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
    memcpy(ms.pData, OurVertices, sizeof(OurVertices));                 // copy the data
    devcon->Unmap(pVBuffer, NULL);                                      // unmap the buffer
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
    //unsigned width = GetSystemMetrics(SM_CXSCREEN);
    //unsigned height = GetSystemMetrics(SM_CYSCREEN);
    unsigned width = window_width;
    unsigned height = window_height;
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
