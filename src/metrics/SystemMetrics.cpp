#include "metrics/SystemMetrics.h"
#pragma comment(lib, "pdh.lib")

namespace gm {

bool SystemMetrics::initialize()
{
    if (PdhOpenQuery(nullptr, 0, &m_query) != ERROR_SUCCESS)
        return false;

    if (PdhAddEnglishCounterW(m_query,
            L"\\Processor(_Total)\\% Processor Time",
            0, &m_cpuCounter) != ERROR_SUCCESS) {
        PdhCloseQuery(m_query);
        m_query = nullptr;
        return false;
    }

    // Prime: PDH needs one collection before it can produce a value.
    PdhCollectQueryData(m_query);
    m_primed = true;
    return true;
}

void SystemMetrics::poll(CpuSnapshot& cpu, RamSnapshot& ram)
{
    // CPU via PDH
    if (m_query) {
        PdhCollectQueryData(m_query);
        PDH_FMT_COUNTERVALUE val{};
        DWORD type = 0;
        if (PdhGetFormattedCounterValue(m_cpuCounter, PDH_FMT_DOUBLE, &type, &val) == ERROR_SUCCESS)
            cpu.totalUsagePercent = static_cast<float>(val.doubleValue);
    }

    // RAM via GlobalMemoryStatusEx
    MEMORYSTATUSEX ms{};
    ms.dwLength = sizeof(ms);
    if (GlobalMemoryStatusEx(&ms)) {
        constexpr float toMB = 1.f / (1024.f * 1024.f);
        ram.totalMB = static_cast<float>(ms.ullTotalPhys)  * toMB;
        ram.usedMB  = static_cast<float>(ms.ullTotalPhys - ms.ullAvailPhys) * toMB;
    }
}

void SystemMetrics::shutdown()
{
    if (m_query) {
        PdhCloseQuery(m_query);
        m_query = nullptr;
    }
}

} // namespace gm
