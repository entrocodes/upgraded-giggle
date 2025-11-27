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

        transform->pos = context->camera.homography.worldToImage(p);

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
        shadowTransform->pos = context->camera.homography.worldToImage(shadowTransform3D->pos_m);
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
            Debug::debugPrint("Ball Screen Pos", transform->pos);
            Debug::debugPrint("Shadow Screen Pos", shadowTransform->pos);
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

    Vec3& vel = cVelocity3D->vel_mps;
    Vec3& spin = ballComp->spin;
    auto& table = context->tableParameters;

    const float R = ballComp->ballRadius;
    const float Fn = ballComp->mass * 9.8f;

    // Snap to table
    cTransform3D->pos_m.y = table.tableY;

    // Bounce (vertical)
    if (vel.y < 0.f)
        vel.y = -vel.y * ballComp->restitution * table.tableRestitution;

    // Compute forward-ground speed and roll speed from spin
    float forwardSpeed = std::sqrt(vel.x * vel.x + vel.z * vel.z);
    float spinRollingSpeed = std::fabs(spin.x * R);

    // Blend sliding <-> rolling regime (not binary)
    float mismatch = std::fabs(forwardSpeed - spinRollingSpeed);
    float t = std::clamp(mismatch / 0.5f, 0.f, 1.f); // sliding weight

    // Linear velocity shift from spin when sliding
    vel.z += (-spin.x * table.spinToLinearFactor) * t;
    vel.x += (spin.z * table.spinToLinearFactor) * t;

    // Spin decay blended
    float decay = table.rollSpinDecayRate * (1.f - t) +
        table.tableSpinDecayRate * t;

    spin.x *= (1.f - decay);
    spin.z *= (1.f - decay);

    // Ground friction
    float frictionAccel = table.tableFrictionCoefficient * Fn * dt;
    if (vel.x > 0) vel.x = std::max(0.f, vel.x - frictionAccel);
    if (vel.x < 0) vel.x = std::min(0.f, vel.x + frictionAccel);
    if (vel.z > 0) vel.z = std::max(0.f, vel.z - frictionAccel);
    if (vel.z < 0) vel.z = std::min(0.f, vel.z + frictionAccel);
}
void BallMovementSystem::handleFloorContact(GameContext* context, Entity& ball, float dt) {
    auto [ballComp, cTransform3D, cVelocity3D] =
        context->registry.getComponents<CBall, CTransform3D, CVelocity3D>(ball);
    if (!ballComp || !cTransform3D || !cVelocity3D) return;

    Vec3& vel = cVelocity3D->vel_mps;
    Vec3& spin = ballComp->spin;
    auto& table = context->tableParameters;

    const float R = ballComp->ballRadius;
    const float Fn = ballComp->mass * 9.8f;

    // Snap to floor
    cTransform3D->pos_m.y = table.floorY;

    // Bounce
    if (vel.y < 0.f)
        vel.y = -vel.y * ballComp->restitution * table.floorRestitution;

    float forwardSpeed = std::sqrt(vel.x * vel.x + vel.z * vel.z);
    float spinRollingSpeed = std::fabs(spin.x * R);

    float mismatch = std::fabs(forwardSpeed - spinRollingSpeed);
    float t = std::clamp(mismatch / 0.5f, 0.f, 1.f);

    // Spin-driven sliding
    vel.z += (-spin.x * table.spinToLinearFactor) * t;
    vel.x += (spin.z * table.spinToLinearFactor) * t;

    float decay = table.rollSpinDecayRate * (1.f - t) +
        table.floorSpinDecayRate * t;

    spin.x *= (1.f - decay);
    spin.z *= (1.f - decay);

    // Strong sliding friction on floor
    float frictionAccel = table.floorFrictionCoefficient * Fn * dt;
    if (vel.x > 0) vel.x = std::max(0.f, vel.x - frictionAccel);
    if (vel.x < 0) vel.x = std::min(0.f, vel.x + frictionAccel);
    if (vel.z > 0) vel.z = std::max(0.f, vel.z - frictionAccel);
    if (vel.z < 0) vel.z = std::min(0.f, vel.z + frictionAccel);
}
