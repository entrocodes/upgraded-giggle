#include "BallGravitySystem.hpp"
#include "../math/Vec2.hpp"
#include "../components/Components.hpp"
#include "../ecs/Entity.hpp"
#include "../debug/Debug.hpp"
#include "../math/GridTransform.hpp"
#include "BallGravitySystem.hpp"
#include "../math/physics/forces/CalculateBallGravity.hpp"
#include "../math/physics/forces/CalculateMagnusForce.hpp"
#include "../math/physics/forces/CalculateBallDrag.hpp"
#include <algorithm>
#include <iostream>

void BallForceSystem::update(GameContext* context, float dt) {
    for (auto e : context->registry.getEntitiesWith<CBall, CTransform>()) {
        BallForces bForces;

        auto [transform, ballComp, ballVelVec2] = context->registry.getComponents<CTransform, CBall, Velocity>(e);
        if (!transform || !ballComp) continue;

        // Access shadow position
        Entity shadowEntity = ballComp->ballShadow;
        auto [shadowTransform, shadowVelocity] = context->registry.getComponents<CTransform, Velocity>(shadowEntity);
        if (!shadowTransform || !shadowVelocity) continue;
        
        Vec2 shadowPos = shadowTransform->position;
        Vec3 velocity = { shadowVelocity->velocity.x, ballComp->verticalVel, shadowVelocity->velocity.y };

        // Calculate Forces
        bForces.forceGravity = CalculateBallGravity::calculateForceGravity(ballComp->mass);
        bForces.forceMagnus = CalculateForceMagnus::calculateForceMagnus(Vec3(2.0f, 2.0f, 2.0f) /*test values for spin*/, velocity);
        bForces.forceDrag = CalculateBallDrag::calculateForceDrag(velocity);
       
        bForces.totalForces = bForces.forceMagnus + bForces.forceGravity + bForces.forceMagnus;



        //TO-DO: apply forces to velocity and verticalVel.
        bForces.acceleration = bForces.totalForces / ballComp->mass;
        
        ballComp->verticalVel = bForces.acceleration.y * dt;
        shadowVelocity->velocity = { bForces.acceleration.x * dt, bForces.acceleration.z * dt };
        ballVelVec2->velocity = { bForces.acceleration.x * dt, bForces.acceleration.z * dt }
        //table bounce
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
        //if the ball has gone off the table
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
