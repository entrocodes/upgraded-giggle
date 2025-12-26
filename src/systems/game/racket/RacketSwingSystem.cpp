#include "RacketSwingSystem.hpp"
#include "components/Components.hpp"
#include <algorithm>
#include <cmath>
#include "debug/Debug.hpp"

SystemExec RacketSwingSystem::update(GameContext* context) {
    for (auto entity : context->registry.getEntitiesWith<CRacketSwing, CRacketHandle, CArm, CTransform3D>()) {
        auto [cAuthorization, cRacketSwing, cRacketHandle, cArm, cTransform3D] = context->registry.getComponents<CAuthorization, CRacketSwing, CRacketHandle, CArm, CTransform3D>(entity);

        float dt = context->frameStats.dt;

        StrokeState& strokeState = cRacketSwing->strokeState;
        StrokeState& prevStrokeState = cRacketSwing->prevStrokeState;
        prevStrokeState = strokeState;
        // 1. Torso Load (X and B buttons)
        cRacketSwing->torsoLeftLoad = cAuthorization->floatMap["TorsoLeftLoad"];
        cRacketSwing->torsoRightLoad = cAuthorization->floatMap["TorsoRightLoad"];
        float totalTorsoLoad = cRacketSwing->torsoLeftLoad + cRacketSwing->torsoRightLoad;

        // 2. PHASE 1 & 2: BACKSWING (LT)
        // StartAttack = LT Pressed, ReleaseAttack = LT Released
        if (cAuthorization->boolMap["RequestBackswing"] && strokeState == StrokeState::Idle) {
            strokeState = StrokeState::Backswing;
        }
        if (cAuthorization->boolMap["RequestPush"] && strokeState == StrokeState::Idle) {
            strokeState = StrokeState::Push;
        }
        if (cAuthorization->boolMap["RequestStopPush"] && strokeState == StrokeState::Push) {
            strokeState = StrokeState::PushRecovery;    
        }
        if (cAuthorization->boolMap["RequestReleaseSwing"] && (strokeState == StrokeState::Backswing || strokeState == StrokeState::BrakedBackSwing)) {
            strokeState = StrokeState::Swing;
        }
        if (cAuthorization->boolMap["RequestStopBackswing"] && strokeState == StrokeState::Backswing) {
            strokeState = StrokeState::BrakedBackSwing;
        }
        Vec2 steer = cAuthorization->vec2Map["SteerIntent"];
        if (strokeState == StrokeState::Backswing || strokeState == StrokeState::BrakedBackSwing) {
            if (prevStrokeState != StrokeState::Backswing && prevStrokeState != StrokeState::BrakedBackSwing) {
                cRacketSwing->backswingTime = 0.0f;
            }
            if (strokeState != StrokeState::BrakedBackSwing) {
                cRacketSwing->backswingTime = std::min(
                    cRacketSwing->backswingTime + dt,
                    cRacketSwing->maxBackswing
                );
            }
   
            float t = cRacketSwing->backswingTime / cRacketSwing->maxBackswing;


            cRacketHandle->strokeWeight = 0.0f;
            cRacketHandle->swingOffset_m = { 0,0,0 };

            Vec3 pocketPos = { steer.x * 0.4f, -steer.y * 0.4f, -0.3f };

            cRacketHandle->swingOffset_m = pocketPos * t;

            cRacketHandle->strokeWeight = 1.0f; 
        }
        if (cRacketSwing->strokeState == StrokeState::Push || cRacketSwing->strokeState == StrokeState::Idle) {
            float sensitivity = .8f;
            if (cRacketSwing->strokeState == StrokeState::Push) {
                float sensitivityZ = 5.0f;
                float manualZ = cAuthorization->floatMap["ManualReachZ"];
                cRacketHandle->freeOffset_m.z += manualZ * sensitivity * sensitivityZ * dt; //make pushoffset separable from free offset later
            }
            cRacketHandle->freeOffset_m.x += steer.x * sensitivity * dt;
            cRacketHandle->freeOffset_m.y += -steer.y * sensitivity * dt;
        }
        if (strokeState == StrokeState::PushRecovery) {
            if (cRacketHandle->freeOffset_m.z > 0) {
                float pushResetRate = 1.0f;
                cRacketHandle->freeOffset_m.z -= pushResetRate * dt;
                if (cRacketHandle->freeOffset_m.z <= 0) cRacketHandle->freeOffset_m.z = 0;
            }
            else {
                strokeState = StrokeState::Idle;
            }
        }
        if (strokeState == StrokeState::Swing) {
            if (prevStrokeState == StrokeState::Backswing || prevStrokeState == StrokeState::BrakedBackSwing) {
                cRacketSwing->strokeTime_ms = 0.f;
            }

            float chargePct = cRacketSwing->backswingTime / cRacketSwing->maxBackswing;
            cRacketSwing->swingSpeed = (0.5f + chargePct + totalTorsoLoad) * 6.0f;
            cRacketSwing->strokeTime_ms += dt * 1000.f;
            float ms = cRacketSwing->strokeTime_ms;

            // Reach Power Dampening
            float currentExtension = cRacketHandle->swingOffset_m.length();
            float reachRatio = currentExtension / cArm->maxReach_m;
            float stiffness = std::clamp((reachRatio - 0.85f) / 0.15f, 0.0f, 1.0f);
            float powerMult = 1.0f - (stiffness * 0.9f);
            // Generative steering
            Vec3 j1Steer(steer.x, -steer.y, 0.f);
            Vec3 forwardVel(0, 0, cRacketSwing->swingSpeed * powerMult);

            if (ms < 80.f) {      // COMMIT WINDOW (High steering)
                forwardVel.x += j1Steer.x * 4.5f * powerMult;
                forwardVel.y += j1Steer.y * 4.5f * powerMult;
            }
            else if (ms < 180.f) { // ACCELERATION (Low steering)
                forwardVel.x += j1Steer.x * 1.0f * powerMult;
                forwardVel.y += j1Steer.y * 1.0f * powerMult;
            }

            cRacketHandle->swingOffset_m += forwardVel * dt;
            // HARD LIMIT: Prevents racket from detaching during ballistic swing
            if (cRacketHandle->swingOffset_m.length() > cArm->maxReach_m) {
                cRacketHandle->swingOffset_m = cRacketHandle->swingOffset_m.normalized() * cArm->maxReach_m;
            }
            // End of Stroke condition
            if (ms > 300.f){//TODO: better conditions 
                strokeState = StrokeState::SwingRecovery;
                cRacketHandle->freeOffset_m = cRacketHandle->swingOffset_m;
            }
        }
        if (strokeState == StrokeState::SwingRecovery) {
            cRacketHandle->strokeWeight = std::clamp(cRacketHandle->strokeWeight - dt * 4.f, 0.f, 1.f);
            cRacketHandle->swingOffset_m = cRacketHandle->swingOffset_m * (1.f - dt * 8.f);
            if (cRacketHandle->strokeWeight == 0) {
                strokeState = StrokeState::Idle;
            }
        }

    }
    
    return { SystemExecResult::Ran };
}