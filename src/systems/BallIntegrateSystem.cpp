#include "BallIntegrateSystem.hpp"
#include "math/Vec2.hpp"
#include "math/Bounds3D.hpp"
#include "components/Components.hpp"
#include "ecs/Entity.hpp"
#include "debug/Debug.hpp"
#include "math/GridTransform.hpp"
#include "math/MathHelpers.hpp"
#include "math/Constants.hpp"
#include <algorithm>
#include <iostream>

SystemExec BallIntegrateSystem::update(GameContext* context) {
    updateOffTable(context);

    bool didWork = false;

    for (auto eBall : context->registry.getEntitiesWith<CBall>()) {
        auto [cBallBall, cBallBoundingBox3D, cBallTransform3D, cBallVelocity3D] =
            context->registry.getComponents<
            CBall, CBoundingBox3D, CTransform3D, CVelocity3D
            >(eBall);

        if (!cBallBall || !cBallBoundingBox3D ||
            !cBallTransform3D || !cBallVelocity3D)
            continue;

        // ✅ Count as work once we reach here
        didWork = true;

        cBallTransform3D->lastPos_m = cBallTransform3D->pos_m;

        // --- INTEGRATE POSITION ---
        cBallVelocity3D->vel_mps +=
            cBallBall->bForces.acceleration * context->frameStats.dt;

        cBallTransform3D->pos_m +=
            cBallVelocity3D->vel_mps * context->frameStats.dt;

        Vec3 p = cBallTransform3D->pos_m;

        cBallBall->contactingTable = (p.y <= context->tableParameters.tableY);
        cBallBall->onFloor = (p.y <= context->tableParameters.floorY);

        if (cBallBall->contactingTable && !cBallBall->offTable && !cBallBall->hasFallen) {
            handleTableContact(context, eBall);
        }
        else if (cBallBall->onFloor && cBallBall->offTable) {
            handleFloorContact(context, eBall);
        }

        if (!cBallBall->hasFallen && cBallBall->offTable && p.y < 0.f) {
            cBallBall->hasFallen = true;
        }

        cBallBoundingBox3D->box =
            Bounds3D(p - cBallBall->ballRadius, p + cBallBall->ballRadius);

        // --- DEBUG: SPIN / MAGNUS VISUALIZATION ---

        if (context->physicsDebug.debugBallSpinArrows) {
            Vec3 pos = cBallTransform3D->pos_m;

            // ---- Spin axis (world angular velocity) ----
            // spin is in rev/s → visualize direction only, scale for readability
            Vec3 spinAxis = cBallBall->spin;
            float spinMag = spinAxis.length();

            if (spinMag > 0.001f) {
                Vec3 spinDir = spinAxis.normalized();

                Debug::queueArrow3D(
                    pos,
                    pos + spinDir * 0.15f,
                    sf::Color::Magenta   // Spin axis
                );
            }

            // ---- Magnus force direction ----
            // ω (rad/s) × v
            Vec3 omega = cBallBall->spin * (2.f * 3.14159265f);
            Vec3 magnusDir = MathHelpers::cross(omega, cBallVelocity3D->vel_mps);

            if (magnusDir.lengthSq() > 1e-6f) {
                magnusDir = magnusDir.normalized();

                Debug::queueArrow3D(
                    pos,
                    pos + magnusDir * 0.15f,
                    sf::Color::Cyan      // Magnus force direction
                );
            }

            // ---- Velocity reference (optional but helpful) ----
            if (context->physicsDebug.debugBallVelocityArrows) {
                Vec3 vDir = cBallVelocity3D->vel_mps.normalized();

                Debug::queueArrow3D(
                    pos,
                    pos + vDir * 0.15f,
                    sf::Color::Yellow    // Ball velocity
                );
            }
            // Magnus must be perpendicular to velocity
            float dotMV = MathHelpers::dot(magnusDir, cBallVelocity3D->vel_mps);
            assert(std::abs(dotMV) < 1e-3f);

            // Spin magnitude sanity (rev/s)
            assert(cBallBall->spin.length() < 2000.f); // temporary hard ceiling


        }




        // --- SHADOW ---
        Entity eBallShadow = cBallBall->ballShadow;
        auto [cBallShadowTransform, cBallShadowTransform3D] =
            context->registry.getComponents<CTransform, CTransform3D>(eBallShadow);

        if (cBallShadowTransform && cBallShadowTransform3D) {
            // IMPORTANT: Store the last position for interpolation!
            cBallShadowTransform3D->lastPos_m = cBallShadowTransform3D->pos_m;
            cBallShadowTransform3D->lastScale_m = cBallShadowTransform3D->scale_m;
            float shadowY = cBallBall->offTable ? context->tableParameters.floorY : context->tableParameters.tableY;

            // Update current position
            cBallShadowTransform3D->pos_m = Vec3(p.x, shadowY, p.z);

            // Update scale logic
            float heightAboveSurface = p.y - shadowY;
            float scale = std::max(0.5f, 1.5f - 0.2f * heightAboveSurface);
            cBallShadowTransform3D->scale_m = { scale, 1, scale };
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
    cBall->hitRacket = false;
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
}void BallIntegrateSystem::handleFloorContact(GameContext* context, Entity& ball)
{
    auto [cBall, cTransform3D, cVelocity3D] =
        context->registry.getComponents<CBall, CTransform3D, CVelocity3D>(ball);
    if (!cBall || !cTransform3D || !cVelocity3D) return;

    Vec3& vel = cVelocity3D->vel_mps;
    Vec3& spin = cBall->spin;          // world-space axis, rev/s
    auto& table = context->tableParameters;

    const float R = cBall->ballRadius;
    const float m = cBall->mass;
    const float g = 9.8f;

    // --- Snap to floor ---
    cTransform3D->pos_m.y = table.floorY;

    // --- Normal bounce ---
    if (vel.y < 0.f)
        vel.y = -vel.y * cBall->restitution * table.floorRestitution;

    // ============================
    //  Rolling / Sliding Physics
    // ============================

    // Floor normal
    const Vec3 n{ 0.f, 1.f, 0.f };

    // Contact lever arm (center → contact point)
    const Vec3 r = -n * R;

    // Convert spin from rev/s → rad/s
    Vec3 omega = spin * (2.f * PI);

    // Tangential velocity at contact point from spin
    Vec3 vSpinAtContact = omega.cross(r); // m/s

    // Relative tangential velocity at contact
    Vec3 vRelAtContact{
        vel.x + vSpinAtContact.x,
        0.f,
        vel.z + vSpinAtContact.z
    };

    float slipSpeed = std::sqrt(
        vRelAtContact.x * vRelAtContact.x +
        vRelAtContact.z * vRelAtContact.z
    );

    // Blend sliding ↔ rolling (stable, energy-safe)
    float t = std::clamp(slipSpeed / 0.5f, 0.f, 1.f);

    // Frictional velocity correction (never injects energy)
    vel.x -= vRelAtContact.x * table.spinToLinearFactor * t;
    vel.z -= vRelAtContact.z * table.spinToLinearFactor * t;

    // --- Spin decay ---
    float decay =
        table.rollSpinDecayRate * (1.f - t) +
        table.floorSpinDecayRate * t;

    spin *= (1.f - decay);

    // ============================
    //  Coulomb sliding friction
    // ============================

    float frictionAccel =
        (table.floorFrictionCoefficient * m * g / m) *
        context->frameStats.dt;

    if (vel.x > 0.f) vel.x = std::max(0.f, vel.x - frictionAccel);
    if (vel.x < 0.f) vel.x = std::min(0.f, vel.x + frictionAccel);
    if (vel.z > 0.f) vel.z = std::max(0.f, vel.z - frictionAccel);
    if (vel.z < 0.f) vel.z = std::min(0.f, vel.z + frictionAccel);
}

