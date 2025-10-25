#include "MovementSystem.hpp"
#include "BoundarySystem.hpp"
#include "../ecs/Registry.hpp"
#include "../components/Transform.hpp"
#include "../components/Velocity.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
BoundarySystem Boundaries;

void MovementSystem::update(Registry& registry, sf::RenderWindow& window, float dt) {
    for (auto e : registry.getEntitiesWith<Transform, Velocity>()) {
        auto* transform = registry.getComponent<Transform>(e);
        auto* velocity = registry.getComponent<Velocity>(e);
        if (transform && velocity) {
            transform->position += velocity->velocity * dt;
        }
        Boundaries.updatePlayer(registry, window, dt);
    }
  

}

