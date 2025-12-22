#include "RacketCollisionSystem.hpp"
#include "components/Components.hpp"
#include "math/Vec3.hpp"
#include "math/physics/collision/BallObjectIntersection.hpp"
#include "debug/Debug.hpp"

SystemExec RacketCollisionSystem::update(GameContext* context)
{
    // Iterate over all rackets with physical properties
    for (auto eRacket : context->registry.getEntitiesWith<CRacketPhysical>()) {
        auto [cRacketRacketPhysical, cRacketTransform3D, cRacketBoundingBox3D, cRacketVelocity3D] =
            context->registry.getComponents<
            CRacketPhysical,
            CTransform3D,
            CBoundingBox3D,
            CVelocity3D
            >(eRacket);

        if (!cRacketRacketPhysical || !cRacketTransform3D || !cRacketBoundingBox3D || !cRacketVelocity3D)
            return { SystemExecResult::EarlyExit, "One or more rackets missing key components" }; // skip this racket, don't bail the whole system

        // Reset color after some frames without a hit
        cRacketBoundingBox3D->iter_color++;
        if (cRacketBoundingBox3D->iter_color >= 50) {
            cRacketBoundingBox3D->color = sf::Color::Green;
        }

        // Sweep for balls
        for (auto eBall : context->registry.getEntitiesWith<CBall, CTransform3D, CBoundingBox3D, CVelocity3D>()) {

            auto [cBallBall, cBallTransform3D, cBallBoundingBox3D, cBallVelocity3D] =
                context->registry.getComponents<
                CBall,
                CTransform3D,
                CBoundingBox3D,
                CVelocity3D
                >(eBall);

            if (!cBallBall || !cBallTransform3D || !cBallBoundingBox3D || !cBallVelocity3D)
                continue;

            // Broad-phase intersection: ball AABB vs racket AABB
            if (!BallObjectIntersection::intersects(cBallBoundingBox3D->box,
                cRacketBoundingBox3D->box))
                continue;

            // Relative motion (ball vs racket)
            Vec3 relativeVelocity3D = cBallVelocity3D->vel_mps - cRacketVelocity3D->vel_mps;

            // Velocity along racket normal
            float racketVelocityNormal = relativeVelocity3D.dot(cRacketRacketPhysical->normal);

            // If ball moving away from the racket, skip
            if (racketVelocityNormal >= 0.f)
                continue;

            // Decompose relative velocity
            Vec3 vNormal = cRacketRacketPhysical->normal * racketVelocityNormal;
            Vec3 vTang = relativeVelocity3D - vNormal;

            // Bounce: reflect + scale normal, keep tangential
            Vec3 newVNormal = -vNormal * cRacketRacketPhysical->restitution;
            Vec3 newVel = newVNormal + vTang;

            // Add racket's own motion back
            cBallVelocity3D->vel_mps = newVel + cRacketVelocity3D->vel_mps;

            // Spin generation from tangential contact
            Vec3 spinDelta = cRacketRacketPhysical->normal.cross(vTang) *
                (cRacketRacketPhysical->friction * 0.5f);
            cBallBall->spin += spinDelta;

            // Push ball outside racket volume along normal (simple separation)
            float pushOutDist = cBallBall->radius_m;
            cBallTransform3D->pos_m += cRacketRacketPhysical->normal * pushOutDist;

            // Visual feedback on racket hitbox
            cRacketBoundingBox3D->color = sf::Color::Red;
            cRacketBoundingBox3D->iter_color = 0;

            Debug::debugPrint("Racket Hit", "vN: " + std::to_string(racketVelocityNormal));

            // Optional debug draws
            if (context->physicsDebug.debugSpinArrows) {
                Debug::queueArrow3D(
                    cBallTransform3D->pos_m,
                    cBallTransform3D->pos_m + cBallVelocity3D->vel_mps.normalized() * 0.2f,
                    sf::Color::Yellow
                );
            }
        }
    }
    return { SystemExecResult::Ran };
}
