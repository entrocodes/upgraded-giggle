#pragma once
#include <SFML/Graphics.hpp>
#include "../ecs/Registry.hpp"

struct RenderSystem {
    void render(sf::RenderWindow& window, Registry& registry);
};
