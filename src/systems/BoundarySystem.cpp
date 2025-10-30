#include "BoundarySystem.hpp"
#include "../ecs/Registry.hpp"
#include "../math/GridTransform.hpp"
void BoundarySystem::update(Registry& registry, sf::RenderWindow& window) {
    for (auto e : registry.getEntitiesWith<CTransform, BoundingBox, Velocity>()) {
        auto* transform = registry.getComponent<CTransform>(e);
        auto* boundingBox = registry.getComponent<BoundingBox>(e);
        auto* velocity = registry.getComponent<Velocity>(e);
        if (registry.getComponent<Player>(e)) {
            float playerLeftX = transform->position.x - boundingBox->rect.width / 2;
            float playerRightX = transform->position.x + boundingBox->rect.width / 2;
            //float playerLowerY = transform->position.y + boundingBox->rect.height / 2;
            //float playerUpperY = transform->position.y - boundingBox->rect.height / 2;
            Vec2 windowSize = Vec2(window.getSize().x, window.getSize().y);
            if (playerLeftX <= Grid::toWorldX(1.8)) {
                if (velocity->velocity.x < 0) {
                    transform->position.x = Grid::toWorldX(1.8) + boundingBox->rect.width / 2;
                }
                
            }
            else if (playerRightX >= Grid::toWorldX(8)) {
                if (velocity->velocity.x > 0) {
                    transform->position.x = Grid::toWorldX(8) - boundingBox->rect.width / 2;
                }
            }
            //if (playerUpperY <= 0) {
            //    transform->position.y = boundingBox->rect.height / 2;
            //}
            //else if (playerLowerY >= windowSize.y) {
            //    transform->position.y = windowSize.y - boundingBox->rect.height / 2;
            //}
        }
    }

}

