// PoseForwardKinematicsSystem.cpp
#include "PoseForwardKinematicsSystem.hpp"
#include "components/Components.hpp"
#include "math/MathHelpers.hpp"

static constexpr PoseBoneID kSolveDownOrder[] = {
    PoseBoneID::Spine,
    PoseBoneID::LeftShoulder, PoseBoneID::LeftUpperArm,
    PoseBoneID::LeftLowerArm, PoseBoneID::RacketHand,
    PoseBoneID::RightShoulder, PoseBoneID::RightUpperArm,
    PoseBoneID::RightLowerArm,
};

SystemExec PoseForwardKinematicsSystem::update(GameContext* context) {

    for (auto [e, cPose] : context->registry.getEntitiesWithComponents<CPose>()) {

        Pose& pose = cPose->pose;

        // --------------------------------------------------
        // FK
        // --------------------------------------------------
        pose.centerPelvis().rotWorld_rad =
            pose.centerPelvis().restRotation_rad +
            pose.centerPelvis().deltaRotation_rad;

        for (PoseBoneID id : kSolveDownOrder) {
            PoseBone& b = pose.bone(id);
            PoseJoint& parent = pose.joint(b.joint1);
            PoseJoint& child = pose.joint(b.joint2);

            Vec3 localRot = child.restRotation_rad + child.deltaRotation_rad;
            child.rotWorld_rad = parent.rotWorld_rad + localRot;

            Vec3 localOffset = child.baseOffset_m;
            if (b.joint1 == PoseJointID::CenterPelvis)
                localOffset += child.deltaOffset_m;

            Vec3 rotated =
                MathHelpers::rotateByEuler(localOffset, parent.rotWorld_rad);

            child.pos_m =
                parent.pos_m + MathHelpers::compMul(rotated, pose.scale);
        }

        //// --------------------------------------------------
        //// Rollback validation (LEFT WRIST)
        //// --------------------------------------------------
        //PoseJoint& wr = pose.leftWrist();

        //if (wr.ikTargetActive) {

        //    const Vec3 elbowWorldPos = pose.leftElbow().pos_m;
        //    const Vec3 wristBindOffsetWorld =
        //        MathHelpers::compMul(wr.baseOffset_m + wr.restOffset_m, pose.scale);

        //    const Vec3 wristBindWorldPos =
        //        elbowWorldPos + wristBindOffsetWorld;

        //    const Vec3 lastTargetWorld =
        //        wristBindWorldPos +
        //        MathHelpers::compMul(wr.lastTargetOffsetFromBind, pose.scale);

        //    const Vec3 currTargetWorld =
        //        wristBindWorldPos +
        //        MathHelpers::compMul(wr.targetOffsetFromBind, pose.scale);

        //    const float lastDist =
        //        (lastTargetWorld - wr.lastWristWorldPos).length();

        //    const float currDist =
        //        (currTargetWorld - wr.pos_m).length();

        //    const float progressEpsilon = 1e-4f;

        //    const bool madeProgress =
        //        currDist < (lastDist - progressEpsilon);

        //    if (!madeProgress) {
        //        wr.targetOffsetFromBind = wr.lastTargetOffsetFromBind;
        //    }
        //}

        //// --------------------------------------------------
        //// Snapshot for next frame
        //// --------------------------------------------------
        //wr.lastWristWorldPos = wr.pos_m;
    }

    return { SystemExecResult::Ran };
}
