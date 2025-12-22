#include "RacketCollisionSystem.hpp"
#include "components/Components.hpp"
#include "math/Vec3.hpp"
#include "math/physics/collision/BallObjectIntersection.hpp"
#include "debug/Debug.hpp"

SystemExec RacketCollisionSystem::update(GameContext* context)
{
    for (auto ePlayer : context->registry.getEntitiesWith<Player>()) {
        auto [cPlayerRacketHandle, cPlayerRacketSwing] = context->registry.getComponents<CRacketHandle, CRacketSwing>(ePlayer);
        auto eRacket = cPlayerRacketHandle->racketEntity;
        auto [cRacketPhysical, cRacketTransform3D, cRacketBoundingBox3D, cRacketVelocity3D] =
            context->registry.getComponents<CRacketPhysical, CTransform3D, CBoundingBox3D, CVelocity3D>(eRacket);

        if (!cRacketPhysical || !cRacketTransform3D || !cRacketBoundingBox3D || !cRacketVelocity3D)
            continue;

        for (auto eBall : context->registry.getEntitiesWith<CBall, CTransform3D, CVelocity3D>()) {
            auto [cBallBall, cBallTransform3D, cBallVelocity3D] =
                context->registry.getComponents<CBall, CTransform3D, CVelocity3D>(eBall);

            // 1. DISTANCE CHECK (Optimization)
            Vec3 toBall = cBallTransform3D->pos_m - cRacketTransform3D->pos_m;
            float distSq = toBall.lengthSq();
            float combinedRadius = 0.25f; // Approx racket size + ball radius
            if (distSq > combinedRadius * combinedRadius) continue;

            // 2. PLANE DISTANCE
            // Project ball vector onto racket normal to see how far "above/below" the face it is
            float distFromPlane = toBall.dot(cRacketPhysical->normal);

            // If ball is further away than its radius, it's not touching the face
            if (std::abs(distFromPlane) > cBallBall->radius_m) continue;

            // 3. RADIAL CHECK
            // Find the point on the racket plane closest to the ball
            Vec3 pointOnPlane = toBall - (cRacketPhysical->normal * distFromPlane);
            if (pointOnPlane.lengthSq() > 0.0225f) continue; // 0.15m racket radius squared

            // 4. RELATIVE VELOCITY (Existing logic, now much more accurate)
            Vec3 relativeVelocity3D = cBallVelocity3D->vel_mps - cRacketVelocity3D->vel_mps;
            float velocityNormal = relativeVelocity3D.dot(cRacketPhysical->normal);

            // Ball must be moving TOWARD the face (relative to the normal)
            if (velocityNormal >= 0.f) continue;

            // --- RESOLUTION ---
            // (Your existing bounce and spin code remains, but it's now triggered by the tilt)
            Vec3 vNormal = cRacketPhysical->normal * velocityNormal;
            Vec3 vTang = relativeVelocity3D - vNormal;

            float impactOomph = 1.0f + (cPlayerRacketSwing->swingSpeed * 0.1f);
            Vec3 newVNormal = -vNormal * (cRacketPhysical->restitution * impactOomph);
            cBallVelocity3D->vel_mps = newVNormal + vTang + cRacketVelocity3D->vel_mps;

            // Apply spin based on friction
            cBallBall->spin += cRacketPhysical->normal.cross(vTang) * (cRacketPhysical->friction * 0.5f);

            // Push out to avoid sticking
            cBallTransform3D->pos_m += cRacketPhysical->normal * (cBallBall->radius_m - distFromPlane);

            // Trigger visual hit
            cRacketBoundingBox3D->color = sf::Color::Red;
            cRacketBoundingBox3D->iter_color = 0;
        }
    }
    return { SystemExecResult::Ran };
}