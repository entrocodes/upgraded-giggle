// PoseArmIKSystem.cpp
#include "PoseArmIKSystem.hpp"
#include "components/Components.hpp"
#include "math/MathHelpers.hpp"
#include "debug/Debug.hpp"
#include <algorithm>
#include <cmath>

static float boneWorldLen(Pose& pose, PoseBoneID id) {
    PoseBone& b = pose.bone(id);
    PoseJoint& child = pose.joint(b.joint2);
    return MathHelpers::compMul(child.baseOffset_m, pose.scale).length();
}

static Vec3 reject(const Vec3& v, const Vec3& dirUnit) {
    return v - dirUnit * v.dot(dirUnit);
}

static Vec3 safeNormalized(const Vec3& v, const Vec3& fallback) {
    float len = v.length();
    if (len < 1e-6f) return fallback;
    return v / len;
}

SystemExec PoseArmIKSystem::update(GameContext* context) {

    const float straightEnter = 0.015f;
    const float straightExit = 0.030f;

    for (auto [e, cPose] : context->registry.getEntitiesWithComponents<CPose>()) {

        Pose& pose = cPose->pose;

        PoseJoint& sh = pose.leftShoulder();
        PoseJoint& el = pose.leftElbow();
        PoseJoint& wr = pose.leftWrist();

        // --------------------------------------------------
        // Build world-space IK target
        // --------------------------------------------------
        const Vec3 elbowWorldPos = el.lastPos_m;
        const Vec3 wristBindOffsetWorld =
            MathHelpers::compMul(wr.baseOffset_m + wr.restOffset_m, pose.scale);

        const Vec3 wristBindWorldPos = elbowWorldPos + wristBindOffsetWorld;
        const Vec3 ikTargetWorld =
            wristBindWorldPos + MathHelpers::compMul(wr.targetOffsetFromBind, pose.scale);

        Debug::queueSphere3D(ikTargetWorld, 0.06f, sf::Color::Red);

        // --------------------------------------------------
        // Geometry
        // --------------------------------------------------
        const Vec3 shoulderPos = sh.pos_m;

        const float L1 = boneWorldLen(pose, PoseBoneID::LeftUpperArm);
        const float L2 = boneWorldLen(pose, PoseBoneID::LeftLowerArm);

        Vec3 toTarget = ikTargetWorld - shoulderPos;
        float dist = toTarget.length();
        if (dist < 1e-6f) continue;

        const float minReach = std::fabs(L1 - L2) + 1e-4f;
        const float maxReach = (L1 + L2) - 1e-4f;
        dist = std::clamp(dist, minReach, maxReach);

        const Vec3 dir = toTarget.normalized();

        // --------------------------------------------------
        // IK activation + straightness latch
        // --------------------------------------------------
        const bool ikActive = wr.ikTargetActive;
        const bool ikJustEnabled = ikActive && !el.ikWasActiveLastFrame;
        el.ikWasActiveLastFrame = ikActive;

        const float straightness = (L1 + L2) - dist;

        if (!el.nearStraightLatched) {
            if (straightness < straightEnter)
                el.nearStraightLatched = true;
        }
        else {
            if (straightness > straightExit)
                el.nearStraightLatched = false;
        }

        const bool nearStraight = el.nearStraightLatched;

        // --------------------------------------------------
        // Bend plane locking
        // --------------------------------------------------
        auto chooseSeedNormal = [&]() -> Vec3 {
            Vec3 shToEl = el.lastPos_m - sh.pos_m;
            Vec3 n = shToEl.cross(toTarget);
            if (n.length() > 1e-5f) return n;

            Vec3 worldUp(0, 1, 0);
            n = worldUp.cross(dir);
            if (n.length() > 1e-5f) return n;

            return Vec3(0, 0, 1);
            };

        const bool reseedPlane =
            ikJustEnabled || nearStraight || !el.lockBendValid;

        if (reseedPlane) {
            Vec3 seed = reject(chooseSeedNormal(), dir);
            el.lockBendNormalW = safeNormalized(seed, Vec3(0, 0, 1));

            Vec3 initBendDir = el.lockBendNormalW.cross(dir);
            el.lockBendDirW = safeNormalized(initBendDir, Vec3(0, 1, 0));

            el.lockBendValid = true;
        }

        Vec3 bendNormal =
            safeNormalized(reject(el.lockBendNormalW, dir), Vec3(0, 0, 1));
        el.lockBendNormalW = bendNormal;

        Vec3 bendDir =
            safeNormalized(bendNormal.cross(dir), el.lockBendDirW);

        if (bendDir.dot(el.lockBendDirW) < 0.0f)
            bendDir = -bendDir;

        el.lockBendDirW = bendDir;

        // --------------------------------------------------
        // Two-bone solve
        // --------------------------------------------------
        float cosA =
            (L1 * L1 + dist * dist - L2 * L2) / (2.f * L1 * dist);

        cosA = std::clamp(cosA, -1.f, 1.f);
        float a = std::acos(cosA);

        const Vec3 elbowPos =
            shoulderPos
            + dir * (std::cos(a) * L1)
            + bendDir * (std::sin(a) * L1);

        // --------------------------------------------------
        // Convert to rotation deltas
        // --------------------------------------------------
        const Vec3 shoulderInvRot = -sh.rotWorld_rad;
        const Vec3 elbowInvRot = -el.rotWorld_rad;

        const Vec3 upperBind = el.baseOffset_m;
        const Vec3 upperNowLocal =
            MathHelpers::rotateByEuler(elbowPos - shoulderPos, shoulderInvRot);

        const Vec3 lowerBind = wr.baseOffset_m;
        const Vec3 lowerNowLocal =
            MathHelpers::rotateByEuler(ikTargetWorld - elbowPos, elbowInvRot);

        const Vec3 shoulderErr =
            MathHelpers::rotationFromToEuler(upperBind, upperNowLocal);

        const Vec3 elbowErr =
            MathHelpers::rotationFromToEuler(lowerBind, lowerNowLocal);

        const float ikGain = 0.6f;
        sh.deltaRotation_rad += shoulderErr * ikGain;
        el.deltaRotation_rad += elbowErr * ikGain;
    }

    return { SystemExecResult::Ran };
}
