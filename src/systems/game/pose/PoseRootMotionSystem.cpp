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

        if (pose.requestedSquat > 0) {
            pelvis.desiredDeltaOffset_m.y -= pose.requestedSquat;
            Debug::debugPrint("desired delta offset", pelvis.desiredDeltaOffset_m);
        }

        // Build proposal (driver space)
        Vec2 proposedXZ(
            pelvis.restOffset_m.x + pelvis.deltaOffset_m.x,
            pelvis.restOffset_m.z + pelvis.deltaOffset_m.z
        );

        float proposedY =
            pelvis.restOffset_m.y + pelvis.deltaOffset_m.y;

        Vec2 overflowXZ{ 0.f, 0.f };

        if (!context->playerMovement.bodyMovement.disablePoseConstraints) {
            float maxXZ = pelvis.maxOffset;
            float d2 = lenSq(proposedXZ);

            if (d2 > maxXZ * maxXZ) {
                Vec2 clamped = safeNormalize(proposedXZ) * maxXZ;
                overflowXZ = proposedXZ - clamped;
                proposedXZ = clamped;
            }
        }
        // Rewrite deltaOffset so FK sees clamped proposal
        pelvis.deltaOffset_m.x = proposedXZ.x - pelvis.restOffset_m.x;
        pelvis.deltaOffset_m.z = proposedXZ.y - pelvis.restOffset_m.z;
        pelvis.deltaOffset_m.y = proposedY - pelvis.restOffset_m.y;

        //// Convert overflow into transform locomotion
        //cTransform3D->pos_m += Vec3(overflowXZ.x, 0.f, overflowXZ.y);

        pelvis.pos_m = cTransform3D->pos_m + compMul(pelvis.baseOffset_m + pelvis.restOffset_m + pelvis.deltaOffset_m, pose.scale);
    }
    return { SystemExecResult::Ran };
}
