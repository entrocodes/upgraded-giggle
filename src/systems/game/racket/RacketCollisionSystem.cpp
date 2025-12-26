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

            checkIntersection(context, cRacketTransform3D, cBallTransform3D);

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

            // --- 4. RELATIVE VELOCITY ---
            Vec3 relativeVelocity3D = cBallVelocity3D->vel_mps - cRacketVelocity3D->vel_mps;
            float velocityNormal = relativeVelocity3D.dot(cRacketPhysical->normal);

            if (velocityNormal >= 0.f) continue;

            // --- RESOLUTION ---

            // 1. Calculate Relative Velocity
            // This includes the racket's speed from the generative arc.
            Vec3 relVel = cBallVelocity3D->vel_mps - cRacketVelocity3D->vel_mps;

            // 2. Decompose into Normal (Bounce) and Tangential (Friction)
            // Note: cRacketPhysical->normal is controlled by J2 in your rotation system
            float vNormalMag = relVel.dot(cRacketPhysical->normal);

            // Guard: Ball must be moving toward the face
            if (vNormalMag >= 0.0f) continue;

            Vec3 vNormal = cRacketPhysical->normal * vNormalMag;
            Vec3 vTang = relVel - vNormal;

            // --- Inside RacketCollisionSystem::update, after calculating relVel ---

            // 1. Calculate Surface Velocity from Incoming Spin
            Vec3 surfaceVelFromSpin = cRacketPhysical->normal.cross(cBallBall->spin) * cBallBall->radius_m;

            // 2. Total Tangential Velocity
            Vec3 totalSlipVel = vTang + surfaceVelFromSpin;

            // 3. APPLY BOUNCE (Normal Force)
            float restitution = cRacketPhysical->restitution * cBallBall->restitution;
            Vec3 bounceImpulse = -vNormal * (1.0f + restitution);

            // 4. THE "PUSH" GRIP (Tangential Force)
            // frictionImpulse now accounts for the ball's spin "biting" into the rubber.
            float friction = cRacketPhysical->friction;
            Vec3 frictionImpulse = -totalSlipVel * friction;
           
            float distFromCenterSq = pointOnPlane.lengthSq();
            float maxRadiusSq = 0.0225f; // 0.15m squared
            float normalizedDist = distFromCenterSq / maxRadiusSq; // 0.0 at center, 1.0 at edge

            // Sweet spot factor: 1.0 in center, drops to 0.4 at the very edge
            float sweetSpotFactor = std::clamp(1.0f - (normalizedDist * 0.6f), 0.4f, 1.0f);

            // Apply to impulses
            frictionImpulse *= sweetSpotFactor;

            // 6. UPDATE BALL SPIN (The "Exchange")
            // We reduce the old spin (damping) and add the new spin from the racket's movement.
            Vec3 newSpinGenerated = cRacketPhysical->normal.cross(frictionImpulse) / cBallBall->radius_m;

            // Conservation of spin: The rubber kills some incoming spin and adds the new "brush"
            cBallBall->spin = (cBallBall->spin * 0.3f) + (newSpinGenerated * context->physicsDebug.racketGripFactor);


            // 1. Calculate the 'Desired' Velocity change (as we discussed before)
            Vec3 targetNormalVel = cRacketPhysical->normal * (vNormalMag * -restitution);
            Vec3 targetVel = vTang + targetNormalVel + frictionImpulse;

            // 2. Convert Velocity Change into Force (F = m * deltaV / dt)
            Vec3 deltaV = targetVel - cBallVelocity3D->vel_mps;
            cBallBall->bForces.racketImpulseForce = (deltaV * cBallBall->mass) / context->frameStats.dt;
            
            // 7. ANTI-TUNNELING: Manually push the ball to the surface so it doesn't collide twice
            float penetrationDepth = cBallBall->radius_m - distFromPlane;
            cBallTransform3D->pos_m += cRacketPhysical->normal * penetrationDepth;
        }
    }
    return { SystemExecResult::Ran };

}
void RacketCollisionSystem::checkIntersection(GameContext* context, const CTransform3D* cRacketTransform3D, const CTransform3D* cBallTransform3D) {
    float ballZ_prev = cBallTransform3D->lastPos_m.z;
    float ballZ_curr = cBallTransform3D->pos_m.z;
    float racketZ = cRacketTransform3D->pos_m.z;

    // Check if the ball crossed the racket's Z-plane this frame
    // (One was behind, the other is now in front)
    if ((ballZ_prev > racketZ && ballZ_curr <= racketZ) ||
        (ballZ_prev < racketZ && ballZ_curr >= racketZ))
    {
        float denom = ballZ_curr - ballZ_prev;
        float t = (std::abs(denom) > 0.0001f) ? (racketZ - ballZ_prev) / denom : 0.0f;
        float intersectY = cBallTransform3D->lastPos_m.y + (cBallTransform3D->pos_m.y - cBallTransform3D->lastPos_m.y) * t;
        float intersectX = cBallTransform3D->lastPos_m.x + (cBallTransform3D->pos_m.x - cBallTransform3D->lastPos_m.x) * t;
        context->physicsDebug.yAtPlaneContact = intersectY - cRacketTransform3D->pos_m.y;
        //context->physicsDebug.xAtPlaneContact = intersectX - cRacketTrans->pos_m.x;
    }
}