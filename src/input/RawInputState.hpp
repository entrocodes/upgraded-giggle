#pragma once
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Joystick.hpp>
#include <unordered_map>
#include <map> // joyAxisPositions is std::map
#include <string>
#include "math/Vec2.hpp"

struct GameContext;

struct RawInputState {
    // Add to RawInputState.hpp
    struct AxisDirection {
        sf::Joystick::Axis axis;
        float threshold; // e.g., 15.0f for LT, -15.0f for RT
    };

    // Map virtual names like "LT" to their axis and direction
    static inline std::unordered_map<std::string, AxisDirection> axisMap = {
        {"LT", {sf::Joystick::Z, 15.0f}},
        {"RT", {sf::Joystick::Z, -15.0f}}
    };

    // Add to your class members
    std::unordered_map<std::string, bool> axisActiveStates;
    std::unordered_map<std::string, bool> prevAxisActiveStates;
    std::unordered_map<std::string, int> frameAxisActivated;
    // --- State Members ---
    std::unordered_map<sf::Keyboard::Key, bool> keyStates;
    std::unordered_map<sf::Keyboard::Key, bool> prevKeyStates;
    std::unordered_map<sf::Keyboard::Key, int> frameKeyPressed;

    std::unordered_map<sf::Mouse::Button, bool> mouseButtonStates;
    std::unordered_map<sf::Mouse::Button, bool> prevMouseButtonStates;
    
    Vec2 mousePosition;

    std::unordered_map<unsigned int, bool> padStates;
    std::unordered_map<unsigned int, bool> prevPadStates;
    std::unordered_map<unsigned int, int> framePadPressed;
    std::map<unsigned int, float> joyAxisPositions; // Polled raw axis position (-100 to 100)
    std::map<unsigned int, float> prevJoyAxisPositions; // Polled raw axis position from previous frame

    // Xbox / Switch-style button mapping for SFML index
    static inline std::unordered_map<std::string, unsigned int> buttonMap = {
        {"A", 0}, {"B", 1}, {"X", 2}, {"Y", 3},
        {"LB", 4}, {"RB", 5},
        {"Select", 6}, {"Start", 7},
        {"LSB", 8}, {"RSB", 9}, // Usually the stick clicks (L3/R3)
    };

    // --- Keyboard helpers ---
    bool isKeyDown(sf::Keyboard::Key k) const;
    bool isKeyJustPressed(sf::Keyboard::Key k) const;
    bool isKeyReleased(sf::Keyboard::Key k) const;
    int keyHeldFor(GameContext* context, sf::Keyboard::Key k) const;
    // --- Mouse helpers ---
    bool isMouseButtonDown(sf::Mouse::Button b) const;
    bool isMouseButtonJustPressed(sf::Mouse::Button b) const;
    bool isMouseButtonReleased(sf::Mouse::Button b) const;
    bool isGamepadDown(const std::string& btn) const;
    bool isGamepadJustPressed(const std::string& btn) const;
    bool isGamepadReleased(const std::string& btn) const;
    int gamePadHeldFor(GameContext* context, const std::string& btn) const;
    bool isAxisDown(const std::string& name) const;
    bool isAxisJustPressed(const std::string& name) const;
    bool isAxisReleased(const std::string& name) const;
    int axisHeldFor(GameContext* context, const std::string& name) const;
    void updateAxisStates(GameContext* context);
    void nextFrame();

};