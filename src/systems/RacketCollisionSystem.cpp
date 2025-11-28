#include "RacketCollisionSystem.hpp"
#include "../components/Components.hpp"
#include "../math/Vec3.hpp"
#include "../math/physics/collision/BallObjectIntersection.hpp"
#include "../debug/Debug.hpp"
void RacketCollisionSystem::update(GameContext* context, float dt)
{
    for (auto racket : context->registry.getEntitiesWith<CRacketPhysical>()) {
        auto [cRacketPhys, cRacketPos3D, cRacketBounds, cRacketVel] =
            context->registry.getComponents<
            CRacketPhysical, CTransform3D, CBoundingBox3D, CVelocity3D>(racket);

        if (!cRacketPhys || !cRacketPos3D || !cRacketBounds || !cRacketVel) return;
        cRacketBounds->color = sf::Color::Green;
        // Sweep for balls
        for (auto ball : context->registry.getEntitiesWith<CBall, CTransform3D, CBoundingBox3D, CVelocity3D>()) {

            auto [cBall, cBallPos, cBallBounds, cBallVel] =
                context->registry.getComponents<CBall, CTransform3D, CBoundingBox3D, CVelocity3D>(ball);

            if (!cBall) continue;

            // Broad-phase
            if (!BallObjectIntersection::intersects(cBallBounds->box,
                cRacketBounds->box))
                continue;

            // Compute relative motion and impact normal direction
            Vec3 relVel = cBallVel->vel_mps - cRacketVel->vel_mps;
            float vN = relVel.dot(cRacketPhys->normal);

            if (vN >= 0.f)
                continue; // ball moving away

            // Normal / tangent components
            Vec3 vNormal = cRacketPhys->normal * vN;
            Vec3 vTang = relVel - vNormal;

            // Bounce
            Vec3 newVNormal = -vNormal * cRacketPhys->restitution;
            Vec3 newVel = newVNormal + vTang;

            // Apply racket motion back
            cBallVel->vel_mps = newVel + cRacketVel->vel_mps;

            // Spin generation (contact tangent)
            Vec3 spinDelta = cRacketPhys->normal.cross(vTang) *
                cRacketPhys->friction * 0.5f;
            cBall->spin += spinDelta;

            // Push ball outside racket volume
            float pushOutDist = cBall->radius_m;
            cBallPos->pos_m += cRacketPhys->normal * pushOutDist;
            cRacketBounds->color = sf::Color::Red;
            Debug::debugPrint("Racket Hit", "vN: " + std::to_string(vN));

            // Optional debug draws
            if (context->physicsDebug.debugSpinArrows) {
                Debug::queueArrow3D(cBallPos->pos_m,
                    cBallPos->pos_m + newVel.normalized() * 0.2f,
                    sf::Color::Yellow);
            }
        }
    }
    
}
