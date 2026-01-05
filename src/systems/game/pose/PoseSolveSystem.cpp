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

static inline Vec3 safeNormalize(const Vec3& v) {
    float len = safeLen(v);
    if (len < 1e-6f) return { 0,0,0 };
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
            Vec3 rootDesired = centerPelvis.restOffset_m + centerPelvis.deltaOffset_m;

            if (!context->playerMovement.bodyMovement.disablePoseConstraints &&
                rootDesired.lengthSq() > centerPelvis.maxOffset * centerPelvis.maxOffset) {
                Vec3 clamped = safeNormalize(rootDesired) * centerPelvis.maxOffset;
                centerPelvis.restOffset_m = clamped;
                centerPelvis.overflow_m = rootDesired - clamped;
            }
            else {
                centerPelvis.restOffset_m = rootDesired;
                centerPelvis.overflow_m = { 0,0,0 };
            }

            centerPelvis.deltaOffset_m = { 0,0,0 };
            cTransform3D->pos_m += centerPelvis.overflow_m;
            centerPelvis.overflow_m = { 0,0,0 };

            centerPelvis.offset_m = centerPelvis.baseOffset_m + centerPelvis.restOffset_m;
            centerPelvis.pos_m = cTransform3D->pos_m + compMul(centerPelvis.offset_m, pose.scale);
        }

        // 5) Forward solve + bone length constraints that WRITE BACK into driver (restOffset)
        for (PoseBoneID id : kSolveDownOrder) {
            PoseBone& b = pose.bone(id);
            PoseJoint& parent = pose.joint(b.joint1);
            PoseJoint& child = pose.joint(b.joint2);

            // Consume bone stretch delta into persistent stretch (then clear)
            b.restStretch += b.deltaStretch;
            b.deltaStretch = 0.f;

            // Clamp the *target* stretch relative to bind length
            float minLenLocal = b.baseLength + b.maxCompression;
            float maxLenLocal = b.baseLength + b.maxStretch;

            float targetLenLocal = b.baseLength + b.restStretch;
            targetLenLocal = std::clamp(targetLenLocal, minLenLocal, maxLenLocal);

            // If clamped, reflect that back into restStretch (so it doesn't keep fighting)
            b.restStretch = targetLenLocal - b.baseLength;

            // Build the local vector using rotation + joint offset driver
            Vec3 rotatedBaseLocal = MathHelpers::rotateByEuler(child.baseOffset_m, child.restRotation_rad);
            Vec3 finalLocal = rotatedBaseLocal + child.restOffset_m;

            // Compute current length
            float lenLocal = safeLen(finalLocal);
            if (lenLocal < 1e-6f) { child.pos_m = parent.pos_m + compMul(rotatedBaseLocal, pose.scale); continue; }

            float clampedLen = std::clamp(lenLocal, minLenLocal, maxLenLocal);
            b.restStretch = clampedLen - b.baseLength;

            // Now *build* the desired local vector at that length by scaling the direction,
            // but ONLY along the bone axis (not by injecting big restOffset)
            Vec3 dir = safeNormalize(finalLocal);
            Vec3 correctedLocal = dir * (b.baseLength + b.restStretch);

            // Convert back to restOffset relative to rotated base, but clamp to maxOffset
            Vec3 wantedRestOffset = correctedLocal - rotatedBaseLocal;

            float maxOff = child.maxOffset;
            if (wantedRestOffset.lengthSq() > maxOff * maxOff) {
                wantedRestOffset = safeNormalize(wantedRestOffset) * maxOff;
            }
            child.restOffset_m = wantedRestOffset;
            finalLocal = rotatedBaseLocal + child.restOffset_m;
            child.pos_m = parent.pos_m + compMul(finalLocal, pose.scale);
            child.trueRestOffset_m = child.pos_m - parent.pos_m - compMul(child.baseOffset_m, pose.scale);
        }
    }

    return { SystemExecResult::Ran };
}
