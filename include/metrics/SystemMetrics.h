#pragma once
#include "metrics/MetricsStore.h"
#include <windows.h>
#include <pdh.h>

namespace gm {

// Polls CPU/RAM via PDH + GlobalMemoryStatusEx, and GPU busy via PDH GPU Engine counters.
class SystemMetrics {
public:
    bool initialize();
    void poll(CpuSnapshot& cpu, RamSnapshot& ram, float& gpuBusyOut);
    void shutdown();

private:
    PDH_HQUERY   m_query            = nullptr;
    PDH_HCOUNTER m_cpuCounter       = nullptr;
    PDH_HCOUNTER m_interruptCounter = nullptr;
    PDH_HCOUNTER m_gpuBusyCounter   = nullptr;
    bool         m_primed           = false;
};

} // namespace gm
