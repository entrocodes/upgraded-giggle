#pragma once

#include "Pose.hpp"

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
    PoseIntentType type;
    Vec3 desiredDelta_m;
    float weight; //multiplier
    float order = 1;
    float magnitude = 1;
};
