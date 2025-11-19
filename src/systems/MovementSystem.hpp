#pragma once
#include <SFML/System.hpp> // for sf::Time or deltaTime
#include "../game/utils/GameContext.hpp"
#include "../systems/BoundarySystem.hpp"
#include "../systems/BallMovementSystem.hpp"
class MovementSystem {
    BoundarySystem boundaries;
    BallMovementSystem ballMovement;
public:
    void update(GameContext* context, sf::Time dt);
};
