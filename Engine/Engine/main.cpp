// Dear ImGui: standalone example application for DirectX 11
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "imgui/imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"
#include "Scene.h"
#include "timerclass.h"
#include <d3d11.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>
#include <Python.h>
#include <iostream>

// Data
static ID3D11Device*            g_pd3dDevice = NULL;
static ID3D11DeviceContext*     g_pd3dDeviceContext = NULL;
static IDXGISwapChain*          g_pSwapChain = NULL;
static ID3D11RenderTargetView*  g_mainRenderTargetView = NULL;
static ID3D11DepthStencilView*  g_pDepthStencilView = NULL;
static ID3D11DepthStencilState* g_depthStencilState = NULL;

static int                      screenWidth = 1280;
static int                      screenHeight = 800;
static const float              screenDepth = 200.0f;
static const float              screenNear = 1.0f;
static Matrix                   g_pProjectionMatrix;
static Matrix                   g_pWorldMatrix;
static Matrix                   g_pOrthoMatrix;

static TimerClass*               g_Timer;
static GraphicsClass*           g_GraphicsClass = NULL;
static KataVictim*              BoxObj = NULL;
static KataVictim*              SphereObj = NULL;

// Forward declarations of helper functions
HRESULT CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#define MOD_NAME "calc"
#define FUNC_NAME "calculation"

PyObject* pName = NULL, * pModule = NULL, * pDict = NULL, * pFunc = NULL, *pArgs = NULL, *pValue = NULL;

int EndPython() {
    Py_DECREF(pValue);
    Py_DECREF(pName);
    Py_Finalize();
    return 1;
}

int cleanup(const string& text = string(), int exitCode = 1) {
    Py_Finalize();
    if (!text.empty())
        cout << text << endl;
    cout << "Press ENTER to return...\n";
    cin.get();
    return exitCode;
}

