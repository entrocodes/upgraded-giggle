#include "PoseCommitSystem.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"

SystemExec PoseCommitSystem::update(GameContext* context) {
    for (auto [e, cPose] : context->registry.getEntitiesWithComponents<CPose>())
    {
        Pose& pose = cPose->pose;
        context->poseRuntime.lastLeftAnkleErr = context->poseRuntime.ankleErr;

        pose.forEachJoint([&](PoseJoint& j, PoseJointID id) {
            j.lastPos_m = j.pos_m;
            j.restRotation_rad += j.deltaRotation_rad;
            j.restOffset_m += j.deltaOffset_m;
            });

        auto [cRacketHandle] = context->registry.getComponents<CRacketHandle>(e);
        auto eRacket = cRacketHandle->racketEntity;
        auto [cRacketTransform3D, cRacketPhysical, cRacketVelocity3D] =
            context->registry.getComponents<CTransform3D, CRacketPhysical, CVelocity3D>(eRacket);

        Vec3 lastPos = cRacketTransform3D->pos_m;
        Vec3 newPos = cPose->pose.racket().pos_m;

        cRacketTransform3D->lastPos_m = lastPos;
        cRacketTransform3D->pos_m = newPos;

        Vec3 instantVel = (newPos - lastPos) / context->frameStats.dt;
        cRacketVelocity3D->vel_mps = instantVel * 0.5f + cRacketVelocity3D->vel_mps * 0.5f;
    }
    return { SystemExecResult::Ran };
}