#include "InputSystem.hpp"

void InputSystem::update(sf::RenderWindow& window, RawInputState& rawInput) {
    // Poll real-time key states each frame for smooth input
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

    // Still handle window events (close, etc.)
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
        // Optionally: mouse input or other events
    }
}