// Main code
int main(int, char**)
{
    Py_Initialize();
    PyRun_SimpleString("import sys;\n");
    PyRun_SimpleString("sys.path.append(r'C:\\CustomGameEngine\\Engine\\Engine\')");//the folder where the calc.py is located*/
    PyRun_SimpleString("print(sys.path);");
    pName = PyUnicode_FromString(MOD_NAME);
    if (pName == NULL) {
        return cleanup("PyUnicode_FromString returned NULL");
    }
    pModule = PyImport_Import(pName);
    Py_DECREF(pName);
    if (pModule == NULL) {
        return cleanup(string("NULL module: '") + MOD_NAME + "'");
    }
    pDict = PyModule_GetDict(pModule);
    if (pDict == NULL) {
        return cleanup("NULL module dict");
    }

    pArgs = PyTuple_New(2);
    for (int i = 0; i < 2; i++) {
        pValue = PyLong_FromLong(i + 4);
        PyTuple_SetItem(pArgs, i, pValue);
    }
    
    pFunc = PyDict_GetItemString(pDict, FUNC_NAME);
    if (pFunc == NULL) {
        cout << "pFunc error\n";
        EndPython();
    }
    pValue = PyObject_CallObject(pFunc, pArgs);
    cout << "Return of call: " << PyLong_AsLong(pValue) << endl;

    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("Dear ImGui DirectX11 Example"), WS_OVERLAPPEDWINDOW, 100, 20, screenWidth, screenHeight, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (FAILED(CreateDeviceD3D(hwnd)))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    bool show_demo_window = false;
    bool show_another_window = false;
    bool my_tool_active = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Create scene
    Scene* m_Scene = new Scene;
    if (!m_Scene)
    {
        return 0;
    }
    
    // Load scene resources
    if (FAILED(m_Scene->Init(g_pd3dDevice, g_mainRenderTargetView, g_GraphicsClass, screenWidth, screenHeight, hwnd)))
    {
        return 0;
    }

    g_Timer = new TimerClass;
    g_Timer->Initialize();

    // Main loop
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            continue;
        }

        int axisX = 0, axisY = 0;

        g_Timer->Frame();

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame(screenWidth, screenHeight);
        ImGui_ImplWin32_NewFrame(axisX, axisY);
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        {
            bool modes_editor = false;
            // Create a window called "My First Too", with a menu bar.
            ImGui::Begin("Modes", &modes_editor, ImGuiWindowFlags_MenuBar);
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
                    if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
                    if (ImGui::MenuItem("Close", "Ctrl+W")) { modes_editor = false; }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            ImGui::Separator();

            ImGui::LabelText("label", "Value");

            {
                // List box
                const char* items[] = { "Recently Placed", "Basic", "Lights", "Geometry", "Imported" };
                static int item_current = 1;
                ImGui::ListBox("", &item_current, items, IM_ARRAYSIZE(items), 4);
            }

            ImGui::SameLine();

            {
                ImGui::BeginChild("Scrolling");
                if (ImGui::Button("Box"))
                {
                    BoxObj = new KataVictim;
                    BoxObj->Init(hwnd, L"Data\\Objects\\m_cube.obj", L"Data\\Objects\\brick.tga", Vector3(4.75f, 4.75f, 4.75f), g_pd3dDevice);
                    BoxObj->Place(Vector3(rand() % 20 - 1, -0.45f, 0.0f));
                    g_GraphicsClass->SetRenderable((Gameobject*)BoxObj, BoxObj->m_Model);
                }
                if (ImGui::Button("Sphere"))
                {
                    SphereObj = new KataVictim;
                    SphereObj->Init(hwnd, L"Data\\Objects\\sphere.obj", L"Data\\Objects\\moonmap.tga", Vector3(0.75f, 0.75f, 0.75f), g_pd3dDevice);
                    SphereObj->Place(Vector3(rand() % 30 - 1, -0.35f, 0.0f));
                    g_GraphicsClass->SetRenderable((Gameobject*)SphereObj, SphereObj->m_Model);
                }
                ImGui::EndChild();
            }

            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        ImGui_ImplDX11_UpdateRenderData(g_mainRenderTargetView, clear_color, g_pDepthStencilView, g_depthStencilState);
        /*g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&clear_color);*/
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData(), screenWidth, screenHeight, screenDepth, screenNear);

        float fieldOfView, screenAspect;
        // Setup the projection matrix.
        fieldOfView = (float)XM_PI / 4.0f;
        screenAspect = (float)screenWidth / (float)screenHeight;

        // Create the projection matrix for 3D rendering.
        //D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);
        g_pProjectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);
        // Initialize the world matrix to the identity matrix.
        //D3DXMatrixIdentity(&m_worldMatrix);
        g_pWorldMatrix = XMMatrixIdentity();
        // Create an orthographic projection matrix for 2D rendering.
        //D3DXMatrixOrthoLH(&m_orthoMatrix, (float)screenWidth, (float)screenHeight, screenNear, screenDepth);
        g_pOrthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

        g_pSwapChain->Present(0, 0); // Present with vsync
        //g_pSwapChain->Present(0, 0); // Present without vsync

        m_Scene->Update(g_Timer->GetDeltaTime(), g_pd3dDeviceContext, g_pSwapChain, g_mainRenderTargetView, g_pDepthStencilView, g_depthStencilState,
            clear_color, axisX, axisY, g_pProjectionMatrix, g_pWorldMatrix, g_pOrthoMatrix);
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    // Delete spawned objects
    BoxObj->Unload();
    delete BoxObj;
    BoxObj = NULL;

    SphereObj->Unload();
    delete SphereObj;
    SphereObj = NULL;

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

