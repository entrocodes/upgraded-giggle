#include "RacketIdleSystem.hpp"
#include "game/pose/PoseIntent.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"
#include <algorithm>

SystemExec RacketIdleSystem::update(GameContext* context) {
    for (auto [entity, cPose, cRacketHandle, cPoseIntentBuffer, cStrokeState] : context->registry.getEntitiesWithComponents<CPose, CRacketHandle, CPoseIntentBuffer, CStrokeState>()) {

        Entity eRacket = cRacketHandle->racketEntity;
        auto [cRacketSwing] = context->registry.getComponents<CRacketSwing>(eRacket);
        Vec2 steer = cRacketSwing->steerIntent;
        float dt = context->frameStats.dt;
        if (dt <= 0.0f) continue;

        StrokeState& strokeState = cRacketSwing->strokeState;

        //seems like the wrong place for this if loop
        if (strokeState == StrokeState::Idle) {
            auto cBodyCollision = context->registry.getComponent<CBodyTableCollision>(entity);
            if (cBodyCollision) {
                cRacketHandle->freeOffset_m.z += cBodyCollision->penetration.z;
                context->registry.removeComponent<CBodyTableCollision>(entity);
            }
        }

        if (strokeState == StrokeState::Idle) {
            cPose->pose.armState = ArmState::FreeMove;
            float sensitivity = .8f;
            cRacketSwing->desiredHandDelta = { steer.x * sensitivity * dt, -steer.y * sensitivity * dt, 0.0f };
            if (steer.length() != 0) {
                cPoseIntentBuffer->intents.push_back(PoseIntent{ PoseJointID::LeftWrist, PoseIntentPhase::Translate, 0, PoseIntentType::Translate, cRacketSwing->desiredHandDelta });
            }
        }
    }
    return { SystemExecResult::Ran };
}
