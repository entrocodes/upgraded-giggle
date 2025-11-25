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
        if (context->physicsDebug.debugSpinArrows) {
            Debug::queueArrow3D(p, p + cBallVelocity3D->vel_mps.normalized() * 0.15f, sf::Color::Blue);
            Debug::queueArrow3D(p, p + ballComp->spin.normalized() * 0.10f, sf::Color::Yellow);
            Debug::queueArrow3D(p, p + ballComp->bForces.forceMagnus.normalized() * 20.0f, sf::Color::Red);
        }

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
    auto [ballComp, cTransform3D, cVelocity3D] =
        context->registry.getComponents<CBall, CTransform3D, CVelocity3D>(ball);

    if (!ballComp || !cTransform3D || !cVelocity3D) return;

    // Snap to table surface
    cTransform3D->pos_m.y = context->tableParameters.tableY;

    Vec3& v = cVelocity3D->vel_mps;
    Vec3& s = ballComp->spin;

    // Bounce happens when falling onto table
    if (v.y < 0.f)
    {
        v.y = -v.y * ballComp->restitution * context->tableParameters.tableRestitution;

        // top/backspin: spin.x affects forward/back speed
        // sidespin: spin.y affects left/right speed
        float spinKickFactor = context->tableParameters.tableSpinToVelocityFactor;

        v.x += -s.y * spinKickFactor; // sidespin → lateral kick
        v.z += s.x * spinKickFactor; // topspin/backspin → forward/back kick

        float spinLoss = context->tableParameters.tableSpinLossOnBounce;
        s = s * (1.0f - spinLoss);
    }

    // 4️⃣ Sliding friction while touching surface (only x,z drag)
    float mu = context->tableParameters.tableFrictionCoefficient;
    float Fn = ballComp->mass * 9.8f;
    float frictionAccel = mu * Fn * dt;

    if (v.x > 0) v.x = std::max(0.f, v.x - frictionAccel);
    if (v.x < 0) v.x = std::min(0.f, v.x + frictionAccel);
    if (v.z > 0) v.z = std::max(0.f, v.z - frictionAccel);
    if (v.z < 0) v.z = std::min(0.f, v.z + frictionAccel);
}


void BallMovementSystem::handleFloorContact(GameContext* context, Entity& ball, float dt)
{
    auto [ballComp, cTransform3D, cVelocity3D] =
        context->registry.getComponents<CBall, CTransform3D, CVelocity3D>(ball);

    if (!ballComp || !cTransform3D || !cVelocity3D)
        return;

    Vec3& v = cVelocity3D->vel_mps;
    Vec3& s = ballComp->spin;

    // Snap to the floor plane
    cTransform3D->pos_m.y = context->tableParameters.floorY;

    // Bounce (vertical only)
    if (v.y < 0.f)
    {
        v.y = -v.y * ballComp->restitution * context->tableParameters.floorRestitution;

        // Spin loss due to floor grip (heavier than on table)
        float spinLoss = context->tableParameters.floorSpinLossOnBounce;
        s *= (1.0f - spinLoss);
    }

    // Strong friction on floor → quickly stop X/Z sliding
    float mu = context->tableParameters.floorFrictionCoefficient;
    float Fn = ballComp->mass * 9.8f;
    float frictionAccel = mu * Fn * dt;

    if (v.x > 0) v.x = std::max(0.f, v.x - frictionAccel);
    if (v.x < 0) v.x = std::min(0.f, v.x + frictionAccel);
    if (v.z > 0) v.z = std::max(0.f, v.z - frictionAccel);
    if (v.z < 0) v.z = std::min(0.f, v.z + frictionAccel);

    // Additional flight drag already handled elsewhere (good)
}

