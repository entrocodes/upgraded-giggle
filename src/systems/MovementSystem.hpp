#pragma once
#include "../ecs/Registry.hpp"
#include "../components/Transform.hpp"
#include "../components/Velocity.hpp"
#include <SFML/System.hpp> // for sf::Time or deltaTime
#include <SFML/Graphics.hpp>

class MovementSystem {
public:
    void update(Registry& registry, sf::RenderWindow& window, float deltaTime);
};
