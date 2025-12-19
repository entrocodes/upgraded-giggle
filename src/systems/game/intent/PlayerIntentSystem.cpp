#include "PlayerIntentSystem.hpp"

#include <cmath>
#include <algorithm> // For std::clamp if needed

#include "helpers/JoystickUtils.hpp"
#include "components/Components.hpp"
#include "game/utils/GameContext.hpp"
#include "input/RawInputState.hpp"
SystemExec PlayerIntentSystem::update(GameContext* context) {
    if (context->inputBlocked) return { SystemExecResult::EarlyExit, "input blocked" };

    RawInputState& raw = context->rawInput;
    bool gamepadConnected = sf::Joystick::isConnected(0);

    for (auto e : context->registry.getEntitiesWith<Player, CInput>()) {
        auto cInput = context->registry.getComponent<CInput>(e);
        if (!cInput) continue;

        cInput->actions.clear();
        cInput->axes.clear();

        cInput->actions["AttackDown"] =
            raw.isKeyDown(sf::Keyboard::L) ||
            (gamepadConnected && raw.isGamepadDown("LT")); // Assuming "LB" is key name in buttonMap

        cInput->actions["ReleaseAttack"] =
            raw.isKeyReleased(sf::Keyboard::L) ||
            (gamepadConnected && raw.isGamepadReleased("LT")); // Use isGamepadReleased for release

        cInput->actions["StopBackswing"] =
            raw.isKeyDown(sf::Keyboard::K) ||
            (gamepadConnected && raw.isGamepadDown("RT"));

        if (raw.isKeyDown(sf::Keyboard::A)) {
            cInput->holdTime["MoveLeft"] = raw.keyHeldFor(context, sf::Keyboard::A);
        }
        else if (raw.isGamepadDown("LB")) {
            cInput->holdTime["MoveLeft"] = raw.gamePadHeldFor(context, "LB");
            context->playerMovement.moveTriggered = false;
        }
        
        if (raw.isGamepadReleased("LB") || raw.isKeyReleased(sf::Keyboard::A)){
            cInput->actions["MoveLeft"] = true;
        }

        if (raw.isKeyDown(sf::Keyboard::D)) {
            cInput->holdTime["MoveRight"] = raw.keyHeldFor(context, sf::Keyboard::D);
        }
        else if (raw.isGamepadDown("RB")) {
            cInput->holdTime["MoveRight"] = raw.gamePadHeldFor(context, "RB");
            context->playerMovement.moveTriggered = false;
        }
        
        if (raw.isGamepadReleased("RB") || raw.isKeyReleased(sf::Keyboard::A)){
            cInput->actions["MoveRight"] = true;
        }

        float aimX = 0.f, aimY = 0.f;

        if (gamepadConnected) {
            // Read raw axis data from the InputSystem's poll result
            // ✅ Safe lookup: use count() or a lambda to provide a default value
            float rawAimX = raw.joyAxisPositions.count(sf::Joystick::U) ? raw.joyAxisPositions.at(sf::Joystick::U) : 0.f;
            float rawAimY = raw.joyAxisPositions.count(sf::Joystick::V) ? raw.joyAxisPositions.at(sf::Joystick::V) : 0.f;

            aimX = JoystickUtils::processAxis(rawAimX, context->controllerParameters.joyUVDeadZone);
            aimY = JoystickUtils::processAxis(-rawAimY, context->controllerParameters.joyUVDeadZone);

            aimX = JoystickUtils::processAxis(rawAimX, context->controllerParameters.joyUVDeadZone);
            aimY = JoystickUtils::processAxis(-rawAimY, context->controllerParameters.joyUVDeadZone); // Assuming -Y is Forward/Up
        }

        cInput->axes["AimX"] = aimX;
        cInput->axes["AimY"] = aimY;
    }
    return { SystemExecResult::Ran };
}