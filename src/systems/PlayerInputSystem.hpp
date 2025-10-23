#pragma once
#include "../ecs/Registry.hpp"
#include "../components/Velocity.hpp"
#include "../components/Player.hpp"
#include <SFML/Window/Keyboard.hpp>

class PlayerInputSystem {
public:
    void update(Registry& registry, float dt);
};
