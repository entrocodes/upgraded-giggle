#define NOMINMAX

#include "RacketSwingSystem.hpp"
#include "../components/Components.hpp"
#include <algorithm>

void RacketSwingSystem::update(GameContext* context, float dt) {
    if (dt <= 0.f) return;

    auto* player = context->registry.getEntity("player");
    if (!player) return;

    auto [input, swing, handle] =
        context->registry.getComponents<
        CInput,
        CRacketSwing,
        CRacketHandle
        >(*player);

    if (!input || !swing || !handle) return;

    // Edge-based button logic ------------------------------
    bool nowDown = input->actions["AttackDown"];   // Held this frame
    bool stopBackswing = input->actions["StopBackswing"];
    bool wasDown = swing->wasAttackDownLastFrame;  // Held last frame
    if (stopBackswing) {
        swing->backswingStopped = true;
        swing->isCharging = false;
    }
    // Press edge
    if (nowDown && !wasDown && !swing->backswingStopped) {
        swing->isCharging = true;
        // don't reset backswingTime here, allow accumulating
    }

    // Release edge
    if (!nowDown && wasDown) {
        swing->isCharging = false;

        // Compute charge 0..1
        float charge = 0.f;
        if (swing->maxBackswing > 0.f)
            charge = swing->backswingTime / swing->maxBackswing;
        charge = std::clamp(charge, 0.f, 1.f);

        // Convert charge ¡ú forward swing speed
        swing->swingSpeed = charge * swing->forwardMultiplier;

        // Reset charge accumulation for next time
        swing->backswingTime = 0.f;

        // Reset offset (return to base)
        handle->swingOffset_m.z = 0.f;

        // Signal the burst to the RacketHandleSystem
        swing->swingTriggered = true;
        swing->backswingStopped = false;
    }

    swing->wasAttackDownLastFrame = nowDown;
    // -------------------------------------------------------

    // Accumulate backswing while holding
    if (swing->isCharging) {
        swing->backswingTime =
            std::min(swing->backswingTime + dt, swing->maxBackswing);

        float charge = swing->maxBackswing > 0.f ?
            swing->backswingTime / swing->maxBackswing : 0.f;
        charge = std::clamp(charge, 0.f, 1.f);

        // Backward offset = -Z
        handle->strokeWeight =
            std::clamp(handle->strokeWeight + dt * 6.f, 0.f, 1.f);

    }
    else {
        // Not charging: if burst already consumed, snap to base
        if (!swing->swingTriggered) {
            handle->swingOffset_m.z = 0.f;
        }
        handle->strokeWeight =
            std::clamp(handle->strokeWeight - dt * 8.f, 0.f, 1.f);

    }
}
