#pragma once
#include "../ecs/Component.hpp"
#include "../math/Vec2.hpp"

struct CVelocity : public Component {
    Vec2 velocity{ 0.f, 0.f }; // default stationary
    CVelocity() = default;
    CVelocity(const Vec2& v) : velocity(v) {}
};
