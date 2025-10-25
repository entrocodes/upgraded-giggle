#pragma once
#include "../ecs/Registry.hpp"
#include "../components/InputComponent.hpp"
#include "../components/Velocity.hpp"
#include "../components/Transform.hpp"

class PlayerActionSystem {
public:
    void update(Registry& registry, float dt);
};
