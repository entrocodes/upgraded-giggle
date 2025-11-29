#include "RacketSwingSystem.hpp"
#include "../components/Components.hpp"

void RacketSwingSystem::update(GameContext* context, float dt) {
    auto* player = context->registry.getEntity("player");
    if (!player) return;

    auto [input, swing] =
        context->registry.getComponents<InputComponent, CRacketSwing>(*player);

    if (!input || !swing) return;

    // Start charging backswing
    if (input->actions["PressAttack"]) {
        swing->isCharging = true;
        swing->swingTriggered = false;
    }

    // Release ¡ú FIRE
    if (input->actions["ReleaseAttack"]) {
        swing->isCharging = false;
        swing->swingTriggered = true;

        float charge = swing->backswingTime / swing->maxBackswing;
        charge = std::min(charge, 1.f);

        swing->swingSpeed = charge * swing->forwardMultiplier;
        swing->backswingTime = 0.f;
    }

    // Accumulate backswing time
    if (swing->isCharging) {
        swing->backswingTime =
            std::min(swing->backswingTime + dt, swing->maxBackswing);
    }
}
