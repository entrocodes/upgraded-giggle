#pragma once
#include "../ecs/Registry.hpp"
#include "../components/InputComponent.hpp"
#include <SFML/Window/Keyboard.hpp>

class PlayerInputSystem {
public:
    void update(Registry& registry);
};
