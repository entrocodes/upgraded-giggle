#pragma once

#include "Pose.hpp"

enum class PoseIntentType { Translate, Rotate, LoadAnkle };

struct PoseIntent {
    PoseJointID joint;
    PoseIntentType type;
    Vec3 desiredDelta_m;
    float weight; //multiplier
    float priority = 1;
    float magnitude = 1;
};
