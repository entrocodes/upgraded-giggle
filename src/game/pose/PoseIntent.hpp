#pragma once

#include "Pose.hpp"

enum class PoseIntentType { Translate, Rotate };

struct PoseIntent {
    PoseJointID joint;
    PoseIntentType type;
    Vec3 desiredDelta_m;
    float weight;
    float priority;
};
