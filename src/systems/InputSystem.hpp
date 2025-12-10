#pragma once
#include <SFML/Graphics.hpp>
#include "../game/utils/GameContext.hpp"
#include "../ecs/system/ISystem.hpp"
class InputSystem : public ISystem{
public:
    SystemExec update(GameContext* context);
};
