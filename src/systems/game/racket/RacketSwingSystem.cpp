#define NOMINMAX

#include "RacketSwingSystem.hpp"
#include "../components/Components.hpp"
#include <algorithm>

SystemExec RacketSwingSystem::update(GameContext* context) {
    if (context->frameStats.dt <= 0.f)
        return { SystemExecResult::EarlyExit, "dt <= 0" };

    auto* player = context->registry.getEntity("player");
    if (!player)
        return { SystemExecResult::EarlyExit, "player entity not found" };

    auto [input, swing, handle] =
        context->registry.getComponents<
        CInput,
        CRacketSwing,
        CRacketHandle
        >(*player);

    if (!input || !swing || !handle)
        return { SystemExecResult::EarlyExit, "missing player components" };

    const float dt = context->frameStats.dt;

    // --------------------------------------------------
    // Edge-based input
    // --------------------------------------------------
    bool nowDown = input->actions["AttackDown"];
    bool wasDown = swing->wasAttackDownLastFrame;
    bool stopBackswing = input->actions["StopBackswing"];

    if (stopBackswing) {
        swing->backswingStopped = true;
        swing->isCharging = false;
    }

    // Press edge → start charging
    if (nowDown && !wasDown && !swing->backswingStopped) {
        swing->isCharging = true;
    }

    // Release edge → trigger swing
    if (!nowDown && wasDown) {
        swing->isCharging = false;

        float charge = (swing->maxBackswing > 0.f)
            ? swing->backswingTime / swing->maxBackswing
            : 0.f;
        charge = std::clamp(charge, 0.f, 1.f);

        swing->swingSpeed =
            std::max(charge * swing->forwardMultiplier, 0.5f);

        swing->backswingTime = 0.f;
        swing->swingTriggered = true;
        swing->backswingStopped = false;
    }

    swing->wasAttackDownLastFrame = nowDown;

    // --------------------------------------------------
    // Charging (backswing)
    // --------------------------------------------------
    if (swing->isCharging) {
        swing->backswingTime =
            std::min(swing->backswingTime + dt, swing->maxBackswing);

        float charge = (swing->maxBackswing > 0.f)
            ? swing->backswingTime / swing->maxBackswing
            : 0.f;
        charge = std::clamp(charge, 0.f, 1.f);

        const float backswingDistance_m = 0.25f;
        handle->swingOffset_m.z = -charge * backswingDistance_m;

        handle->strokeWeight =
            std::clamp(handle->strokeWeight + dt * 6.f, 0.f, 1.f);
    }
    else {
        // --------------------------------------------------
        // Forward swing (burst)
        // --------------------------------------------------
        if (swing->swingTriggered) {
            handle->swingOffset_m.z =
                std::min(handle->swingOffset_m.z + swing->swingSpeed * dt, 0.f);

            if (handle->swingOffset_m.z >= -0.0001f) {
                handle->swingOffset_m.z = 0.f;
                swing->swingTriggered = false;
            }
        }

        handle->strokeWeight =
            std::clamp(handle->strokeWeight - dt * 8.f, 0.f, 1.f);
    }

    return { SystemExecResult::Ran };
}
