#include "BoundarySystem.hpp"
#include "../math/GridTransform.hpp"
void BoundarySystem::update(GameContext* context) {
    for (auto e : context->registry.getEntitiesWith<CTransform, BoundingBox, Velocity>()) {
        auto [transform, boundingBox, velocity] = context->registry.getComponents<CTransform, BoundingBox, Velocity>(e);
        if (context->registry.getComponent<Player>(e)) {
            float leftBoundary = 520.0f;
            float rightBoundary = 844.0f;
            float playerLeftX = transform->position.x - boundingBox->rect.width / 2;
            float playerRightX = transform->position.x + boundingBox->rect.width / 2;
            Vec2 windowSize = Vec2(context->window.getSize().x, context->window.getSize().y);
            if (playerLeftX <= leftBoundary && velocity->velocity.x < 0) {
                transform->position.x = leftBoundary + boundingBox->rect.width / 2;
            }
            else if (playerRightX >= rightBoundary && velocity->velocity.x > 0) {
                transform->position.x = rightBoundary - boundingBox->rect.width / 2;
            }
        }
    }

}

