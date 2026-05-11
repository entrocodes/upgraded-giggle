#pragma once

#include "game/utils/GameContext.hpp"
#include "ecs/system/ISystem.hpp"

class RacketPushRecoverySystem : public ISystem {
public:
    SystemExec update(GameContext* context);

};
