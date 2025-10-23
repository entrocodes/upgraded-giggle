#pragma once
#include "../ecs/Component.hpp"
#include "../math/Vec2.hpp"

struct Transform : public Component {
    Vec2 position;
    float rotation = 0.f;
    Transform() = default;
    Transform(const Vec2 pos, float rot = 0.f)
        : position(pos), rotation(rot) {
    }
};
