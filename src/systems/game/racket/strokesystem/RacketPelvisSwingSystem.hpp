#pragma once

#include "game/utils/GameContext.hpp"
#include "ecs/system/ISystem.hpp"

class RacketPelvisSwingSystem : public ISystem {
public:
    SystemExec update(GameContext* context);

};
