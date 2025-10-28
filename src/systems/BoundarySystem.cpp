#include "BoundarySystem.hpp"
#include "../ecs/Registry.hpp"
#include "../components/Transform.hpp"
#include "../components/BoundingBox.hpp"
#include "../components/Player.hpp"
#include "../components/Enemy.hpp"
#include "../components/Velocity.hpp"

void BoundarySystem::update(Registry& registry, float dt, const Vec2& windowSize) {
    for (auto e : registry.getEntitiesWith<Transform, BoundingBox, Velocity>()) {
        auto* transform = registry.getComponent<Transform>(e);
        auto* boundingBox = registry.getComponent<BoundingBox>(e);
        auto* velocity = registry.getComponent<Velocity>(e);
        if (transform && boundingBox && velocity) {
            float leftX = transform->position.x - boundingBox->rect.height / 2;
            float rightX = transform->position.x + boundingBox->rect.height / 2;
            float lowerY = transform->position.y + boundingBox->rect.width / 2;
            float upperY = transform->position.y - boundingBox->rect.width / 2;
            float* velocityX = &velocity->velocity.x;
            float* velocityY = &velocity->velocity.y;
            if (registry.getComponent<Player>(e)) {
                if (leftX <= 0) {
                    transform->position.x = boundingBox->rect.width / 2;
                }
                else if (rightX >= windowSize.x) {
                    transform->position.x = windowSize.x - boundingBox->rect.width / 2;
                }
                if (upperY <= 0) {
                    transform->position.y = boundingBox->rect.height / 2;
                }
                else if (lowerY >= windowSize.y) {
                    transform->position.y = windowSize.y - boundingBox->rect.height / 2;
                }
            }
            else if (registry.getComponent<Enemy>(e)) {
                if (leftX <= 0) {
                    transform->position.x = boundingBox->rect.width / 2;
                    if (*velocityX < 0) {
                        *velocityX = -*velocityX;
                    }
                }
                else if (rightX >= windowSize.x) {
                    transform->position.x = windowSize.x - boundingBox->rect.width / 2;
                    if (*velocityX > 0) {
                        *velocityX = -*velocityX;
                    }
                }
                if (upperY <= 0) {
                    transform->position.y = boundingBox->rect.height / 2;
                    if (*velocityY < 0) {
                        *velocityY = -*velocityY;
                    }
                }
                else if (lowerY >= windowSize.y) {
                    transform->position.y = windowSize.y - boundingBox->rect.height / 2;
                    if (*velocityY > 0) {
                        *velocityY = -*velocityY;
                    }
                }
            }
        }

    }

}

