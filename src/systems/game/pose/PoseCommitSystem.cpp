#include "PoseCommitSystem.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"

SystemExec PoseCommitSystem::update(GameContext* context) {

    for (auto [e, cPose] : context->registry.getEntitiesWithComponents<CPose>())
    {
        Pose& pose = cPose->pose;

        pose.forEachJoint([&](PoseJoint& j, PoseJointID id) {
            j.restOffset_m += j.deltaOffset_m;
            j.restRotation_rad += j.deltaRotation_rad;
            });
    }
    return { SystemExecResult::Ran };
}
