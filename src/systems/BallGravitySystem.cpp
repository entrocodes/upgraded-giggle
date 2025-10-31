#include "BallGravitySystem.hpp"
#include "../math/Vec2.hpp"
#include "../components/Components.hpp"
#include "../ecs/Entity.hpp"
#include "../debug/Debug.hpp"
#include "../math/GridTransform.hpp"
#include <iostream>
void BallGravitySystem::update(Registry& registry) {
    for (auto e : registry.getEntitiesWith<CBall, CTransform>()) {
        auto* transform = registry.getComponent<CTransform>(e);
        auto* ballComp = registry.getComponent<CBall>(e);
        if (!transform || !ballComp) {
            std::cout << "transform or ball component doesn't exist" << std::endl;
            continue; // safety check
        }
        // Access the shadow entity
        Entity shadowEntity = ballComp->ballShadow;
        auto* shadowTransform = registry.getComponent<CTransform>(shadowEntity);
        Vec2 shadowPos = shadowTransform->position;
        // Safety check (shadow might have been destroyed or not have a CTransform)
        if (!shadowTransform) {
            std::cout << "shadow component doesn't exist" << std::endl;
            continue;
        }

        float& ballHeight = ballComp->ballHeight;

        if (ballHeight > 0.f) {
            ballHeight -= Grid::toWorldX(0.001f);
            transform->position = shadowPos - Vec2(0.f, ballHeight);
        }
        else {
            ballHeight = 0.f;
            transform->position = shadowTransform->position; // snap to ground
        }
        /*shadowTransform->scale = Vec2(ballHeight * 3, ballHeight * 3);*/ //adjust
        Debug::debugPrint("Ball Position", transform->position);
        Debug::debugPrint("Ball Shadow Position", shadowPos);
        Debug::debugPrint("Ball Height", ballHeight);
        Debug::debugPrint("Ball Shadow Scale", shadowTransform->scale);
    }
}
