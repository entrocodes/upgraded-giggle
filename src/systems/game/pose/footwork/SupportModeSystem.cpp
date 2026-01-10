#include "SupportModeSystem.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"

SystemExec SupportModeSystem::update(GameContext* context) {
    for (auto [eCharacter, cPose, cFootworkState] : context->registry.getEntitiesWithComponents<CPose, CFootworkState>()) {
        auto& pose = cPose->pose;
        if (cFootworkState->kind == StepKind::Reach) {
            pose.forEachAnkle([&](PoseJoint& ankle, PoseJointID) {
                ankle.supportMode = SupportMode::Grounded;
                ankle.lockWeight = 1.0f;
                ankle.lockedWorldPos_m = ankle.pos_m;
                });
        }
    }
    return { SystemExecResult::Ran };
}
