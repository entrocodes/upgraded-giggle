// PoseForwardKinematicsSystem.cpp
#include "PoseForwardKinematicsSystem.hpp"
#include "components/Components.hpp"
#include "math/MathHelpers.hpp"
#include "debug/Debug.hpp"
#include <algorithm>
#include <cmath>

// Bones solved before arm IK
static constexpr PoseBoneID kPreIKOrder[] = {
    PoseBoneID::Spine,
    PoseBoneID::LeftShoulder,
    PoseBoneID::RightShoulder, PoseBoneID::RightUpperArm,
    PoseBoneID::RightLowerArm, PoseBoneID::RightHipBone,
    PoseBoneID::LeftHipBone,   PoseBoneID::RightUpperLeg,
    PoseBoneID::LeftUpperLeg,  PoseBoneID::RightLowerLeg,
    PoseBoneID::LeftLowerLeg
};

// Bones solved after arm IK
static constexpr PoseBoneID kPostIKOrder[] = {
    PoseBoneID::LeftUpperArm,
    PoseBoneID::LeftLowerArm,
    PoseBoneID::RacketHand
};

static float boneWorldLen(Pose& pose, PoseBoneID id) {
    PoseBone& b = pose.bone(id);
    PoseJoint& child = pose.joint(b.joint2);
    return MathHelpers::compMul(child.baseOffset_m, pose.scale).length();
}

static Vec3 reject(const Vec3& v, const Vec3& dirUnit) {
    return v - dirUnit * v.dot(dirUnit);
}

static Vec3 safeNormalized(const Vec3& v, const Vec3& fallback) {
    float len = v.length();
    if (len < 1e-6f) return fallback;
    return v / len;
}

static void solveBone(Pose& pose, PoseBoneID id) {
    PoseBone& b = pose.bone(id);
    PoseJoint& parent = pose.joint(b.joint1);
    PoseJoint& child = pose.joint(b.joint2);

    Vec3 localRot = child.restRotation_rad + child.deltaRotation_rad;
    child.rotWorld_rad = parent.rotWorld_rad + localRot;
    Vec3 localOffset = child.baseOffset_m;
    if (b.joint1 == PoseJointID::CenterPelvis) localOffset += child.deltaOffset_m;
    Vec3 rotated = MathHelpers::rotateByEuler(localOffset, parent.rotWorld_rad);
    child.pos_m = parent.pos_m + MathHelpers::compMul(rotated, pose.scale);
}

