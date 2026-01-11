// PoseAnkleLockIKSystem.cpp
#include "PoseAnkleLockIKSystem.hpp"
#include "components/Components.hpp"
#include "math/MathHelpers.hpp"
#include <algorithm>
#include <cmath>




static inline float clampf(float v, float a, float b) { return std::max(a, std::min(b, v)); }
static inline Vec3 compMul(const Vec3& a, const Vec3& b) { return { a.x * b.x, a.y * b.y, a.z * b.z }; }

static inline float safeLen2(float y, float z) { return std::sqrt(y * y + z * z); }

static void refreshPelvisWorldPos(Pose& pose, CTransform3D* cTransform3D) {
    PoseJoint& pelvis = pose.centerPelvis();
    pelvis.offset_m = pelvis.baseOffset_m + pelvis.restOffset_m;
    pelvis.pos_m = cTransform3D->pos_m + compMul(pelvis.offset_m, pose.scale);
}

static PoseJointID hipFromAnkle(PoseJointID ankle) {
    return (ankle == PoseJointID::LeftAnkle) ? PoseJointID::LeftPelvis : PoseJointID::RightPelvis;
}
static PoseJointID kneeFromAnkle(PoseJointID ankle) {
    return (ankle == PoseJointID::LeftAnkle) ? PoseJointID::LeftKnee : PoseJointID::RightKnee;
}

static float pitchFromVectorYZ(const Vec3& v) {
    // pitch about X that points bind (0,-1,0) toward v in YZ plane.
    // Using atan2(-z, y) gives "forward (z-) = positive pitch" style.
    return std::atan2(-v.z, v.y);
}

static void solveLeg2BoneIK_YZ(
    Pose& pose,
    PoseJointID hipId,
    PoseJointID kneeId,
    PoseJointID ankleId,
    const Vec3& ankleTargetW,
    float lockWeight
) {
    PoseJoint& hip = pose.joint(hipId);
    PoseJoint& knee = pose.joint(kneeId);
    PoseJoint& ankle = pose.joint(ankleId);

    if (!ankle.locked) return;

    float w = clampf(lockWeight, 0.f, 1.f);
    if (w <= 0.f) return;

    // World-space hip -> target vector
    Vec3 toTargetW = ankleTargetW - hip.pos_m;

    // We solve in the sagittal plane (YZ). Keep X as-is.
    float dy = toTargetW.y;
    float dz = toTargetW.z;
    float d = safeLen2(dy, dz);
    if (d < 1e-6f) return;

    // Segment bind vectors (scaled)
    // hip->knee is knee.baseOffset (because knee is child of hip)
    Vec3 b1W = compMul(knee.baseOffset_m, pose.scale);
    Vec3 b2W = compMul(ankle.baseOffset_m, pose.scale);

    float L1 = safeLen2(b1W.y, b1W.z);
    float L2 = safeLen2(b2W.y, b2W.z);

    if (L1 < 1e-5f || L2 < 1e-5f) return;

    // Clamp reachable distance (classic 2-bone IK)
    float dClamped = clampf(d, std::fabs(L1 - L2) + 1e-4f, (L1 + L2) - 1e-4f);

    // Triangle solve:
    // a = distance from hip to knee along the hip->target ray
    float a = (L1 * L1 - L2 * L2 + dClamped * dClamped) / (2.f * dClamped);
    float h2 = std::max(0.f, L1 * L1 - a * a);
    float h = std::sqrt(h2);

    // Unit direction in YZ
    float uy = dy / d;
    float uz = dz / d;

    // Choose bend direction: for squat, we generally want the knee to go "forward"
    // If forward is -Z in your space, then we want the perpendicular to bias toward -Z.
    // We’ll pick a consistent sign based on target direction (works well in practice).
    float bendSign = (dz > 0.f) ? -1.f : +1.f;

    // Knee position in YZ plane
    float kneeY = hip.pos_m.y + uy * a + (-uz) * h * bendSign;
    float kneeZ = hip.pos_m.z + uz * a + (uy)*h * bendSign;

    Vec3 kneeSolvedW = { knee.pos_m.x, kneeY, kneeZ };

    // Now compute desired segment directions in YZ
    Vec3 v1W = kneeSolvedW - hip.pos_m;
    Vec3 v2W = ankleTargetW - kneeSolvedW;

    // Compute desired pitches (about X) relative to bind
    float bindPitch1 = pitchFromVectorYZ(b1W);
    float bindPitch2 = pitchFromVectorYZ(b2W);

    float desiredPitch1 = pitchFromVectorYZ(v1W);
    float desiredPitch2 = pitchFromVectorYZ(v2W);

    float deltaPitch1 = (desiredPitch1 - bindPitch1) * w;
    float deltaPitch2 = (desiredPitch2 - bindPitch2) * w;

    // Apply to joint rotations (pitch about X)
    knee.restRotation_rad.x += deltaPitch1;
    ankle.restRotation_rad.x += deltaPitch2;

    // Clamp to joint rotation limits (important!)
    knee.restRotation_rad.x = clampf(knee.restRotation_rad.x, knee.minRot.x, knee.maxRot.x);
    ankle.restRotation_rad.x = clampf(ankle.restRotation_rad.x, ankle.minRot.x, ankle.maxRot.x);

    // after FK has run once this frame, ankle.pos_m is current.
    // If locked, enforce the position by pushing error into the chain.
    Vec3 errorW = ankle.lockedWorldPos_m - ankle.pos_m;
    if (errorW.lengthSq() > 1e-8f) {
        float w = clampf(ankle.lockWeight, 0.f, 1.f);

        Vec3 invScale = { 1.f / pose.scale.x, 1.f / pose.scale.y, 1.f / pose.scale.z };
        Vec3 errorLocal = { errorW.x * invScale.x, errorW.y * invScale.y, errorW.z * invScale.z };

        // push into offsets (driver space)
        knee.restOffset_m += errorLocal * (0.45f * w);
        hip.restOffset_m += errorLocal * (0.35f * w);

        // IMPORTANT: if you touch centerPelvis.restOffset_m, you must refresh pelvis world pos
        pose.centerPelvis().restOffset_m += errorLocal * (0.20f * w);
    }

}

SystemExec PoseAnkleLockIKSystem::update(GameContext* context) {
    for (auto [eBody, cTransform3D, cPose] :
        context->registry.getEntitiesWithComponents<CTransform3D, CPose>()) {

        Pose& pose = cPose->pose;

        // Solve both legs if ankles are locked
        solveLeg2BoneIK_YZ(
            pose,
            PoseJointID::LeftPelvis,
            PoseJointID::LeftKnee,
            PoseJointID::LeftAnkle,
            pose.leftAnkle().lockedWorldPos_m,
            pose.leftAnkle().lockWeight
        );

        solveLeg2BoneIK_YZ(
            pose,
            PoseJointID::RightPelvis,
            PoseJointID::RightKnee,
            PoseJointID::RightAnkle,
            pose.rightAnkle().lockedWorldPos_m,
            pose.rightAnkle().lockWeight
        );

        refreshPelvisWorldPos(pose, cTransform3D);
    }

    return { SystemExecResult::Ran };
}
