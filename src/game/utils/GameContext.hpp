#pragma once

#include <SFML/Graphics.hpp>

#include "../display/DisplayConfig.hpp"
#include "Assets.hpp"
#include "../ecs/Registry.hpp"
#include "../input/RawInputState.hpp"
struct GameContext {
    sf::RenderWindow window;
    DisplayConfig display;
    Assets assets;
    Registry registry;
    RawInputState rawInput;
};