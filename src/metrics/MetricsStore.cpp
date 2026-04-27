#include "metrics/MetricsStore.h"

namespace gm {

void MetricsStore::pushGpu(const GpuSnapshot& s)
{
    std::lock_guard lock(mtx);
    latestGpu = s;
    if (s.valid) {
        gpuUtil.push(s.utilizationPercent);
        gpuVramUsed.push(s.vramUsedMB);
        gpuTemp.push(s.temperatureCelsius);
        gpuPower.push(s.powerDrawWatts);
        gpuCoreClock.push(s.coreClockMHz);
        gpuMemClock.push(s.memClockMHz);
    }
}

void MetricsStore::pushCpu(const CpuSnapshot& s)
{
    std::lock_guard lock(mtx);
    latestCpu = s;
    cpuUtil.push(s.totalUsagePercent);
}

void MetricsStore::pushRam(const RamSnapshot& s)
{
    std::lock_guard lock(mtx);
    latestRam = s;
    ramUsed.push(s.usedMB);
}

MetricsStore::Frame MetricsStore::readFrame()
{
    std::lock_guard lock(mtx);
    return { latestGpu, latestCpu, latestRam };
}

} // namespace gm
