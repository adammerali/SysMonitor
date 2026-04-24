#pragma once

namespace gm {

struct Config {
    int   updateIntervalMs = 1000;   // poller tick rate
    float windowWidth      = 900.f;
    float windowHeight     = 700.f;
};

} // namespace gm
