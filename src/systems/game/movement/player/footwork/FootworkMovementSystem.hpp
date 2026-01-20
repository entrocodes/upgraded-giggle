#pragma once

#include "game/utils/GameContext.hpp"
#include "ecs/system/ISystem.hpp"
#include "components/Components.hpp"
#include "movement/Step.hpp"
#include "math/Vec3.hpp"
#include "ecs/Entity.hpp"
class FootworkMovementSystem : public ISystem {
public:
    SystemExec update(GameContext* context);
private:
    void startStep(GameContext* context, Entity eCharacter, CFootworkState& state, CPoseIntentBuffer& poseBuffer, StepRaw raw, const Vec3& direction);
    StepProfile convertStepFromRaw(GameContext* context, const StepRaw& rawStep);
};
