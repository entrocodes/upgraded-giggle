#pragma once
#include "game/utils/GameContext.hpp"
#include "ecs/system/ISystem.hpp"
class RacketTransformSystem : public ISystem {
public:
    SystemExec update(GameContext* context);

};
