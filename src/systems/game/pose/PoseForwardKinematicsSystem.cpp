// PoseForwardKinematicsSystem.cpp
#include "PoseForwardKinematicsSystem.hpp"
#include "components/Components.hpp"
#include "math/MathHelpers.hpp"
#include "debug/Debug.hpp"
static inline Vec3 compMul(const Vec3& a, const Vec3& b) { return { a.x * b.x, a.y * b.y, a.z * b.z }; }

// Parent -> child order
static constexpr PoseBoneID kSolveDownOrder[] = {
    PoseBoneID::Spine,
    PoseBoneID::LeftShoulder, PoseBoneID::LeftUpperArm, PoseBoneID::LeftLowerArm, PoseBoneID::RacketHand,
    PoseBoneID::RightShoulder, PoseBoneID::RightUpperArm, PoseBoneID::RightLowerArm,
    PoseBoneID::LeftPelvisBone, PoseBoneID::LeftUpperLeg, PoseBoneID::LeftLowerLeg,
    PoseBoneID::RightPelvisBone, PoseBoneID::RightUpperLeg, PoseBoneID::RightLowerLeg
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

            Vec3 newPos = parent.pos_m + compMul(finalLocal, pose.scale);
            //if (child.locked) {
            //    child.pos_m = Vec3(newPos.x, child.lockedWorldPos_m.y, newPos.z);
            //}
            //else {
            //    child.pos_m = newPos;
            //}
            child.pos_m = newPos;
        }
    }

    return { SystemExecResult::Ran };
}
