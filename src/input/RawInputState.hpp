#pragma once
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include "../math/Vec2.hpp"

struct RawInputState {
    std::unordered_map<sf::Keyboard::Key, bool> keyStates;
    std::unordered_map<sf::Keyboard::Key, bool> prevKeyStates;

    std::unordered_map<sf::Mouse::Button, bool> mouseButtonStates;
    std::unordered_map<sf::Mouse::Button, bool> prevMouseButtonStates;

    Vec2 mousePosition;

    // --- Keyboard ---
    bool isKeyDown(sf::Keyboard::Key key) const {
        auto it = keyStates.find(key);
        return it != keyStates.end() && it->second;
    }

    bool isKeyPressed(sf::Keyboard::Key key) const {
        bool prev = prevKeyStates.count(key) ? prevKeyStates.at(key) : false;
        bool curr = keyStates.count(key) ? keyStates.at(key) : false;
        return !prev && curr;
    }

    bool isKeyReleased(sf::Keyboard::Key key) const {
        bool prev = prevKeyStates.count(key) ? prevKeyStates.at(key) : false;
        bool curr = keyStates.count(key) ? keyStates.at(key) : false;
        return prev && !curr;
    }

    // --- Mouse ---
    bool isMouseButtonDown(sf::Mouse::Button button) const {
        auto it = mouseButtonStates.find(button);
        return it != mouseButtonStates.end() && it->second;
    }

    bool isMouseButtonPressed(sf::Mouse::Button button) const {
        bool prev = prevMouseButtonStates.count(button) ? prevMouseButtonStates.at(button) : false;
        bool curr = mouseButtonStates.count(button) ? mouseButtonStates.at(button) : false;
        return !prev && curr;
    }

    bool isMouseButtonReleased(sf::Mouse::Button button) const {
        bool prev = prevMouseButtonStates.count(button) ? prevMouseButtonStates.at(button) : false;
        bool curr = mouseButtonStates.count(button) ? mouseButtonStates.at(button) : false;
        return prev && !curr;
    }

    // --- Update each frame ---
    void nextFrame() {
        prevKeyStates = keyStates;
        prevMouseButtonStates = mouseButtonStates;
    }
};
            