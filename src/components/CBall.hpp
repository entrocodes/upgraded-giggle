#pragma once
#include "../ecs/Entity.hpp"
#include "../ecs/Component.hpp" // make sure Component is included
#include "../math/Vec3.hpp"
#include "../math/Bounds3D.hpp"
class CBall : public Component {
public:
    // world-space (meters)
    float ballRadius = 0.02f; // 40mm diameter / 2

    Vec3 spin = { 0.0f, 0.0f, 0.0f }; 

    float gravity = 9.8f;      // m/s?(tune for your world scale)
    float restitution = 0.8f;
    float mass = .0027f; //kg
    bool hasFallen = false;
    bool hitNet = false;
    Entity ballShadow;

    CBall() = default;

    explicit CBall(const Entity& shadow)
        : ballShadow(shadow) {
    }
};
