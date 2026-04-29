#include "app/App.h"
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    // Enable Per-Monitor DPI awareness (Windows 10 1703+)
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    gm::App app;
    if (!app.initialize(hInstance))
        return 1;

    return app.run();
}
