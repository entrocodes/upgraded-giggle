#pragma once
#include "../ecs/Entity.hpp"
#include "../ecs/Component.hpp" // make sure Component is included
#include "../math/GridTransform.hpp"
class CBall : public Component {
public:
    float ballHeight = 0.f;
    Entity ballShadow;

    CBall() = default;

    explicit CBall(const Entity& shadow, float height = 0.f)
        : ballHeight(Grid::toWorldX(height)), ballShadow(shadow) {
    }

    float getBallHeight() const { return ballHeight; }
};