HRESULT CreateDeviceD3D(HWND hWnd)
{
    HRESULT result = S_OK;
    IDXGIFactory* factory;
    IDXGIAdapter* adapter;
    IDXGIOutput* adapterOutput;
    unsigned int numModes, i, numerator = 0, denominator = 1, stringLength;
    DXGI_MODE_DESC* displayModeList;

    // Setup backend capabilities flags
    // Create a DirectX graphics interface factory.
    result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
    if (FAILED(result))
    {
        printf("error in CreateDXGIFactory.");
        MessageBox(hWnd, L"error in CreateDXGIFactory.", L"Error", MB_OK);
        return E_FAIL;
    }

    // Use the factory to create an adapter for the primary graphics interface (video card).
    result = factory->EnumAdapters(0, &adapter);
    if (FAILED(result))
    {
        printf("error in EnumAdapters.");
        MessageBox(hWnd, L"error in EnumAdapters.", L"Error", MB_OK);
        return E_FAIL;
    }

    // Enumerate the primary adapter output (monitor).
    result = adapter->EnumOutputs(0, &adapterOutput);
    if (FAILED(result))
    {
        printf("error in EnumOutputs.");
        MessageBox(hWnd, L"error in EnumOutputs.", L"Error", MB_OK);
        return E_FAIL;
    }

    // Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
    if (FAILED(result))
    {
        printf("error in getting the number of modes that fit the display format.");
        MessageBox(hWnd, L"error in getting the number of modes that fit the display format.", L"Error", MB_OK);
        return E_FAIL;
    }

    // Create a list to hold all the possible display modes for this monitor/video card combination.
    displayModeList = new DXGI_MODE_DESC[numModes];
    if (!displayModeList)
    {
        printf("error creating DXGI_MODE_DESC.");
        MessageBox(hWnd, L"error creating DXGI_MODE_DESC.", L"Error", MB_OK);
        return E_FAIL;
    }

    // Now fill the display mode list structures.
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
    if (FAILED(result))
    {
        printf("error filling the display mode list structures.");
        MessageBox(hWnd, L"error filling the display mode list structures.", L"Error", MB_OK);
        return E_FAIL;
    }

    // Now go through all the display modes and find the one that matches the screen width and height.
    // When a match is found store the numerator and denominator of the refresh rate for that monitor.
    for (i = 0; i < numModes; i++)
    {
        if (displayModeList[i].Width == (unsigned int)screenWidth)
        {
            if (displayModeList[i].Height == (unsigned int)screenHeight)
            {
                numerator = displayModeList[i].RefreshRate.Numerator;
                denominator = displayModeList[i].RefreshRate.Denominator;
            }
        }
    }

    // Release the display mode list.
    delete[] displayModeList;
    displayModeList = 0;

    // Release the adapter output.
    adapterOutput->Release();
    adapterOutput = 0;

    // Release the factory.
    factory->Release();
    factory = 0;

    // Release the adapter.
    adapter->Release();
    adapter = 0;

    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    // Initialize the swap chain description.
    ZeroMemory(&sd, sizeof(sd));

    // Set the swap chain to use double buffering.
    sd.BufferCount = 2;

    // Set the width and height of the back buffer.
    sd.BufferDesc.Width = screenWidth;
    sd.BufferDesc.Height = screenHeight;

    // Set regular 32-bit surface for the back buffer.
    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;//DXGI_FORMAT_R8G8B8A8_UNORM;
    
    // Set the refresh rate of the back buffer.
    if (VSYNC_ENABLED)
    {
        sd.BufferDesc.RefreshRate.Numerator = numerator;
        sd.BufferDesc.RefreshRate.Denominator = denominator;
    }
    else
    {
        sd.BufferDesc.RefreshRate.Numerator = 0;
        sd.BufferDesc.RefreshRate.Denominator = 1;
    }

    // Set the usage of the back buffer.
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    // Set the handle for the window to render to.
    sd.OutputWindow = hWnd;

    // Turn multisampling off.
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;

    // Set to full screen or windowed mode.
    if (FULL_SCREEN)
    {
        sd.Windowed = false;
    }
    else
    {
        sd.Windowed = true;
    }

    // Set the scan line ordering and scaling to unspecified.
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    // Discard the back buffer contents after presenting.
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    // Don't set the advanced flags.
    sd.Flags = 0;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

    // Set the feature level to DirectX 11.
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    // Create the swap chain, Direct3D device, and Direct3D device context.
    if (D3D11CreateDeviceAndSwapChain(adapter, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, NULL, &g_pd3dDeviceContext) != S_OK)
    {
        printf("error D3D11CreateDeviceAndSwapChain.");
        MessageBox(hWnd, L"error D3D11CreateDeviceAndSwapChain.", L"Error", MB_OK);
        return E_FAIL;
    }

    CreateRenderTarget();
    return S_OK;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
    if (g_pDepthStencilView) { g_pDepthStencilView->Release(); g_pDepthStencilView = NULL; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    case WM_KEYDOWN:
        // If a key is pressed send it to the input object so it can record that state.
        //std::cout << "Key: " << (unsigned int)wparam << std::endl;

        if ((unsigned int)wParam == 27) PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
