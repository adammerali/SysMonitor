#pragma once
#include "gui/GpuPanel.h"
#include "gui/CpuPanel.h"
#include "app/CsvLogger.h"
#include "metrics/MetricsStore.h"
#include <string_view>
#include <atomic>

namespace gm {

class MainWindow {
public:
    // frame is a plain value copy — no lock needed during render.
    void render(const MetricsStore::Frame& frame,
                const MetricsStore& store,
                std::string_view gpuBackendName,
                std::atomic<int>& updateIntervalMs,
                CsvLogger& csvLogger);

private:
    GpuPanel m_gpuPanel;
    CpuPanel m_cpuPanel;
    bool     m_showSettings = false;
};

} // namespace gm
