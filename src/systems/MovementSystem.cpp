#include "MovementSystem.hpp"
#include "../components/Components.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
void MovementSystem::update(GameContext* context, sf::Time dt) {
    ballMovement.update(context, dt.asSeconds()); //currently handles force calculation and bounce, as well as ball movement
    //apply velocity to position
    for (auto e : context->registry.getEntitiesWith<CTransform, Velocity>()) {
        auto transform = context->registry.getComponent<CTransform>(e);
        auto velocity = context->registry.getComponent<Velocity>(e);
        if (transform && velocity) {
            transform->position += velocity->velocity * dt.asSeconds();
        }
    }
    boundaries.update(context);

}

