#pragma once
#include "metrics/MetricsStore.h"
#include <windows.h>
#include <pdh.h>

namespace gm {

// Polls CPU usage via PDH and RAM via GlobalMemoryStatusEx.
class SystemMetrics {
public:
    bool initialize();
    void poll(CpuSnapshot& cpu, RamSnapshot& ram);
    void shutdown();

private:
    PDH_HQUERY   m_query      = nullptr;
    PDH_HCOUNTER m_cpuCounter = nullptr;
    bool         m_primed     = false;  // first PDH sample collected?
};

} // namespace gm
