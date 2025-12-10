#include "InputSystem.hpp"
#include "../math/Vec2.hpp"
#include "../debug/Debug.hpp"
SystemExec InputSystem::update(GameContext* context) {
    RawInputState& raw = context->rawInput;

    // --- Keyboard poll ---
    for (int k = 0; k < sf::Keyboard::KeyCount; ++k) {
        raw.keyStates[(sf::Keyboard::Key)k] =
            sf::Keyboard::isKeyPressed((sf::Keyboard::Key)k);
    }

    // --- Mouse poll ---
    for (int b = 0; b < sf::Mouse::ButtonCount; ++b) {
        raw.mouseButtonStates[(sf::Mouse::Button)b] =
            sf::Mouse::isButtonPressed((sf::Mouse::Button)b);
    }

    // --- Gamepad poll (only pad 0 currently) ---
    if (sf::Joystick::isConnected(0)) {
        for (auto& kv : RawInputState::buttonMap) {
            raw.padStates[kv.second] =
                sf::Joystick::isButtonPressed(0, kv.second);
        }
    }

    raw.mousePosition = Vec2(
        sf::Mouse::getPosition(context->window).x,
        sf::Mouse::getPosition(context->window).y
    );

    for (unsigned int i = 0; i < 16; i++) {
        if (sf::Joystick::isButtonPressed(0, i)) {
            Debug::debugPrint("Pad", "Pressed: " + std::to_string(i));
        }
    }

    raw.nextFrame(); // <--- CRITICAL
    return { SystemExecResult::Ran };
}
