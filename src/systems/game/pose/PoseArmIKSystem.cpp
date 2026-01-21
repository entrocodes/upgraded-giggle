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
    Vec3 bindW = MathHelpers::compMul(child.baseOffset_m, pose.scale);
    return bindW.length();
}

SystemExec PoseArmIKSystem::update(GameContext* context) {
    for (auto [e, cPose] : context->registry.getEntitiesWithComponents<CPose>()) {
        Pose& pose = cPose->pose;

        PoseJoint& sh = pose.leftShoulder();
        PoseJoint& el = pose.leftElbow();
        PoseJoint& wr = pose.leftWrist();
        PoseJoint& rk = pose.racket();

        // Only engage when racket overflow exists
        if (rk.overflow_m.lengthSq() < 1e-10f) continue;
        Debug::debugPrint("ArmIK running, rk overflow len", std::sqrt(rk.overflow_m.lengthSq()));

        float L1 = boneWorldLen(pose, PoseBoneID::LeftUpperArm);
        float L2 = boneWorldLen(pose, PoseBoneID::LeftLowerArm);

        Vec3 shPos = sh.pos_m;

        // Desired racket world target (approx)
        Vec3 target = rk.pos_m + MathHelpers::compMul(rk.overflow_m, pose.scale);

        Vec3 v = target - shPos;

        // YZ plane solve
        float Dy = v.y;
        float Dz = v.z;

        float d = std::sqrt(Dy * Dy + Dz * Dz);
        if (d < 1e-6f) continue;

        d = std::clamp(d, std::fabs(L1 - L2) + 1e-4f, (L1 + L2) - 1e-4f);

        float phi = std::atan2(-Dz, -Dy);

        float cosA = (L1 * L1 + d * d - L2 * L2) / (2.f * L1 * d);
        cosA = std::clamp(cosA, -1.f, 1.f);
        float alpha = std::acos(cosA);

        float elbowPitch = phi - alpha;

        // Write to elbow hinge (child rotation!)
        wr.deltaRotation_rad.x = elbowPitch - wr.restRotation_rad.x;

        // Optional: tiny shoulder assist so elbow hinge isn't forced to do everything
        /*sh.deltaRotation_rad.x += (Dy * 0.1f);*/
    }

    return { SystemExecResult::Ran };
}
