#include "RacketSwingRecoverySystem.hpp"
#include "game/pose/PoseIntent.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"
#include <algorithm>

SystemExec RacketSwingRecoverySystem::update(GameContext* context) {
    for (auto [entity, cPose, cRacketHandle, cPoseIntentBuffer, cStrokeState] : context->registry.getEntitiesWithComponents<CPose, CRacketHandle, CPoseIntentBuffer, CStrokeState>()) {
        Entity eRacket = cRacketHandle->racketEntity;
        auto [cRacketSwing] = context->registry.getComponents<CRacketSwing>(eRacket);
        float dt = context->frameStats.dt;
        if (dt <= 0.0f) continue;

        StrokeState& strokeState = cRacketSwing->strokeState;
        if (strokeState != StrokeState::SwingRecovery) continue;

        cRacketSwing->recoveryTime_ms += dt * 1000.0f;

        if (cRacketSwing->extraTorsoRotation > 0.f) {
            float step = cRacketSwing->recoverySpeed; // negative value

            if (cRacketSwing->extraTorsoRotation + step <= 0.f) {
                step = -cRacketSwing->extraTorsoRotation;
                cRacketSwing->extraTorsoRotation = 0.f;
                cRacketSwing->backswingDuration_ms = 0;
                cRacketSwing->recoveryTime_ms = 0;
                cRacketSwing->forwardTorsoRotation = 0;
                strokeState = StrokeState::Idle;
                cPose->pose.armState = ArmState::FreeMove;
                cPose->pose.resetWristOffset = true; // restore
            }
            else {
                cRacketSwing->extraTorsoRotation += step;
            }

            cPoseIntentBuffer->intents.push_back(PoseIntent{
                PoseJointID::CenterPelvis, PoseIntentPhase::Translate, 0,
                PoseIntentType::Rotate, {0, step, 0}
                });
        }
        else {
            cPose->pose.resetWristOffset = true; // restore
            cRacketSwing->backswingDuration_ms = 0;
            cRacketSwing->recoveryTime_ms = 0;
            cRacketSwing->forwardTorsoRotation = 0;
            cRacketSwing->extraTorsoRotation = 0;
            strokeState = StrokeState::Idle;
            cPose->pose.armState = ArmState::FreeMove;
        }
    }
    return { SystemExecResult::Ran };
}