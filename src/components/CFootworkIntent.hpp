#pragma once
#include "ecs/Component.hpp"
struct CFootworkIntent : public Component {
    Vec3 direction;  
    float heldFrames;
    float directionalStrength = 1.0f;
};
