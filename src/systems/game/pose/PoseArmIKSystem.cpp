// PoseArmIKSystem.cpp
#include "PoseArmIKSystem.hpp"
#include "components/Components.hpp"
#include "math/MathHelpers.hpp"
#include "debug/Debug.hpp"

static float boneWorldLen(Pose& pose, PoseBoneID id) {
    PoseBone& b = pose.bone(id);
    PoseJoint& child = pose.joint(b.joint2);
    return MathHelpers::compMul(child.baseOffset_m, pose.scale).length();
}

SystemExec PoseArmIKSystem::update(GameContext* context) {
    for (auto [e, cPose] : context->registry.getEntitiesWithComponents<CPose>()) {
        Pose& pose = cPose->pose;
        
        PoseJoint& sh = pose.leftShoulder();
        PoseJoint& el = pose.leftElbow();
        PoseJoint& wr = pose.leftWrist();

        PoseJoint& parent = el; // elbow is wrist parent

        Vec3 parentBindWorld =
            parent.lastPos_m; // or parent.bindWorldPos if you add one

        Vec3 bindWorld =
            parentBindWorld + MathHelpers::compMul(wr.baseOffset_m + wr.restOffset_m, pose.scale);

        Vec3 ikTargetWorld =
            bindWorld + MathHelpers::compMul(wr.desiredDeltaOffset_m, pose.scale);

        Debug::queueSphere3D(ikTargetWorld, 0.06f, sf::Color::Red);
        //if (!wr.ikTargetActive) continue;
        Vec3 wristTarget = ikTargetWorld;
        Vec3 shoulderPos = sh.pos_m;

        float L1 = boneWorldLen(pose, PoseBoneID::LeftUpperArm);
        float L2 = boneWorldLen(pose, PoseBoneID::LeftLowerArm);

        Vec3 v = wristTarget - shoulderPos;
        float d = v.length();
        if (d < 1e-6f) continue;

        d = std::clamp(d, std::fabs(L1 - L2) + 1e-4f, (L1 + L2) - 1e-4f);

        Vec3 dir = v.normalized();

        Vec3 bendNormal;

        // 1) Prefer cached bend plane
        if (el.lockBendValid && el.lockBendNormalW.length() > 1e-4f) {
            bendNormal = el.lockBendNormalW.normalized();
        }
        else {
            // 2) Derive from last elbow position
            Vec3 lastElbowDir = el.lastPos_m - sh.pos_m;
            if (lastElbowDir.length() > 1e-4f) {
                bendNormal = lastElbowDir.cross(v).normalized();
            }
            else {
                // 3) Absolute fallback (never ideal, but safe)
                bendNormal = Vec3(0, 0, 1);
            }
        }

        Vec3 perp = v.cross(bendNormal);
        float perpLen = perp.length();
        if (perpLen < 1e-5f) {
            // Target is aligned with bend plane → freeze elbow this frame
            continue;
        }
        perp /= perpLen;

        Vec3 bendDir = perp.cross(v).normalized();
        Vec3 newBendNormal = (elbowPos - sh.pos_m).cross(wristTarget - sh.pos_m);
        if (newBendNormal.length() > 1e-4f) {
            el.lockBendNormalW = newBendNormal.normalized();
            el.lockBendValid = true;
        }



        float cosA = (L1 * L1 + d * d - L2 * L2) / (2.f * L1 * d);
        cosA = std::clamp(cosA, -1.f, 1.f);
        float a = std::acos(cosA);

        Vec3 elbowPos = shoulderPos + dir * (std::cos(a) * L1) + bendDir * (std::sin(a) * L1);

        // Convert world vectors into parent-local space
        Vec3 shoulderInvRot = -sh.rotWorld_rad;
        Vec3 elbowInvRot = -el.rotWorld_rad;

        Vec3 upperBind = el.baseOffset_m;
        Vec3 upperNowLocal = MathHelpers::rotateByEuler(elbowPos - shoulderPos, shoulderInvRot);

        Vec3 lowerBind = wr.baseOffset_m;
        Vec3 lowerNowLocal = MathHelpers::rotateByEuler(wristTarget - elbowPos, elbowInvRot);

        Vec3 shoulderErr = MathHelpers::rotationFromToEuler(upperBind, upperNowLocal);
        Vec3 elbowErr = MathHelpers::rotationFromToEuler(lowerBind, lowerNowLocal);

        const float ikGain = 0.6f;

        sh.deltaRotation_rad += shoulderErr * ikGain;
        el.deltaRotation_rad += elbowErr * ikGain;



        Debug::event(Debug::Channel::IK, "ArmIK", { {"d",d},{"L1",L1},{"L2",L2} });

    }

    return { SystemExecResult::Ran };
}
