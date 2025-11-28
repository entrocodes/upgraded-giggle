#include "InputSystem.hpp"

void InputSystem::update(GameContext* context) {
    sf::Joystick::update();

    if (sf::Joystick::isConnected(0)) {

        // Right Stick
        context->rawInput.moveX = sf::Joystick::getAxisPosition(0, sf::Joystick::U) / 100.f;
        context->rawInput.moveY = sf::Joystick::getAxisPosition(0, sf::Joystick::V) / 100.f;

        // Left Stick 
        context->rawInput.aimX = sf::Joystick::getAxisPosition(0, sf::Joystick::X) / 100.f;
        context->rawInput.aimY = sf::Joystick::getAxisPosition(0, sf::Joystick::Y) / 100.f;
    }


    // --- Keyboard polling ---
    sf::Keyboard::Key keys[] = {
        sf::Keyboard::W,
        sf::Keyboard::A,
        sf::Keyboard::S,
        sf::Keyboard::D,
        sf::Keyboard::P,
        sf::Keyboard::Escape
    };

    for (auto key : keys) {
        context->rawInput.keyStates[key] = sf::Keyboard::isKeyPressed(key);
    }

    // --- Mouse polling ---
    sf::Mouse::Button buttons[] = {
        sf::Mouse::Left,
        sf::Mouse::Right,
        sf::Mouse::Middle
    };

    for (auto button : buttons) {
        context->rawInput.mouseButtonStates[button] = sf::Mouse::isButtonPressed(button);
    }

    context->rawInput.mousePosition = Vec2(sf::Mouse::getPosition(context->window).x, sf::Mouse::getPosition(context->window).y);

}
