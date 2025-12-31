#pragma once
#include "ecs/Component.hpp"
#include "math/Vec3.hpp"
#include "ecs/Entity.hpp"

class CRacketPhysical : public Component {
public: // <--- Add this line
    float restitution = 0.6f;
    float friction = 0.50f;
    Vec3 worldNormal;
    Entity racketShadow;
    bool offTable = false;
    Vec3 localNormal = { 0.f, 0.f, 1.f };
    bool validContact = true;

    CRacketPhysical() = default;
    explicit CRacketPhysical(const Entity& shadow)
        : racketShadow(shadow) {
    }
};