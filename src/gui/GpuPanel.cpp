#include "gui/GpuPanel.h"
#include "imgui.h"
#include "implot.h"
#include <cstdio>

namespace gm {

static float RingBufferGetter(void* data, int idx)
{
    auto* rb = static_cast<const RingBuffer<>*>(data);
    int   sz  = static_cast<int>(rb->data.size());
    int   pos = (rb->offset - rb->count + idx + sz) % sz;
    return rb->data[static_cast<std::size_t>(pos)];
}

void GpuPanel::renderGraph(const char* label,
                            const RingBuffer<>& rb,
                            float yMin, float yMax,
                            const char* unit,
                            float currentValue)
{
    char title[64];
    std::snprintf(title, sizeof(title), "%.1f %s##%s", currentValue, unit, label);

    if (ImPlot::BeginPlot(label, ImVec2(-1, 100), ImPlotFlags_NoLegend | ImPlotFlags_NoMouseText)) {
        ImPlot::SetupAxes(nullptr, nullptr,
                          ImPlotAxisFlags_NoDecorations,
                          ImPlotAxisFlags_NoDecorations);
        ImPlot::SetupAxisLimits(ImAxis_Y1, yMin, yMax, ImPlotCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, static_cast<double>(rb.data.size()), ImPlotCond_Always);
        ImPlot::PlotLineG(label, RingBufferGetter, const_cast<RingBuffer<>*>(&rb), rb.count);
        ImPlot::EndPlot();
    }
    ImGui::SameLine();
    ImGui::Text("%.1f %s", currentValue, unit);
}

void GpuPanel::render(const GpuSnapshot& latest,
                       const MetricsStore& store,
                       std::string_view backendName)
{
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.f));
    ImGui::BeginChild("GPU", ImVec2(0, 0), true);

    if (!latest.valid) {
        ImGui::TextDisabled("No GPU SDK available.");
        ImGui::TextDisabled("Install the CUDA Toolkit (NVIDIA) or place the ADLX SDK");
        ImGui::TextDisabled("in third_party/adlx/ (AMD), then rebuild.");
        ImGui::EndChild();
        ImGui::PopStyleColor();
        return;
    }

    ImGui::Text("GPU: %s", latest.gpuName.c_str());
    ImGui::SameLine();
    ImGui::TextDisabled("via %.*s", static_cast<int>(backendName.size()), backendName.data());
    ImGui::Separator();

    // Utilization
    ImGui::Text("Utilization");
    renderGraph("Util##gpu", store.gpuUtil, 0.f, 100.f, "%%", latest.utilizationPercent);

    // GPU Busy (PDH cross-vendor)
    ImGui::Text("GPU Busy");
    renderGraph("Busy##gpu", store.gpuBusy, 0.f, 100.f, "%%", latest.gpuBusyPercent);

    // VRAM
    float vramMax = latest.vramTotalMB > 0.f ? latest.vramTotalMB : 8192.f;
    ImGui::Text("VRAM  (%.0f MB total)", latest.vramTotalMB);
    renderGraph("VRAM##gpu", store.gpuVramUsed, 0.f, vramMax, "MB", latest.vramUsedMB);

    // Temperature
    ImGui::Text("Temperature");
    renderGraph("Temp##gpu", store.gpuTemp, 0.f, 110.f, "\xc2\xb0""C", latest.temperatureCelsius);

    // Power
    ImGui::Text("Power Draw");
    renderGraph("Power##gpu", store.gpuPower, 0.f, 400.f, "W", latest.powerDrawWatts);

    // Core clock
    ImGui::Text("Core Clock");
    renderGraph("CoreClk##gpu", store.gpuCoreClock, 0.f, 3500.f, "MHz", latest.coreClockMHz);

    // Memory clock
    ImGui::Text("Mem Clock");
    renderGraph("MemClk##gpu", store.gpuMemClock, 0.f, 2500.f, "MHz", latest.memClockMHz);

    ImGui::EndChild();
    ImGui::PopStyleColor();
}

} // namespace gm
