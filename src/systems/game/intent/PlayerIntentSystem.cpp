#include "PlayerIntentSystem.hpp"

#include <cmath>
#include <algorithm> // For std::clamp if needed

#include "components/Components.hpp"
#include "game/utils/GameContext.hpp"
#include "input/RawInputState.hpp"
SystemExec PlayerIntentSystem::update(GameContext* context) {
    if (context->inputBlocked) return { SystemExecResult::EarlyExit, "input blocked" };

    RawInputState& raw = context->rawInput;

    for (auto e : context->registry.getEntitiesWith<Player, CInput>()) {
        auto cInput = context->registry.getComponent<CInput>(e);
        if (!cInput) continue;

        // --- 1. Reset Actions ---
        cInput->actions["MoveLeft"] = false;
        cInput->actions["MoveRight"] = false;
        cInput->actions["MoveForward"] = false;
        cInput->actions["MoveBackward"] = false;
        cInput->actions["StartAttack"] = false;
        cInput->actions["ReleaseAttack"] = false;

        // --- 2. Shoulder/Button Movement (Hold-to-Charge) ---
        // Helper to process the hold logic for movement buttons
        auto processHold = [&](SDL_GameControllerButton btn, const std::string& actionName) {
            if (raw.isButtonDown(btn)) {
                // Calculate hold time using the tick index we stored in InputSystem
                int startTick = raw.framePadPressed.count(static_cast<int>(btn))
                    ? raw.framePadPressed[static_cast<int>(btn)]
                    : context->frameStats.tickIndex;

                cInput->holdTime[actionName] = context->frameStats.tickIndex - startTick;
            }

            if (raw.isButtonJustReleased(btn)) {
                cInput->actions[actionName] = true;
            }
            };

        processHold(SDL_CONTROLLER_BUTTON_LEFTSHOULDER, "MoveLeft");
        processHold(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, "MoveRight");
        processHold(SDL_CONTROLLER_BUTTON_Y, "MoveForward");
        processHold(SDL_CONTROLLER_BUTTON_A, "MoveBackward");

        // --- 3. Analog Axes (SDL Independent Triggers) ---
        // Using the string keys we mapped in the InputSystem poll
        float j1X = raw.getAxis("J1X");
        float j1Y = raw.getAxis("J1Y");
        float j2X = raw.getAxis("J2X");
        float j2Y = raw.getAxis("J2Y");

        // Use LT for Attack Timing, RT for "Torso Load" (Power)
        cInput->actions["StartAttack"] = raw.isAxisJustPressed("LT");
        cInput->actions["ReleaseAttack"] = raw.isAxisReleased("LT");

        // --- 4. Update CInput Component State ---
        cInput->axes["J1X"] = j1X;
        cInput->axes["J1Y"] = j1Y;
        cInput->axes["J2X"] = j2X;
        cInput->axes["J2Y"] = j2Y;
        cInput->axes["LT"] = raw.getAxis("LT");
        cInput->axes["RT"] = raw.getAxis("RT");
    }

    return { SystemExecResult::Ran };
}