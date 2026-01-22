// PoseForwardKinematicsSystem.cpp
#include "PoseForwardKinematicsSystem.hpp"
#include "components/Components.hpp"
#include "math/MathHelpers.hpp"

static constexpr PoseBoneID kSolveDownOrder[] = {
    PoseBoneID::Spine,
    PoseBoneID::LeftShoulder, PoseBoneID::LeftUpperArm, PoseBoneID::LeftLowerArm, PoseBoneID::RacketHand,
    PoseBoneID::RightShoulder, PoseBoneID::RightUpperArm, PoseBoneID::RightLowerArm,
    PoseBoneID::LeftHipBone, PoseBoneID::LeftUpperLeg, PoseBoneID::LeftLowerLeg,
    PoseBoneID::RightHipBone, PoseBoneID::RightUpperLeg, PoseBoneID::RightLowerLeg
};

SystemExec PoseForwardKinematicsSystem::update(GameContext* context) {
    for (auto [e, cTransform3D, cPose] :
        context->registry.getEntitiesWithComponents<CTransform3D, CPose>()) {

        Pose& pose = cPose->pose;

        pose.centerPelvis().pos_m = cTransform3D->pos_m;
        pose.centerPelvis().rotWorld_rad = pose.centerPelvis().restRotation_rad + pose.centerPelvis().deltaRotation_rad;

        for (PoseBoneID id : kSolveDownOrder) {
            PoseBone& b = pose.bone(id);
            PoseJoint& parent = pose.joint(b.joint1);
            PoseJoint& child = pose.joint(b.joint2);

            Vec3 localRot = child.restRotation_rad + child.deltaRotation_rad;
            child.rotWorld_rad = parent.rotWorld_rad + localRot;

            Vec3 localOffset = child.baseOffset_m;
            if (b.joint1 == PoseJointID::CenterPelvis) {
                localOffset += child.deltaOffset_m;
            }
            Vec3 rotated = MathHelpers::rotateByEuler(localOffset, parent.rotWorld_rad);
            child.pos_m = parent.pos_m + MathHelpers::compMul(rotated, pose.scale);
        }

        if (pose.leftAnkle().locked)  pose.leftAnkle().pos_m = pose.leftAnkle().lockedWorldPos_m;
        if (pose.rightAnkle().locked) pose.rightAnkle().pos_m = pose.rightAnkle().lockedWorldPos_m;
    }

    return { SystemExecResult::Ran };
}
