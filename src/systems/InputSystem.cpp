#include "InputSystem.hpp"
#include "../math/Vec2.hpp"
#include "../debug/Debug.hpp"
#include <SFML/Window/Joystick.hpp>

SystemExec InputSystem::update(GameContext* context) {
    RawInputState& raw = context->rawInput;

    // --- 1. CRITICAL: Advance Frame State FIRST ---
    // This moves 'current' to 'previous' so we can poll new 'current' values.
    raw.nextFrame();

    bool gamepadConnected = sf::Joystick::isConnected(0);

    // --- 2. Keyboard Poll ---
    for (int k = 0; k < sf::Keyboard::KeyCount; ++k) {
        auto key = static_cast<sf::Keyboard::Key>(k);
        raw.keyStates[key] = sf::Keyboard::isKeyPressed(key);
    }

    // --- 3. Mouse Button Poll ---
    for (int b = 0; b < sf::Mouse::ButtonCount; ++b) {
        auto button = static_cast<sf::Mouse::Button>(b);
        raw.mouseButtonStates[button] = sf::Mouse::isButtonPressed(button);
    }

    // --- 4. Mouse Position Poll ---
    raw.mousePosition = Vec2(
        static_cast<float>(sf::Mouse::getPosition(context->window).x),
        static_cast<float>(sf::Mouse::getPosition(context->window).y)
    );

    // --- 5. Gamepad Poll (pad 0) ---
    if (gamepadConnected) {
        // Poll Buttons
        for (auto& kv : RawInputState::buttonMap) {
            raw.padStates[kv.second] = sf::Joystick::isButtonPressed(0, kv.second);
        }

        // Poll Axes
        // We poll all common axes to ensure map is fully populated for helpers
        std::vector<sf::Joystick::Axis> axes = {
            sf::Joystick::X, sf::Joystick::Y, sf::Joystick::Z,
            sf::Joystick::R, sf::Joystick::U, sf::Joystick::V,
            sf::Joystick::PovX, sf::Joystick::PovY
        };

        for (auto axis : axes) {
            if (sf::Joystick::hasAxis(0, axis)) {
                raw.joyAxisPositions[axis] = sf::Joystick::getAxisPosition(0, axis);
            }
        }
    }
    else {
        raw.joyAxisPositions.clear();
        raw.padStates.clear();
    }

    return { SystemExecResult::Ran };
}