#include "PlayerIntentSystem.hpp"

#include <algorithm>

#include "components/Components.hpp"
#include "game/utils/GameContext.hpp"
#include "input/RawInputState.hpp"

SystemExec PlayerIntentSystem::update(GameContext* context)
{
    if (context->inputBlocked)
        return { SystemExecResult::EarlyExit };

    RawInputState& raw = context->rawInput;
    int currentTick = context->frameStats.tickIndex;

    for (auto e : context->registry.getEntitiesWith<Player, CInput>()) {
        auto cInput = context->registry.getComponent<CInput>(e);
        if (!cInput) continue;

        // -------------------------------------------------
        // Helper: merge controller + keyboard
        // -------------------------------------------------
        auto mapHold = [&](SDL_GameControllerButton btn, sf::Keyboard::Key key, const std::string& action) {
            bool btnDown = raw.isButtonDown(btn);
            bool keyDown = raw.isKeyDown(key);

            bool btnReleased = raw.isButtonJustReleased(btn);
            bool keyReleased = raw.isKeyReleased(key);

            bool isDown = btnDown || keyDown;
            bool isReleased = btnReleased || keyReleased;

            if (isDown) {
                // If it's a controller, we can use the raw duration
                if (btnDown) {
                    cInput->holdTime[action] = raw.getButtonHoldDuration(btn, currentTick);
                }
                // If it's a keyboard, increment manually or set a flag
                else {
                    cInput->holdTime[action] = raw.getKeyHoldDuration(key, currentTick);
                }
            }
            else if (!isReleased) {
                // Only reset to 0 if it wasn't JUST released this frame
                cInput->holdTime[action] = 0;
            }

            // This will be true ONLY on the frame the button is let go
            cInput->actions[action] = isReleased;
        };

        // -------------------------------------------------
        // Movement
        // -------------------------------------------------
        mapHold(SDL_CONTROLLER_BUTTON_LEFTSHOULDER,
            sf::Keyboard::Left,
            "MoveLeft");

        mapHold(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,
            sf::Keyboard::Right,
            "MoveRight");

        mapHold(SDL_CONTROLLER_BUTTON_Y,
            sf::Keyboard::Up,
            "MoveForward");

        mapHold(SDL_CONTROLLER_BUTTON_A,
            sf::Keyboard::Down,
            "MoveBackward");

        // Optional torso rotation
        mapHold(SDL_CONTROLLER_BUTTON_X,
            sf::Keyboard::Comma,
            "TorsoLeft");

        mapHold(SDL_CONTROLLER_BUTTON_B,
            sf::Keyboard::Period,
            "TorsoRight");

        // -------------------------------------------------
        // Analog axes
        // -------------------------------------------------
        cInput->axes["ReachZ"] = raw.getAxis("LT");
        cInput->axes["J1X"] = raw.getAxis("J1X");
        cInput->axes["J1Y"] = raw.getAxis("J1Y");
        cInput->axes["J2X"] = raw.getAxis("J2X");
        cInput->axes["J2Y"] = raw.getAxis("J2Y");
        // -------------------------------------------------
        // Semantic attack states
        // -------------------------------------------------
        cInput->actions["StartBackswing"] = raw.isAxisJustPressed("LT");
        cInput->actions["EnableBackswing"] = raw.isAxisDown("RT");
        cInput->actions["StartAttack"] = raw.isAxisReleased("LT");
        cInput->actions["StopPush"] = raw.isAxisReleased("LT");
        cInput->actions["StopBackswing"] = raw.isAxisReleased("RT");

        if (raw.isKeyReleased(sf::Keyboard::A)) {
            cInput->axes["J1X"] -= 5;
        }
        if (raw.isKeyReleased(sf::Keyboard::D)) {
            cInput->axes["J1X"] += 5;
        }
        if (raw.isKeyReleased(sf::Keyboard::W)) {
            cInput->axes["J1Y"] -= 5;
        }
        if (raw.isKeyReleased(sf::Keyboard::S)) {
            cInput->axes["J1Y"] += 5;
        }
        if (raw.isKeyReleased(sf::Keyboard::I)) {
            cInput->axes["J2X"] -= 5;
        }
        if (raw.isKeyReleased(sf::Keyboard::K)) {
            cInput->axes["J2X"] += 5;
        }
        if (raw.isKeyReleased(sf::Keyboard::J)) {
            cInput->axes["J2Y"] -= 5;
        }
        if (raw.isKeyReleased(sf::Keyboard::L)) {
            cInput->axes["J2Y"] += 5;
        }
    }

    return { SystemExecResult::Ran };
}
