#pragma once
#include "../math/Vec2.hpp"
#include "../ecs/Component.hpp"


struct CTransform : public Component {
    Vec2 position;
    float rotation = 0.f;
    CTransform() = default;
    CTransform(const Vec2 pos, float rot = 0.f)
        : position(pos), rotation(rot) {
    }
};
