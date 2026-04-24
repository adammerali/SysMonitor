#pragma once
#include "app/Config.h"
#include "backend/GPUBackend.h"
#include "metrics/MetricsStore.h"
#include "metrics/SystemMetrics.h"
#include <d3d11.h>
#include <memory>
#include <thread>
#include <atomic>

namespace gm {

class App {
public:
    App();
    ~App();

    bool initialize(HINSTANCE hInstance);
    int  run();

private:
    // DX11 setup / teardown
    bool createDeviceAndSwapChain(HWND hwnd);
    void createRenderTarget();
    void destroyRenderTarget();
    void shutdown();

    // ImGui
    void initImGui(HWND hwnd);
    void shutdownImGui();

    // Poller
    void pollerThreadFunc(std::stop_token st);

    // Win32 message proc (static trampoline)
    static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    // DX11
    ID3D11Device*            m_device      = nullptr;
    ID3D11DeviceContext*     m_context     = nullptr;
    IDXGISwapChain*          m_swapChain   = nullptr;
    ID3D11RenderTargetView*  m_mainRTV     = nullptr;
    HWND                     m_hwnd        = nullptr;
    WNDCLASSEXW              m_wc{};

    // Subsystems
    std::unique_ptr<IGPUBackend> m_gpuBackend;
    SystemMetrics                m_sysMetrics;
    MetricsStore                 m_store;
    Config                       m_config;

    // Polling thread
    std::jthread          m_pollerThread;
    std::atomic<int>      m_updateIntervalMs{ 1000 };

    bool m_running = false;
};

} // namespace gm
