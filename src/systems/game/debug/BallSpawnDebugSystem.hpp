#pragma once
#include "components/Components.hpp"
#include "game/utils/GameContext.hpp"
#include "ecs/system/ISystem.hpp"
class BallSpawnDebugSystem : public ISystem {
public:
    SystemExec update(GameContext* context);
    void spawnDebugBall(GameContext* context);
};
