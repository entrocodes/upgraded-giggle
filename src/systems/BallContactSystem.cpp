#include "BallContactSystem.hpp"

#include "components/Components.hpp"
#include "math/Constants.hpp"

SystemExec BallContactSystem::update(GameContext* context) {
    for (auto eBall : context->registry.getEntitiesWith<CBall>()) {
        auto [cBall, cBallBoundingBox3D, cBallTransform3D, cBallVelocity3D] = context->registry.getComponents<CBall, CBoundingBox3D, CTransform3D, CVelocity3D>(eBall);

        if (!cBall || !cBallBoundingBox3D || !cBallTransform3D || !cBallVelocity3D) continue;

        if (cBall->contactSurface != ContactSurface::None) {
            handleContact(context, eBall);
        }
    }
        return { SystemExecResult::Ran };
}
void BallContactSystem::handleContact(GameContext* context, Entity& ball) {
    auto [cBall, cTransform3D, cVelocity3D] =
        context->registry.getComponents<CBall, CTransform3D, CVelocity3D>(ball);
    if (!cBall || !cTransform3D || !cVelocity3D) return;

    if (cBall->contactSurface == ContactSurface::None) {
        cBall->contactingSurface = false;
        cBall->impactResolved = false;
        return;
    }
    Vec3& vel = cVelocity3D->vel_mps;
    Vec3& spin = cBall->spin;          // world-space axis, rev/s

    const float R = cBall->ballRadius;
    const float m = cBall->mass;
    const float g = 9.8f;

    ContactSurface contactType = cBall->contactSurface;

    auto& tableParameters = context->tableParameters;
    float surfaceY;
    float surfaceRestitution;
    float surfaceDecayRate;
    float surfaceFrictionCoefficient;
    if (contactType == ContactSurface::Table) {
        surfaceY = tableParameters.tableY;
        surfaceRestitution = tableParameters.tableRestitution;
        surfaceDecayRate = tableParameters.tableSpinDecayRate;
        surfaceFrictionCoefficient = tableParameters.tableFrictionCoefficient;
    }
    else {
        surfaceY = tableParameters.floorY;
        surfaceRestitution = tableParameters.floorRestitution;
        surfaceDecayRate = tableParameters.floorSpinDecayRate;
        surfaceFrictionCoefficient = tableParameters.floorFrictionCoefficent;
    }
  
    float targetY = surfaceY + R;
    float penetration = targetY - cTransform3D->pos_m.y;


    // NOT touching → fully reset contact state
    if (penetration < -0.001f) {
        cBall->impactResolved = false;
        cBall->contactSurface = ContactSurface::None;
        return;
    }
    cBall->contactingSurface = (penetration >= -0.001f);

    if (!cBall->contactingSurface) {
        cBall->impactResolved = false;
        return;
    }


    // --- Normal bounce ---
    if (!cBall->impactResolved && vel.y < 0.f) {
        vel.y = -vel.y * cBall->restitution * surfaceRestitution;
        cBall->impactResolved = true;
    }
    // --- Snap to surface ---
    cTransform3D->pos_m.y = surfaceY + R;
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
    vel.x -= vRelAtContact.x * tableParameters.spinToLinearFactor * t;
    vel.z -= vRelAtContact.z * tableParameters.spinToLinearFactor * t;

    // --- Spin decay ---
    float decay =
        tableParameters.rollSpinDecayRate * (1.f - t) +
        surfaceDecayRate * t;

    spin *= (1.f - decay);

    // ============================
    //  Coulomb sliding friction
    // ============================

    float frictionAccel =
        (surfaceFrictionCoefficient * m * g / m) *
        context->frameStats.dt;

    if (vel.x > 0.f) vel.x = std::max(0.f, vel.x - frictionAccel);
    if (vel.x < 0.f) vel.x = std::min(0.f, vel.x + frictionAccel);
    if (vel.z > 0.f) vel.z = std::max(0.f, vel.z - frictionAccel);
    if (vel.z < 0.f) vel.z = std::min(0.f, vel.z + frictionAccel);

    cBall->impactResolved = true;
}

