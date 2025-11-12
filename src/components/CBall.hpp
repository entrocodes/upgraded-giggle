#pragma once
#include "../ecs/Entity.hpp"
#include "../ecs/Component.hpp" // make sure Component is included
#include "../math/Vec3.hpp"
class CBall : public Component {
public:
    // world-space (meters)
    Vec3 pos_m = { 0.f, 0.f, 0.f };   // (x, y, z) in meters
    Vec3 vel_mps = { 0.f, 0.f, 0.f }; // (vx, vy, vz) m/s
    Vec3 spin = { 0.0f, 0.0f, 0.0f }; 

    float gravity = 9.8f;      // m/s?(tune for your world scale)
    float restitution = 0.8f;
    float mass = .0027f; //kg
    bool hasFallen = false;
    Entity ballShadow;

    CBall() = default;

    explicit CBall(const Entity& shadow, Vec3 pPos_m = {0.0f, 0.0f, 0.0f}, Vec3 pVel_mps = {0.0f,0.0f,0.0f})
        : pos_m(pPos_m), vel_mps(pVel_mps), ballShadow(shadow) {
    }
};
