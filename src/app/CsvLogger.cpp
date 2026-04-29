#include "app/CsvLogger.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace gm {

bool CsvLogger::start()
{
    // Build filename: sysmonitor_YYYYMMDD_HHMMSS.csv
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream ss;
    ss << "sysmonitor_" << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".csv";
    m_path = ss.str();

    m_file.open(m_path);
    if (!m_file.is_open())
        return false;

    // Header
    m_file << "timestamp,"
           << "gpu_util_%,gpu_busy_%,vram_used_mb,vram_total_mb,"
           << "temp_c,power_w,core_mhz,mem_mhz,"
           << "cpu_util_%,cpu_interrupts_s,"
           << "ram_used_mb,ram_total_mb\n";
    return true;
}

void CsvLogger::stop()
{
    if (m_file.is_open())
        m_file.close();
}

void CsvLogger::log(const GpuSnapshot& gpu, const CpuSnapshot& cpu, const RamSnapshot& ram)
{
    if (!m_file.is_open())
        return;

    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif

    m_file << std::put_time(&tm, "%H:%M:%S") << ","
           << gpu.utilizationPercent << ","
           << gpu.gpuBusyPercent     << ","
           << gpu.vramUsedMB         << ","
           << gpu.vramTotalMB        << ","
           << gpu.temperatureCelsius << ","
           << gpu.powerDrawWatts     << ","
           << gpu.coreClockMHz       << ","
           << gpu.memClockMHz        << ","
           << cpu.totalUsagePercent  << ","
           << cpu.interruptsPerSec   << ","
           << ram.usedMB             << ","
           << ram.totalMB            << "\n";

    m_file.flush();
}

} // namespace gm
