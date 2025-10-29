#pragma once
#include "../ecs/Registry.hpp"
#include "../components/Components.hpp"
#include <SFML/System.hpp> // for sf::Time or deltaTime
#include <SFML/Graphics.hpp>
class BoundarySystem {
public:
    void update(Registry& registry, sf::RenderWindow& window);
};
