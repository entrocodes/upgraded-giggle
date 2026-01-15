#include "PoseCommitSystem.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"

SystemExec PoseCommitSystem::update(GameContext* context) {

    for (auto [e, cPose] : context->registry.getEntitiesWithComponents<CPose>())
    {
        Pose& pose = cPose->pose;
        context->poseRuntime.lastLeftAnkleErr = context->poseRuntime.ankleErr;
        bool anyFootLocked = pose.leftAnkle().locked || pose.rightAnkle().locked;

        pose.forEachJoint([&](PoseJoint& j, PoseJointID id) {
            j.lastPos_m = j.pos_m;
            j.restRotation_rad += j.deltaRotation_rad;
            j.restOffset_m += j.deltaOffset_m;
            });

    }
    return { SystemExecResult::Ran };
}
