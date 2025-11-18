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
        auto [transform, ballComp, boundingBox3D, cBallTransform3D, cBallVelocity3D] = context->registry.getComponents<CTransform, CBall, CBoundingBox3D, CTransform3D, CVelocity3D>(ball);
        if (!transform || !ballComp || !boundingBox3D || !cBallTransform3D || !cBallVelocity3D) continue;

        cBallTransform3D->lastPos_m = cBallTransform3D->pos_m;
        // --- SHADOW ENTITY ---
        Entity shadowEntity = ballComp->ballShadow;
        auto shadowTransform = context->registry.getComponent<CTransform>(shadowEntity);
        if (!shadowTransform) continue;
        
        // --- CHECK IF BALL IS OFF TABLE ---
        bool offTable = (
            cBallTransform3D->pos_m.x < 0.f || cBallTransform3D->pos_m.x > context->tableParameters.tableWidth ||
            cBallTransform3D->pos_m.z < 0.f || cBallTransform3D->pos_m.z > context->tableParameters.tableLength
            );

        // --- PHYSICS FORCES ---
        Vec3 spin = context->physicsDebug.debugSpinEnabled
            ? context->physicsDebug.debugBallSpin
            : ballComp->spin;

        bForces.forceGravity = calcBallGrav.calculateForceGravity(ballComp->mass);
        bForces.forceMagnus = calcMagnus.calculateForceMagnus(spin, cBallVelocity3D->vel_mps, context->physicsDebug.debugKMagnus);
        bForces.forceDrag = calcBallDrag.calculateForceDrag(cBallVelocity3D->vel_mps);

        bForces.totalForces = bForces.forceGravity + bForces.forceMagnus + bForces.forceDrag;
        bForces.acceleration = bForces.totalForces / ballComp->mass;

        // --- INTEGRATE POSITION ---
        cBallVelocity3D->vel_mps += bForces.acceleration * dt;
        cBallTransform3D->pos_m += cBallVelocity3D->vel_mps * dt;

        // --- BOUNCE & EDGE HANDLING (only if on-table) ---
        if (!offTable) {
            // vertical bounce
            if (cBallTransform3D->pos_m.y <= context->tableParameters.tableY) {

                cBallTransform3D->pos_m.y = context->tableParameters.tableY;
                cBallVelocity3D->vel_mps.y = -cBallVelocity3D->vel_mps.y * ballComp->restitution;
                if (std::abs(cBallVelocity3D->vel_mps.y) < 0.1f)
                    cBallVelocity3D->vel_mps.y = 0.f;
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
        boundingBox3D->box = Bounds3D(cBallTransform3D->pos_m - ballComp->ballRadius, cBallTransform3D->pos_m + ballComp->ballRadius);
        netCollision.resolve(context, ball);
        // --- PROJECT TO SCREEN USING HOMOGRAPHY ---
        Vec2 screenBase = context->camera.homography.worldToImage({
            cBallTransform3D->pos_m.x,
            cBallTransform3D->pos_m.z
            });
        shadowTransform->position = screenBase;
        float scale = std::max(0.5f, 1.5f - 0.2f * cBallTransform3D->pos_m.y);
        shadowTransform->scale = { scale, scale };


        // --- BALL SPRITE OFFSET (height in meters → pixels) ---
        transform->position = screenBase - Vec2(0.f, cBallTransform3D->pos_m.y * context->tableParameters.pixelsPerMeter);

        // --- DEBUG OUTPUT ---
        if (context->physicsDebug.enableConsoleDebugOutput) {
            Debug::debugPrint("Velocity (m/s)", cBallVelocity3D->vel_mps);
            Debug::debugPrint("Pos (m)", cBallTransform3D->pos_m);
            Debug::debugPrint("Ball Screen Pos", transform->position);
            Debug::debugPrint("Shadow Screen Pos", shadowTransform->position);
        }
    }
}
