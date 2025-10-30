#pragma once
#include "../math/Vec2.hpp"
#include "../ecs/Component.hpp"


struct CTransform : public Component {
    Vec2 position;
    Vec2 scale;
    float rotation = 0.f;
    CTransform() = default;
    CTransform(const Vec2 pos, const Vec2 s = { 0.f, 0.f }, float rot = 0.f)
        : position(pos), scale(s), rotation(rot){
    }
};
