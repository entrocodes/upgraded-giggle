#include "BallMovementSystem.hpp"
#include "../math/Vec2.hpp"
#include "../math/Bounds3D.hpp"
#include "../components/Components.hpp"
#include "../ecs/Entity.hpp"
#include "../debug/Debug.hpp"
#include "../math/GridTransform.hpp"

#include <algorithm>
#include <iostream>

void BallMovementSystem::update(GameContext* context, float dt) {
    ballForceSystem.update(context, dt); //updates bForces
    updateOffTable(context);
    for (auto ball : context->registry.getEntitiesWith<CBall, CTransform>()) {
        auto [transform, ballComp, boundingBox3D, cBallTransform3D, cBallVelocity3D] = context->registry.getComponents<CTransform, CBall, CBoundingBox3D, CTransform3D, CVelocity3D>(ball);
        if (!transform || !ballComp || !boundingBox3D || !cBallTransform3D || !cBallVelocity3D) continue;



        cBallTransform3D->lastPos_m = cBallTransform3D->pos_m;



        // --- INTEGRATE POSITION ---
        cBallVelocity3D->vel_mps += ballComp->bForces.acceleration * dt;
        cBallTransform3D->pos_m += cBallVelocity3D->vel_mps * dt;
        Vec3 p = cBallTransform3D->pos_m;
        ballComp->contactingTable = (p.y <= context->tableParameters.tableY);
        ballComp->onFloor = (p.y <= context->tableParameters.floorY);
        if (ballComp->contactingTable && !ballComp->offTable && !ballComp->hasFallen) {
            handleTableContact(context, ball, dt);
        }
        else if (ballComp->onFloor && ballComp->offTable) {
            handleFloorContact(context, ball, dt);
        }
        
  

        if (!ballComp->hasFallen && ballComp->offTable && cBallTransform3D->pos_m.y < 0) {
            ballComp->hasFallen = true;
            // Trigger event here (e.g. scoring or reset)
        }
        //handle net collision
        boundingBox3D->box = Bounds3D(p - ballComp->ballRadius, p + ballComp->ballRadius);
        ballForceSystem.netCollision.resolve(context, ball);

        transform->position = context->camera.homography.worldToImage(p);

        // --- SHADOW ENTITY ---
        Entity shadowEntity = ballComp->ballShadow;
        auto [shadowTransform, shadowTransform3D] = context->registry.getComponents<CTransform, CTransform3D>(shadowEntity);
        if (!shadowTransform || !shadowTransform3D) continue;
        
        
        // --- PROJECT TO SCREEN USING HOMOGRAPHY ---
        float scale = 1.0f;
        
        if (!ballComp->offTable) {
            shadowTransform3D->pos_m = Vec3(p.x, context->tableParameters.tableY, p.z);
            scale = std::max(0.5f, 1.5f - 0.2f * p.y);

        }
        else {
            shadowTransform3D->pos_m = Vec3(p.x, context->tableParameters.floorY, p.z);
            scale = std::max(0.5f, 1.5f - 0.2f * (p.y - context->tableParameters.floorY));
        }
        shadowTransform->position = context->camera.homography.worldToImage(shadowTransform3D->pos_m);
        shadowTransform->scale = { scale, scale };

        // --- DEBUG OUTPUT ---
        if (context->physicsDebug.enableConsoleDebugOutput) {
            Debug::debugPrint("Velocity (m/s)", cBallVelocity3D->vel_mps);
            Debug::debugPrint("Pos (m)", cBallTransform3D->pos_m);
            Debug::debugPrint("Ball Screen Pos", transform->position);
            Debug::debugPrint("Shadow Screen Pos", shadowTransform->position);
        }
    }
}


void BallMovementSystem::updateOffTable(GameContext* context) {
    for (auto ball : context->registry.getEntitiesWith<CBall, CTransform3D>()) {
        auto [cBall, cBallTransform3D] = context->registry.getComponents<CBall, CTransform3D>(ball);
        // --- CHECK IF BALL IS OFF TABLE ---
        cBall->offTable = (
            cBallTransform3D->pos_m.x < 0.f || cBallTransform3D->pos_m.x > context->tableParameters.tableWidth ||
            cBallTransform3D->pos_m.z < 0.f || cBallTransform3D->pos_m.z > context->tableParameters.tableLength
            );
        cBall->contactingTable = (cBallTransform3D->pos_m.y <= context->tableParameters.tableY + 0.001f);

    }

}

void BallMovementSystem::handleTableContact(GameContext* context, Entity& ball, float dt) {
    auto [ballComp, cTransform3D, cVelocity3D] = context->registry.getComponents<CBall, CTransform3D, CVelocity3D>(ball);
    if (!ballComp || !cTransform3D || !cVelocity3D) exit;
        // Snap to surface
        cTransform3D->pos_m.y = context->tableParameters.tableY;

    // Only reverse direction on first contact, not every frame
    if (cVelocity3D->vel_mps.y < 0.f) {
        BounceForce bounce({ 0.f, 1.f, 0.f }, ballComp->restitution * context->tableParameters.floorRestitution);
        cVelocity3D->vel_mps = bounce.apply(cVelocity3D->vel_mps);
    }
        
    // Friction applied *every frame* while touching surface
    ballForceSystem.applyFriction.applyFriction(context, ball, dt, context->tableParameters.tableFrictionCoefficient);
}

void BallMovementSystem::handleFloorContact(GameContext* context, Entity& ball, float dt) {
    auto [ballComp, cTransform3D, cVelocity3D] = context->registry.getComponents<CBall, CTransform3D, CVelocity3D>(ball);
    if (!ballComp || !cTransform3D || !cVelocity3D) exit;
    // Snap to surface
    cTransform3D->pos_m.y = context->tableParameters.floorY;

    // Only reverse direction on first contact, not every frame
    if (cVelocity3D->vel_mps.y < 0.f) {
        BounceForce bounce({ 0.f, 1.f, 0.f }, ballComp->restitution * context->tableParameters.floorRestitution);
        cVelocity3D->vel_mps = bounce.apply(cVelocity3D->vel_mps);
    }

    // Friction applied *every frame* while touching surface
    ballForceSystem.applyFriction.applyFriction(context, ball, dt);
}