static void solveArmIK(Pose& pose, GameContext* context) {
    if (pose.debugDisableIK) return;
    if (!pose.leftWrist().ikTargetActive) return;

    PoseJoint& sh = pose.leftShoulder();
    PoseJoint& el = pose.leftElbow();
    PoseJoint& wr = pose.leftWrist();

    // Bind positions for debug logging
    Vec3 elbowBindLocal = MathHelpers::rotateByEuler(el.baseOffset_m, sh.rotWorld_rad);
    Vec3 elbowBindWorld = sh.pos_m + MathHelpers::compMul(elbowBindLocal, pose.scale);
    Vec3 wristBindLocal = MathHelpers::rotateByEuler(wr.baseOffset_m, sh.rotWorld_rad);
    Vec3 wristBindWorldPos = elbowBindWorld + MathHelpers::compMul(wristBindLocal, pose.scale);

    // Stable world-space target — anchored to pelvis, not to arm bind positions,
    // so it doesn't shift when arm rest rotations are zeroed each frame.
    const Vec3 ikTargetWorld = wr.ikTargetWorldPos;

    Debug::event(Debug::Channel::IK, "ArmIK_Positions", {}, {
        {"sh.pos_m",             sh.pos_m},
        {"elbowBindWorld",       elbowBindWorld},
        {"wristBindWorldPos",    wristBindWorldPos},
        {"ikTargetWorld",        ikTargetWorld},
        {"targetOffsetFromBind", wr.targetOffsetFromBind}
        });

    const float L1 = boneWorldLen(pose, PoseBoneID::LeftUpperArm);
    const float L2 = boneWorldLen(pose, PoseBoneID::LeftLowerArm);
    Vec3 toTarget = ikTargetWorld - sh.pos_m;
    float dist = toTarget.length();

    Debug::event(Debug::Channel::IK, "ArmIK_Geometry", {
        {"L1",           L1},
        {"L2",           L2},
        {"dist",         dist},
        {"maxReach",     L1 + L2},
        {"straightness", (L1 + L2) - dist}
        });
   
    if (dist < 1e-6f) return;
    const Vec3 dir = toTarget.normalized();

    // bendHint in pelvis-local space so elbow tracks body rotation
    const Vec3 pelvisRot = pose.centerPelvis().rotWorld_rad;
    Vec3 bendHintLocal = (pose.armState == ArmState::SwingStroke)
        ? Vec3(-1.f, 0.f, 0.3f)
        : Vec3(-1.f, 0.f, 0.f);
    Vec3 bendHintWorld = MathHelpers::rotateByEuler(bendHintLocal, pelvisRot);
    Vec3 bendFallback = MathHelpers::rotateByEuler(Vec3(-1.f, 0.f, 0.f), pelvisRot);
    Vec3 bendDir = safeNormalized(reject(bendHintWorld, dir), bendFallback);

    float cosA = std::clamp(
        (L1 * L1 + dist * dist - L2 * L2) / (2.f * L1 * dist),
        -1.f, 1.f);
    float a = std::acos(cosA);

    const Vec3 elbowPos =
        sh.pos_m
        + dir * (std::cos(a) * L1)
        + bendDir * (std::sin(a) * L1);

    Debug::event(Debug::Channel::IK, "ArmIK_Solve", {}, {
        {"dir",      dir},
        {"bendDir",  bendDir},
        {"elbowPos", elbowPos}
        });

    const Vec3 upperNowLocal = MathHelpers::rotateByEuler(elbowPos - sh.pos_m, -sh.rotWorld_rad);
    const Vec3 shoulderErr = MathHelpers::rotationFromToEuler(el.baseOffset_m, upperNowLocal);

    const Vec3 lowerDir = (ikTargetWorld - elbowPos).normalized();
    const Vec3 lowerNowLocal = MathHelpers::rotateByEuler(lowerDir, -sh.rotWorld_rad - shoulderErr);
    const Vec3 elbowErr = MathHelpers::rotationFromToEuler(wr.baseOffset_m.normalized(), lowerNowLocal);

    Debug::queueSphere3D(ikTargetWorld, .04, sf::Color::Red);
    Debug::event(Debug::Channel::IK, "ArmIK_Errors", {}, {
        {"shoulderErr", shoulderErr},
        {"elbowErr",    elbowErr}
        });

    sh.deltaRotation_rad = shoulderErr;
    el.deltaRotation_rad = elbowErr - el.restRotation_rad;

    //if (pose.armState == ArmState::SwingStroke) {
    //    Debug::setChannelEnabled(Debug::Channel::IK, true);
    //}
    //else {
    //    Debug::setChannelEnabled(Debug::Channel::IK, false);
    //}
}

SystemExec PoseForwardKinematicsSystem::update(GameContext* context) {
    for (auto [e, cPose] : context->registry.getEntitiesWithComponents<CPose>()) {
        Pose& pose = cPose->pose;

        // Zero arm joint rest rotations before every FK pass.
        // PoseCommitSystem accumulates deltaRotation into restRotation each frame,
        // but for arm joints the IK recomputes a full fresh delta from the stable
        // world-space ikTargetWorldPos. Zeroing here means the IK solve is always
        // relative to bind, so it can reach anywhere without feedback drift.
        if (context->physicsDebug.ikJointsNoAccumulate) {
            pose.leftElbow().restRotation_rad = { 0, 0, 0 };
        }

        // Root
        pose.centerPelvis().rotWorld_rad =
            pose.centerPelvis().restRotation_rad +
            pose.centerPelvis().deltaRotation_rad;

        // Pre-IK FK — spine, shoulders, legs, right arm
        for (PoseBoneID id : kPreIKOrder)
            solveBone(pose, id);

        // Arm IK — sh.pos_m and sh.rotWorld_rad are now valid
        solveArmIK(pose, context);

        // Post-IK FK — left arm using IK-computed deltas
        for (PoseBoneID id : kPostIKOrder)
            solveBone(pose, id);
    }

    return { SystemExecResult::Ran };
}