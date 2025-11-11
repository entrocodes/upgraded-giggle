#pragma once
#include "../ecs/Entity.hpp"
#include "../ecs/Component.hpp" // make sure Component is included
#include "../math/Vec2.hpp"
class CBall : public Component {
public:
    float ballHeight = 0.f;
    float verticalVel = 0.f;
    float gravity = 9.8f;      // m/s?(tune for your world scale)
    float restitution = 0.8f;
    float mass = .0027f; //kg
    Vec2 spin = { 0, 0 };
    Entity ballShadow;

    CBall() = default;

    explicit CBall(const Entity& shadow, float height = 0.f)
        : ballHeight(height), ballShadow(shadow) {
    }

    float getBallHeight() const { return ballHeight; }
};
