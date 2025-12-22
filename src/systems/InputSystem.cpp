#include "InputSystem.hpp"
#include "math/Vec2.hpp"
#include "debug/Debug.hpp"
#include <SFML/Window/Joystick.hpp>

SystemExec InputSystem::update(GameContext* context) {
    RawInputState& ctxRawInput = context->rawInput;

    // 1. nextFrame() stays here to cycle curr -> prev


    // 2. ONLY Poll things that don't have discrete events or need high precision

    // Mouse Position (Needs to be fresh every frame)
    ctxRawInput.mousePosition = Vec2(
        static_cast<float>(sf::Mouse::getPosition(context->window).x),
        static_cast<float>(sf::Mouse::getPosition(context->window).y)
    );

    // Joystick Axes (Polling is better here than events)
    if (sf::Joystick::isConnected(0)) {
        std::vector<sf::Joystick::Axis> axes = {
            sf::Joystick::X, sf::Joystick::Y, sf::Joystick::Z,
            sf::Joystick::R, sf::Joystick::U, sf::Joystick::V,
            sf::Joystick::PovX, sf::Joystick::PovY
        };

        for (auto axis : axes) {
            if (sf::Joystick::hasAxis(0, axis)) {
                float pos = sf::Joystick::getAxisPosition(0, axis);
                // Apply a small deadzone so the character doesn't drift
                if (std::abs(pos) > 5.0f) {
                    ctxRawInput.joyAxisPositions[axis] = pos;
                }
                else {
                    ctxRawInput.joyAxisPositions.erase(axis);
                }
            }
        }
    }
    else {
        ctxRawInput.joyAxisPositions.clear();
    }

    // NOTE: keyStates and padStates are handled EXCLUSIVELY 
    // by the Event Loop in GameEngine.cpp. 
    // Do NOT poll them here or you will break the 'JustPressed' logic.

    return { SystemExecResult::Ran };
}