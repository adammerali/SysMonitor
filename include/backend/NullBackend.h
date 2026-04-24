#pragma once
#include "backend/GPUBackend.h"

namespace gm {

// Returned when no GPU vendor SDK is available.
// All metric fields are 0, valid = false.
class NullBackend final : public IGPUBackend {
public:
    bool             initialize()             override { return true; }
    GpuSnapshot      poll()                   override { return {}; }
    std::string_view backendName() const      override { return "None"; }
    void             shutdown()               override {}
};

} // namespace gm
