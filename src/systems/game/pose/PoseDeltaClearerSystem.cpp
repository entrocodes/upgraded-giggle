#include "PoseDeltaClearerSystem.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"

SystemExec PoseDeltaClearerSystem::update(GameContext* context) {
    for (auto [eCharacter, cPose] : context->registry.getEntitiesWithComponents<CPose>()) {
        auto& pose = cPose->pose;
        pose.requestedSquat = 0;
        pose.requestedRaise = 0;
        pose.forEachJoint([&](PoseJoint& j, PoseJointID id) {
            j.lastTargetOffset = j.targetOffsetFromBind;
            j.targetOffsetFromBind = { 0,0,0 };
            j.deltaOffset_m = { 0,0,0 };
            j.deltaRotation_rad = { 0,0,0 };
            j.ikTargetActive = false;
            });

        pose.forEachBone([&](PoseBone& b, PoseBoneID) {
            b.deltaStretch = 0.f;
            });

    }
    
    return { SystemExecResult::Ran };
}
