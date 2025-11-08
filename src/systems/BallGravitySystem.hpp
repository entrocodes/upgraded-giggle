#pragma once
#include <SFML/System.hpp> // for sf::Time or deltaTime
#include <SFML/Graphics.hpp>
#include "../game/utils/GameContext.hpp"
class BallGravitySystem {
public:
    void update(GameContext* context, float dt);
};
