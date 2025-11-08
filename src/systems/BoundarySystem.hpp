#pragma once
#include "../components/Components.hpp"
#include <SFML/System.hpp> // for sf::Time or deltaTime
#include "../game/utils/GameContext.hpp"
class BoundarySystem {
public:
    void update(GameContext* context);
};
