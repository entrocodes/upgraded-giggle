#pragma once

#include "ecs/Component.hpp"

struct CBodyTableCollision : public Component {
    Vec3 penetration = { 0,0,0 };
    CBodyTableCollision(const Vec3 pPenetration)
        : penetration(pPenetration) { }
};
