#include "PlayerInputSystem.hpp"
#include "../components/components.hpp"
#include "../game/utils/GameContext.hpp"
#include <cmath>

static float applyDeadzone(float v, float dz = 0.15f) {
    return (std::fabs(v) < dz) ? 0.f : v;
}

void PlayerInputSystem::update(GameContext* context) {
    if (context->inputBlocked) return;

    bool gamepadConnected = sf::Joystick::isConnected(0);

    for (auto e : context->registry.getEntitiesWith<Player, CInput>()) {
        auto cInput = context->registry.getComponent<CInput>(e);
        if (!cInput) continue;

        cInput->actions.clear();
        cInput->axes.clear();

        // 🔹 Attack buttons (backhand hold/release)
        cInput->actions["AttackDown"] =
            context->rawInput.isKeyDown(sf::Keyboard::L) ||
            (gamepadConnected && context->rawInput.isGamepadDown("LB"));

        cInput->actions["ReleaseAttack"] =
            context->rawInput.isKeyReleased(sf::Keyboard::L) ||
            (gamepadConnected && context->rawInput.isGamepadDown("LB"));

        cInput->actions["StopBackswing"] =
            context->rawInput.isKeyDown(sf::Keyboard::K) ||
            (gamepadConnected && context->rawInput.isGamepadDown("RB"));

        float moveX = 0.f, moveZ = 0.f;
        float aimX = 0.f, aimY = 0.f;

        if (gamepadConnected) {
            moveX = applyDeadzone(sf::Joystick::getAxisPosition(0, sf::Joystick::X) / 100.f, context->controllerParameters.joyXYDeadZone);
            moveZ = applyDeadzone(-sf::Joystick::getAxisPosition(0, sf::Joystick::Y) / 100.f, context->controllerParameters.joyXYDeadZone);

            aimX = applyDeadzone(sf::Joystick::getAxisPosition(0, sf::Joystick::U) / 100.f, context->controllerParameters.joyUVDeadZone);
            aimY = applyDeadzone(-sf::Joystick::getAxisPosition(0, sf::Joystick::V) / 100.f, context->controllerParameters.joyUVDeadZone);
        }
        else {
            if (context->rawInput.isKeyDown(sf::Keyboard::A)) moveX -= 1.f;
            if (context->rawInput.isKeyDown(sf::Keyboard::D)) moveX += 1.f;
            if (context->rawInput.isKeyDown(sf::Keyboard::W)) moveZ += 1.f;
            if (context->rawInput.isKeyDown(sf::Keyboard::S)) moveZ -= 1.f;
        }

        cInput->axes["MoveX"] = moveX;
        cInput->axes["MoveZ"] = moveZ;
        cInput->axes["AimX"] = aimX;
        cInput->axes["AimY"] = aimY;
    }
}
