// PoseAnkleLockIKSystem.cpp
#include "PoseAnkleLockIKSystem.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"
#include "math/MathHelpers.hpp"
#include <algorithm>
#include <cmath>
static void solvePelvisZ(Pose& pose) {
    constexpr float squatRatioZ = 0.4f;
    PoseJoint& pelvis = pose.centerPelvis();

    pelvis.deltaOffset_m.y = pelvis.desiredDeltaOffset_m.y;

    pelvis.deltaOffset_m.z = pelvis.deltaOffset_m.y * squatRatioZ;
    Debug::debugPrint("delta offset Y", pelvis.desiredDeltaOffset_m.y);
}

static void solveLeg(Pose& pose, PoseJointID id) {
    PoseJoint& knee = (id == PoseJointID::LeftAnkle) ? pose.leftKnee() : pose.rightKnee();
    PoseJoint& ankle = (id == PoseJointID::LeftAnkle) ? pose.leftAnkle() : pose.rightAnkle();
    PoseJoint& hip = (id == PoseJointID::LeftAnkle) ? pose.leftHip() : pose.rightHip();
    PoseJoint& pelvis = pose.centerPelvis();
    PoseBone& upperLeg = (id == PoseJointID::LeftAnkle) ? pose.bone(PoseBoneID::LeftUpperLeg) : pose.bone(PoseBoneID::RightUpperLeg);
    PoseBone& lowerLeg = (id == PoseJointID::LeftAnkle) ? pose.bone(PoseBoneID::LeftLowerLeg) : pose.bone(PoseBoneID::RightLowerLeg); 

    float lowerLegLength = (lowerLeg.baseLength + lowerLeg.restStretch + lowerLeg.deltaStretch) / 2;
    float upperLegLength = (upperLeg.baseLength + upperLeg.restStretch + upperLeg.deltaStretch) / 2;
    Vec3 hipPos_m = hip.pos_m + pelvis.deltaOffset_m;
    Debug::queueSphere3D(hipPos_m, .02, sf::Color::Yellow);
    Vec3 anklePos_m = ankle.pos_m;

    float Dy = (anklePos_m.y - hipPos_m.y);
    float Dz = (anklePos_m.z - hipPos_m.z);
    float d = std::sqrt(Dy * Dy + Dz * Dz);

    d = std::clamp(d, std::fabs(upperLegLength - lowerLegLength) + 1e-5f, (upperLegLength + lowerLegLength) - 1e-5f);

    float pitchFromYZ = std::atan2(-Dz, -Dy);
    
    float cosAlpha = (upperLegLength * upperLegLength + d * d - lowerLegLength * lowerLegLength) / (2.f * upperLegLength * d);
    cosAlpha = std::clamp(cosAlpha, -1.f, 1.f);
    float alpha = std::acos(cosAlpha);     // offset from phi for the first segment

    float cosBeta = (upperLegLength * upperLegLength + lowerLegLength * lowerLegLength - d * d) / (2.f * upperLegLength * lowerLegLength);
    cosBeta = std::clamp(cosBeta, -1.f, 1.f);
    float beta = std::acos(cosBeta);       // interior angle between segments

    // Solution 1
    float kneePitch1 = pitchFromYZ - alpha;
    float anklePitch1 = kneePitch1 + M_PI - beta;

    //// Solution 2 (mirror)
    float kneePitch2 = pitchFromYZ + alpha;
    float anklePitch2 = kneePitch2 - (M_PI - beta);

    knee.deltaRotation_rad.x = kneePitch1 - knee.restRotation_rad.x;
    ankle.deltaRotation_rad.x = anklePitch1 - ankle.restRotation_rad.x;

}
// System entry
SystemExec PoseAnkleLockIKSystem::update(GameContext* context) {
    for (auto [eBody, cTransform3D, cPose] :

        context->registry.getEntitiesWithComponents<CTransform3D, CPose>()) {
        Pose& pose = cPose->pose;
        float dt = context->frameStats.dt;

        const bool leftLocked = pose.leftAnkle().locked;
        const bool rightLocked = pose.rightAnkle().locked;

        if (leftLocked && rightLocked) {
            solvePelvisZ(pose);
            solveLeg(pose, PoseJointID::LeftAnkle);
            solveLeg(pose, PoseJointID::RightAnkle);
        }
    }

    return { SystemExecResult::Ran };
}
