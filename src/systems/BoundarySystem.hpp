#pragma once
#include "components/Components.hpp"
#include <SFML/System.hpp> // for sf::Time or deltaTime
#include "game/utils/GameContext.hpp"
#include "ecs/system/ISystem.hpp"
class BoundarySystem : public ISystem {
public:
    SystemExec update(GameContext* context);
};
