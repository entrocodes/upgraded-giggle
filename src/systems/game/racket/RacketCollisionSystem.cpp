// RacketCollisionSystem.cpp
#include "RacketCollisionSystem.hpp"

#include "components/Components.hpp"
#include "math/Vec3.hpp"
#include "math/Constants.hpp" // expects PI, DEG2RAD, etc.
#include "math/physics/collision/ObjectIntersection.hpp"
#include "debug/Debug.hpp"
#include "systems/game/imgui/GameImGuiConsole.hpp"
#include <algorithm>
#include <cmath>

namespace {
    inline void clampSpinRevPerSec(Vec3& spinRevPerSec, float maxRevPerSec) {
        float s = spinRevPerSec.length();
        if (s > maxRevPerSec && s > 1e-6f) {
            spinRevPerSec *= (maxRevPerSec / s);
        }
    }

    inline float safeLength(const Vec3& v) {
        float l = v.length();
        return (l < 1e-8f) ? 0.f : l;
    }
}

SystemExec RacketCollisionSystem::update(GameContext* context)
{
    for (auto entity : context->registry.getEntitiesWith<CRacketHandle, CRacketSwing>()) {
        auto [cRacketHandle, cRacketSwing] =
            context->registry.getComponents<CRacketHandle, CRacketSwing>(entity);

        Entity eRacket = cRacketHandle->racketEntity;

        auto [cRacketPhysical, cRacketTransform3D, cRacketBoundingBox3D, cRacketVelocity3D] =
            context->registry.getComponents<CRacketPhysical, CTransform3D, CBoundingBox3D, CVelocity3D>(eRacket);

        if (!cRacketPhysical || !cRacketTransform3D || !cRacketBoundingBox3D || !cRacketVelocity3D)
            continue;

        // Face normal (world), must point OUT of the rubber face
        Vec3 n = cRacketPhysical->worldNormal.normalized();

        for (auto eBall : context->registry.getEntitiesWith<CBall, CTransform3D, CVelocity3D>()) {
            auto [cBall, cBallTransform3D, cBallVelocity3D, cBallBoundingBox3D] =
                context->registry.getComponents<CBall, CTransform3D, CVelocity3D, CBoundingBox3D>(eBall);

            if (!cBall || !cBallTransform3D || !cBallVelocity3D || !cBallBoundingBox3D)
                continue;

            checkIntersection(context, cRacketTransform3D, cBallTransform3D);
            if (cBall->racketLastContact) continue;
            const float spinBefore = cBall->spin.length();

            // --- Broad phase ---
            Vec3 toBall = cBallTransform3D->pos_m - cRacketTransform3D->pos_m;
            float distSq = toBall.lengthSq();
            float combinedRadius = 0.25f; // approx racket size + ball radius
            if (distSq > combinedRadius * combinedRadius) continue;

            // --- Plane distance (signed) ---
            float distFromPlane = toBall.dot(n);
            float R = cBall->ballRadius;
            if (std::abs(distFromPlane) > R) continue;

            // --- Radial (in-plane) check ---
            Vec3 pointOnPlane = toBall - (n * distFromPlane);
            if (pointOnPlane.lengthSq() > 0.0225f) continue; // 0.15m radius squared

            // --- Relative velocity (racket treated kinematic) ---
            Vec3 relVel = cBallVelocity3D->vel_mps - cRacketVelocity3D->vel_mps;

            // Must be moving toward the face
            float vN = relVel.dot(n);
            if (vN >= 0.f) continue;

            // --- Coefficients ---
            float e = cRacketPhysical->restitution * cBall->restitution; // restitution
            float mu = cRacketPhysical->friction;                            // Coulomb coefficient
            float m = cBall->mass;

            // --- Contact lever arm (approx center -> contact point) ---
            // Contact point on the face in direction -n
            Vec3 r = -n * R;

            // --- Slip velocity at contact ---
            // IMPORTANT: spin is in REV/S (world). Convert to RAD/S for ¦Ø¡Ár.
            Vec3 omega = cBall->spin * (2.f * PI); // rad/s
            Vec3 vSpinAtContact = omega.cross(r);      // m/s
            Vec3 vRelAtContact = relVel + vSpinAtContact;

            // Decompose contact-relative velocity
            float vNc = vRelAtContact.dot(n);           // should be < 0
            if (vNc >= 0.f) continue;                   // if spin makes it separating, skip
            Vec3 vTc = vRelAtContact - n * vNc;         // tangential slip at contact

            // --- Normal impulse magnitude (kinematic racket) ---
            // Jn = -(1+e) * vNc * m   (kg*m/s), vNc is negative -> Jn positive
            float Jn = -(1.f + e) * vNc * m;

            // --- Tangential impulse (Coulomb limited) ---
            // Desire: cancel tangential slip (simple approximation): Jt_des = -m * vTc
            Vec3 Jt_des = -vTc * m * context->physicsDebug.slipKillFactor;
            
            float racketSpeed = cRacketVelocity3D->vel_mps.length();
            float muEff = mu;

            if (racketSpeed < 0.3f) {
                muEff *= 0.5f;
            }
            // Clamp by Coulomb: |Jt| <= mu * Jn
            float maxJt = muEff * Jn;

            float normalSpeed = -vNc; // positive
            float spinGate = std::clamp(normalSpeed / 3.0f, 0.f, 1.f);
            Jt_des *= spinGate;

            // Sweet spot scaling (kept from your original intent)
            float distFromCenterSq = pointOnPlane.lengthSq();
            float maxRadiusSq = 0.0225f;
            float normalizedDist = distFromCenterSq / maxRadiusSq;
            float sweetSpotFactor = std::clamp(1.0f - (normalizedDist * 0.6f), 0.4f, 1.0f);

            maxJt *= sweetSpotFactor;

            Vec3 Jt = Jt_des;
            float JtMag = safeLength(Jt);
            if (JtMag > maxJt && JtMag > 1e-8f) {
                Jt *= (maxJt / JtMag);
            }

            // --- Apply impulses to LINEAR velocity ---
            Vec3 J = n * Jn + Jt;                 // total impulse on ball
            cBallVelocity3D->vel_mps += J / m;    // v += J/m

            // --- Apply impulse to SPIN ---
            // Angular impulse: ¦¤L = r ¡Á Jt
            Vec3 deltaL = r.cross(Jt);            // kg*m^2/s

            // Sphere inertia: I = 2/5 m R^2
            float I = (2.f / 5.f) * m * R * R;

            Vec3 deltaOmega = (I > 1e-8f) ? (deltaL / I) : Vec3(0, 0, 0); // rad/s
            Vec3 deltaSpinRev = deltaOmega / (2.f * PI);                 // rev/s

            // Damping + grip
            cBall->spin =
                (cBall->spin * 0.3f) +
                (deltaSpinRev * context->physicsDebug.racketGripFactor);

            // Clamp spin
            clampSpinRevPerSec(cBall->spin, context->physicsDebug.maxBallSpin_revps);

            // --- Anti-tunneling position correction ---
            // penetration depth (how far inside plane along normal)
            float penetrationDepth = R - std::abs(distFromPlane);
            float side = (distFromPlane >= 0.f) ? 1.f : -1.f;
            cBallTransform3D->pos_m += n * (side * (penetrationDepth + 1e-4f));

            // Debug colors
            cBallBoundingBox3D->color = sf::Color::Red;
            cRacketBoundingBox3D->color = sf::Color::Red;
            
            cBall->racketLastContact = true;
            if (context->physicsDebug.logImpulses) {

                // After you update cBall->spin and clamp it:
                const float spinAfter = cBall->spin.length();

                ImGuiConsoleQueue(
                    "[RacketContact] vNc=" + std::to_string(vNc) +
                    " |vTc|=" + std::to_string(vTc.length()) +
                    " Jn=" + std::to_string(Jn) +
                    " |Jt|=" + std::to_string(Jt.length()) +
                    " maxJt=" + std::to_string(maxJt) +
                    " spin " + std::to_string(spinBefore) + "->" + std::to_string(spinAfter) + " rev/s"
                );
            }

             if (context->physicsDebug.debugDrawImpulses) {
                 Vec3 pos = cBallTransform3D->pos_m;
                 Debug::queueArrow3D(pos, pos + (n * 0.15f), sf::Color::Cyan);
                 Vec3 jtDir = (Jt.length() > 1e-6f) ? Jt.normalized() : Vec3(0,0,0);
                 Debug::queueArrow3D(pos, pos + jtDir * 0.15f, sf::Color::Magenta);
             }

        }
    }

    return { SystemExecResult::Ran };
}

