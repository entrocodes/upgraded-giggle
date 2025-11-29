#pragma once
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Joystick.hpp>
#include <unordered_map>
#include "../math/Vec2.hpp"

struct RawInputState {
    // Keyboard
    std::unordered_map<sf::Keyboard::Key, bool> keyStates;
    std::unordered_map<sf::Keyboard::Key, bool> prevKeyStates;

    // Mouse
    std::unordered_map<sf::Mouse::Button, bool> mouseButtonStates;
    std::unordered_map<sf::Mouse::Button, bool> prevMouseButtonStates;
    Vec2 mousePosition;

    // Gamepad (index 0 only for now)
    std::unordered_map<unsigned int, bool> padStates;
    std::unordered_map<unsigned int, bool> prevPadStates;

    // --- Keyboard helpers ---
    bool isKeyDown(sf::Keyboard::Key k) const {
        auto it = keyStates.find(k);
        return it != keyStates.end() && it->second;
    }
    bool isKeyPressed(sf::Keyboard::Key k) const {
        bool prev = prevKeyStates.count(k) ? prevKeyStates.at(k) : false;
        bool curr = keyStates.count(k) ? keyStates.at(k) : false;
        return !prev && curr;
    }
    bool isKeyReleased(sf::Keyboard::Key k) const {
        bool prev = prevKeyStates.count(k) ? prevKeyStates.at(k) : false;
        bool curr = keyStates.count(k) ? keyStates.at(k) : false;
        return prev && !curr;
    }

    // --- Mouse helpers ---
    bool isMouseButtonDown(sf::Mouse::Button b) const {
        auto it = mouseButtonStates.find(b);
        return it != mouseButtonStates.end() && it->second;
    }
    bool isMouseButtonPressed(sf::Mouse::Button b) const {
        bool prev = prevMouseButtonStates.count(b) ? prevMouseButtonStates.at(b) : false;
        bool curr = mouseButtonStates.count(b) ? mouseButtonStates.at(b) : false;
        return !prev && curr;
    }
    bool isMouseButtonReleased(sf::Mouse::Button b) const {
        bool prev = prevMouseButtonStates.count(b) ? prevMouseButtonStates.at(b) : false;
        bool curr = mouseButtonStates.count(b) ? mouseButtonStates.at(b) : false;
        return prev && !curr;
    }

    // --- Gamepad helpers ---
    bool isGamepadDown(const std::string& btn) const {
        unsigned int b = buttonMap.at(btn);
        auto it = padStates.find(b);
        return it != padStates.end() && it->second;
    }
    bool isGamepadPressed(const std::string& btn) const {
        unsigned int b = buttonMap.at(btn);
        bool prev = prevPadStates.count(b) ? prevPadStates.at(b) : false;
        bool curr = padStates.count(b) ? padStates.at(b) : false;
        return !prev && curr;
    }
    bool isGamepadReleased(const std::string& btn) const {
        unsigned int b = buttonMap.at(btn);
        bool prev = prevPadStates.count(b) ? prevPadStates.at(b) : false;
        bool curr = padStates.count(b) ? padStates.at(b) : false;
        return prev && !curr;
    }

    // Call once per frame at END of InputSystem::update()
    void nextFrame() {
        prevKeyStates = keyStates;
        prevMouseButtonStates = mouseButtonStates;
        prevPadStates = padStates;
    }

    // Xbox / Switch-style button mapping for SFML index
    static inline std::unordered_map<std::string, unsigned int> buttonMap = {
        {"A", 0}, {"B", 1}, {"X", 2}, {"Y", 3},
        {"LB", 4}, {"RB", 5},
        {"Select", 6}, {"Start", 7}
    };
};
