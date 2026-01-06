// PoseForwardKinematicsSystem.cpp
#include "PoseForwardKinematicsSystem.hpp"
#include "components/Components.hpp"
#include "math/MathHelpers.hpp"
#include <cmath>

static inline Vec3 compMul(const Vec3& a, const Vec3& b) { return { a.x * b.x, a.y * b.y, a.z * b.z }; }

static constexpr PoseBoneID kSolveDownOrder[] = {
    PoseBoneID::Spine,
    PoseBoneID::LeftShoulder, PoseBoneID::LeftUpperArm, PoseBoneID::LeftLowerArm, PoseBoneID::RacketHand,
    PoseBoneID::RightShoulder, PoseBoneID::RightUpperArm, PoseBoneID::RightLowerArm,
    PoseBoneID::LeftPelvisBone, PoseBoneID::LeftUpperLeg, PoseBoneID::LeftLowerLeg,
    PoseBoneID::RightPelvisBone, PoseBoneID::RightUpperLeg, PoseBoneID::RightLowerLeg
};

SystemExec PoseForwardKinematicsSystem::update(GameContext* context) {
    for (auto [eBody, cTransform3D, cPose] : context->registry.getEntitiesWithComponents<CTransform3D, CPose>()) {
        Pose& pose = cPose->pose;

        // Rotation commit (persistent driver update) lives here OR in a separate system.
        // If you prefer, move this into its own PoseRotationCommitSystem.
        pose.forEachJoint([&](PoseJoint& j, PoseJointID) {
            Vec3 desired = j.restRotation_rad + j.deltaRotation_rad;
            desired.x = std::clamp(desired.x, j.minRot.x, j.maxRot.x);
            desired.y = std::clamp(desired.y, j.minRot.y, j.maxRot.y);
            desired.z = std::clamp(desired.z, j.minRot.z, j.maxRot.z);
            if (!j.disablerotationCalc) j.restRotation_rad = desired;
            j.deltaRotation_rad = { 0,0,0 };
            });

        // FK
        for (PoseBoneID id : kSolveDownOrder) {
            PoseBone& b = pose.bone(id);
            PoseJoint& parent = pose.joint(b.joint1);
            PoseJoint& child = pose.joint(b.joint2);

            Vec3 rotatedBaseLocal = MathHelpers::rotateByEuler(child.baseOffset_m, child.restRotation_rad);
            Vec3 finalLocal = rotatedBaseLocal + child.restOffset_m;

            child.pos_m = parent.pos_m + compMul(finalLocal, pose.scale);
            child.offset_m = child.baseOffset_m + child.restOffset_m;
        }
    }

    return { SystemExecResult::Ran };
}
