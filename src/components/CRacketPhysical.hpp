#pragma once
#include "../ecs/Component.hpp"
#include "../math/Vec3.hpp"

struct CRacketPhysical : public Component {
    float restitution = 0.85f;   // bounce
    float friction = 0.50f;      // spin transfer
    Vec3 normal = { 0, 0, 1 };     // facing opponent by default
    bool validContact = true;
};
