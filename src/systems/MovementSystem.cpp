#include "MovementSystem.hpp"
#include "BoundarySystem.hpp"
#include "BallGravitySystem.hpp"
#include "BallBounceSystem.hpp"
#include "../ecs/Registry.hpp"
#include "../components/Components.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
BoundarySystem Boundaries;
BallGravitySystem BallGravity;
BallBounceSystem BallBounce;
void MovementSystem::update(Registry& registry, sf::RenderWindow& window, DisplayConfig& display, sf::Time dt) {
    BallGravity.update(registry, dt.asSeconds()); //currently handles gravity and bounce
    //BallBounce.update(registry);
    for (auto e : registry.getEntitiesWith<CTransform, Velocity>()) {
        auto* transform = registry.getComponent<CTransform>(e);
        auto* velocity = registry.getComponent<Velocity>(e);
        if (transform && velocity) {
            transform->position += velocity->velocity * dt.asSeconds();
            //set the animation speed to match velocity
            for (auto e : registry.getEntitiesWith<CBall>()) {
                auto* animation = registry.getComponent<CAnimation>(e);
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
    Boundaries.update(registry, window, display);

}

