#define NOMINMAX

#include "RacketSwingSystem.hpp"
#include "components/Components.hpp"
#include <algorithm>

SystemExec RacketSwingSystem::update(GameContext* context) {
    if (context->frameStats.dt <= 0.f)
        return { SystemExecResult::EarlyExit, "dt <= 0" };

    auto* ePlayer = context->registry.getEntity("player");
    if (!ePlayer)
        return { SystemExecResult::EarlyExit, "player entity not found" };

    auto [cPlayerInput, cPlayerRacketSwing, cPlayerRacketHandle] =
        context->registry.getComponents<
        CInput,
        CRacketSwing,
        CRacketHandle
        >(*ePlayer);

    if (!cPlayerInput || !cPlayerRacketSwing || !cPlayerRacketHandle)
        return { SystemExecResult::EarlyExit, "missing player components" };

    const float dt = context->frameStats.dt;

    // --------------------------------------------------
    // Edge-based input
    // --------------------------------------------------
    if (cPlayerInput->actions["StartAttack"]) {
        cPlayerRacketSwing->nowDown = true;
    }
    else if (cPlayerInput->actions["ReleaseAttack"]) {
        cPlayerRacketSwing->nowDown = false;
    }
    bool wasDown = cPlayerRacketSwing->wasAttackDownLastFrame;

    // Press edge → start charging
    if (cPlayerRacketSwing->nowDown) {
        cPlayerRacketSwing->isCharging = true;
    }

    // Release edge → trigger swing
    if (!cPlayerRacketSwing->nowDown && wasDown) {
        cPlayerRacketSwing->isCharging = false;

        float charge = (cPlayerRacketSwing->maxBackswing > 0.f)
            ? cPlayerRacketSwing->backswingTime / cPlayerRacketSwing->maxBackswing
            : 0.f;
        charge = std::clamp(charge, 0.f, 1.f);

        cPlayerRacketSwing->swingSpeed =
            std::max(charge * cPlayerRacketSwing->forwardMultiplier, 0.5f);

        cPlayerRacketSwing->backswingTime = 0.f;
        cPlayerRacketSwing->swingTriggered = true;
        cPlayerRacketSwing->backswingStopped = false;
    }

    cPlayerRacketSwing->wasAttackDownLastFrame = cPlayerRacketSwing->nowDown;

    // --------------------------------------------------
    // Charging (backswing)
    // --------------------------------------------------
    if (cPlayerRacketSwing->isCharging) {
        cPlayerRacketSwing->backswingTime =
            std::min(cPlayerRacketSwing->backswingTime + dt, cPlayerRacketSwing->maxBackswing);

        float charge = (cPlayerRacketSwing->maxBackswing > 0.f)
            ? cPlayerRacketSwing->backswingTime / cPlayerRacketSwing->maxBackswing
            : 0.f;
        charge = std::clamp(charge, 0.f, 1.f);

        const float backswingDistance_m = 0.25f;
        cPlayerRacketHandle->swingOffset_m.z = -charge * backswingDistance_m;

        cPlayerRacketHandle->strokeWeight =
            std::clamp(cPlayerRacketHandle->strokeWeight + dt * 6.f, 0.f, 1.f);
    }
    else {
        // --------------------------------------------------
        // Forward swing (burst)
        // --------------------------------------------------
        if (cPlayerRacketSwing->swingTriggered) {
            cPlayerRacketHandle->swingOffset_m.z =
                std::min(cPlayerRacketHandle->swingOffset_m.z + cPlayerRacketSwing->swingSpeed * dt, 0.f);

            if (cPlayerRacketHandle->swingOffset_m.z >= -0.0001f) {
                cPlayerRacketHandle->swingOffset_m.z = 0.f;
                cPlayerRacketSwing->swingTriggered = false;
            }
        }

        cPlayerRacketHandle->strokeWeight =
            std::clamp(cPlayerRacketHandle->strokeWeight - dt * 8.f, 0.f, 1.f);
    }

    return { SystemExecResult::Ran };
}
