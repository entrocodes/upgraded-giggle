#include "RacketBackswingSystem.hpp"
#include "game/pose/PoseIntent.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"
#include <algorithm>

SystemExec RacketBackswingSystem::update(GameContext* context) {
    for (auto [entity, cRacketHandle, cPoseIntentBuffer, cStrokeState] : context->registry.getEntitiesWithComponents<CRacketHandle, CPoseIntentBuffer, CStrokeState>()) {

        Entity eRacket = cRacketHandle->racketEntity;
        auto [cRacketSwing] = context->registry.getComponents<CRacketSwing>(eRacket);

        float dt = context->frameStats.dt;
        if (dt <= 0.0f) continue;

        StrokeState& strokeState = cRacketSwing->strokeState;
        float swingRotationSpeedConstant = -dt * 5;

        if (strokeState == StrokeState::Backswing) {
            if (-cRacketSwing->backswingTorsoRotation < cRacketSwing->maxBackSwingTorsoRotation) {
                cRacketSwing->backswingDuration_ms += dt * 1000.0f;
                cRacketSwing->backswingTorsoRotation += swingRotationSpeedConstant;
                cPoseIntentBuffer->intents.push_back(PoseIntent{ PoseJointID::CenterPelvis, PoseIntentPhase::Translate, 0, PoseIntentType::Rotate, {0, swingRotationSpeedConstant, 0} });
            }
            else {
                strokeState = StrokeState::BrakedBackswing;
            }
        }
    }
    return { SystemExecResult::Ran };
}