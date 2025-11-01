#include "BallBounceSystem.hpp"
#include "../ecs/Component.hpp"
#include "../components/Components.hpp"
#include "../ecs/Entity.hpp"
#include "../debug/Debug.hpp"
void BallBounceSystem::update(Registry& registry) {
    for (auto e : registry.getEntitiesWith<CBall, Velocity>()) {
        auto* velocity = registry.getComponent<Velocity>(e);
        auto* ballComp = registry.getComponent<CBall>(e);
        Entity shadowEntity = ballComp->ballShadow;
        auto* shadowVelocity = registry.getComponent<Velocity>(shadowEntity);
        float& ballHeight = ballComp->ballHeight;

        if (ballHeight <= 0.f) {
            velocity->velocity = Vec2(0,0);
            shadowVelocity->velocity = Vec2(0,0);
            /*Debug::debugPrint("Ball Velocity", velocity->velocity);*/
        }
    }
}
