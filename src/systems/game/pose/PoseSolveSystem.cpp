// PoseSolveSystem.cpp
#include "PoseSolveSystem.hpp"

#include "math/MathHelpers.hpp"
#include "components/Components.hpp"
#include "math/Constants.hpp"
#include <algorithm>
#include <cmath>

// leaf -> root (good for overflow propagation)
static constexpr PoseBoneID kSolveUpOrder[] = {
    PoseBoneID::LeftLowerLeg, PoseBoneID::LeftUpperLeg, PoseBoneID::LeftPelvisBone,
    PoseBoneID::RightLowerLeg, PoseBoneID::RightUpperLeg, PoseBoneID::RightPelvisBone,
    PoseBoneID::RacketHand,
    PoseBoneID::LeftLowerArm, PoseBoneID::LeftUpperArm, PoseBoneID::LeftShoulder,
    PoseBoneID::RightLowerArm, PoseBoneID::RightUpperArm, PoseBoneID::RightShoulder,
    PoseBoneID::Spine
};

// parent -> child (canonical forward solve)
static constexpr PoseBoneID kSolveDownOrder[] = {
    PoseBoneID::Spine,
    PoseBoneID::LeftShoulder, PoseBoneID::LeftUpperArm, PoseBoneID::LeftLowerArm, PoseBoneID::RacketHand,
    PoseBoneID::RightShoulder, PoseBoneID::RightUpperArm, PoseBoneID::RightLowerArm,
    PoseBoneID::LeftPelvisBone, PoseBoneID::LeftUpperLeg, PoseBoneID::LeftLowerLeg,
    PoseBoneID::RightPelvisBone, PoseBoneID::RightUpperLeg, PoseBoneID::RightLowerLeg
};

static inline float safeLen(const Vec3& v) { return std::sqrt(v.lengthSq()); }
static inline float safeLen(const Vec2& v) { return std::sqrt(v.length() * v.length()); }

static inline Vec3 safeNormalize(const Vec3& v) {
    float len = safeLen(v);
    if (len < 1e-6f) return { 0,0,0 };
    return v / len;
}
static inline Vec2 safeNormalize(const Vec2& v) {
    float len = safeLen(v);
    if (len < 1e-6f) return { 0,0 };
    return v / len;
}

static inline Vec3 compMul(const Vec3& a, const Vec3& b) { return { a.x * b.x, a.y * b.y, a.z * b.z }; }

