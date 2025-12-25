#include "RacketSwingSystem.hpp"
#include "components/Components.hpp"
#include <algorithm>
#include <cmath>
#include "debug/Debug.hpp"

SystemExec RacketSwingSystem::update(GameContext* context) {
    auto* ePlayer = context->registry.getEntity("player");
    auto [cPlayerInput, cPlayerRacketSwing, cPlayerRacketHandle, cPlayerArm, cPlayerPos] =
        context->registry.getComponents<CInput, CRacketSwing, CRacketHandle, CArm, CTransform3D>(*ePlayer);

    float dt = context->frameStats.dt;

    // 1. Torso Load (X and B buttons)
    // Assuming you have holdTime mapped for both left/right rotation
    cPlayerRacketSwing->torsoLeftLoad = cPlayerInput->holdTime["X"];
    cPlayerRacketSwing->torsoRightLoad = cPlayerInput->holdTime["B"];
    // Combine for a master "Torso Factor" for lean/twist
    float totalTorsoLoad = cPlayerRacketSwing->torsoLeftLoad + cPlayerRacketSwing->torsoRightLoad;

    // 2. PHASE 1 & 2: BACKSWING (LT)
    // StartAttack = LT Pressed, ReleaseAttack = LT Released
    if (cPlayerInput->actions["StartAttack"]) {
        cPlayerRacketSwing->isCharging = true;
        cPlayerRacketSwing->swingTriggered = false; // Reset if interrupted
    }

    if (cPlayerRacketSwing->isCharging) {
        // --- THE RT BRAKE LOGIC ---
        // If RT is held (mapped to "Brake" or checked via raw axis), we STOP the backswing timer
        bool isBraking = cPlayerInput->axes["RT"] > 0.5f;

        if (!isBraking) {
            cPlayerRacketSwing->backswingTime = std::min(
                cPlayerRacketSwing->backswingTime + dt,
                cPlayerRacketSwing->maxBackswing
            );
        }

        // J1 defines the START POINT of the Arc
        // Even if braking, J1 is "free move" within the reach sphere
        float t = cPlayerRacketSwing->backswingTime / cPlayerRacketSwing->maxBackswing;

        // J1 X/Y defines the "pocket" position (Lateral/Height)
        cPlayerRacketHandle->arcStartPoint.x = cPlayerInput->axes["J1X"] * 0.4f;
        cPlayerRacketHandle->arcStartPoint.y = -cPlayerInput->axes["J1Y"] * 0.4f;
        // Z is the pull-back depth, now "freezable" by RT
        cPlayerRacketHandle->arcStartPoint.z = -t * 0.5f;

        // During charge, racket follows the start point exactly
        cPlayerRacketHandle->swingOffset_m = cPlayerRacketHandle->arcStartPoint;
        cPlayerRacketHandle->strokeWeight = 1.0f;
    }

    // 3. TRANSITION: RELEASE LT (The Hand-off)
    if (cPlayerInput->actions["ReleaseAttack"] && cPlayerRacketSwing->isCharging) {
        cPlayerRacketSwing->isCharging = false;
        cPlayerRacketSwing->swingTriggered = true;
        cPlayerRacketHandle->strokeTime_ms = 0.f;

        // Calculate exit velocity
        float chargePct = cPlayerRacketSwing->backswingTime / cPlayerRacketSwing->maxBackswing;
        // Swing speed is a mix of backswing depth and total torso rotation
        cPlayerRacketSwing->swingSpeed = (0.5f + chargePct + totalTorsoLoad) * 6.0f;
    }

    // 4. PHASE 3: FORWARD SWING (The Generative Arc)
    if (cPlayerRacketSwing->swingTriggered) {
        cPlayerRacketHandle->strokeTime_ms += dt * 1000.f;
        float ms = cPlayerRacketHandle->strokeTime_ms;

        // Reach Power Dampening
        float currentExtension = cPlayerRacketHandle->swingOffset_m.length();
        float reachRatio = currentExtension / cPlayerArm->maxReach_m;
        float stiffness = std::clamp((reachRatio - 0.85f) / 0.15f, 0.0f, 1.0f);
        float powerMult = 1.0f - (stiffness * 0.9f);

        // Generative steering
        Vec3 j1Steer(cPlayerInput->axes["J1X"], -cPlayerInput->axes["J1Y"], 0.f);
        Vec3 forwardVel(0, 0, cPlayerRacketSwing->swingSpeed * powerMult);

        if (ms < 80.f) {      // COMMIT WINDOW (High steering)
            forwardVel.x += j1Steer.x * 4.5f * powerMult;
            forwardVel.y += j1Steer.y * 4.5f * powerMult;
        }
        else if (ms < 180.f) { // ACCELERATION (Low steering)
            forwardVel.x += j1Steer.x * 1.0f * powerMult;
            forwardVel.y += j1Steer.y * 1.0f * powerMult;
        }

        cPlayerRacketHandle->swingOffset_m += forwardVel * dt;

        // Physical Arm Constraint
        if (cPlayerRacketHandle->swingOffset_m.length() > cPlayerArm->maxReach_m) {
            cPlayerRacketHandle->swingOffset_m = cPlayerRacketHandle->swingOffset_m.normalized() * cPlayerArm->maxReach_m;
        }

        // End of Stroke condition
        if (ms > 250.f || cPlayerRacketHandle->swingOffset_m.z > 0.6f) {
            cPlayerRacketSwing->swingTriggered = false;
            cPlayerRacketSwing->backswingTime = 0.f;
            cPlayerRacketHandle->freeOffset_m = cPlayerRacketHandle->swingOffset_m;
        }
    }
    else if (!cPlayerRacketSwing->isCharging) {
        // Return to neutral blend
        cPlayerRacketHandle->strokeWeight = std::clamp(cPlayerRacketHandle->strokeWeight - dt * 4.f, 0.f, 1.f);
        cPlayerRacketHandle->swingOffset_m = cPlayerRacketHandle->swingOffset_m * (1.f - dt * 8.f);
    }

    return { SystemExecResult::Ran };
}