#pragma once
#include "../ecs/Registry.hpp"
#include <SFML/System.hpp> // for sf::Time or deltaTime
#include <SFML/Graphics.hpp>

class BallGravitySystem {
public:
    void update(Registry& registry, sf::RenderWindow& window);
};
