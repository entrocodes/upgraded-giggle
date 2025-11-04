#pragma once
#include "../ecs/Registry.hpp"
#include <SFML/System.hpp> // for sf::Time or deltaTime
#include <SFML/Graphics.hpp>
#include "../display/DisplayConfig.hpp"
class MovementSystem {
public:
    void update(Registry& registry, sf::RenderWindow& window, DisplayConfig& display, sf::Time dt);
};
