// PoseForwardKinematicsSystem.cpp
#include "PoseForwardKinematicsSystem.hpp"
#include "components/Components.hpp"
#include "math/MathHelpers.hpp"
#include "debug/Debug.hpp"

// Parent -> child order
static constexpr PoseBoneID kSolveDownOrder[] = {
    PoseBoneID::Spine,
    PoseBoneID::LeftShoulder, PoseBoneID::LeftUpperArm, PoseBoneID::LeftLowerArm, PoseBoneID::RacketHand,
    PoseBoneID::RightShoulder, PoseBoneID::RightUpperArm, PoseBoneID::RightLowerArm,
    PoseBoneID::LeftHipBone, PoseBoneID::LeftUpperLeg, PoseBoneID::LeftLowerLeg,
    PoseBoneID::RightHipBone, PoseBoneID::RightUpperLeg, PoseBoneID::RightLowerLeg
};

SystemExec PoseForwardKinematicsSystem::update(GameContext* context) {
    for (auto [eBody, cTransform3D, cPose] :
        context->registry.getEntitiesWithComponents<CTransform3D, CPose>()) {

        Pose& pose = cPose->pose;

        for (PoseBoneID id : kSolveDownOrder) {


            PoseBone& b = pose.bone(id);
            PoseJoint& parent = pose.joint(b.joint1);
            PoseJoint& child = pose.joint(b.joint2);


            Vec3 rot = child.restRotation_rad + child.deltaRotation_rad;
            Vec3 rotatedBaseLocal = MathHelpers::rotateByEuler(child.baseOffset_m, rot);

            Vec3 finalLocal = rotatedBaseLocal + (child.restOffset_m + child.deltaOffset_m);

            Vec3 newPos = parent.pos_m + MathHelpers::compMul(finalLocal, pose.scale);

            child.pos_m = newPos;
        }
        if (pose.leftAnkle().locked) {
            pose.leftAnkle().pos_m = pose.leftAnkle().lockedWorldPos_m;
        }
        if (pose.rightAnkle().locked) {
            pose.rightAnkle().pos_m = pose.rightAnkle().lockedWorldPos_m;
        }

        context->poseRuntime.ankleErr = (pose.leftAnkle().pos_m - pose.leftAnkle().lockedWorldPos_m).length();
    }

    return { SystemExecResult::Ran };
}
