#include "PoseConstraintSystem.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"
#include <cmath>
// PoseConstraintSystem.cpp
#include "PoseConstraintSystem.hpp"
#include "components/Components.hpp"
#include "math/MathHelpers.hpp"
#include <algorithm>
#include <cmath>

static inline float clampf(float v, float a, float b) { return std::max(a, std::min(b, v)); }
static inline Vec3 clampVec3(const Vec3& v, const Vec3& mn, const Vec3& mx) {
    return { clampf(v.x, mn.x, mx.x), clampf(v.y, mn.y, mx.y), clampf(v.z, mn.z, mx.z) };
}

SystemExec PoseConstraintSystem::update(GameContext* context) {
    for (auto [e, cPose] : context->registry.getEntitiesWithComponents<CPose>()) {
        Pose& pose = cPose->pose;

        // A) Joint rotation + translation constraints (delta shaping only)
        pose.forEachJoint([&](PoseJoint& j, PoseJointID id) {
            //if ((id == PoseJointID::LeftAnkle || id == PoseJointID::RightAnkle) && j.locked) {
            //    // Zero translation deltas when locked
            //    j.deltaOffset_m = { 0,0,0 };

            //    // Allow rotation clamping only
            //    Vec3 proposedRot = j.restRotation_rad + j.deltaRotation_rad;
            //    Vec3 clampedRot = clampVec3(proposedRot, j.minRot, j.maxRot);
            //    j.deltaRotation_rad = clampedRot - j.restRotation_rad;
            //    return;
            //} 
            if (id == PoseJointID::CenterPelvis) {
                // XZ sway clamp
                Vec2 xz(j.deltaOffset_m.x, j.deltaOffset_m.z);
                if (xz.length() > j.maxOffset) {
                    xz = xz.normalized() * j.maxOffset;
                    j.deltaOffset_m.x = xz.x;
                    j.deltaOffset_m.z = xz.y;
                }

                // Y squat clamp — DO NOT use maxOffset
                float proposedY = j.restOffset_m.y + j.deltaOffset_m.y;
                float clampedY = std::clamp(proposedY, pose.pelvisMinY, pose.pelvisMaxY);
                j.deltaOffset_m.y = clampedY - j.restOffset_m.y;
                return;
            }


            // Rotation: clamp (rest + delta), rewrite delta
            Vec3 proposedRot = j.restRotation_rad + j.deltaRotation_rad;
            Vec3 clampedRot = clampVec3(proposedRot, j.minRot, j.maxRot);
            j.deltaRotation_rad = clampedRot - j.restRotation_rad;

            // Translation: clamp (rest + delta) magnitude, rewrite delta
            float maxOff = j.maxOffset;
            if (maxOff > 0.f) {
                Vec3 proposedOff = j.restOffset_m + j.deltaOffset_m;
                float len2 = proposedOff.lengthSq();
                float max2 = maxOff * maxOff;
                if (len2 > max2) {
                    float len = std::sqrt(std::max(len2, 1e-8f));
                    Vec3 clampedOff = proposedOff * (maxOff / len);
                    j.deltaOffset_m = clampedOff - j.restOffset_m;
                }
            }

            });

        // B) Bone length constraints (hard clamp via child.deltaOffset_m)
        pose.forEachBone([&](PoseBone& b, PoseBoneID) {
            PoseJoint& child = pose.joint(b.joint2);

            Vec3 rot = child.restRotation_rad + child.deltaRotation_rad;
            Vec3 base = MathHelpers::rotateByEuler(child.baseOffset_m, rot);

            Vec3 proposedOff = child.restOffset_m + child.deltaOffset_m;
            Vec3 v = base + proposedOff;

            float len = v.length();
            if (len < 1e-6f) return;

            float minL = b.minLenLocal();
            float maxL = b.maxLenLocal();
            float clamped = clampf(len, minL, maxL);

            if (std::fabs(clamped - len) < 1e-5f) return;

            Vec3 dir = v * (1.f / len);
            Vec3 v2 = dir * clamped;
            Vec3 correction = v2 - v;

            child.deltaOffset_m += correction;
            });
    }


    return { SystemExecResult::Ran };
}

