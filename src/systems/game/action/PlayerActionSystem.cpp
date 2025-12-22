#include "PlayerActionSystem.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"
#include "game/utils/GameContext.hpp"
#include <cmath>
#include <algorithm>

SystemExec PlayerActionSystem::update(GameContext* context) {
    Entity* ePlayer = context->registry.getEntity("player");
    if (!ePlayer) return { SystemExecResult::EarlyExit };

    RawInputState& raw = context->rawInput;

    // Helper for Shoulder Button Movement
    auto handleMove = [&](SDL_GameControllerButton btn, Vec3 dir) {
        if (raw.isButtonJustReleased(btn)) { // Trigger on let-go
            int btnIdx = static_cast<int>(btn);
            int held = 0;

            if (raw.framePadPressed.count(btnIdx)) {
                held = context->frameStats.tickIndex - raw.framePadPressed[btnIdx];
            }

            auto& cPlayerFootworkIntent = context->registry.addComponent<CFootworkIntent>(*ePlayer);
            cPlayerFootworkIntent.direction = dir;
            cPlayerFootworkIntent.heldFrames = held; // This now correctly passes 5, 20, or 60+ frames
            if (btn == SDL_CONTROLLER_BUTTON_A || btn == SDL_CONTROLLER_BUTTON_Y) {
                cPlayerFootworkIntent.directionalStrength = .3f;
            }
            cPlayerFootworkIntent.directionalStrength = 1.0f;
            context->playerMovement.moveTriggered = true;
        }
        };

    // Map LB to Left and RB to Right
    handleMove(SDL_CONTROLLER_BUTTON_LEFTSHOULDER, { -1, 0, 0 });
    handleMove(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, { 1, 0, 0 });
    handleMove(SDL_CONTROLLER_BUTTON_Y, { 0, 1, 0 });
    handleMove(SDL_CONTROLLER_BUTTON_A, { 0, -1, 0 });

    return { SystemExecResult::Ran };
}
