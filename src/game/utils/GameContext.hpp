#pragma once

#include <SFML/Graphics.hpp>

#include "../../display/DisplayConfig.hpp"
#include "Assets.hpp"

struct GameContext {
    sf::RenderWindow window;
    DisplayConfig display;
    Assets assets;
};