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

        handleBounce(context, ball);
        ballForceSystem.applyFriction.applyFriction(context, ball, dt);
        if (!ballComp->hasFallen && ballComp->offTable) {
            ballComp->hasFallen = true;
            // Trigger event here (e.g. scoring or reset)
        }
        //handle net collision
        boundingBox3D->box = Bounds3D(cBallTransform3D->pos_m - ballComp->ballRadius, cBallTransform3D->pos_m + ballComp->ballRadius);
        ballForceSystem.netCollision.resolve(context, ball);
        
        // --- SHADOW ENTITY ---
        Entity shadowEntity = ballComp->ballShadow;
        auto shadowTransform = context->registry.getComponent<CTransform>(shadowEntity);
        if (!shadowTransform) continue;
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


void BallMovementSystem::updateOffTable(GameContext* context) {
    for (auto ball : context->registry.getEntitiesWith<CBall, CTransform3D>()) {
        auto [cBall, cBallTransform3D] = context->registry.getComponents<CBall, CTransform3D>(ball);
        // --- CHECK IF BALL IS OFF TABLE ---
        cBall->offTable = (
            cBallTransform3D->pos_m.x < 0.f || cBallTransform3D->pos_m.x > context->tableParameters.tableWidth ||
            cBallTransform3D->pos_m.z < 0.f || cBallTransform3D->pos_m.z > context->tableParameters.tableLength
            );
    }

}

void BallMovementSystem::handleBounce(GameContext* context, Entity& ball) {
    auto [ballComp, cBallTransform3D, cBallVelocity3D] = context->registry.getComponents<CBall, CTransform3D, CVelocity3D>(ball);
    if (!ballComp || !cBallTransform3D || !cBallVelocity3D) return;

    // Handle bounce when ball hits the table
    //this will currently break if a ball rolls under the table.
    if (cBallTransform3D->pos_m.y <= context->tableParameters.tableY) {
        // Bounce on Y-axis (vertical)
        BounceForce bounce(Vec3(0.f, 1.f, 0.f), ballComp->restitution); // Bounce in the positive Y direction (up)
        cBallVelocity3D->vel_mps = bounce.apply(cBallVelocity3D->vel_mps);
        cBallTransform3D->pos_m.y = context->tableParameters.tableY;  // Snap to the table surface
    }

    //// Handle bounce for the sides (X and Z axes)
    //if (cBallTransform3D->pos_m.x < 0.0f || cBallTransform3D->pos_m.x > context->tableParameters.tableWidth) {
    //    BounceForce bounce(Vec3(1.f, 0.f, 0.f), ballComp->restitution);  // Bounce on X-axis (side walls)
    //    cBallVelocity3D->vel_mps = bounce.apply(cBallVelocity3D->vel_mps);
    //    cBallTransform3D->pos_m.x = std::clamp(cBallTransform3D->pos_m.x, 0.f, tableWidth);  // Snap to table boundaries
    //}
    //if (cBallTransform3D->pos_m.z < 0.0f || cBallTransform3D->pos_m.z > context->tableParameters.tableLength) {
    //    BounceForce bounce(Vec3(0.f, 0.f, 1.f), ballComp->restitution);  // Bounce on Z-axis (front/back walls)
    //    cBallVelocity3D->vel_mps = bounce.apply(cBallVelocity3D->vel_mps);
    //    cBallTransform3D->pos_m.z = std::clamp(cBallTransform3D->pos_m.z, 0.f, tableLength);  // Snap to table boundaries
    //}
}

