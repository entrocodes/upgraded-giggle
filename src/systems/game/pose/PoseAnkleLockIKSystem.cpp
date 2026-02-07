// PoseAnkleLockIKSystem.cpp
#include "PoseAnkleLockIKSystem.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"
#include "math/MathHelpers.hpp"
#include <algorithm>
#include <cmath>
static void solvePelvisZ(Pose& pose) {
    constexpr float squatRatioZ = 0.4f;
    PoseJoint& pelvis = pose.centerPelvis();

    pelvis.deltaOffset_m.y = pelvis.targetOffsetFromBind.y;
    pelvis.deltaOffset_m.z = pelvis.deltaOffset_m.y * squatRatioZ;

    Debug::debugPrint("pelvisY", pelvis.deltaOffset_m.y);
}
static void solveLeg(Pose& pose, PoseJointID ankleId) {

    const bool left = (ankleId == PoseJointID::LeftAnkle);

    PoseJoint& hip = left ? pose.leftHip() : pose.rightHip();
    PoseJoint& knee = left ? pose.leftKnee() : pose.rightKnee();
    PoseJoint& ankle = left ? pose.leftAnkle() : pose.rightAnkle();

    PoseBone& upperLeg = left ? pose.bone(PoseBoneID::LeftUpperLeg)
        : pose.bone(PoseBoneID::RightUpperLeg);
    PoseBone& lowerLeg = left ? pose.bone(PoseBoneID::LeftLowerLeg)
        : pose.bone(PoseBoneID::RightLowerLeg);

    auto worldLen = [&](PoseBone& b) {
        PoseJoint& child = pose.joint(b.joint2);
        return MathHelpers::compMul(child.baseOffset_m, pose.scale).length();
        };

    float L1 = worldLen(upperLeg);
    float L2 = worldLen(lowerLeg);

    Vec3 hipPos = hip.pos_m;
    Vec3 anklePos = ankle.pos_m;

    Debug::queueSphere3D(hipPos, 0.02f, sf::Color::Yellow);
    Debug::queueSphere3D(anklePos, 0.02f, sf::Color::Cyan);

    // Solve in YZ plane (your existing convention)
    Vec3 v = anklePos - hipPos;
    float Dy = v.y;
    float Dz = v.z;

    float d = std::sqrt(Dy * Dy + Dz * Dz);
    if (d < 1e-6f) return;

    d = std::clamp(d, std::fabs(L1 - L2) + 1e-5f, (L1 + L2) - 1e-5f);

    float phi = std::atan2(-Dz, -Dy);

    float cosA = (L1 * L1 + d * d - L2 * L2) / (2.f * L1 * d);
    cosA = std::clamp(cosA, -1.f, 1.f);
    float alpha = std::acos(cosA);

    float cosB = (L1 * L1 + L2 * L2 - d * d) / (2.f * L1 * L2);
    cosB = std::clamp(cosB, -1.f, 1.f);
    float beta = std::acos(cosB);

    // Primary solution (knee forward)
    float hipPitch = phi - alpha;
    float kneePitch = M_PI - beta;

    // Apply rotations (parent-driven!)
    hip.deltaRotation_rad.x = hipPitch - hip.restRotation_rad.x;
    knee.deltaRotation_rad.x = kneePitch - knee.restRotation_rad.x;

    Debug::event(Debug::Channel::IK, "LegIK",
        { {"d", d}, {"L1", L1}, {"L2", L2}, {"hipPitch", hipPitch}, {"kneePitch", kneePitch} });
}
SystemExec PoseAnkleLockIKSystem::update(GameContext* context) {
    for (auto [e, cTransform, cPose] :
        context->registry.getEntitiesWithComponents<CTransform3D, CPose>()) {

        Pose& pose = cPose->pose;

        if (pose.leftAnkle().locked && pose.rightAnkle().locked) {
            solvePelvisZ(pose);
            solveLeg(pose, PoseJointID::LeftAnkle);
            solveLeg(pose, PoseJointID::RightAnkle);
        }
    }

    return { SystemExecResult::Ran };
}
