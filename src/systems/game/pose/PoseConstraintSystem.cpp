#include "PoseConstraintSystem.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"
#include "math/MathHelpers.hpp"
#include "game/pose/PoseIDs.hpp"
#include <algorithm>
#include <cmath>

static inline float clampf(float v, float a, float b) {
    return std::max(a, std::min(b, v));
}

static inline Vec3 clampVec3(const Vec3& v, const Vec3& mn, const Vec3& mx) {
    return {
        clampf(v.x, mn.x, mx.x),
        clampf(v.y, mn.y, mx.y),
        clampf(v.z, mn.z, mx.z)
    };
}
SystemExec PoseConstraintSystem::update(GameContext* context) {
    for (auto [e, cPose] : context->registry.getEntitiesWithComponents<CPose>()) {
        Pose& pose = cPose->pose;

        // A) JOINT ROTATION + TRANSLATION CONSTRAINTS
        pose.forEachJoint([&](PoseJoint& j, PoseJointID id) {
            if (j.ikTargetActive)
                return; // IK owns this joint this frame

            // --- Center pelvis: XZ sway clamp ---
            if (id == PoseJointID::CenterPelvis) {
                Vec2 xz(j.deltaOffset_m.x, j.deltaOffset_m.z);
                float len = xz.length();

                if (len > j.maxOffset) {
                    Vec2 clamped = xz.normalized() * j.maxOffset;
                    Vec2 correction = clamped - xz;

                    j.deltaOffset_m.x = clamped.x;
                    j.deltaOffset_m.z = clamped.y;

                    Debug::event(
                        Debug::Channel::Constraint,
                        "PelvisXZClamp",
                        {
                            {"len", len},
                            {"max", j.maxOffset},
                            {"corr", correction.length()}
                        }
                    );
                }
                return;
            }
            if (id == PoseJointID::Racket) {
                Vec3 proposed = j.restOffset_m + j.deltaOffset_m;

                float hardMax = 0.24f;

                Vec3 clamped = proposed;
                if (proposed.lengthSq() > hardMax * hardMax) {
                    clamped = proposed.normalized() * hardMax;
                }

                Vec3 overflow = proposed - clamped;

                j.deltaOffset_m = clamped - j.restOffset_m;
                j.overflow_m += overflow;

                if (overflow.lengthSq() > 1e-10f)
                    Debug::debugPrint("RacketOverflow", overflow);

                return; //skip generic clamp for racket
            }
            // --- Rotation clamp ---
            Vec3 proposedRot = j.restRotation_rad + j.deltaRotation_rad;
            Vec3 clampedRot = clampVec3(proposedRot, j.minRot, j.maxRot);
            Vec3 rotCorrection = clampedRot - proposedRot;

            if (rotCorrection.lengthSq() > 1e-8f) {Debug::event(Debug::Channel::Constraint,"RotClampX",{{"req", proposedRot.x},{"min", j.minRot.x},{"max", j.maxRot.x}},{},{ {"joint", poseJointName(id)} }); }
            if (rotCorrection.lengthSq() > 1e-8f) {Debug::event(Debug::Channel::Constraint,"RotClampY",{{"req", proposedRot.y},{"min", j.minRot.y},{"max", j.maxRot.y}},{},{ {"joint", poseJointName(id)} }); }
            if (rotCorrection.lengthSq() > 1e-8f) {Debug::event(Debug::Channel::Constraint,"RotClampZ",{{"req", proposedRot.z},{"min", j.minRot.z},{"max", j.maxRot.z}},{},{ {"joint", poseJointName(id)} }); }

            j.deltaRotation_rad = clampedRot - j.restRotation_rad;

            // --- Translation clamp ---
            
            if (j.maxOffset > 0.f) {
                Vec3 proposedOff = j.restOffset_m + j.deltaOffset_m;
                float lenSq = proposedOff.lengthSq();
                float maxSq = j.maxOffset * j.maxOffset;

                if (lenSq > maxSq) {
                    float len = std::sqrt(lenSq);
                    Vec3 clampedOff = proposedOff * (j.maxOffset / len);
                    Vec3 correction = clampedOff - proposedOff;

                    j.deltaOffset_m = clampedOff - j.restOffset_m;
                    j.overflow_m += correction;
                    Debug::event(Debug::Channel::Constraint,"OffsetClamp",{{"len", len},{"max", j.maxOffset},{"corr", correction.length()}},{},{ {"joint", poseJointName(id)} } );
                }
            }
            });

        // B) BONE LENGTH CONSTRAINTS (IK FRICTION)
        pose.forEachBone([&](PoseBone& b, PoseBoneID boneId) {
            bool isArmBone =
                boneId == PoseBoneID::LeftUpperArm ||
                boneId == PoseBoneID::LeftLowerArm ||
                boneId == PoseBoneID::RightUpperArm ||
                boneId == PoseBoneID::RightLowerArm ||
                boneId == PoseBoneID::RacketHand;

            if (isArmBone)
                return;

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

            if (std::fabs(clamped - len) > 1e-5f) {
                Vec3 dir = v / len;
                Vec3 v2 = dir * clamped;
                Vec3 correction = v2 - v;

                child.deltaOffset_m += correction;
                Debug::event(Debug::Channel::Constraint,"BoneLenClamp",{{"len", len},{"min", minL},{"max", maxL},{"corr", correction.length()}}, {},{{"bone", poseBoneName(boneId)}, {"child", poseJointName(b.joint2)}});}
            });

    }

    return { SystemExecResult::Ran };
}
