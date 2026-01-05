#pragma once

#include "game/utils/GameContext.hpp"
#include "ecs/system/ISystem.hpp"
#include "movement/Step.hpp"
class FootworkMovementSystem : public ISystem {
public:
    SystemExec update(GameContext* context);
private:
    StepProfile convertStepFromRaw(GameContext* context, const StepRaw& rawStep);
};
