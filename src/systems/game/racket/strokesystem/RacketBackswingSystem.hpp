#pragma once

#include "game/utils/GameContext.hpp"
#include "ecs/system/ISystem.hpp"

class RacketBackswingSystem : public ISystem {
public:
    SystemExec update(GameContext* context);
};
