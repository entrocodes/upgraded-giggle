#include "RacketSwingSystem.hpp"
#include "components/Components.hpp"
#include <algorithm>
#include <cmath>
#include "debug/Debug.hpp"
#include <SFML/Graphics.hpp>
SystemExec RacketSwingSystem::update(GameContext* context) {
    auto* ePlayer = context->registry.getEntity("player");
    auto [cPlayerInput, cPlayerRacketSwing, cPlayerRacketHandle, cPlayerArm, cPlayerTransform3D, cPlayerRotation3D] = context->registry.getComponents<CInput, CRacketSwing, CRacketHandle, CArm, CTransform3D, CRotation3D>(*ePlayer);

    float dt = context->frameStats.dt;
    bool nowDown = cPlayerInput->actions.count("StartAttack") && cPlayerInput->actions["StartAttack"];

    // 1. Handle RT (Waist) Load
    cPlayerRacketSwing->torsoLoad = cPlayerInput->axes["RT"];
    cPlayerRotation3D->euler_deg.y = cPlayerRacketSwing->torsoLoad * 45.f;
    // 2. State Transition: Charge to Swing
    if (nowDown) cPlayerRacketSwing->isCharging = true;

    if (cPlayerInput->actions["ReleaseAttack"] && cPlayerRacketSwing->isCharging) {
        cPlayerRacketSwing->isCharging = false;
        cPlayerRacketSwing->swingTriggered = true;

        // Impulse: Speed is a product of Backswing Time + Torso Load
        float chargePct = std::clamp(cPlayerRacketSwing->backswingTime / cPlayerRacketSwing->maxBackswing, 0.1f, 1.0f);
        cPlayerRacketSwing->swingSpeed = (chargePct + (cPlayerRacketSwing->torsoLoad * 1.5f)) * 8.0f; // Multiplier for "oomph"
        cPlayerRacketSwing->backswingTime = 0.f;
    }

    // 3. Backswing (Pulling back)
    // 3. Backswing (Pulling back)
    if (cPlayerRacketSwing->isCharging) {
        cPlayerRacketSwing->backswingTime = std::min(cPlayerRacketSwing->backswingTime + dt, cPlayerRacketSwing->maxBackswing);
        float t = cPlayerRacketSwing->backswingTime / cPlayerRacketSwing->maxBackswing;

        // Use the analog value of LT to control the weight of the pull
        float ltSqueeze = cPlayerInput->axes["LT"];
        cPlayerRacketHandle->strokeWeight = std::clamp(cPlayerRacketHandle->strokeWeight + dt * (2.f + ltSqueeze * 5.f), 0.f, 1.f);

        cPlayerRacketHandle->swingOffset_m.z = -t * 0.4f;
        cPlayerRacketHandle->swingOffset_m.x = t * 0.1f;
    }
    // 4. Forward Swing (The Snap)
    else if (cPlayerRacketSwing->swingTriggered) {
        // Accelerate through the 0-point
        cPlayerRacketHandle->swingOffset_m.z += cPlayerRacketSwing->swingSpeed * dt;

        // Finish swing when follow-through is done
        if (cPlayerRacketHandle->swingOffset_m.z >= 0.2f) {
            cPlayerRacketSwing->swingTriggered = false;
        }
    }
    else {
        // Return to neutral
        cPlayerRacketHandle->strokeWeight = std::clamp(cPlayerRacketHandle->strokeWeight - dt * 4.f, 0.f, 1.f);
        cPlayerRacketHandle->swingOffset_m = cPlayerRacketHandle->swingOffset_m * (1.f - dt * 10.f);
    }

    return { SystemExecResult::Ran };
}