#include "BallForceSystem.hpp"
#include "../math/Vec2.hpp"
#include "../components/Components.hpp"
#include "../ecs/Entity.hpp"
#include "../debug/Debug.hpp"
#include "../math/GridTransform.hpp"

#include <algorithm>
#include <iostream>

void BallForceSystem::update(GameContext* context, float dt) {
    for (auto e : context->registry.getEntitiesWith<CBall, CTransform>()) {
        BallForces bForces;
        auto [transform, ballComp] = context->registry.getComponents<CTransform, CBall>(e);
        if (!transform || !ballComp) continue;

        // --- SHADOW ENTITY ---
        Entity shadowEntity = ballComp->ballShadow;
        auto shadowTransform = context->registry.getComponent<CTransform>(shadowEntity);
        if (!shadowTransform) continue;
        // --- CHECK IF BALL IS OFF TABLE ---
        bool offTable = (
            ballComp->pos_m.x < 0.f || ballComp->pos_m.x > tableWidth ||
            ballComp->pos_m.z < 0.f || ballComp->pos_m.z > tableDepth
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
            if (ballComp->pos_m.y <= tableY) {
                ballComp->pos_m.y = tableY;
                ballComp->vel_mps.y = -ballComp->vel_mps.y * ballComp->restitution;
                if (std::abs(ballComp->vel_mps.y) < 0.1f)
                    ballComp->vel_mps.y = 0.f;
            }

            //// edge reflection (optional)
            //if (ballComp->pos_m.x < 0.0f) {
            //    ballComp->pos_m.x = 0.0f;
            //    ballComp->vel_mps.x = -ballComp->vel_mps.x * ballComp->restitution;
            //}
            //else if (ballComp->pos_m.x > tableWidth) {
            //    ballComp->pos_m.x = tableWidth;
            //    ballComp->vel_mps.x = -ballComp->vel_mps.x * ballComp->restitution;
            //}

            //if (ballComp->pos_m.z < 0.0f) {
            //    ballComp->pos_m.z = 0.0f;
            //    ballComp->vel_mps.z = -ballComp->vel_mps.z * ballComp->restitution;
            //}
            //else if (ballComp->pos_m.z > tableDepth) {
            //    ballComp->pos_m.z = tableDepth;
            //    ballComp->vel_mps.z = -ballComp->vel_mps.z * ballComp->restitution;
            //}
        }
        else {
            // OFF-TABLE BEHAVIOR
            if (!ballComp->hasFallen) {
                ballComp->hasFallen = true;
                // Trigger event here (e.g. scoring or reset)
            }

            // Gravity-only fall and friction
            ballComp->vel_mps.x *= 0.99f;
            ballComp->vel_mps.z *= 0.99f;

            // Stop once below certain depth
            if (ballComp->pos_m.y <= stopBelow) {
                ballComp->vel_mps = { 0.f, 0.f, 0.f };
            }
        }

        // --- PROJECT TO SCREEN USING HOMOGRAPHY ---
        Vec2 screenBase = context->camera.homography.worldToImage({
            ballComp->pos_m.x,
            ballComp->pos_m.z
            });



        if (!offTable) {
            shadowTransform->position = screenBase;
            float scale = std::max(0.5f, 1.5f - 0.2f * ballComp->pos_m.y);
            shadowTransform->scale = { scale, scale };
        }
        else {
            // move shadow below table visually
            const float groundOffsetPx = 80.f;  // how far below the table the ground is
            const float fallFactor = std::clamp(ballComp->pos_m.y * 2.f, 0.f, 1.f);

            // freeze x/z at table edge, drop y
            Vec2 edgeScreen = screenBase;
            edgeScreen.y += groundOffsetPx * fallFactor;

            shadowTransform->position = edgeScreen;

            // fade and shrink the shadow
            float fade = std::max(0.0f, 1.0f - ballComp->pos_m.y * 0.8f);
            shadowTransform->scale = { fade, fade };
        }


        // --- BALL SPRITE OFFSET (height in meters → pixels) ---
        transform->position = screenBase - Vec2(0.f, ballComp->pos_m.y * pixelsPerMeter);

        // --- DEBUG OUTPUT ---
        if (context->physicsDebug.enableConsoleDebugOutput) {
            Debug::debugPrint("Velocity (m/s)", ballComp->vel_mps);
            Debug::debugPrint("Pos (m)", ballComp->pos_m);
            Debug::debugPrint("Ball Screen Pos", transform->position);
            Debug::debugPrint("Shadow Screen Pos", shadowTransform->position);
        }
    }
}
