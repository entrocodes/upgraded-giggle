// PoseArmIKSystem.cpp
#include "PoseArmIKSystem.hpp"
#include "components/Components.hpp"
#include "math/MathHelpers.hpp"
#include "debug/Debug.hpp"
#include <algorithm>
#include <cmath>

static inline float clampf(float v, float a, float b) { return std::max(a, std::min(b, v)); }

static inline float safeSqrt(float v) { return (v <= 0.f) ? 0.f : std::sqrt(v); }

static inline float dot3(const Vec3& a, const Vec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

static inline Vec3 cross3(const Vec3& a, const Vec3& b) { return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }

static inline float len3(const Vec3& v) { return std::sqrt(std::max(0.f, v.x * v.x + v.y * v.y + v.z * v.z)); }

static inline Vec3 norm3(const Vec3& v) { float l = len3(v); return (l > 1e-8f) ? (v / l) : Vec3(0, 0, 0); }

static inline Vec3 compDiv(const Vec3& a, const Vec3& b) { return Vec3((b.x != 0.f) ? a.x / b.x : 0.f, (b.y != 0.f) ? a.y / b.y : 0.f, (b.z != 0.f) ? a.z / b.z : 0.f); }

struct Quat { float w = 1.f, x = 0.f, y = 0.f, z = 0.f; };

static inline Quat quatNorm(const Quat& q) { float l = std::sqrt(std::max(1e-12f, q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z)); return Quat{ q.w / l,q.x / l,q.y / l,q.z / l }; }

static inline Quat quatMul(const Quat& a, const Quat& b) { return Quat{ a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z, a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y, a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x, a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w }; }

static inline Quat quatFromAxisAngle(const Vec3& axisN, float angleRad) { float h = 0.5f * angleRad; float s = std::sin(h); return quatNorm(Quat{ std::cos(h), axisN.x * s, axisN.y * s, axisN.z * s }); }

static inline Quat quatFromTo(const Vec3& from, const Vec3& to) {
    Vec3 f = norm3(from);
    Vec3 t = norm3(to);
    float c = clampf(dot3(f, t), -1.f, 1.f);
    Vec3 axis = cross3(f, t);
    float axisLen = len3(axis);
    if (axisLen < 1e-6f && c > 0.9999f) return Quat{ 1,0,0,0 };
    if (axisLen < 1e-6f && c < -0.9999f) { Vec3 ortho = (std::fabs(f.x) < 0.9f) ? cross3(f, Vec3(1, 0, 0)) : cross3(f, Vec3(0, 1, 0)); return quatFromAxisAngle(norm3(ortho), 3.1415926f); }
    float angle = std::acos(c);
    return quatFromAxisAngle(axis / axisLen, angle);
}

// Euler XYZ from quaternion (matches typical rotateByEuler if that is X then Y then Z; if your helper uses different order, swap here).
static inline Vec3 quatToEulerXYZ(const Quat& qn) {
    Quat q = quatNorm(qn);
    float sinr_cosp = 2.f * (q.w * q.x + q.y * q.z);
    float cosr_cosp = 1.f - 2.f * (q.x * q.x + q.y * q.y);
    float rollX = std::atan2(sinr_cosp, cosr_cosp);
    float sinp = 2.f * (q.w * q.y - q.z * q.x);
    float pitchY = (std::fabs(sinp) >= 1.f) ? std::copysign(1.5707963f, sinp) : std::asin(sinp);
    float siny_cosp = 2.f * (q.w * q.z + q.x * q.y);
    float cosy_cosp = 1.f - 2.f * (q.y * q.y + q.z * q.z);
    float yawZ = std::atan2(siny_cosp, cosy_cosp);
    return Vec3(rollX, pitchY, yawZ);
}

static inline float boneWorldLen(Pose& pose, PoseBoneID id) {
    PoseBone& b = pose.bone(id);
    PoseJoint& child = pose.joint(b.joint2);
    Vec3 bindW = MathHelpers::compMul(child.baseOffset_m, pose.scale);
    return bindW.length();
}

// Computes a deltaRotation that aims the joint's "rest-applied" base vector at desiredLocalVec, and zeros translation (rotation-only solve).
static inline void aimChildBoneByRotationOnly(Pose& pose, PoseJoint& child, const Vec3& desiredWorldVec, const Vec3& parentPosWorld, const Vec3& childPosWorld) {
    Vec3 desiredW = desiredWorldVec;
    if (desiredW.lengthSq() < 1e-10f) return;
    Vec3 desiredLocal = compDiv(desiredW, pose.scale);
    Vec3 baseRest = MathHelpers::rotateByEuler(child.baseOffset_m, child.restRotation_rad);
    Vec3 from = baseRest;
    Vec3 to = desiredLocal;
    if (from.lengthSq() < 1e-10f || to.lengthSq() < 1e-10f) return;
    Quat q = quatFromTo(from, to);
    Vec3 e = quatToEulerXYZ(q);
    child.deltaRotation_rad = e;
    child.deltaOffset_m = Vec3(0, 0, 0);
}

// Persistent-ish target: store it in the wrist joint using fields you already have.
// If you don't want to add new fields, re-use lockedWorldPos_m as a generic IK target storage when not locked.
static inline bool hasWristIKTarget(const PoseJoint& wr) { return wr.ikTargetActive; }

static inline Vec3 getWristIKTarget(const PoseJoint& wr) { return wr.ikTargetWorldPos; }

static inline void setWristIKTarget(PoseJoint& wr, const Vec3& w) { wr.ikTargetWorldPos = w; wr.lockRefPelvisValid = true; }

static inline void clearWristIKTarget(PoseJoint& wr) { 
    wr.ikTargetActive = false;
    wr.ikTargetWorldPos = wr.pos_m;
}

SystemExec PoseArmIKSystem::update(GameContext* context) {
    for (auto [e, cPose] : context->registry.getEntitiesWithComponents<CPose>()) {
        Pose& pose = cPose->pose;
        PoseJoint& sh = pose.leftShoulder();
        PoseJoint& el = pose.leftElbow();
        PoseJoint& wr = pose.leftWrist();

        // 1) Convert wrist deltaOffset into an IK target update, then clear deltaOffset so the arm is rotation-driven.
        if (wr.deltaOffset_m.lengthSq() > 1e-10f) { Vec3 deltaW = MathHelpers::compMul(wr.deltaOffset_m, pose.scale); Vec3 tgt = hasWristIKTarget(wr) ? (getWristIKTarget(wr) + deltaW) : (wr.pos_m + deltaW); setWristIKTarget(wr, tgt); wr.deltaOffset_m = Vec3(0, 0, 0); }

        // 2) If no target, nothing to do.
        if (!hasWristIKTarget(wr)) { 
            continue;
        }

        Vec3 targetW = getWristIKTarget(wr);
        Vec3 shW = sh.pos_m;

        // 3) Bone lengths in WORLD space (based on your bind baseOffset and pose.scale).
        float L1 = boneWorldLen(pose, PoseBoneID::LeftUpperArm);
        float L2 = boneWorldLen(pose, PoseBoneID::LeftLowerArm);

        // 4) Direction + clamped distance.
        Vec3 st = targetW - shW;
        float d = len3(st);
        if (d < 1e-6f) continue;
        float dClamped = clampf(d, std::fabs(L1 - L2) + 1e-4f, (L1 + L2) - 1e-4f);
        Vec3 dir = st / d;

        // 5) Choose bend normal (world). If you have a stored elbow plane normal, use it; otherwise default to +Z or +X depending on your camera.
        Vec3 bendN = Vec3(0, 0, 1);
        if (wr.lockBendValid && wr.lockBendNormalW.lengthSq() > 1e-8f) bendN = norm3(wr.lockBendNormalW);
        bendN = norm3(bendN);
        if (len3(cross3(bendN, dir)) < 1e-5f) bendN = norm3(cross3(dir, Vec3(0, 1, 0)));

        // 6) Analytic 2-bone triangle: compute elbow position.
        float a = (L1 * L1 - L2 * L2 + dClamped * dClamped) / (2.f * dClamped);
        float h = safeSqrt(std::max(0.f, L1 * L1 - a * a));
        Vec3 perp = norm3(cross3(bendN, dir));
        Vec3 elW = shW + dir * a + perp * h;

        // 7) Desired world vectors for each child-driven bone.
        Vec3 shToElW = elW - shW;
        Vec3 elToWrW = targetW - elW;

        // 8) Apply rotations (child joints drive their incoming bones in your FK).
        aimChildBoneByRotationOnly(pose, el, shToElW, shW, el.pos_m);
        aimChildBoneByRotationOnly(pose, wr, elToWrW, el.pos_m, wr.pos_m);

        // 9) Optional: draw + debug.
        Debug::queueLine3D(shW, elW, sf::Color::Cyan);
        Debug::queueLine3D(elW, targetW, sf::Color::Yellow);
        Debug::queueSphere3D(targetW, 0.015f, sf::Color::Red);
        Debug::event(Debug::Channel::IK, "ArmIK2Bone", { {"d", dClamped}, {"L1", L1}, {"L2", L2}, {"a", a}, {"h", h} }, { {"tgt", targetW}, {"elW", elW} }, {});

        // 10) If you want the target to decay when stick released, you can clear it elsewhere; do not clear here unless you want single-frame IK.
    }
    return { SystemExecResult::Ran };
}
