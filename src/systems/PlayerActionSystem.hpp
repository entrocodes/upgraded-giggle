#pragma once
#include "../ecs/Registry.hpp"

class PlayerActionSystem {
public:
    void update(Registry& registry, float dt);
};
