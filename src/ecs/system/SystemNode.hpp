#pragma once

#include <memory>
#include <string>
#include "ISystem.hpp"
#include "TickPhase.hpp"

struct SystemDebugInfo {
    uint64_t lastFrameRan = 0;
    uint64_t runCount = 0;
    uint64_t earlyExitCount = 0;

    bool exitedEarlyLastRun = false;
    std::string lastEarlyExitReason;
};


struct SystemNode {
    std::unique_ptr<ISystem> system;

    int order = 0;
    TickPhase phase = TickPhase::Fixed;
    bool enabled = true;
    bool pausable = false;

    SystemDebugInfo debug;
};
