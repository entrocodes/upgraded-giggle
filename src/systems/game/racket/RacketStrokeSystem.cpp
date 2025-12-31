#include "RacketStrokeSystem.hpp"
#include "components/Components.hpp"
#include <algorithm>

SystemExec RacketStrokeSystem::update(GameContext* context) {
    for (auto entity : context->registry.getEntitiesWith<CRacketHandle, CArm>()) {
        auto [cRacketHandle, cArm] = context->registry.getComponents<CRacketHandle, CArm>(entity);

        Entity eRacket = cRacketHandle->racketEntity;
        auto [cRacketSwing] = context->registry.getComponents<CRacketSwing>(eRacket);

        float dt = context->frameStats.dt;
        if (dt <= 0.0f) continue;

        StrokeState& strokeState = cRacketSwing->strokeState;
        StrokeState& prevStrokeState = cRacketSwing->prevStrokeState;
        prevStrokeState = strokeState;

        float totalTorsoLoad = cRacketSwing->torsoLeftLoad + cRacketSwing->torsoRightLoad;
        Vec2 steer = cRacketSwing->steerIntent;
        if (cRacketSwing->strokeState == StrokeState::Idle) {
            auto cBodyCollision = context->registry.getComponent<CBodyTableCollision>(entity);
            if (cBodyCollision) {
                cRacketHandle->freeOffset_m.z += cBodyCollision->penetration.z;
                context->registry.removeComponent<CBodyTableCollision>(entity);
            }
        }
        if (cRacketSwing->requestBackswing && strokeState == StrokeState::Idle) strokeState = StrokeState::Backswing;
        if (cRacketSwing->requestPush && strokeState == StrokeState::Idle) strokeState = StrokeState::Push;
        if (cRacketSwing->requestStopPush && strokeState == StrokeState::Push) strokeState = StrokeState::PushRecovery;
        if (cRacketSwing->requestReleaseSwing &&
            (strokeState == StrokeState::Backswing || strokeState == StrokeState::BrakedBackSwing)) {

            strokeState = StrokeState::Swing;
            cRacketSwing->strokeTime_ms = 0.0f;
            cRacketSwing->swingDelta_m = Vec3{ 0,0,0 };
            cRacketSwing->backswingOffset_m = Vec3{ 0,0,0 };
            cRacketHandle->strokeWeight = 1.0f;
            cRacketSwing->backswingTime = 0.0f;

        }

        if (cRacketSwing->requestStopBackswing && strokeState == StrokeState::Backswing) strokeState = StrokeState::BrakedBackSwing;

        if (strokeState == StrokeState::Backswing || strokeState == StrokeState::BrakedBackSwing) {
            if (prevStrokeState != StrokeState::Backswing && prevStrokeState != StrokeState::BrakedBackSwing) cRacketSwing->backswingTime = 0.0f;
            if (strokeState != StrokeState::BrakedBackSwing) cRacketSwing->backswingTime = std::min(cRacketSwing->backswingTime + dt, cRacketSwing->maxBackswing);

            float t = cRacketSwing->backswingTime / cRacketSwing->maxBackswing;

            Vec3 pocketPos = { steer.x * 0.4f, -steer.y * 0.4f, -0.3f };
            cRacketSwing->backswingOffset_m = pocketPos * t;
            cRacketHandle->strokeWeight = 0.0f;
        }

        if (strokeState == StrokeState::Push || strokeState == StrokeState::Idle) {
            float sensitivity = 0.8f;
            if (strokeState == StrokeState::Push) {
                float sensitivityZ = 2.0f;
                cRacketHandle->pushOffset_m.z += cRacketSwing->manualReachZ * sensitivity * sensitivityZ * dt;
            }
            cRacketHandle->freeOffset_m.x += steer.x * sensitivity * dt;
            cRacketHandle->freeOffset_m.y += -steer.y * sensitivity * dt;
        }

        if (strokeState == StrokeState::PushRecovery) {
            float pushResetRate = 4.0f;
            float freeResetRate = 5.5f;

            if (cRacketHandle->pushOffset_m.z > 0.0f) cRacketHandle->pushOffset_m.z -= pushResetRate * dt;
            else if (cRacketHandle->freeOffset_m.z > 0.0f) cRacketHandle->freeOffset_m.z -= freeResetRate * dt;

            if (cRacketHandle->freeOffset_m.z <= 0.0f) cRacketHandle->freeOffset_m.z = 0.0f;
            if (cRacketHandle->pushOffset_m.z <= 0.0f) cRacketHandle->pushOffset_m.z = 0.0f;

            if (cRacketHandle->freeOffset_m.z <= 0.0f && cRacketHandle->pushOffset_m.z <= 0.0f) strokeState = StrokeState::Idle;
        }
        if (strokeState == StrokeState::Swing) {

            float chargePct = cRacketSwing->backswingTime / cRacketSwing->maxBackswing;
            cRacketSwing->swingSpeed = (0.5f + chargePct + totalTorsoLoad) * context->physicsDebug.strokeSettings.swingSpeedFactor;
            cRacketSwing->strokeTime_ms += dt * 1000.0f;

            float ms = cRacketSwing->strokeTime_ms;
            float effectiveReach = (cRacketHandle->freeOffset_m + cRacketHandle->pushOffset_m + cRacketSwing->swingDelta_m).length();
            float reachRatio = effectiveReach / cArm->maxReach_m;

            float stiffness = std::clamp((reachRatio - 0.85f) / 0.15f, 0.0f, 1.0f);
            float powerMult = 1.0f - stiffness * 0.9f;

            Vec3 j1Steer{ steer.x, -steer.y, 0.0f };
            Vec3 forwardVel{ 0.0f, 0.0f, cRacketSwing->swingSpeed };


            cRacketSwing->strokeBlend = std::min(1.0f, cRacketSwing->strokeBlend + dt * 12.0f);
            cRacketHandle->strokeWeight = cRacketSwing->strokeBlend;

            if (ms < 80.0f) { forwardVel.x += j1Steer.x * 4.5f; forwardVel.y += j1Steer.y * 4.5f; }
            else if (ms < 180.0f) { forwardVel.x += j1Steer.x * 1.0f; forwardVel.y += j1Steer.y * 1.0f; }

            Vec3 delta = forwardVel * dt;

            cRacketSwing->swingDelta_m += delta * powerMult;

            if (cRacketSwing->swingDelta_m.length() > cArm->maxReach_m)
                cRacketSwing->swingDelta_m = cRacketSwing->swingDelta_m.normalized() * cArm->maxReach_m;
            


            if (ms > 300.0f) strokeState = StrokeState::SwingRecovery;
        }

        if (strokeState == StrokeState::SwingRecovery) {
            cRacketHandle->strokeWeight = std::clamp(cRacketHandle->strokeWeight - dt * 4.0f, 0.0f, 1.0f);
            cRacketSwing->swingDelta_m *= (1.0f - dt * 8.0f);
            if (cRacketHandle->strokeWeight == 0.0f) { cRacketSwing->swingDelta_m = Vec3{ 0,0,0 }; strokeState = StrokeState::Idle; }
        }
    }

    return { SystemExecResult::Ran };
}