void RacketCollisionSystem::checkIntersection(GameContext* context,
    const CTransform3D* cRacketTransform3D,
    const CTransform3D* cBallTransform3D)
{
    float ballZ_prev = cBallTransform3D->lastPos_m.z;
    float ballZ_curr = cBallTransform3D->pos_m.z;
    float racketZ = cRacketTransform3D->pos_m.z;

    // Debug-only: did the ball cross the racket's Z plane this frame?
    if ((ballZ_prev > racketZ && ballZ_curr <= racketZ) ||
        (ballZ_prev < racketZ && ballZ_curr >= racketZ))
    {
        float denom = ballZ_curr - ballZ_prev;
        float t = (std::abs(denom) > 0.0001f) ? (racketZ - ballZ_prev) / denom : 0.0f;

        float intersectY =
            cBallTransform3D->lastPos_m.y +
            (cBallTransform3D->pos_m.y - cBallTransform3D->lastPos_m.y) * t;

        float intersectX =
            cBallTransform3D->lastPos_m.x +
            (cBallTransform3D->pos_m.x - cBallTransform3D->lastPos_m.x) * t;

        context->physicsDebug.yAtPlaneContact = intersectY - cRacketTransform3D->pos_m.y;
        //context->physicsDebug.xAtPlaneContact = intersectX - cRacketTransform3D->pos_m.x;
    }
}