SystemExec PoseSolveSystem::update(GameContext* context) {
    for (auto [eBody, cTransform3D, cPose] :
        context->registry.getEntitiesWithComponents<CTransform3D, CPose>()) {

        Pose& pose = cPose->pose;
        PoseJoint& centerPelvis = pose.centerPelvis();

        if (pose.resetRest) {
            pose.forEachJoint([&](PoseJoint& j, PoseJointID) {
                j.restOffset_m = { 0,0,0 };
                j.restRotation_rad = { 0,0,0 };
                j.deltaOffset_m = { 0,0,0 };
                j.deltaRotation_rad = { 0,0,0 };
                j.overflow_m = { 0,0,0 };
                });
            pose.forEachBone([&](PoseBone& b, PoseBoneID) {
                b.restStretch = 0.f;
                b.deltaStretch = 0.f;
                });
            pose.resetRest = false;
        }

        // 1) Translation intents -> restOffset, clamp by maxOffset, overflow pushes up
        for (PoseBoneID id : kSolveUpOrder) {
            PoseBone& b = pose.bone(id);
            PoseJoint& parent = pose.joint(b.joint1);
            PoseJoint& child = pose.joint(b.joint2);

            Vec3 desired = child.restOffset_m + child.deltaOffset_m;

            if (!context->playerMovement.bodyMovement.disablePoseConstraints &&
                desired.lengthSq() > child.maxOffset * child.maxOffset) {
                Vec3 clamped = safeNormalize(desired) * child.maxOffset;
                child.restOffset_m = clamped;
                child.overflow_m = desired - clamped;
            }
            else {
                child.restOffset_m += child.deltaOffset_m;
                child.overflow_m = { 0,0,0 };
            }

            child.deltaOffset_m = { 0,0,0 };
            parent.deltaOffset_m += child.overflow_m * child.overflowTransfer;
            child.overflow_m = { 0,0,0 };
        }

        // 2) Pelvis stance compensation (delta space)
        {
            Vec3 pelvisDelta = centerPelvis.deltaOffset_m;
            if (pelvisDelta.lengthSq() > 0.00001f) {
                float lateralShare = 0.8f;
                Vec3 lateral = { pelvisDelta.x, 0.f, pelvisDelta.z };
                pose.leftPelvis().deltaOffset_m += lateral * (0.5f * lateralShare);
                pose.rightPelvis().deltaOffset_m += lateral * (0.5f * lateralShare);
                centerPelvis.deltaOffset_m = pelvisDelta * (1.f - lateralShare);
            }
        }

        // 3) Commit rotation state (persistent), clamp per-axis, clear deltas
        pose.forEachJoint([&](PoseJoint& j, PoseJointID) {
            Vec3 desiredRot = j.restRotation_rad + j.deltaRotation_rad;
            desiredRot.x = std::clamp(desiredRot.x, j.minRot.x, j.maxRot.x);
            desiredRot.y = std::clamp(desiredRot.y, j.minRot.y, j.maxRot.y);
            desiredRot.z = std::clamp(desiredRot.z, j.minRot.z, j.maxRot.z);
            if (!j.disablerotationCalc) j.restRotation_rad = desiredRot;
            j.deltaRotation_rad = { 0,0,0 };
            });

        // 4) Root clamp + overflow -> transform only
        {
            // Separate desired offsets
            Vec2 desiredXZ(
                centerPelvis.restOffset_m.x + centerPelvis.deltaOffset_m.x,
                centerPelvis.restOffset_m.z + centerPelvis.deltaOffset_m.z
            );

            float desiredY =
                centerPelvis.restOffset_m.y + centerPelvis.deltaOffset_m.y;

            // ---- XZ locomotion clamp ----
            Vec2 overflowXZ(0.f, 0.f);

            if (!context->playerMovement.bodyMovement.disablePoseConstraints) {
                float maxXZ = centerPelvis.maxOffset;
                float lenSq = desiredXZ.length() * desiredXZ.length();

                if (lenSq > maxXZ * maxXZ) {
                    Vec2 clampedXZ = desiredXZ.normalized() * maxXZ;
                    overflowXZ = desiredXZ - clampedXZ;
                    desiredXZ = clampedXZ;
                }
            }

            // ---- Commit pelvis restOffset (Y is NEVER clamped here) ----
            centerPelvis.restOffset_m = {
                desiredXZ.x,
                desiredY,
                desiredXZ.y
            };

            centerPelvis.deltaOffset_m = { 0,0,0 };

            // ---- Apply locomotion ONLY in XZ ----
            cTransform3D->pos_m += Vec3(overflowXZ.x, 0.f, overflowXZ.y);

            // ---- Final pelvis world position ----
            centerPelvis.offset_m = centerPelvis.baseOffset_m + centerPelvis.restOffset_m;
            centerPelvis.pos_m = cTransform3D->pos_m + compMul(centerPelvis.offset_m, pose.scale);

        }

        // 5) Forward solve + bone length constraints that WRITE BACK into driver (restOffset)
        for (PoseBoneID id : kSolveDownOrder) {
            PoseBone& b = pose.bone(id);
            PoseJoint& parent = pose.joint(b.joint1);
            PoseJoint& child = pose.joint(b.joint2);

            b.restStretch += b.deltaStretch;
            b.deltaStretch = 0.f;

            float minLenLocal = b.baseLength + b.maxCompression;
            float maxLenLocal = b.baseLength + b.maxStretch;
            float targetLenLocal = std::clamp(b.baseLength + b.restStretch, minLenLocal, maxLenLocal);
            b.restStretch = targetLenLocal - b.baseLength;

            Vec3 rotatedBaseLocal = MathHelpers::rotateByEuler(child.baseOffset_m, child.restRotation_rad);

            // Bone axis (where "length" is allowed to change)
            Vec3 axis = safeNormalize(rotatedBaseLocal);
            if (axis.lengthSq() < 1e-8f) axis = { 0.f, -1.f, 0.f }; // fallback

            // Build current local vector
            Vec3 finalLocal = rotatedBaseLocal + child.restOffset_m;

            // Decompose finalLocal into parallel + perpendicular to the axis
            float parallelLen = finalLocal.dot(axis);
            Vec3 parallel = axis * parallelLen;
            Vec3 perp = finalLocal - parallel;

            // Rebuild: enforce length ONLY in parallel component
            Vec3 correctedLocal = axis * targetLenLocal + perp;

            // Convert back to restOffset relative to rotated base
            Vec3 wantedRestOffset = correctedLocal - rotatedBaseLocal;

            // Clamp by joint maxOffset (so it can't fight step 1 next frame)
            float maxOff = child.maxOffset;
            if (wantedRestOffset.lengthSq() > maxOff * maxOff) {
                wantedRestOffset = safeNormalize(wantedRestOffset) * maxOff;
            }

            child.restOffset_m = wantedRestOffset;

            // Final world position
            Vec3 finalLocal2 = rotatedBaseLocal + child.restOffset_m;
            child.pos_m = parent.pos_m + compMul(finalLocal2, pose.scale);

            // Optional debug (yours)
            child.trueRestOffset_m = child.pos_m - parent.pos_m - compMul(child.baseOffset_m, pose.scale);
        }

        
    }

    return { SystemExecResult::Ran };
}
