#include "RacketStrokeStateSystem.hpp"
#include "game/pose/PoseIntent.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"
#include <algorithm>

SystemExec RacketStrokeStateSystem::update(GameContext* context) {
    for (auto [entity, cPose, cRacketHandle, cPoseIntentBuffer, cStrokeState] : context->registry.getEntitiesWithComponents<CPose, CRacketHandle, CPoseIntentBuffer, CStrokeState>()) {

        Entity eRacket = cRacketHandle->racketEntity;
        auto [cRacketSwing] = context->registry.getComponents<CRacketSwing>(eRacket);

        float dt = context->frameStats.dt;
        if (dt <= 0.0f) continue;

        StrokeState& strokeState = cRacketSwing->strokeState;
        StrokeState& prevStrokeState = cRacketSwing->prevStrokeState;
        prevStrokeState = strokeState;

        if (cRacketSwing->requestPush && strokeState == StrokeState::Idle) strokeState = StrokeState::Push;
        if (cRacketSwing->requestStopPush && strokeState == StrokeState::Push) strokeState = StrokeState::PushRecovery;
        if (cRacketSwing->requestBackswing && strokeState == StrokeState::Idle) {
            strokeState = StrokeState::Backswing;
            cPose->pose.armState = ArmState::SwingStroke;
            cRacketSwing->preStrokeWristTarget = cPose->pose.leftWrist().targetOffsetFromBind; // snapshot
            cRacketSwing->forwardTorsoRotation = 0.0f;
        }

        //brake backswing
        if (cRacketSwing->requestStopBackswing && strokeState == StrokeState::Backswing) {
            strokeState = StrokeState::BrakedBackswing;
        }
        // release swing after backswing
        if (cRacketSwing->requestReleaseSwing &&
            (strokeState == StrokeState::Backswing || strokeState == StrokeState::BrakedBackswing)) {
            strokeState = StrokeState::Swing;
            cRacketSwing->swingTime_ms = 0.0f;
            cRacketSwing->backswingOffset_m = Vec3{ 0,0,0 };
        }

    }
    return { SystemExecResult::Ran };
}
