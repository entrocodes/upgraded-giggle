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
        auto mapHold = [&](SDL_GameControllerButton btn,
            sf::Keyboard::Key key,
            const std::string& action)
            {
                bool btnDown = raw.isButtonDown(btn);
                bool btnReleased = raw.isButtonJustReleased(btn);

                bool keyDown = raw.isKeyDown(key);
                bool keyReleased = raw.isKeyReleased(key);

                bool down = btnDown || keyDown;
                bool released = btnReleased || keyReleased;

                if (down) {
                    // Controller gets duration; keyboard is frame-based (>=1)
                    int hold = btnDown
                        ? raw.getButtonHoldDuration(btn, currentTick)
                        : 1;

                    cInput->holdTime[action] = hold;
                }
                else {
                    cInput->holdTime[action] = 0;
                }

                cInput->actions[action] = released;
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
