#include "RacketPushRecoverySystem.hpp"
#include "game/pose/PoseIntent.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"
#include <algorithm>

SystemExec RacketPushRecoverySystem::update(GameContext* context) {
    for (auto [entity, cRacketHandle, cPoseIntentBuffer, cStrokeState] : context->registry.getEntitiesWithComponents<CRacketHandle, CPoseIntentBuffer, CStrokeState>()) {

        Entity eRacket = cRacketHandle->racketEntity;
        auto [cRacketSwing] = context->registry.getComponents<CRacketSwing>(eRacket);

        float dt = context->frameStats.dt;
        if (dt <= 0.0f) continue;

        StrokeState& strokeState = cRacketSwing->strokeState;
        if (strokeState == StrokeState::PushRecovery) {
            float pushResetRate = 4.0f;
            float freeResetRate = 5.5f;

            if (cRacketHandle->pushOffset_m.z > 0.0f)
            {
                cRacketHandle->pushOffset_m.z -= pushResetRate * dt;
                cPoseIntentBuffer->intents.push_back(PoseIntent{ PoseJointID::LeftWrist, PoseIntentPhase::Translate, 0, PoseIntentType::Translate, {0,0,  -pushResetRate * dt} });
            }
            else if (cRacketHandle->freeOffset_m.z > 0.0f) {
                cRacketHandle->freeOffset_m.z -= freeResetRate * dt;
                cPoseIntentBuffer->intents.push_back(PoseIntent{ PoseJointID::LeftWrist, PoseIntentPhase::Translate, 0, PoseIntentType::Translate, {0,0,  -freeResetRate * dt} });
            }

            if (cRacketHandle->freeOffset_m.z <= 0.0f) cRacketHandle->freeOffset_m.z = 0.0f;
            if (cRacketHandle->pushOffset_m.z <= 0.0f) cRacketHandle->pushOffset_m.z = 0.0f;

            if (cRacketHandle->freeOffset_m.z <= 0.0f && cRacketHandle->pushOffset_m.z <= 0.0f) strokeState = StrokeState::Idle;
        }
    }
    return { SystemExecResult::Ran };
}

