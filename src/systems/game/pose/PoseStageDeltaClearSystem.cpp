#include "PoseStageDeltaClearSystem.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"

SystemExec PoseStageDeltaClearSystem::update(GameContext* context) {
    for (auto [eCharacter, cPose] : context->registry.getEntitiesWithComponents<CPose>()) {
        auto& pose = cPose->pose;
        pose.forEachJoint([&](PoseJoint& j, PoseJointID) {
            j.deltaOffset_m = { 0,0,0 };
            j.deltaRotation_rad = { 0,0,0 };
            });

        pose.forEachBone([&](PoseBone& b, PoseBoneID) {
            b.deltaStretch = 0.f;
            });

    }
    return { SystemExecResult::Ran };
}
