#include "BallForceSystem.hpp"
#include "../math/Vec2.hpp"
#include "../math/Bounds3D.hpp"
#include "../components/Components.hpp"
#include "../ecs/Entity.hpp"
#include "../debug/Debug.hpp"
#include "../math/GridTransform.hpp"

#include <algorithm>
#include <iostream>

void BallForceSystem::update(GameContext* context, float dt) {
    for (auto ball : context->registry.getEntitiesWith<CBall, CTransform>()) {
        BallForces bForces;
        auto [transform, ballComp] = context->registry.getComponents<CTransform, CBall>(ball);
        if (!transform || !ballComp) continue;

        
        // --- SHADOW ENTITY ---
        Entity shadowEntity = ballComp->ballShadow;
        auto shadowTransform = context->registry.getComponent<CTransform>(shadowEntity);
        if (!shadowTransform) continue;
        
        // --- CHECK IF BALL IS OFF TABLE ---
        bool offTable = (
            ballComp->pos_m.x < 0.f || ballComp->pos_m.x > context->tableParameters.tableLength ||
            ballComp->pos_m.z < 0.f || ballComp->pos_m.z > context->tableParameters.tableWidth
            );

        // --- PHYSICS FORCES ---
        Vec3 spin = context->physicsDebug.debugSpinEnabled
            ? context->physicsDebug.debugBallSpin
            : ballComp->spin;

        bForces.forceGravity = calcBallGrav.calculateForceGravity(ballComp->mass);
        bForces.forceMagnus = calcMagnus.calculateForceMagnus(spin, ballComp->vel_mps, context->physicsDebug.debugKMagnus);
        bForces.forceDrag = calcBallDrag.calculateForceDrag(ballComp->vel_mps);

        bForces.totalForces = bForces.forceGravity + bForces.forceMagnus + bForces.forceDrag;
        bForces.acceleration = bForces.totalForces / ballComp->mass;

        // --- INTEGRATE POSITION ---
        ballComp->vel_mps += bForces.acceleration * dt;
        ballComp->pos_m += ballComp->vel_mps * dt;

        // --- BOUNCE & EDGE HANDLING (only if on-table) ---
        if (!offTable) {
            // vertical bounce
            if (ballComp->pos_m.y <= context->tableParameters.tableY) {
                ballComp->pos_m.y = context->tableParameters.tableY;
                ballComp->vel_mps.y = -ballComp->vel_mps.y * ballComp->restitution;
                if (std::abs(ballComp->vel_mps.y) < 0.1f)
                    ballComp->vel_mps.y = 0.f;
            }

        }
        else {
            // OFF-TABLE BEHAVIOR
            if (!ballComp->hasFallen) {
                ballComp->hasFallen = true;
                // Trigger event here (e.g. scoring or reset)
            }
        }
        //handle net collision
        ballComp->ballBounds3D = Bounds3D(ballComp->pos_m - ballComp->ballRadius, ballComp->pos_m + ballComp->ballRadius);
        netCollision.resolve(context, ball);
        // --- PROJECT TO SCREEN USING HOMOGRAPHY ---
        Vec2 screenBase = context->camera.homography.worldToImage({
            ballComp->pos_m.x,
            ballComp->pos_m.z
            });
        shadowTransform->position = screenBase;
        float scale = std::max(0.5f, 1.5f - 0.2f * ballComp->pos_m.y);
        shadowTransform->scale = { scale, scale };


        // --- BALL SPRITE OFFSET (height in meters → pixels) ---
        transform->position = screenBase - Vec2(0.f, ballComp->pos_m.y * context->tableParameters.pixelsPerMeter);

        // --- DEBUG OUTPUT ---
        if (context->physicsDebug.enableConsoleDebugOutput) {
            Debug::debugPrint("Velocity (m/s)", ballComp->vel_mps);
            Debug::debugPrint("Pos (m)", ballComp->pos_m);
            Debug::debugPrint("Ball Screen Pos", transform->position);
            Debug::debugPrint("Shadow Screen Pos", shadowTransform->position);
        }
    }
}
