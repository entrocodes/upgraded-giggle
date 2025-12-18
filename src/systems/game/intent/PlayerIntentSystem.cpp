#include "PlayerIntentSystem.hpp"
#include "../components/components.hpp"
#include "../game/utils/GameContext.hpp"
#include <cmath>
#include <algorithm> // For std::clamp if needed

#include "../helpers/JoystickUtils.hpp"


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

        cInput->actions["StepLeft"] = 
            raw.isKeyJustPressed(sf::Keyboard::A) ||
            (gamepadConnected && raw.isGamepadDown("LB"));
        cInput->actions["StepRight"] =
            raw.isKeyJustPressed(sf::Keyboard::D) ||
            (gamepadConnected && raw.isGamepadDown("RB"));

        float moveX = 0.f, moveZ = 0.f;
        float aimX = 0.f, aimY = 0.f;

        if (gamepadConnected) {
            // Read raw axis data from the InputSystem's poll result
            float rawAimX = raw.joyAxisPositions.at(sf::Joystick::U);
            float rawAimY = raw.joyAxisPositions.at(sf::Joystick::V);

            // Apply deadzone and conversion using the raw polled values


            aimX = JoystickUtils::processAxis(rawAimX, context->controllerParameters.joyUVDeadZone);
            aimY = JoystickUtils::processAxis(-rawAimY, context->controllerParameters.joyUVDeadZone); // Assuming -Y is Forward/Up
        }
        else {
            // Keyboard Input (WASD)
            if (raw.isKeyDown(sf::Keyboard::A)) aimX -= 1.f;
            if (raw.isKeyDown(sf::Keyboard::D)) aimX += 1.f;
            if (raw.isKeyDown(sf::Keyboard::W)) aimZ += 1.f; // Assuming W is Forward/Up
            if (raw.isKeyDown(sf::Keyboard::S)) aimZ -= 1.f; // Assuming S is Back/Down

            // Mouse Aiming (Implementation depends on game type, here we'll assume a point)
            // If you need mouse aim, you'd calculate the vector from player pos to mouse pos here
        }

        // --- Store Processed Axes (Intent) ---
        cInput->axes["MoveX"] = moveX;
        cInput->axes["MoveZ"] = moveZ;
        cInput->axes["AimX"] = aimX;
        cInput->axes["AimY"] = aimY;
    }
    return { SystemExecResult::Ran };
}