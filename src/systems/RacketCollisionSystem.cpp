#include "RacketCollisionSystem.hpp"
#include "../components/Components.hpp"
#include "../math/Vec3.hpp"
#include "../math/physics/collision/BallObjectIntersection.hpp"
#include "../debug/Debug.hpp"

void RacketCollisionSystem::update(GameContext* context, float dt)
{
    // Iterate over all rackets with physical properties
    for (auto racket : context->registry.getEntitiesWith<CRacketPhysical>()) {
        auto [cRacketPhys, cRacketPos3D, cRacketBounds, cRacketVel] =
            context->registry.getComponents<
            CRacketPhysical,
            CTransform3D,
            CBoundingBox3D,
            CVelocity3D
            >(racket);

        if (!cRacketPhys || !cRacketPos3D || !cRacketBounds || !cRacketVel)
            continue; // skip this racket, don't bail the whole system

        // Reset color after some frames without a hit
        cRacketBounds->iter_color++;
        if (cRacketBounds->iter_color >= 50) {
            cRacketBounds->color = sf::Color::Green;
        }

        // Sweep for balls
        for (auto ball : context->registry.getEntitiesWith<CBall, CTransform3D, CBoundingBox3D, CVelocity3D>()) {

            auto [cBall, cBallPos, cBallBounds, cBallVel] =
                context->registry.getComponents<
                CBall,
                CTransform3D,
                CBoundingBox3D,
                CVelocity3D
                >(ball);

            if (!cBall || !cBallPos || !cBallBounds || !cBallVel)
                continue;

            // Broad-phase intersection: ball AABB vs racket AABB
            if (!BallObjectIntersection::intersects(cBallBounds->box,
                cRacketBounds->box))
                continue;

            // Relative motion (ball vs racket)
            Vec3 relVel = cBallVel->vel_mps - cRacketVel->vel_mps;

            // Velocity along racket normal
            float vN = relVel.dot(cRacketPhys->normal);

            // If ball moving away from the racket, skip
            if (vN >= 0.f)
                continue;

            // Decompose relative velocity
            Vec3 vNormal = cRacketPhys->normal * vN;
            Vec3 vTang = relVel - vNormal;

            // Bounce: reflect + scale normal, keep tangential
            Vec3 newVNormal = -vNormal * cRacketPhys->restitution;
            Vec3 newVel = newVNormal + vTang;

            // Add racket's own motion back
            cBallVel->vel_mps = newVel + cRacketVel->vel_mps;

            // Spin generation from tangential contact
            Vec3 spinDelta = cRacketPhys->normal.cross(vTang) *
                (cRacketPhys->friction * 0.5f);
            cBall->spin += spinDelta;

            // Push ball outside racket volume along normal (simple separation)
            float pushOutDist = cBall->radius_m;
            cBallPos->pos_m += cRacketPhys->normal * pushOutDist;

            // Visual feedback on racket hitbox
            cRacketBounds->color = sf::Color::Red;
            cRacketBounds->iter_color = 0;

            Debug::debugPrint("Racket Hit", "vN: " + std::to_string(vN));

            // Optional debug draws
            if (context->physicsDebug.debugSpinArrows) {
                Debug::queueArrow3D(
                    cBallPos->pos_m,
                    cBallPos->pos_m + cBallVel->vel_mps.normalized() * 0.2f,
                    sf::Color::Yellow
                );
            }
        }
    }
}
