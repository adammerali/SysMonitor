#include "backend/GPUBackend.h"
#include "backend/NullBackend.h"

#ifdef GM_HAS_NVML
#include "backend/NvidiaBackend.h"
#endif

#ifdef GM_HAS_ADLX
#include "backend/AmdBackend.h"
#endif

#ifdef GM_HAS_INTEL
#include "backend/IntelBackend.h"
#endif

namespace gm {

std::unique_ptr<IGPUBackend> createGPUBackend()
{
#ifdef GM_HAS_NVML
    {
        auto b = std::make_unique<NvidiaBackend>();
        if (b->initialize())
            return b;
    }
#endif

#ifdef GM_HAS_ADLX
    {
        auto b = std::make_unique<AmdBackend>();
        if (b->initialize())
            return b;
    }
#endif

#ifdef GM_HAS_INTEL
    {
        auto b = std::make_unique<IntelBackend>();
        if (b->initialize())
            return b;
    }
#endif

    return std::make_unique<NullBackend>();
}

} // namespace gm
