#include "BallGravitySystem.hpp"
#include "../math/Vec2.hpp"
#include "../components/Components.hpp"
#include "../ecs/Entity.hpp"
#include "../debug/Debug.hpp"
void BallGravitySystem::update(Registry& registry) {
    for (auto e : registry.getEntitiesWith<CBall, CTransform>()) {
        auto* transform = registry.getComponent<CTransform>(e);
        auto* ballComp = registry.getComponent<CBall>(e);

        if (!transform || !ballComp) continue; // safety check

        // Access the shadow entity
        Entity shadowEntity = ballComp->ballShadow;
        auto* shadowTransform = registry.getComponent<CTransform>(shadowEntity);

        // Safety check (shadow might have been destroyed or not have a CTransform)
        if (!shadowTransform) continue;

        float& ballHeight = ballComp->ballHeight;

        if (ballHeight > 0.f) {
            /*ballHeight -= 0.0005f;*/
            transform->position = shadowTransform->position + Vec2(0.f, ballHeight);
        }
        else {
            ballHeight = 0.f;
            transform->position = shadowTransform->position; // snap to ground
        }
        /*shadowTransform->scale = Vec2(ballHeight * 3, ballHeight * 3);*/ //adjust
        Debug::debugPrint("Ball Position", transform->position);
        Debug::debugPrint("Ball Shadow Position", shadowTransform->position);
        Debug::debugPrint("Ball Height", ballHeight);
        Debug::debugPrint("Ball Shadow Scale", shadowTransform->scale);
    }
}
