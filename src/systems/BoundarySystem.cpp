#include "BoundarySystem.hpp"
#include "../ecs/Registry.hpp"
#include "../components/Transform.hpp"
#include "../components/BoundingBox.hpp"
#include "../components/Player.hpp"
#include "../components/Velocity.hpp"

void BoundarySystem::updatePlayer(Registry& registry, sf::RenderWindow& window, float dt) {
    for (auto e : registry.getEntitiesWith<Transform, Player, BoundingBox, Velocity>()) {
        auto* transform = registry.getComponent<Transform>(e);
        auto* boundingBox = registry.getComponent<BoundingBox>(e);
        auto* velocity = registry.getComponent<Velocity>(e);
        if (transform && boundingBox && velocity) {
            float playerLeftX = transform->position.x - boundingBox->rect.height / 2;
            float playerRightX = transform->position.x + boundingBox->rect.height / 2;
            float playerLowerY = transform->position.y + boundingBox->rect.width / 2;
            float playerUpperY = transform->position.y - boundingBox->rect.width / 2;
            Vec2 windowSize = Vec2(window.getSize().x, window.getSize().y);
            if (playerLeftX <= 0) {
                transform->position.x = boundingBox->rect.width / 2;
            }
            else if (playerRightX >= windowSize.x) {
                transform->position.x = windowSize.x - boundingBox->rect.width / 2;
            }
            if (playerUpperY <= 0) {
                transform->position.y = boundingBox->rect.height / 2;
            }
            else if (playerLowerY >= windowSize.y) {
                transform->position.y = windowSize.y - boundingBox->rect.height / 2;
            }
        }
    }

}

