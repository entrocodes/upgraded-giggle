// PoseConstraintSolveSystem.cpp
#include "PoseConstraintSolveSystem.hpp"
#include "components/Components.hpp"
#include "math/MathHelpers.hpp"
#include <algorithm>
#include <cmath>

// This is a *first-pass* constraint solver that keeps locked joints in place by
// pushing corrections into restOffset_m up the chain (no fancy IK yet).
//
// It fixes your immediate issue: pelvis Y offset won't drag feet down if ankles are locked.
// Then later you can upgrade this file to a proper 2-bone IK that adjusts rotations.

static inline Vec3 compMul(const Vec3& a, const Vec3& b) { return { a.x * b.x, a.y * b.y, a.z * b.z }; }

static inline float safeLen(const Vec3& v) { return std::sqrt(v.lengthSq()); }

static inline Vec3 safeNormalize(const Vec3& v) {
    float len = safeLen(v);
    if (len < 1e-6f) return { 0,0,0 };
    return v / len;
}

static inline Vec3 clampVecLen(const Vec3& v, float maxLen) {
    float lsq = v.lengthSq();
    if (lsq <= maxLen * maxLen) return v;
    return safeNormalize(v) * maxLen;
}

// FK order used for a refresh pass after constraint edits
static constexpr PoseBoneID kSolveDownOrder[] = {
    PoseBoneID::Spine,
    PoseBoneID::LeftShoulder, PoseBoneID::LeftUpperArm, PoseBoneID::LeftLowerArm, PoseBoneID::RacketHand,
    PoseBoneID::RightShoulder, PoseBoneID::RightUpperArm, PoseBoneID::RightLowerArm,
    PoseBoneID::LeftPelvisBone, PoseBoneID::LeftUpperLeg, PoseBoneID::LeftLowerLeg,
    PoseBoneID::RightPelvisBone, PoseBoneID::RightUpperLeg, PoseBoneID::RightLowerLeg
};

static void runFK(Pose& pose) {
    for (PoseBoneID id : kSolveDownOrder) {
        PoseBone& b = pose.bone(id);
        PoseJoint& parent = pose.joint(b.joint1);
        PoseJoint& child = pose.joint(b.joint2);

        Vec3 rotatedBaseLocal = MathHelpers::rotateByEuler(child.baseOffset_m, child.restRotation_rad);
        Vec3 finalLocal = rotatedBaseLocal + child.restOffset_m;
        child.pos_m = parent.pos_m + compMul(finalLocal, pose.scale);
    }
}

static void solveAnkleLock_LegChain(Pose& pose,
    PoseJointID ankleId, PoseJointID kneeId, PoseJointID hipId, PoseJointID rootPelvisId,
    float lockWeight)
{
    PoseJoint& ankle = pose.joint(ankleId);
    PoseJoint& knee = pose.joint(kneeId);
    PoseJoint& hip = pose.joint(hipId);
    PoseJoint& root = pose.joint(rootPelvisId);

    if (!ankle.lockPosition) return;

    Vec3 targetW = ankle.lockedWorldPos_m;
    Vec3 errorW = targetW - ankle.pos_m;
    if (errorW.lengthSq() < 1e-8f) return;

    // Distribute error up the chain. This creates squat/lunge-like adaptation:
    // - knee soaks some
    // - hip soaks some
    // - root pelvis soaks the rest (but only via restOffset driver, not transform)
    // Tune shares; start conservative.
    float w = std::max(0.f, std::min(1.f, lockWeight));
    float kneeShare = 0.45f * w;
    float hipShare = 0.35f * w;
    float rootShare = 0.20f * w;

    // Convert world correction to LOCAL/pre-scale-ish by dividing by scale (component-wise)
    // (This is approximate with rotation, but good enough for the first pass.)
    Vec3 invScale = { 1.f / pose.scale.x, 1.f / pose.scale.y, 1.f / pose.scale.z };
    Vec3 errorLocal = { errorW.x * invScale.x, errorW.y * invScale.y, errorW.z * invScale.z };

    // Push into restOffsets. We clamp to each joint's maxOffset so it won't explode.
    knee.restOffset_m = clampVecLen(knee.restOffset_m + errorLocal * kneeShare, knee.maxOffset);
    hip.restOffset_m = clampVecLen(hip.restOffset_m + errorLocal * hipShare, hip.maxOffset);
    root.restOffset_m = clampVecLen(root.restOffset_m + errorLocal * rootShare, root.maxOffset);
}

SystemExec PoseConstraintSolveSystem::update(GameContext* context) {
    for (auto [eBody, cTransform3D, cPose] : context->registry.getEntitiesWithComponents<CTransform3D, CPose>()) {
        Pose& pose = cPose->pose;

        solveAnkleLock_LegChain(pose, PoseJointID::LeftAnkle, PoseJointID::LeftKnee, PoseJointID::LeftPelvis, PoseJointID::CenterPelvis, pose.leftAnkle().lockWeight);
        solveAnkleLock_LegChain(pose, PoseJointID::RightAnkle, PoseJointID::RightKnee, PoseJointID::RightPelvis, PoseJointID::CenterPelvis, pose.rightAnkle().lockWeight);

        // Re-run FK once to reflect constraint edits
        runFK(pose);
    }

    return { SystemExecResult::Ran };
}
