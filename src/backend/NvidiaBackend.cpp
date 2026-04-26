#ifdef GM_HAS_NVML
#include "backend/NvidiaBackend.h"
#include <cstring>

namespace gm {

bool NvidiaBackend::initialize()
{
    if (nvmlInit_v2() != NVML_SUCCESS)
        return false;

    if (nvmlDeviceGetHandleByIndex(0, &m_device) != NVML_SUCCESS) {
        nvmlShutdown();
        return false;
    }

    m_initialized = true;
    return true;
}

GpuSnapshot NvidiaBackend::poll()
{
    if (!m_initialized)
        return {};

    GpuSnapshot s;
    s.valid = true;

    // GPU name
    char name[NVML_DEVICE_NAME_BUFFER_SIZE]{};
    if (nvmlDeviceGetName(m_device, name, sizeof(name)) == NVML_SUCCESS)
        s.gpuName = name;

    // Utilization
    nvmlUtilization_t util{};
    if (nvmlDeviceGetUtilizationRates(m_device, &util) == NVML_SUCCESS)
        s.utilizationPercent = static_cast<float>(util.gpu);

    // VRAM
    nvmlMemory_t mem{};
    if (nvmlDeviceGetMemoryInfo(m_device, &mem) == NVML_SUCCESS) {
        s.vramUsedMB  = static_cast<float>(mem.used)  / (1024.f * 1024.f);
        s.vramTotalMB = static_cast<float>(mem.total) / (1024.f * 1024.f);
    }

    // Temperature
    unsigned int temp = 0;
    if (nvmlDeviceGetTemperature(m_device, NVML_TEMPERATURE_GPU, &temp) == NVML_SUCCESS)
        s.temperatureCelsius = static_cast<float>(temp);

    // Power (milliwatts → watts)
    unsigned int power = 0;
    if (nvmlDeviceGetPowerUsage(m_device, &power) == NVML_SUCCESS)
        s.powerDrawWatts = static_cast<float>(power) / 1000.f;

    // Clocks
    unsigned int clock = 0;
    if (nvmlDeviceGetClockInfo(m_device, NVML_CLOCK_GRAPHICS, &clock) == NVML_SUCCESS)
        s.coreClockMHz = static_cast<float>(clock);

    clock = 0;
    if (nvmlDeviceGetClockInfo(m_device, NVML_CLOCK_MEM, &clock) == NVML_SUCCESS)
        s.memClockMHz = static_cast<float>(clock);

    return s;
}

void NvidiaBackend::shutdown()
{
    if (m_initialized) {
        nvmlShutdown();
        m_initialized = false;
    }
}

} // namespace gm
#endif // GM_HAS_NVML
