#pragma once
#include "../ecs/Component.hpp"
#include "../math/Vec2.hpp"

struct Velocity : public Component {
    Vec2 velocity{ 0.f, 0.f }; // default stationary
    Velocity() = default;
    Velocity(const Vec2& v) : velocity(v) {}
};
