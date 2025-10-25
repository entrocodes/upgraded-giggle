#pragma once
#include "../ecs/Registry.hpp"
#include "../input/RawInputState.hpp"

class PlayerInputSystem {
public:
    void update(Registry& registry, const RawInputState& input);
};
