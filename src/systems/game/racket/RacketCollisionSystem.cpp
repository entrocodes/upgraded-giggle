#include "RacketCollisionSystem.hpp"
#include "components/Components.hpp"
#include "math/Constants.hpp"
#include "math/MathHelpers.hpp"
#include "systems/game/imgui/GameImGuiConsole.hpp"
#include <algorithm>

SystemExec RacketCollisionSystem::update(GameContext* context)
{
    float dt = context->frameStats.dt;
    if (dt <= 0.0f)
        return { SystemExecResult::Ran };

    auto ballEntities = context->registry.getEntitiesWith<CBall, CTransform3D, CVelocity3D>();
    auto racketEntities = context->registry.getEntitiesWith<CRacketPhysical, CRacketSwing, CTransform3D, CVelocity3D>();

    for (auto eBall : ballEntities)
    {
        auto [cBall, cBallTransform3D, cBallVelocity3D] = context->registry.getComponents<CBall, CTransform3D, CVelocity3D>(eBall);
        
        if (cBall->lastContactSurface == ContactSurface::Racket) continue;

        Vec3 bStart = cBallTransform3D->lastPos_m;
        Vec3 bEnd = cBallTransform3D->pos_m;

        bool hitThisFrame = false;

        for (auto eRacket : racketEntities)
        {
            auto [cRacketPhysical, cRacketSwing, cRacketTransform3D, cRacketVelocity3D] = context->registry.getComponents<CRacketPhysical, CRacketSwing, CTransform3D, CVelocity3D>(eRacket);
            Vec3 rStart = cRacketTransform3D->lastPos_m;
            Vec3 rEnd = cRacketTransform3D->pos_m;
            // 1. CCD closest approach
            MathHelpers::ClosestPoints cp = MathHelpers::findClosestPoints(bStart, bEnd, rStart, rEnd);
            if ((rEnd - rStart).lengthSq() < .00001) //if racket is still
            {
                cp.posRacket = rEnd;
                cp.posBall = MathHelpers::closestPointOnSegment(bStart, bEnd, rEnd);
            }
            // 2. Radius check
            float generosity = (cRacketSwing->strokeState != StrokeState::Idle) ? 0.06f : 0.02f;
            float combinedRadius = cBall->radius_m + cRacketPhysical->radius_m + generosity; //ball radius + racket radius + a lil extra

            // --- Z-plane diagnostic ---
            float z0 = bStart.z;
            float z1 = bEnd.z;
            float rZ = rEnd.z;
            
            // DEBUG
            if ((z0 - rZ) * (z1 - rZ) <= 0.0f && z0 != z1)
            {
                float tPlane = (rZ - z0) / (z1 - z0);
                Vec3 bAtPlane = bStart + (bEnd - bStart) * tPlane;
                context->physicsDebug.yAtPlaneContact = bAtPlane.y - rEnd.y;
            }
            // END DEBUG

            if ((cp.posBall - cp.posRacket).lengthSq() > combinedRadius * combinedRadius) continue;
            // 3. Front-face test
            Vec3 n = cRacketPhysical->worldNormal;
            if ((cp.posBall - cp.posRacket).dot(n) >= 0.0f) continue;
            // 4. Temporal snap
            float snapT = std::clamp(cp.tBall - 0.01f, 0.0f, 1.0f);
            cBallTransform3D->pos_m = bStart + (bEnd - bStart) * snapT;
            // 5. Normal impulse
            Vec3 vBall = cBallVelocity3D->vel_mps;
            Vec3 vRacket = cRacketVelocity3D->vel_mps;

            float vDotN = vBall.dot(n);
            if (vDotN >= 0.0f) continue;

            float restitution = cRacketPhysical->restitution * cBall->restitution;
            float jn = -(1.0f + restitution) * vDotN;

            Vec3 vNormalOut = n * jn;
            Vec3 vTanBall = vBall - n * vDotN;
            // 6. Racket push
            float powerMult = 1.0f + (cRacketSwing->torsoLeftLoad + cRacketSwing->torsoRightLoad) * 0.75f;
            Vec3 push =  n * std::max(0.0f, vRacket.dot(n)) * powerMult * 1.3f;
            // 7. Spin computation
            Vec3 vTanRacket = vRacket - n * vRacket.dot(n);
            Vec3 omegaRad = cBall->spin * (2.0f * PI);
            Vec3 r = -n * cBall->radius_m;    
            Vec3 vSpinSurface = omegaRad.cross(r);

            Vec3 vTanRel = vTanRacket - (vTanBall + vSpinSurface);

            float tanSpeed = vTanRel.length();

            Vec3 spinRev{ 0,0,0 };
            float spinGain = 1;
            float authority = 1;
            if (tanSpeed > 0.0001f)
            {
                Vec3 racketUp = cRacketPhysical->worldUp;
                racketUp = (racketUp - n * racketUp.dot(n)).normalized();
                Vec3 racketRight = n.cross(racketUp).normalized();

                float slipUp = vTanRel.dot(racketUp);
                float slipSide = vTanRel.dot(racketRight);

                float friction = cRacketPhysical->friction;
                float jtMax = friction * jn;

                float reqUp = std::abs(slipUp) * 0.15f;
                float reqSide = std::abs(slipSide) * 0.10f;

                Vec2 jtReq{ reqSide, reqUp };              // (side, up)
                float jtLen = jtReq.length();

                float jtScale = (jtLen > 1e-6f) ? std::min(1.0f, jtMax / jtLen) : 0.0f;
                float jtUp = reqUp * jtScale;
                float jtSide = reqSide * jtScale;


                float spinUpRad = (jtUp / cBall->radius_m) * (slipUp > 0.0f ? -1.0f : 1.0f);
                float spinSideRad = (jtSide / cBall->radius_m) * (slipSide > 0.0f ? 1.0f : -1.0f);

                Vec3 spinRad = racketRight * spinUpRad + racketUp * spinSideRad;

                float maxSpinRad = 200.0f;
                float spinMag = spinRad.length();
                if (spinMag > maxSpinRad) spinRad *= maxSpinRad / spinMag;

                spinRev = spinRad / (2.0f * PI);


                //float spinAuthority =
                //    std::clamp((jn - context->physicsDebug.jnMin) / (context->physicsDebug.jnMax - context->physicsDebug.jnMin), 0.0f, 1.0f);
                //authority =
                //    tanSpeed / (tanSpeed + 3.5f);

                //authority *= spinAuthority;


                spinGain = tanSpeed / (tanSpeed + context->physicsDebug.kSpinResistance);

                spinRev *= authority * spinGain;

            }

            // 8. Final velocity
            cBallVelocity3D->vel_mps = vNormalOut + vTanBall + push + vTanRel * (0.15f + 0.1f * authority);

            float stickiness = std::clamp(authority * spinGain, 0.0f, 1.0f);
            Vec3 targetSpin = cBall->spin + spinRev * powerMult;

            // blend: low authority => keep old spin, high => approach target spin
            cBall->spin = cBall->spin * (1.0f - stickiness) + targetSpin * stickiness;


            // 9. DEBUG OUTPUT
            ImGuiConsoleQueue("=== RACKET CONTACT ===");
            ImGuiConsoleQueue("tBall", cp.tBall);
            ImGuiConsoleQueue("worldNormal", n);

            ImGuiConsoleQueue("vBall", vBall);
            ImGuiConsoleQueue("vBall dot N", vDotN);
            ImGuiConsoleQueue("vNormal component", n * vDotN);

            ImGuiConsoleQueue("vTanBall", vTanBall);
            ImGuiConsoleQueue("vTanRacket", vTanRacket);
            ImGuiConsoleQueue("vTanRel", vTanRel);
            ImGuiConsoleQueue("Tangential speed", tanSpeed);

            ImGuiConsoleQueue("Normal impulse jn", jn);
            ImGuiConsoleQueue("Spin rev/frame", spinRev);

            hitThisFrame = true;
            cBall->lastContactSurface = ContactSurface::Racket;
            break;
        }

        if (hitThisFrame)
            continue;
    }

    return { SystemExecResult::Ran };
}
