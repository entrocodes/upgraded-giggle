#include "BallIntegrateSystem.hpp"

#include "components/Components.hpp"
#include "debug/Debug.hpp"
#include "math/MathHelpers.hpp"
#include "math/Constants.hpp"

#include <algorithm>
#include <iostream>

SystemExec BallIntegrateSystem::update(GameContext* context) {
    bool didWork = false;

    const float dt = context->frameStats.dt;
    auto& tp = context->tableParameters;

    for (auto eBall : context->registry.getEntitiesWith<CBall>()) {
        auto [cBall, cBallBoundingBox3D, cBallTransform3D, cBallVelocity3D] =
            context->registry.getComponents<CBall, CBoundingBox3D, CTransform3D, CVelocity3D>(eBall);

        if (!cBall || !cBallTransform3D || !cBallVelocity3D)
            continue;

        didWork = true;

        cBallTransform3D->lastPos_m = cBallTransform3D->pos_m;

        // --- INTEGRATE ---
        cBallVelocity3D->vel_mps += cBall->bForces.acceleration * dt;
        cBallTransform3D->pos_m += cBallVelocity3D->vel_mps * dt;

        const Vec3 p = cBallTransform3D->pos_m;
        const float R = cBall->ballRadius;

        // --- TABLE BOUNDS (XZ) ---
        // If you want to treat the ball as a sphere for bounds, include R:
        cBall->offTable =
            (p.x < -R) || (p.x > tp.tableWidth + R) ||
            (p.z < -R) || (p.z > tp.tableLength + R);

        // --- SURFACE CONTACT FLAGS (radius-aware) ---
        // "Touching" means the sphere intersects/penetrates the plane.
        // Contact resolution system will decide impact vs rolling/sliding.
        constexpr float eps = 0.001f;
        ContactSurface surface = ContactSurface::None;
        cBall->contactingSurface = false;
        if (p.y - R <= tp.tableY + eps && !cBall->offTable) (cBall->contactSurface = ContactSurface::Table );
        if (p.y - R <= tp.floorY + eps) ( cBall->contactSurface = ContactSurface::Floor );

        if (cBall->offTable && p.y < 0.f) {
            cBall->hasFallen = true;
        }

        // --- SHADOW ---
        Entity eBallShadow = cBall->ballShadow;
        auto [cBallShadowTransform, cBallShadowTransform3D] =
            context->registry.getComponents<CTransform, CTransform3D>(eBallShadow);

        if (cBallShadowTransform && cBallShadowTransform3D) {
            cBallShadowTransform3D->lastPos_m = cBallShadowTransform3D->pos_m;
            cBallShadowTransform3D->lastScale_m = cBallShadowTransform3D->scale_m;

            float shadowY = cBall->offTable ? tp.floorY : tp.tableY;
            cBallShadowTransform3D->pos_m = Vec3(p.x, shadowY, p.z);

            float heightAboveSurface = p.y - shadowY;
            float scale = std::max(0.5f, 1.5f - 0.2f * heightAboveSurface);
            cBallShadowTransform3D->scale_m = { scale, 1.f, scale };
        }

        // --- DEBUG: SPIN / MAGNUS ---
        if (context->physicsDebug.debugBallSpinArrows) {
            Vec3 pos = cBallTransform3D->pos_m;

            Vec3 spinAxis = cBall->spin;
            float spinMag = spinAxis.length();
            if (spinMag > 0.001f) {
                Vec3 spinDir = spinAxis.normalized();
                Debug::queueArrow3D(pos, pos + spinDir * 0.15f, sf::Color::Magenta);
            }

            Vec3 omega = cBall->spin * (2.f * 3.14159265f);
            Vec3 magnusDir = MathHelpers::cross(omega, cBallVelocity3D->vel_mps);

            if (magnusDir.lengthSq() > 1e-6f) {
                magnusDir = magnusDir.normalized();
                Debug::queueArrow3D(pos, pos + magnusDir * 0.15f, sf::Color::Cyan);
            }

            if (context->physicsDebug.debugBallVelocityArrows) {
                Vec3 v = cBallVelocity3D->vel_mps;
                if (v.lengthSq() > 1e-6f) {
                    Vec3 vDir = v.normalized();
                    Debug::queueArrow3D(pos, pos + vDir * 0.15f, sf::Color::Yellow);
                }
            }

            float dotMV = MathHelpers::dot(magnusDir, cBallVelocity3D->vel_mps);
            assert(std::abs(dotMV) < 1e-3f);
            assert(cBall->spin.length() < 2000.f);
        }
    }

    if (!didWork)
        return { SystemExecResult::EarlyExit, "No balls to integrate" };

    return { SystemExecResult::Ran };
}
