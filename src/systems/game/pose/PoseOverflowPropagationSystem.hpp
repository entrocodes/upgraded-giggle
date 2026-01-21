#pragma once
#include "game/utils/GameContext.hpp"
#include "game/pose/Pose.hpp"
#include "ecs/system/ISystem.hpp"

class PoseOverflowPropagationSystem : public ISystem {
public:
    SystemExec update(GameContext* context);
private:
    void propagateRacketChain(Pose& pose);
    void propagateArmtoPelvis(Pose& pose);
};
