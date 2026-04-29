#pragma once
#include "backend/GPUBackend.h"
#include <array>
#include <mutex>
#include <cstddef>

namespace gm {

struct CpuSnapshot {
    float totalUsagePercent  = 0.f;  // 0–100, system-wide
    float interruptsPerSec   = 0.f;  // hardware interrupts/sec
};

struct RamSnapshot {
    float usedMB  = 0.f;
    float totalMB = 0.f;
};

// Ring buffer for ImPlot history graphs.
// N = number of samples kept (default 300 ≈ 5 min at 1 Hz).
template<std::size_t N = 300>
struct RingBuffer {
    std::array<float, N> data{};
    int offset = 0;   // next write position
    int count  = 0;   // filled slots, caps at N

    void push(float v) noexcept {
        data[static_cast<std::size_t>(offset)] = v;
        offset = (offset + 1) % static_cast<int>(N);
        if (count < static_cast<int>(N)) ++count;
    }
};

// Central data store. Written by the poller thread, read by the GUI thread.
// All access is protected by a single mutex; GUI copies under the lock then
// renders without holding it.
struct MetricsStore {
    std::mutex mtx;

    GpuSnapshot latestGpu;
    CpuSnapshot latestCpu;
    RamSnapshot latestRam;

    RingBuffer<> gpuUtil;
    RingBuffer<> gpuVramUsed;
    RingBuffer<> gpuTemp;
    RingBuffer<> gpuPower;
    RingBuffer<> gpuCoreClock;
    RingBuffer<> gpuMemClock;
    RingBuffer<> gpuBusy;
    RingBuffer<> cpuUtil;
    RingBuffer<> cpuInterrupts;
    RingBuffer<> ramUsed;

    // Called by the poller thread only.
    void pushGpu(const GpuSnapshot& s);
    void pushCpu(const CpuSnapshot& s);
    void pushRam(const RamSnapshot& s);

    // Lightweight snapshot for the GUI thread: copy under lock, then release.
    struct Frame {
        GpuSnapshot gpu;
        CpuSnapshot cpu;
        RamSnapshot ram;
    };
    Frame readFrame();
};

} // namespace gm
