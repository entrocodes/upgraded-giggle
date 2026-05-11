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
        cInput->actions["StartBackswing"] = (raw.isAxisJustPressed("LT") || raw.isKeyJustPressed(sf::Keyboard::Z));
        cInput->actions["EnableBackswing"] = (raw.isAxisDown("RT") || raw.isKeyDown(sf::Keyboard::C));
        cInput->actions["StartAttack"] = (raw.isAxisReleased("LT") || raw.isKeyReleased(sf::Keyboard::Z));
        cInput->actions["StopPush"] = raw.isAxisReleased("LT");
        cInput->actions["StopBackswing"] = (raw.isAxisReleased("RT") || raw.isKeyReleased(sf::Keyboard::C));

        float kbJ1X = 0.f;
        float kbJ1Y = 0.f;
        float kbJ2X = 0.f;
        float kbJ2Y = 0.f;

        float keyAxisX = 0.f;
        float keyAxisY = 0.f;

        if (raw.isKeyDown(sf::Keyboard::A)) keyAxisX -= 1.f;
        if (raw.isKeyDown(sf::Keyboard::D)) keyAxisX += 1.f;
        if (raw.isKeyDown(sf::Keyboard::W)) keyAxisY -= 1.f;
        if (raw.isKeyDown(sf::Keyboard::S)) keyAxisY += 1.f;

        // Blend with controller
        cInput->axes["J1X"] += keyAxisX;
        cInput->axes["J1Y"] += keyAxisY;

        // Clamp
        cInput->axes["J1X"] = std::clamp(cInput->axes["J1X"], -1.f, 1.f);
        cInput->axes["J1Y"] = std::clamp(cInput->axes["J1Y"], -1.f, 1.f);


        if (raw.isKeyDown(sf::Keyboard::J)) kbJ2X -= 1.f;
        if (raw.isKeyDown(sf::Keyboard::L)) kbJ2X += 1.f;
        if (raw.isKeyDown(sf::Keyboard::I)) kbJ2Y -= 1.f;
        if (raw.isKeyDown(sf::Keyboard::K)) kbJ2Y += 1.f;

        cInput->axes["J2X"] = std::clamp(cInput->axes["J2X"] + kbJ2X, -1.f, 1.f);
        cInput->axes["J2Y"] = std::clamp(cInput->axes["J2Y"] + kbJ2Y, -1.f, 1.f);


    }

    return { SystemExecResult::Ran };
}
