#pragma once

#include <cstdint>

enum class PoseIntentPhase : uint8_t {
    Support = 0,
    Load,
    Commit,
    Translate,
    Recover,
    Count
};

static constexpr const char* kPoseIntentPhaseNames[] = {
    "Support",
    "Load",
    "Commit",
    "Translate",
    "Recover"
};
