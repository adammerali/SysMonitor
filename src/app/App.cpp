#include "app/App.h"
#include "gui/MainWindow.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "implot.h"
#include <stdexcept>
#include <chrono>

// Required by ImGui Win32 backend
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace gm {

// We store a pointer to the App so WndProc can call back into it.
static App* g_appPtr = nullptr;

App::App()
{
    g_appPtr = this;
}

App::~App()
{
    g_appPtr = nullptr;
}

bool App::initialize(HINSTANCE hInstance)
{
    // Register window class
    m_wc = {};
    m_wc.cbSize        = sizeof(WNDCLASSEXW);
    m_wc.style         = CS_CLASSDC;
    m_wc.lpfnWndProc   = WndProc;
    m_wc.hInstance     = hInstance;
    m_wc.lpszClassName = L"GpuMonitorWnd";
    RegisterClassExW(&m_wc);

    m_hwnd = CreateWindowExW(
        0, m_wc.lpszClassName,
        L"GPU/CPU Performance Monitor",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        static_cast<int>(m_config.windowWidth),
        static_cast<int>(m_config.windowHeight),
        nullptr, nullptr, hInstance, nullptr);

    if (!m_hwnd)
        return false;

    if (!createDeviceAndSwapChain(m_hwnd))
        return false;

    createRenderTarget();
    ShowWindow(m_hwnd, SW_SHOWDEFAULT);
    UpdateWindow(m_hwnd);

    initImGui(m_hwnd);

    // Initialise subsystems
    m_gpuBackend = createGPUBackend();
    if (!m_sysMetrics.initialize())
        return false;

    // Start the poller thread
    m_pollerThread = std::jthread([this](std::stop_token st) {
        pollerThreadFunc(st);
    });

    m_running = true;
    return true;
}

bool App::createDeviceAndSwapChain(HWND hwnd)
{
    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount                        = 2;
    sd.BufferDesc.Width                   = 0;
    sd.BufferDesc.Height                  = 0;
    sd.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator   = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags                              = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow                       = hwnd;
    sd.SampleDesc.Count                   = 1;
    sd.SampleDesc.Quality                 = 0;
    sd.Windowed                           = TRUE;
    sd.SwapEffect                         = DXGI_SWAP_EFFECT_DISCARD;

    UINT createFlags = 0;
#ifdef _DEBUG
    createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
        createFlags, levels, 2,
        D3D11_SDK_VERSION, &sd,
        &m_swapChain, &m_device,
        &featureLevel, &m_context);

    return SUCCEEDED(hr);
}

void App::createRenderTarget()
{
    ID3D11Texture2D* back = nullptr;
    m_swapChain->GetBuffer(0, IID_PPV_ARGS(&back));
    m_device->CreateRenderTargetView(back, nullptr, &m_mainRTV);
    back->Release();
}

void App::destroyRenderTarget()
{
    if (m_mainRTV) {
        m_mainRTV->Release();
        m_mainRTV = nullptr;
    }
}

void App::initImGui(HWND hwnd)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename = nullptr;  // don't save imgui.ini

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(m_device, m_context);
}

void App::shutdownImGui()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}

void App::pollerThreadFunc(std::stop_token st)
{
    while (!st.stop_requested()) {
        auto next = std::chrono::steady_clock::now()
                  + std::chrono::milliseconds(m_updateIntervalMs.load(std::memory_order_relaxed));

        GpuSnapshot gpu = m_gpuBackend->poll();
        float gpuBusy = 0.f;

        CpuSnapshot cpu{};
        RamSnapshot ram{};
        m_sysMetrics.poll(cpu, ram, gpuBusy);

        gpu.gpuBusyPercent = gpuBusy;
        m_store.pushGpu(gpu);
        m_store.pushCpu(cpu);
        m_store.pushRam(ram);

        if (m_csvLogger.isLogging())
            m_csvLogger.log(gpu, cpu, ram);

        std::this_thread::sleep_until(next);
    }
}

int App::run()
{
    MainWindow mainWindow;
    constexpr float clearColor[4] = { 0.08f, 0.08f, 0.08f, 1.f };

    MSG msg{};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        auto frame = m_store.readFrame();

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        mainWindow.render(frame, m_store,
                          m_gpuBackend->backendName(),
                          m_updateIntervalMs,
                          m_csvLogger);

        ImGui::Render();

        m_context->OMSetRenderTargets(1, &m_mainRTV, nullptr);
        m_context->ClearRenderTargetView(m_mainRTV, clearColor);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        m_swapChain->Present(1, 0);
    }

    return static_cast<int>(msg.wParam);
}

void App::shutdown()
{
    m_pollerThread.request_stop();
    // jthread destructor joins automatically

    m_gpuBackend->shutdown();
    m_sysMetrics.shutdown();

    shutdownImGui();
    destroyRenderTarget();

    if (m_swapChain) { m_swapChain->Release(); m_swapChain = nullptr; }
    if (m_context)   { m_context->Release();   m_context   = nullptr; }
    if (m_device)    { m_device->Release();     m_device    = nullptr; }

    DestroyWindow(m_hwnd);
    UnregisterClassW(m_wc.lpszClassName, m_wc.hInstance);
}

LRESULT WINAPI App::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {
    case WM_SIZE:
        if (g_appPtr && g_appPtr->m_device && wParam != SIZE_MINIMIZED) {
            g_appPtr->destroyRenderTarget();
            g_appPtr->m_swapChain->ResizeBuffers(0,
                LOWORD(lParam), HIWORD(lParam),
                DXGI_FORMAT_UNKNOWN, 0);
            g_appPtr->createRenderTarget();
        }
        return 0;

    case WM_SYSCOMMAND:
        if ((wParam & 0xFFF0) == SC_KEYMENU)  // suppress Alt menu flicker
            return 0;
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

} // namespace gm
