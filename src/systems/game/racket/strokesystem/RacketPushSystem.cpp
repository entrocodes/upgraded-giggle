#include "RacketPushSystem.hpp"
#include "game/pose/PoseIntent.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"
#include <algorithm>

SystemExec RacketPushSystem::update(GameContext* context) {
    for (auto [entity, cPose, cRacketHandle, cPoseIntentBuffer, cStrokeState] : context->registry.getEntitiesWithComponents<CPose, CRacketHandle, CPoseIntentBuffer, CStrokeState>()) {

        Entity eRacket = cRacketHandle->racketEntity;
        auto [cRacketSwing] = context->registry.getComponents<CRacketSwing>(eRacket);
        Vec2 steer = cRacketSwing->steerIntent;
        float dt = context->frameStats.dt;
        if (dt <= 0.0f) continue;

        StrokeState& strokeState = cRacketSwing->strokeState;
        StrokeState& prevStrokeState = cRacketSwing->prevStrokeState;
        prevStrokeState = strokeState;
        if (strokeState == StrokeState::Push) {
            float sensitivity = .8f;
            cRacketSwing->desiredHandDelta = { steer.x * sensitivity * dt, -steer.y * sensitivity * dt, 0.0f };
            if (steer.length() != 0) {
                cPoseIntentBuffer->intents.push_back(PoseIntent{ PoseJointID::LeftWrist, PoseIntentPhase::Translate, 0, PoseIntentType::Translate, cRacketSwing->desiredHandDelta });
            }
            if (cRacketSwing->manualReachZ > 0) {
                cPose->pose.armState = ArmState::Push;
                float sensitivityZ = 3.2f;
                cRacketHandle->pushOffset_m.z += cRacketSwing->manualReachZ * sensitivity * sensitivityZ * dt;
                cPoseIntentBuffer->intents.push_back(PoseIntent{ PoseJointID::LeftWrist, PoseIntentPhase::Translate, 0, PoseIntentType::Translate, {0,0,  cRacketSwing->manualReachZ * sensitivity * sensitivityZ * dt} });
            }
        }
    }
    return { SystemExecResult::Ran };
}
