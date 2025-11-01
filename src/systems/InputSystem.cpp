#include "InputSystem.hpp"

void InputSystem::update(sf::RenderWindow& window, RawInputState& rawInput) {
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
        rawInput.keyStates[key] = sf::Keyboard::isKeyPressed(key);
    }

    // --- Mouse polling ---
    sf::Mouse::Button buttons[] = {
        sf::Mouse::Left,
        sf::Mouse::Right,
        sf::Mouse::Middle
    };

    for (auto button : buttons) {
        rawInput.mouseButtonStates[button] = sf::Mouse::isButtonPressed(button);
    }

    rawInput.mousePosition = Vec2(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);

    // --- Handle window events (close, resize, etc.) ---
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
    }
}
