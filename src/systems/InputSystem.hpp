#pragma once
#include <SFML/Graphics.hpp>
#include "../input/RawInputState.hpp"

class InputSystem {
public:
    void update(sf::RenderWindow& window, RawInputState& rawInput);
};
