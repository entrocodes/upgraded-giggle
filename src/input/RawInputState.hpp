#pragma once
#include <SFML/Window/Keyboard.hpp>
#include <unordered_map>

struct RawInputState {
    std::unordered_map<sf::Keyboard::Key, bool> keyStates;

    bool isKeyDown(sf::Keyboard::Key key) const {
        auto it = keyStates.find(key);
        return it != keyStates.end() && it->second;
    }
};
