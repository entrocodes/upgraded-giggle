#pragma once
#include "game/utils/GameContext.hpp"
#include "ecs/system/ISystem.hpp"

class PoseRootMotionSystemSystem : public ISystem {
public:
    SystemExec update(GameContext* context);
};
