#pragma once
#include "ecs/Component.hpp"
#include "math/Vec3.hpp"

struct CArm : public Component {
    Vec3 shoulderPos_m;
    float maxReach_m = 0.45f; // ~45 cm
};
