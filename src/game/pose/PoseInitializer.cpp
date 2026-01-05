#include "PoseInitializer.hpp"

#include "components/Components.hpp"

static inline float safeLen(const Vec3& v) { return std::sqrt(v.lengthSq()); }

void PoseInitializer::initialize(GameContext* context) {
    for (auto [eCharacter, cPose, cTransform3D] :
        context->registry.getEntitiesWithComponents<CPose, CTransform3D>()) {

        Pose& pose = cPose->pose;

        // Root
        pose.scale = { .438, .521, .5 };
        pose.centerPelvis().pos_m = cTransform3D->pos_m;
        pose.centerPelvis().baseOffset_m = { 0, -.25, 0 };
        pose.centerPelvis().offset_m = pose.centerPelvis().baseOffset_m;
        pose.centerPelvis().overflow_m = { 0, 0, 0 };

        // Pelvis split
        pose.leftPelvis().baseOffset_m = { -0.15f, 0.0f, 0.0f };
        pose.rightPelvis().baseOffset_m = { 0.15f, 0.0f, 0.0f };

        // Spine
        pose.neckBase().baseOffset_m = { 0.0f, 0.62f, 0.0f };

        // Legs
        pose.leftKnee().baseOffset_m = { -0.09f, -0.35f, 0.0f };
        pose.leftAnkle().baseOffset_m = { -0.03f, -0.28f, 0.0f };

        pose.rightKnee().baseOffset_m = { 0.09f, -0.35f, 0.0f };
        pose.rightAnkle().baseOffset_m = { 0.03f, -0.28f, 0.0f };

        // Arms
        pose.leftShoulder().baseOffset_m = { -0.18f, 0.18f, 0.0f };
        pose.leftElbow().baseOffset_m = { -0.28f, 0.0f, 0.0f };
        pose.leftWrist().baseOffset_m = { -0.05f, -0.25f,0.0f };

        pose.rightShoulder().baseOffset_m = { 0.18f, 0.18f, 0.0f };
        pose.rightElbow().baseOffset_m = { 0.28f, 0.0f, 0.0f };
        pose.rightWrist().baseOffset_m = { 0.05f, -0.25f, 0.0f };

        // Racket (relative to wrist)
        pose.racket().baseOffset_m = { 0.18f, -0.02f, 0.0f };

        // Clear overflow + propagate positions
        pose.forEachJoint([&](PoseJoint& j, PoseJointID) {
            j.overflow_m = { 0, 0, 0 };
            });

        pose.forEachBone([&](PoseBone& b, PoseBoneID id) {
            PoseJoint& parent = pose.joint(b.joint1);
            PoseJoint& child = pose.joint(b.joint2);
            child.pos_m = parent.pos_m + child.baseOffset_m;
            pose.bone(id).baseLength = safeLen(child.baseOffset_m) ;
            });
        

    }
}
