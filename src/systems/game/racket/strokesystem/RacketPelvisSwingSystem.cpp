#include "RacketPelvisSwingSystem.hpp"
#include "game/pose/PoseIntent.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"
#include <algorithm>

SystemExec RacketPelvisSwingSystem::update(GameContext* context) {
    for (auto [entity, cPose, cRacketHandle, cPoseIntentBuffer, cStrokeState] : context->registry.getEntitiesWithComponents<CPose, CRacketHandle, CPoseIntentBuffer, CStrokeState>()) {

        Entity eRacket = cRacketHandle->racketEntity;
        auto [cRacketSwing] = context->registry.getComponents<CRacketSwing>(eRacket);

        float dt = context->frameStats.dt;
        if (dt <= 0.0f) continue;

        StrokeState& strokeState = cRacketSwing->strokeState;
        float forwardTorsoRotation = cRacketSwing->forwardTorsoRotation;
        float backswingTorsoRotation = cRacketSwing->backswingTorsoRotation;
        float finishTorsoRotation = -backswingTorsoRotation * 1.35;

        if (strokeState == StrokeState::Swing) {
            cRacketSwing->swingTime_ms += dt * 1000.0f;

            float swingDuration_ms = cRacketSwing->backswingDuration_ms * .7f;
            float swingSpeed = -(cRacketSwing->backswingTorsoRotation / swingDuration_ms) * dt * 1000.0f;

            if (forwardTorsoRotation < finishTorsoRotation) {
                cPoseIntentBuffer->intents.push_back(PoseIntent{ PoseJointID::CenterPelvis, PoseIntentPhase::Translate, 0, PoseIntentType::Rotate, {0, swingSpeed, 0} });
                cRacketSwing->forwardTorsoRotation += swingSpeed;
            }
            else {
                cRacketSwing->extraTorsoRotation = cRacketSwing->forwardTorsoRotation + cRacketSwing->backswingTorsoRotation;
                Debug::debugPrint("forwardTorsoRot", forwardTorsoRotation, false);
                Debug::debugPrint("backswing torso rotation", cRacketSwing->backswingTorsoRotation, false);
                Debug::debugPrint("extra torso rotation", cRacketSwing->extraTorsoRotation, false);
                Debug::debugPrint("finishRot", finishTorsoRotation, false);
                Debug::debugPrint("pelvis read of delta rotation", cPose->pose.centerPelvis().deltaRotation_rad);
                Debug::debugPrint("pelvis read of rest rotation", cPose->pose.centerPelvis().restRotation_rad);
                cRacketSwing->backswingTorsoRotation = 0;
                float recoveryDuration = cRacketSwing->backswingDuration_ms * 2.f; //change for a slower/faster stroke
                cRacketSwing->recoverySpeed = -(cRacketSwing->extraTorsoRotation / recoveryDuration) * dt * 1000.0f;
                strokeState = StrokeState::SwingRecovery;
            }
        }
    }
    return { SystemExecResult::Ran };
}
