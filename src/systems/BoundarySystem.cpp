#include "BoundarySystem.hpp"
#include "../ecs/Registry.hpp"
#include "../math/GridTransform.hpp"
void BoundarySystem::update(Registry& registry, sf::RenderWindow& window, DisplayConfig& display) {
    for (auto e : registry.getEntitiesWith<CTransform, BoundingBox, Velocity>()) {
        auto* transform = registry.getComponent<CTransform>(e);
        auto* boundingBox = registry.getComponent<BoundingBox>(e);
        auto* velocity = registry.getComponent<Velocity>(e);
        if (registry.getComponent<Player>(e)) {
            float leftBoundary = 520.0f;
            float rightBoundary = 844.0f;
            float playerLeftX = transform->position.x - boundingBox->rect.width / 2;
            float playerRightX = transform->position.x + boundingBox->rect.width / 2;
            Vec2 windowSize = Vec2(window.getSize().x, window.getSize().y);
            if (playerLeftX <= leftBoundary && velocity->velocity.x < 0) {
                transform->position.x = leftBoundary + boundingBox->rect.width / 2;
            }
            else if (playerRightX >= rightBoundary && velocity->velocity.x > 0) {
                transform->position.x = rightBoundary - boundingBox->rect.width / 2;
            }
        }
    }

}

