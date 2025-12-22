#include "PlayerIntentSystem.hpp"

#include <cmath>
#include <algorithm> // For std::clamp if needed

#include "helpers/JoystickUtils.hpp"
#include "components/Components.hpp"
#include "game/utils/GameContext.hpp"
#include "input/RawInputState.hpp"
SystemExec PlayerIntentSystem::update(GameContext* context) {
    if (context->inputBlocked) return { SystemExecResult::EarlyExit, "input blocked" };

    RawInputState& ctxRawInput = context->rawInput;
    bool gamepadConnected = sf::Joystick::isConnected(0);

    for (auto e : context->registry.getEntitiesWith<Player, CInput>()) {
        auto cInput = context->registry.getComponent<CInput>(e);
        if (!cInput) continue;

        cInput->actions["MoveLeft"] = false;
        cInput->actions["MoveRight"] = false;
        cInput->actions["MoveForward"] = false;
        cInput->actions["MoveBackward"] = false;
        cInput->actions["StartAttack"] = false;
        cInput->actions["ReleaseAttack"] = false;

        if (ctxRawInput.isGamepadDown("LB")) {
            cInput->holdTime["MoveLeft"] = ctxRawInput.gamePadHeldFor(context, "LB");
            context->playerMovement.moveTriggered = false;
        }
        if (ctxRawInput.isGamepadReleased("LB")) {
            cInput->actions["MoveLeft"] = true;
        }
        if (ctxRawInput.isGamepadDown("RB")) {
            cInput->holdTime["MoveRight"] = ctxRawInput.gamePadHeldFor(context, "RB");
            context->playerMovement.moveTriggered = false;
        }
        
        if (ctxRawInput.isGamepadReleased("RB")) {
            cInput->actions["MoveRight"] = true;
        }
        if (ctxRawInput.isGamepadDown("Y")) {
            cInput->holdTime["MoveForward"] = ctxRawInput.gamePadHeldFor(context, "Y");
            context->playerMovement.moveTriggered = false;
        }

        if (ctxRawInput.isGamepadReleased("Y")) {
            cInput->actions["MoveForward"] = true;
        }
        if (ctxRawInput.isGamepadDown("A")) {
            cInput->holdTime["MoveBackward"] = ctxRawInput.gamePadHeldFor(context, "A");
            context->playerMovement.moveTriggered = false;
        }

        if (ctxRawInput.isGamepadReleased("A")) {
            cInput->actions["MoveBackward"] = true;
        }


        float aimX = 0.f, aimY = 0.f;

        if (gamepadConnected) {
            // Read raw axis data from the InputSystem's poll result
            // ✅ Safe lookup: use count() or a lambda to provide a default value
            float rawAimX = ctxRawInput.joyAxisPositions.count(sf::Joystick::U) ? ctxRawInput.joyAxisPositions.at(sf::Joystick::U) : 0.f;
            float rawAimY = ctxRawInput.joyAxisPositions.count(sf::Joystick::V) ? ctxRawInput.joyAxisPositions.at(sf::Joystick::V) : 0.f;

            aimX = JoystickUtils::processAxis(rawAimX, context->controllerParameters.joyUVDeadZone);
            aimY = JoystickUtils::processAxis(-rawAimY, context->controllerParameters.joyUVDeadZone);

            cInput->actions["StartAttack"] = ctxRawInput.isAxisJustPressed("LT");
            cInput->actions["ReleaseAttack"] = ctxRawInput.isAxisReleased("LT");
        }

        cInput->axes["AimX"] = aimX;
        cInput->axes["AimY"] = aimY;
    }
    return { SystemExecResult::Ran };
}