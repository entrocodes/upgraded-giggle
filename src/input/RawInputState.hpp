#pragma once
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include "../math/Vec2.hpp"
struct RawInputState {
    std::unordered_map<sf::Keyboard::Key, bool> keyStates;
    std::unordered_map<sf::Mouse::Button, bool> mouseButtonStates;
    Vec2 mousePosition;

    bool isKeyDown(sf::Keyboard::Key key) const {
        auto it = keyStates.find(key);
        return it != keyStates.end() && it->second;
    }

    bool isMouseButtonDown(sf::Mouse::Button button) const {
        auto it = mouseButtonStates.find(button);
        return it != mouseButtonStates.end() && it->second;
    }
};
