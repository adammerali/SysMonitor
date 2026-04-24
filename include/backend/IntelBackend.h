#pragma once
#ifdef GM_HAS_INTEL

#include "backend/GPUBackend.h"
#include <windows.h>
#include <pdh.h>
#include <dxgi.h>
#include <string>

namespace gm {

// Uses PDH GPU Engine counters (available on Windows 10 1903+) filtered
// to the Intel adapter LUID obtained via DXGI enumeration.
class IntelBackend final : public IGPUBackend {
public:
    bool             initialize()             override;
    GpuSnapshot      poll()                   override;
    std::string_view backendName() const      override { return "PDH (Intel iGPU)"; }
    void             shutdown()               override;

private:
    PDH_HQUERY   m_query         = nullptr;
    PDH_HCOUNTER m_utilCounter   = nullptr;
    bool         m_initialized   = false;
    std::wstring m_adapterName;
    float        m_lastUtil      = 0.f;
};

} // namespace gm
#endif // GM_HAS_INTEL
