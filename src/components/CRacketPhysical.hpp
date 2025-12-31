#pragma once
#include "ecs/Component.hpp"
#include "math/Vec3.hpp"
#include "ecs/Entity.hpp"

class CRacketPhysical : public Component {
public: // <--- Add this line
    float restitution = 0.6f;
    float friction = 0.50f;
    Vec3 worldNormal;
    Vec3 worldUp = { 0.f, 1.f, 0.f };
    Entity racketShadow;
    bool offTable = false;
    Vec3 localNormal = { 0.f, 0.f, 1.f };
    bool validContact = true;
    float radius_m = .08f;
    CRacketPhysical() = default;
    explicit CRacketPhysical(const Entity& shadow)
        : racketShadow(shadow) {
    }
};