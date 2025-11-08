#pragma once

#include <SFML/Graphics.hpp>
#include "../display/DisplayConfig.hpp"
#include "Assets.hpp"
#include "../ecs/Registry.hpp"
#include "../input/RawInputState.hpp"
#include "../game/EntityFactory.hpp"   // Full include, since we create an instance

struct GameContext {
    sf::RenderWindow window;
    DisplayConfig display;
    Assets assets;
    Registry registry;
    RawInputState rawInput;
    EntityFactory entityFactory;

    GameContext()
        : entityFactory(registry, display, assets) {
    }
};
