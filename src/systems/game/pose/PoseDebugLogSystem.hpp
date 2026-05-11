
#pragma once
#include "ecs/system/ISystem.hpp"
#include "game/utils/GameContext.hpp"
class PoseDebugLogSystem : public ISystem {
public:
    SystemExec update(GameContext* context);
};