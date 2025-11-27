#include "InputSystem.hpp"

void InputSystem::update(GameContext* context) {
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
