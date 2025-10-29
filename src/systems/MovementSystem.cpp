#include "MovementSystem.hpp"
#include "BoundarySystem.hpp"
#include "../ecs/Registry.hpp"
#include "../components/Components.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
BoundarySystem Boundaries;

void MovementSystem::update(Registry& registry, sf::RenderWindow& window, sf::Time dt) {
    for (auto e : registry.getEntitiesWith<CTransform, Velocity>()) {
        auto* transform = registry.getComponent<CTransform>(e);
        auto* velocity = registry.getComponent<Velocity>(e);
        if (transform && velocity) {
            transform->position += velocity->velocity * dt.asSeconds();
        }
        Boundaries.update(registry, window);
    }
  

}

