#include "BallGravitySystem.hpp"

void BallGravitySystem::update(Registry& registry, sf::RenderWindow& window) {
    for (auto e : registry.getEntitiesWith<CBall, CTransform>()) {
        auto* transform = registry.getComponent<CTransform>(e);
        auto* ballComp = registry.getComponent<CBall>(e);
        if (transform && ballComp) {
            float* ballHeight = ballComp->height;
            if (ballHeight > 0) {
                ballHeight = ballHeight - .05f;

            }

        }

    }


}