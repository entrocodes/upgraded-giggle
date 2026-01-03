#include "PoseInitializer.hpp"

#include "components/Components.hpp"

void PoseInitializer::initialize(GameContext* context) {
    for (auto [eCharacter, cPose, cTransform3D] :
        context->registry.getEntitiesWithComponents<CPose, CTransform3D>()) {

        Pose& pose = cPose->pose;

        // --------------------------------------------------
        // Root
        // --------------------------------------------------
        pose.centerPelvis().pos_m = cTransform3D->pos_m;
        pose.centerPelvis().offset_m = { 0, 0, 0 };
        pose.centerPelvis().overflow_m = { 0, 0, 0 };

        // Pelvis split
        pose.leftPelvis().offset_m = { -0.08f, 0.0f, 0.0f };
        pose.rightPelvis().offset_m = { 0.08f, 0.0f, 0.0f };

        // Spine
        pose.neckBase().offset_m = { 0.0f, 0.22f, 0.0f };

        // Legs
        pose.leftKnee().offset_m = { 0.0f, -0.38f, 0.0f };
        pose.leftAnkle().offset_m = { 0.0f, -0.38f, 0.0f };

        pose.rightKnee().offset_m = { 0.0f, -0.38f, 0.0f };
        pose.rightAnkle().offset_m = { 0.0f, -0.38f, 0.0f };

        // Arms
        pose.leftShoulder().offset_m = { -0.18f, 0.18f, 0.0f };
        pose.leftElbow().offset_m = { -0.28f, 0.0f, 0.0f };
        pose.leftWrist().offset_m = { -0.25f, 0.0f, 0.0f };

        pose.rightShoulder().offset_m = { 0.18f, 0.18f, 0.0f };
        pose.rightElbow().offset_m = { 0.28f, 0.0f, 0.0f };
        pose.rightWrist().offset_m = { 0.25f, 0.0f, 0.0f };

        // Racket (relative to wrist)
        pose.racket().offset_m = { 0.18f, -0.02f, 0.0f };

        // --------------------------------------------------
        // Clear overflow + propagate positions
        // --------------------------------------------------
        pose.forEachJoint([&](PoseJoint& j, PoseJointID) {
            j.overflow_m = { 0, 0, 0 };
            });

        pose.forEachBone([&](PoseBone& b, PoseBoneID) {
            PoseJoint& parent = pose.joint(b.joint1);
            PoseJoint& child = pose.joint(b.joint2);
            child.pos_m = parent.pos_m + child.offset_m;
            });
    }
}
