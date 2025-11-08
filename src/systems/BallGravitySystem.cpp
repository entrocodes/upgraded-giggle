#include "BallGravitySystem.hpp"
#include "../math/Vec2.hpp"
#include "../components/Components.hpp"
#include "../ecs/Entity.hpp"
#include "../debug/Debug.hpp"
#include "../math/GridTransform.hpp"
#include <algorithm>
#include <iostream>

void BallGravitySystem::update(GameContext* context, float dt) {
    const float pixelsPerMeter = 90; // tune this to match table/asset scale
    const float tableBottom = 504.f;
    for (auto e : context->registry.getEntitiesWith<CBall, CTransform>()) {
        auto [transform, ballComp] = context->registry.getComponents<CTransform, CBall>(e);
        if (!transform || !ballComp) continue;

        // Access shadow position
        Entity shadowEntity = ballComp->ballShadow;
        auto shadowTransform = context->registry.getComponent<CTransform>(shadowEntity);
        if (!shadowTransform) continue;
        Vec2 shadowPos = shadowTransform->position;

        // Integrate gravity
        ballComp->verticalVel -= ballComp->gravity * dt;         // velocity update
        ballComp->ballHeight += ballComp->verticalVel * dt;      // position update

        if (shadowPos.y <= tableBottom) {
            // Bounce when hitting table (height <= 0)
            if (ballComp->ballHeight <= 0.0f) {
                ballComp->ballHeight = 0.0f;
                ballComp->verticalVel = -ballComp->verticalVel * ballComp->restitution;

                // Stop tiny residual bounces
                if (std::abs(ballComp->verticalVel) < 0.5f)
                    ballComp->verticalVel = 0.0f;
            }
        }
        else {
            if (ballComp->ballHeight <= -20.0f) {
                ballComp->ballHeight = -20.0f;
                ballComp->verticalVel = -ballComp->verticalVel * ballComp->restitution;

                // Stop tiny residual bounces
                if (std::abs(ballComp->verticalVel) < 0.5f)
                    ballComp->verticalVel = 0.0f;
            }
        }

        // Apply vertical offset to sprite (Y goes up as height increases)
        transform->position = shadowPos - Vec2(0.f, ballComp->ballHeight * pixelsPerMeter);

        // Shadow scale (shrinks slightly as ball rises)
        float shadowScale = std::max(0.5f, 1.5f - ballComp->ballHeight * 0.2f);
        if (shadowPos.y <= tableBottom) {
            shadowTransform->scale = Vec2(shadowScale, shadowScale);
        }
        else {
            shadowTransform->scale = Vec2(0, 0);
        }
        // Optional debugging
         //Debug::debugPrint("Ball Height", ballComp->ballHeight);
         //Debug::debugPrint("Vertical Velocity", ballComp->verticalVel);
    }
}
