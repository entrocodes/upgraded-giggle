#pragma once
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <unordered_map>
#include <string>
#include <SDL.h>
#include "math/Vec2.hpp"

struct GameContext;

struct RawInputState {
    // SDL Handle for the physical controller
    SDL_GameController* controllerHandle = nullptr;

    // --- Normalized Analog Data (SDL Source) ---
    // Keys: "LT", "RT", "J1X", "J1Y", "J2X", "J2Y"
    std::unordered_map<std::string, float> axes;
    std::unordered_map<std::string, float> prevAxes;
    // --- Digital State Members ---
    std::unordered_map<sf::Keyboard::Key, bool> keyStates;
    std::unordered_map<sf::Keyboard::Key, bool> prevKeyStates;
    std::unordered_map<sf::Keyboard::Key, int> frameKeyPressed;

    std::unordered_map<sf::Mouse::Button, bool> mouseButtonStates;
    std::unordered_map<sf::Mouse::Button, bool> prevMouseButtonStates;
    Vec2 mousePosition;

    // SDL Gamepad Buttons (int keys match SDL_GameControllerButton enum)
    std::unordered_map<int, bool> padStates;
    std::unordered_map<int, bool> prevPadStates;
    std::unordered_map<int, int> framePadPressed;

    // --- Keyboard Helpers ---
    bool isKeyDown(sf::Keyboard::Key k) const;
    bool isKeyJustPressed(sf::Keyboard::Key k) const;
    bool isKeyReleased(sf::Keyboard::Key k) const;

    // --- Mouse Helpers ---
    bool isMouseButtonDown(sf::Mouse::Button b) const;

    // --- Gamepad Button Helpers ---
    bool isButtonDown(SDL_GameControllerButton b) const;
    bool isButtonJustPressed(SDL_GameControllerButton b) const;
    bool isButtonJustReleased(SDL_GameControllerButton b) const;
    float getButtonHoldDuration(SDL_GameControllerButton b, int currentTick) const;
    // --- Analog/Axis Helpers ---
    bool isAxisDown(const std::string& name) const;
    bool isAxisJustPressed(const std::string& name) const;
    bool isAxisReleased(const std::string& name) const;
    float getAxis(const std::string& name) const;
    int getKeyHoldDuration(sf::Keyboard::Key k, int currentTick) const;
    // Lifecycle
    void nextFrame();
};