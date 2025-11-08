#pragma once
#include <SFML/System.hpp> // for sf::Time or deltaTime
#include "../game/utils/GameContext.hpp"

class MovementSystem {
public:
    void update(GameContext* context, sf::Time dt);
};
