#pragma once
#include "backend/GPUBackend.h"
#include "metrics/MetricsStore.h"
#include <fstream>
#include <string>

namespace gm {

class CsvLogger {
public:
    // Opens a timestamped CSV file in the current directory. Returns false on failure.
    bool start();
    void stop();
    bool isLogging() const { return m_file.is_open(); }

    void log(const GpuSnapshot& gpu, const CpuSnapshot& cpu, const RamSnapshot& ram);

private:
    std::ofstream m_file;
    std::string   m_path;
};

} // namespace gm
