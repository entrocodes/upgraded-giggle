#include "MovementSystem.hpp"
#include "BoundarySystem.hpp"
#include "BallGravitySystem.hpp"
#include "BallForceSystem.hpp"
#include "../components/Components.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
BoundarySystem Boundaries;
BallForceSystem ballForces;
void MovementSystem::update(GameContext* context, sf::Time dt) {
    ballForces.update(context, dt.asSeconds()); //currently handles gravity and bounce
    for (auto e : context->registry.getEntitiesWith<CTransform, Velocity>()) {
        auto transform = context->registry.getComponent<CTransform>(e);
        auto velocity = context->registry.getComponent<Velocity>(e);
        if (transform && velocity) {
            transform->position += velocity->velocity * dt.asSeconds();
            //set the animation speed to match velocity
            for (auto e : context->registry.getEntitiesWith<CBall>()) {
                auto animation = context->registry.getComponent<CAnimation>(e);
                float animationSpeed;
                if (velocity->velocity.y <= 240 && velocity->velocity.y >= -240) {
                    animationSpeed = velocity->velocity.y  / 4;
                }
                else {
                    animationSpeed = 60;
                }
                animation->animation.setSpeed(velocity->velocity.y);
            }
        }
    }
    Boundaries.update(context);

}

