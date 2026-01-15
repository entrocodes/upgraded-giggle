#pragma once

#include "Pose.hpp"
#include "PoseIntentPhase.hpp"

enum class PoseIntentType {
    LoadBody,      // compress into base (squat / lunge prep)
    ShiftBody,     // shift COM horizontally
    PushOff,       // explode upward / backward
    Recover,        // return to neutral stance
    Translate,      // NEEDS DECOMPED
    Rotate          // NEEDS DECOMPED
};

struct PoseIntent {
    PoseJointID joint;
    PoseIntentPhase phase;
    int stage = 0;
    PoseIntentType type;
    Vec3 desiredDelta_m = Vec3(0,0,0);
};
