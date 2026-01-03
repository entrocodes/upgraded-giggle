#include "PoseSolveSystem.hpp"
#include "components/Components.hpp"
#include <cmath>
SystemExec PoseSolveSystem::update(GameContext* context)
{
    for (auto [eBody, cTransform3D, cPose] :
        context->registry.getEntitiesWithComponents<CTransform3D, CPose>())
    {
        auto pelvis = cPose->pose.centerPelvis();
        pelvis.pos_m = cTransform3D->pos_m + pelvis.offset_m;

        Pose& pose = cPose->pose;
        pose.forEachBone([&](PoseBone& b, PoseBoneID) {
            PoseJoint& j0 = pose.joint(b.joint1);
            PoseJoint& j1 = pose.joint(b.joint2);
            float lenSq = j1.offset_m.lengthSq();
            float maxSq = j1.maxOffset * j1.maxOffset;

            if (lenSq > maxSq) { //clamp based on max
                float len = std::sqrt(lenSq);
                float scale = j1.maxOffset / len;

                Vec3 clamped = j1.offset_m * scale;
                j1.overflow_m = j1.offset_m - clamped;
                j1.offset_m = clamped;
            }
            j1.pos_m = j0.pos_m + (j1.offset_m * pose.scale);

            });

    }

    return { SystemExecResult::Ran };
}
