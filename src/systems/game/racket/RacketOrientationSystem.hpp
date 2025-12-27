#pragma once

#include "game/utils/GameContext.hpp"
#include "ecs/system/ISystem.hpp"

class RacketOrientationSystem : public ISystem {
public:
    SystemExec update(GameContext* context);

};
