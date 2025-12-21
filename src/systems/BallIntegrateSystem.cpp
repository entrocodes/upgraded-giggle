#include "BallIntegrateSystem.hpp"
#include "math/Vec2.hpp"
#include "math/Bounds3D.hpp"
#include "components/Components.hpp"
#include "ecs/Entity.hpp"
#include "debug/Debug.hpp"
#include "math/GridTransform.hpp"

#include <algorithm>
#include <iostream>

SystemExec BallIntegrateSystem::update(GameContext* context) {
    updateOffTable(context);

    bool didWork = false;

    for (auto ball : context->registry.getEntitiesWith<CBall, CTransform>()) {
        auto [cBallTransform, cBall, boundingBox3D, cBallTransform3D, cBallVelocity3D] =
            context->registry.getComponents<
            CTransform, CBall, CBoundingBox3D, CTransform3D, CVelocity3D
            >(ball);

        if (!cBallTransform || !cBall || !boundingBox3D ||
            !cBallTransform3D || !cBallVelocity3D)
            continue;

        // ✅ Count as work once we reach here
        didWork = true;

        cBallTransform3D->lastPos_m = cBallTransform3D->pos_m;

        // --- INTEGRATE POSITION ---
        cBallVelocity3D->vel_mps +=
            cBall->bForces.acceleration * context->frameStats.dt;

        cBallTransform3D->pos_m +=
            cBallVelocity3D->vel_mps * context->frameStats.dt;

        Vec3 p = cBallTransform3D->pos_m;

        cBall->contactingTable = (p.y <= context->tableParameters.tableY);
        cBall->onFloor = (p.y <= context->tableParameters.floorY);

        if (cBall->contactingTable && !cBall->offTable && !cBall->hasFallen) {
            handleTableContact(context, ball);
        }
        else if (cBall->onFloor && cBall->offTable) {
            handleFloorContact(context, ball);
        }

        if (!cBall->hasFallen && cBall->offTable && p.y < 0.f) {
            cBall->hasFallen = true;
        }

        boundingBox3D->box =
            Bounds3D(p - cBall->ballRadius, p + cBall->ballRadius);

        cBallTransform->pos =
            context->camera.homography.worldToImage(p);

        // --- SHADOW ---
        Entity shadowEntity = cBall->ballShadow;
        auto [shadowTransform, shadowTransform3D] =
            context->registry.getComponents<CTransform, CTransform3D>(shadowEntity);

        if (!shadowTransform || !shadowTransform3D)
            continue;

        float scale = 1.0f;

        if (!cBall->offTable) {
            shadowTransform3D->pos_m =
                Vec3(p.x, context->tableParameters.tableY, p.z);
            scale = std::max(0.5f, 1.5f - 0.2f * p.y);
        }
        else {
            shadowTransform3D->pos_m =
                Vec3(p.x, context->tableParameters.floorY, p.z);
            scale = std::max(0.5f,
                1.5f - 0.2f * (p.y - context->tableParameters.floorY));
        }
        shadowTransform->scale = { scale, scale };

        if (context->physicsDebug.debugSpinArrows) {
            Debug::queueArrow3D(
                p,
                p + cBallVelocity3D->vel_mps.normalized() * 0.15f,
                sf::Color::Blue
            );
        }
    }

    if (!didWork)
        return { SystemExecResult::EarlyExit, "No balls to remove" };

    return { SystemExecResult::Ran };

}



void BallIntegrateSystem::updateOffTable(GameContext* context) {
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

void BallIntegrateSystem::handleTableContact(GameContext* context, Entity& ball) {
    auto [cBall, cTransform3D, cVelocity3D] =
        context->registry.getComponents<CBall, CTransform3D, CVelocity3D>(ball);
    if (!cBall || !cTransform3D || !cVelocity3D) return;

    Vec3& vel = cVelocity3D->vel_mps;
    Vec3& spin = cBall->spin;
    auto& table = context->tableParameters;

    const float R = cBall->ballRadius;
    const float Fn = cBall->mass * 9.8f;

    // Snap to table
    cTransform3D->pos_m.y = table.tableY;

    // Bounce (vertical)
    if (vel.y < 0.f)
        vel.y = -vel.y * cBall->restitution * table.tableRestitution;

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
    float frictionAccel = table.tableFrictionCoefficient * Fn * context->frameStats.dt;
    if (vel.x > 0) vel.x = std::max(0.f, vel.x - frictionAccel);
    if (vel.x < 0) vel.x = std::min(0.f, vel.x + frictionAccel);
    if (vel.z > 0) vel.z = std::max(0.f, vel.z - frictionAccel);
    if (vel.z < 0) vel.z = std::min(0.f, vel.z + frictionAccel);
}
void BallIntegrateSystem::handleFloorContact(GameContext* context, Entity& ball) {
    auto [cBall, cTransform3D, cVelocity3D] =
        context->registry.getComponents<CBall, CTransform3D, CVelocity3D>(ball);
    if (!cBall || !cTransform3D || !cVelocity3D) return;

    Vec3& vel = cVelocity3D->vel_mps;
    Vec3& spin = cBall->spin;
    auto& table = context->tableParameters;

    const float R = cBall->ballRadius;
    const float Fn = cBall->mass * 9.8f;

    // Snap to floor
    cTransform3D->pos_m.y = table.floorY;

    // Bounce
    if (vel.y < 0.f)
        vel.y = -vel.y * cBall->restitution * table.floorRestitution;

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
    float frictionAccel = table.floorFrictionCoefficient * Fn * context->frameStats.dt;
    if (vel.x > 0) vel.x = std::max(0.f, vel.x - frictionAccel);
    if (vel.x < 0) vel.x = std::min(0.f, vel.x + frictionAccel);
    if (vel.z > 0) vel.z = std::max(0.f, vel.z - frictionAccel);
    if (vel.z < 0) vel.z = std::min(0.f, vel.z + frictionAccel);
}
