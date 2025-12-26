#include "PlayerIntentSystem.hpp"

#include <cmath>
#include <algorithm> // For std::clamp if needed

#include "components/Components.hpp"
#include "game/utils/GameContext.hpp"
#include "input/RawInputState.hpp"
SystemExec PlayerIntentSystem::update(GameContext* context) {
    if (context->inputBlocked) return { SystemExecResult::EarlyExit };
    RawInputState& raw = context->rawInput;
    int currentTick = context->frameStats.tickIndex;

    for (auto e : context->registry.getEntitiesWith<Player, CInput>()) {
        auto cInput = context->registry.getComponent<CInput>(e);

        // --- Map Movement Keys ---
        auto mapHold = [&](SDL_GameControllerButton btn, const std::string& key) {
            bool released = raw.isButtonJustReleased(btn);
            bool down = raw.isButtonDown(btn); // Assuming you have this helper

            if (down || released) {
                // Capture the duration. If released, this is the final "latched" value
                cInput->holdTime[key] = raw.getButtonHoldDuration(btn, currentTick);
            }
            else {
                cInput->holdTime[key] = 0;
            }

            cInput->actions[key] = released;
            };

        mapHold(SDL_CONTROLLER_BUTTON_LEFTSHOULDER, "MoveLeft");
        mapHold(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, "MoveRight");
        mapHold(SDL_CONTROLLER_BUTTON_Y, "MoveForward");
        mapHold(SDL_CONTROLLER_BUTTON_A, "MoveBackward");
        mapHold(SDL_CONTROLLER_BUTTON_X, "TorsoLeft");
        mapHold(SDL_CONTROLLER_BUTTON_B, "TorsoRight");

        // --- Map Analog Triggers/Sticks ---
        cInput->axes["ReachZ"] = raw.axes["LT"]; // The Push axis

        cInput->axes["J1X"] = raw.axes["J1X"];
        cInput->axes["J1Y"] = raw.axes["J1Y"];

        // Semantic Attack Triggering
        cInput->actions["StartBackswing"] = raw.isAxisJustPressed("LT");
        cInput->actions["EnableBackswing"] = raw.isAxisDown("RT");
        cInput->actions["StartAttack"] = raw.isAxisReleased("LT");
        cInput->actions["StopPush"] = raw.isAxisReleased("LT");
        cInput->actions["StopBackswing"] = raw.isAxisReleased("RT");
    }
    return { SystemExecResult::Ran };
}