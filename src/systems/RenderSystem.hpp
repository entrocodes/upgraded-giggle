#pragma once
#include <SFML/Graphics.hpp>
#include "../ecs/Registry.hpp"
#include "../game/utils/Camera.hpp"

struct RenderSystem {
    // Pass camera to handle view + zoom
    void render(sf::RenderWindow& window, Registry& registry, const Camera& camera);
};
