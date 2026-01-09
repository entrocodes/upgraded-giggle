// PoseRootMotionSystem.cpp
#include "PoseRootMotionSystem.hpp"
#include "components/Components.hpp"
#include "math/Vec2.hpp"
#include "debug/Debug.hpp"
#include <algorithm>
#include <cmath>

static inline float lenSq(const Vec2& v) { return v.x * v.x + v.y * v.y; }
static inline float len(const Vec2& v) { return std::sqrt(lenSq(v)); }

static inline Vec2 safeNormalize(const Vec2& v) {
    float l = len(v);
    if (l < 1e-6f) return { 0,0 };
    return { v.x / l, v.y / l };
}

static inline Vec3 compMul(const Vec3& a, const Vec3& b) { return { a.x * b.x, a.y * b.y, a.z * b.z }; }

SystemExec PoseRootMotionSystem::update(GameContext* context) {
    for (auto [eBody, cTransform3D, cPose] :
        context->registry.getEntitiesWithComponents<CTransform3D, CPose>()) {

        Pose& pose = cPose->pose;
        PoseJoint& pelvis = pose.centerPelvis();
        Debug::debugPrint("Pelvis Delta Offset", pelvis.deltaOffset_m);
        // desired root offsets (driver space)
        Vec2 desiredXZ(
            pelvis.restOffset_m.x + pelvis.deltaOffset_m.x,
            pelvis.restOffset_m.z + pelvis.deltaOffset_m.z
        );
        float desiredY = pelvis.restOffset_m.y + pelvis.deltaOffset_m.y;

        Vec2 overflowXZ(0.f, 0.f);

        if (!context->playerMovement.bodyMovement.disablePoseConstraints) {
            float maxXZ = pelvis.maxOffset;
            float d2 = lenSq(desiredXZ);
            if (d2 > maxXZ * maxXZ) {
                Vec2 clamped = safeNormalize(desiredXZ) * maxXZ;
                overflowXZ = desiredXZ - clamped;
                desiredXZ = clamped;
            }
        }

        // Commit pelvis restOffset (Y allowed here as a driver, not transform locomotion)
        pelvis.restOffset_m = { desiredXZ.x, desiredY, desiredXZ.y };
        Debug::debugPrint("Pelvis Rest Offset", pelvis.restOffset_m);
        Debug::debugPrint("Pelvis Overflow", pelvis.overflow_m);
        pelvis.deltaOffset_m = { 0,0,0 };

        // Locomotion ONLY in XZ
        cTransform3D->pos_m += Vec3(overflowXZ.x, 0.f, overflowXZ.y);

        // Final pelvis world position
        pelvis.offset_m = pelvis.baseOffset_m + pelvis.restOffset_m;
        pelvis.pos_m = cTransform3D->pos_m + compMul(pelvis.offset_m, pose.scale);
    }

    return { SystemExecResult::Ran };
}
