#include "PoseSolveSystem.hpp"
#include "components/Components.hpp"

SystemExec PoseSolveSystem::update(GameContext* context)
{
    for (auto [eBody, cTransform3D, cPose] :
        context->registry.getEntitiesWithComponents<CTransform3D, CPose>())
    {
        Vec3 pelvisOffset = { 0.f,  -.3f,   0.f };
        Vec3 neckOffset = { 0.f,  0.5f,  0.f };

        Vec3 leftShoulder = { -0.15f, 0.f, 0.f };
        Vec3 rightShoulder = { 0.15f, 0.f, 0.f };

        Vec3 upperArm = { 0.f, -0.2f,  0.f };
        Vec3 forearm = { 0.f, -0.25f, 0.f };

        Vec3 leftUpperLeg = { -0.1f, -0.35f, 0.05f };
        Vec3 rightUpperLeg = { 0.1f, -0.35f, 0.05f };
        Vec3 lowerLeg = { 0.f, -0.4f,  0.01f };

        cPose->pose.joints[Pelvis].pos_m = cTransform3D->pos_m + pelvisOffset;
        cPose->pose.joints[NeckBase].pos_m = cPose->pose.joints[Pelvis].pos_m + neckOffset;

        cPose->pose.joints[LeftShoulder].pos_m = cPose->pose.joints[NeckBase].pos_m + leftShoulder;
        cPose->pose.joints[LeftElbow].pos_m = cPose->pose.joints[LeftShoulder].pos_m + upperArm;
        cPose->pose.joints[LeftWrist].pos_m = cPose->pose.joints[LeftElbow].pos_m + forearm;

        cPose->pose.joints[RightShoulder].pos_m = cPose->pose.joints[NeckBase].pos_m + rightShoulder;
        cPose->pose.joints[RightElbow].pos_m = cPose->pose.joints[RightShoulder].pos_m + upperArm;
        cPose->pose.joints[RightWrist].pos_m = cPose->pose.joints[RightElbow].pos_m + forearm;

        cPose->pose.joints[LeftKnee].pos_m = cPose->pose.joints[Pelvis].pos_m + leftUpperLeg;
        cPose->pose.joints[LeftAnkle].pos_m = cPose->pose.joints[LeftKnee].pos_m + lowerLeg;

        cPose->pose.joints[RightKnee].pos_m = cPose->pose.joints[Pelvis].pos_m + rightUpperLeg;
        cPose->pose.joints[RightAnkle].pos_m = cPose->pose.joints[RightKnee].pos_m + lowerLeg;
    }

    return { SystemExecResult::Ran };
}
