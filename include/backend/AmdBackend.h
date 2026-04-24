#pragma once
#ifdef GM_HAS_ADLX

#include "backend/GPUBackend.h"

// Forward declarations to avoid pulling ADLX headers into every TU.
namespace adlx {
    class IADLXSystem;
    class IADLXGPU;
    class IADLXPerformanceMonitoringServices;
}

namespace gm {

class AmdBackend final : public IGPUBackend {
public:
    bool             initialize()             override;
    GpuSnapshot      poll()                   override;
    std::string_view backendName() const      override { return "ADLX (AMD)"; }
    void             shutdown()               override;

private:
    // ADLX uses COM-style ref counting; raw pointers, released in shutdown().
    adlx::IADLXSystem*                       m_system  = nullptr;
    adlx::IADLXGPU*                          m_gpu     = nullptr;
    adlx::IADLXPerformanceMonitoringServices* m_perfSvc = nullptr;
};

} // namespace gm
#endif // GM_HAS_ADLX
