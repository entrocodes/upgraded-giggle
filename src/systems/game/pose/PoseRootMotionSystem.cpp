// PoseRootMotionSystem.cpp
#include "PoseRootMotionSystem.hpp"
#include "components/Components.hpp"
#include "math/Vec2.hpp"
#include "math/Constants.hpp"
#include "debug/Debug.hpp"
#include <algorithm>
#include <cmath>

static inline float safeLenSq(const Vec2& v) { return v.x * v.x + v.y * v.y; }
static inline float safeLen(const Vec2& v) { return std::sqrt(safeLenSq(v)); }

static inline Vec2 safeNormalize(const Vec2& v) {
    float len = safeLen(v);
    if (len < 1e-6f) return { 0,0 };
    return { v.x / len, v.y / len };
}

static inline Vec3 compMul(const Vec3& a, const Vec3& b) { return { a.x * b.x, a.y * b.y, a.z * b.z }; }

SystemExec PoseRootMotionSystem::update(GameContext* context) {
    for (auto [eBody, cTransform3D, cPose] : context->registry.getEntitiesWithComponents<CTransform3D, CPose>()) {
        Pose& pose = cPose->pose;
        PoseJoint& pelvis = pose.centerPelvis();

        // Combine desired root offsets
        Vec2 desiredXZ(pelvis.restOffset_m.x + pelvis.deltaOffset_m.x,
            pelvis.restOffset_m.z + pelvis.deltaOffset_m.z);
        float desiredY = pelvis.restOffset_m.y + pelvis.deltaOffset_m.y;

        Vec2 overflowXZ(0, 0);

        if (!context->playerMovement.bodyMovement.disablePoseConstraints) {
            float maxXZ = pelvis.maxOffset;
            float lenSq = safeLenSq(desiredXZ);
            if (lenSq > maxXZ * maxXZ) {
                Vec2 clampedXZ = safeNormalize(desiredXZ) * maxXZ;
                overflowXZ = desiredXZ - clampedXZ;
                desiredXZ = clampedXZ;
            }
        }

        // Commit pelvis restOffset (Y never locomotes into transform; but Y is allowed in pose driver)
        pelvis.restOffset_m = { desiredXZ.x, desiredY, desiredXZ.y };
        pelvis.deltaOffset_m = { 0,0,0 };

        // Apply locomotion ONLY in XZ
        cTransform3D->pos_m += Vec3(overflowXZ.x, 0.f, overflowXZ.y);

        // Final pelvis world position
        pelvis.offset_m = pelvis.baseOffset_m + pelvis.restOffset_m;
        pelvis.pos_m = cTransform3D->pos_m + compMul(pelvis.offset_m, pose.scale);
    }

    return { SystemExecResult::Ran };
}
