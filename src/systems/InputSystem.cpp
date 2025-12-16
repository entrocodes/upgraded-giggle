#include "InputSystem.hpp"
#include "../math/Vec2.hpp"
#include "../debug/Debug.hpp"
#include <SFML/Window/Joystick.hpp> // Needed for axis polling

SystemExec InputSystem::update(GameContext* context) {
    RawInputState& raw = context->rawInput;
    bool gamepadConnected = sf::Joystick::isConnected(0);

    // --- Keyboard Poll ---
    // Clears raw.keyStates.previousKeyStates and copies current key states to it.
    // The previous state is needed for isKeyReleased / isKeyJustPressed checks.
    for (int k = 0; k < sf::Keyboard::KeyCount; ++k) {
        raw.keyStates[(sf::Keyboard::Key)k] =
            sf::Keyboard::isKeyPressed((sf::Keyboard::Key)k);
    }

    // --- Mouse Button Poll ---
    for (int b = 0; b < sf::Mouse::ButtonCount; ++b) {
        raw.mouseButtonStates[(sf::Mouse::Button)b] =
            sf::Mouse::isButtonPressed((sf::Mouse::Button)b);
    }

    // --- Mouse Position Poll ---
    raw.mousePosition = Vec2(
        sf::Mouse::getPosition(context->window).x,
        sf::Mouse::getPosition(context->window).y
    );

    // --- Gamepad Poll (pad 0) ---
    if (gamepadConnected) {
        // Poll Buttons
        for (auto& kv : RawInputState::buttonMap) {
            raw.padStates[kv.second] =
                sf::Joystick::isButtonPressed(0, kv.second);
        }

        // Poll Axes (Storing raw SFML axis values: -100 to +100)
        raw.joyAxisPositions[sf::Joystick::X] = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
        raw.joyAxisPositions[sf::Joystick::Y] = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);
        raw.joyAxisPositions[sf::Joystick::U] = sf::Joystick::getAxisPosition(0, sf::Joystick::U);
        raw.joyAxisPositions[sf::Joystick::V] = sf::Joystick::getAxisPosition(0, sf::Joystick::V);

    }
    else {
        // Clear joystick states if not connected
        raw.joyAxisPositions.clear();
        raw.padStates.clear();
    }

    // Debugging (optional)
    if (gamepadConnected) {
        for (unsigned int i = 0; i < 16; i++) {
            if (sf::Joystick::isButtonPressed(0, i)) {
                // Moved debug to after button poll
                Debug::debugPrint("Pad", "Pressed: " + std::to_string(i));
            }
        }
    }

    // --- CRITICAL: Advance Frame State ---
    raw.nextFrame();

    return { SystemExecResult::Ran };
}