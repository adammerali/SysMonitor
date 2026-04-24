#pragma once
#ifdef GM_HAS_NVML

#include "backend/GPUBackend.h"
#include <nvml.h>

namespace gm {

class NvidiaBackend final : public IGPUBackend {
public:
    bool             initialize()             override;
    GpuSnapshot      poll()                   override;
    std::string_view backendName() const      override { return "NVML (NVIDIA)"; }
    void             shutdown()               override;

private:
    nvmlDevice_t m_device{};
    bool         m_initialized = false;
};

} // namespace gm
#endif // GM_HAS_NVML
