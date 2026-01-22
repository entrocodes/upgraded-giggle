#pragma once
#include "game/utils/GameContext.hpp"
#include "ecs/system/ISystem.hpp"
#include "game/pose/Pose.hpp"

class PoseArmIKSystem : public ISystem {
public:
    SystemExec update(GameContext* context);


};
