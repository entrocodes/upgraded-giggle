#pragma once
#include "PoseJoint.hpp"
#include "PoseBone.hpp"
#include "PoseIDs.hpp"
#include "math/Vec3.hpp"

struct Pose {
    PoseJoint joints[JointCount];
    PoseBone  bones[(int)PoseBoneID::BoneCount];
    Vec3      scale = { 1, 1, 1 };

    PoseJoint& joint(PoseJointID id) { return joints[id]; }
    PoseBone& bone(PoseBoneID id) { return bones[(int)id]; }

    PoseJoint& neckBase() { return joint(NeckBase); }
    PoseJoint& centerPelvis() { return joint(CenterPelvis); }
    PoseJoint& leftPelvis() { return joint(LeftPelvis); }
    PoseJoint& rightPelvis() { return joint(RightPelvis); }
    PoseJoint& leftShoulder() { return joint(LeftShoulder); }
    PoseJoint& leftElbow() { return joint(LeftElbow); }
    PoseJoint& leftWrist() { return joint(LeftWrist); }
    PoseJoint& leftKnee() { return joint(LeftKnee); }
    PoseJoint& leftAnkle() { return joint(LeftAnkle); }
    PoseJoint& rightShoulder() { return joint(RightShoulder); }
    PoseJoint& rightElbow() { return joint(RightElbow); }
    PoseJoint& rightWrist() { return joint(RightWrist); }
    PoseJoint& rightKnee() { return joint(RightKnee); }
    PoseJoint& rightAnkle() { return joint(RightAnkle); }
    PoseJoint& racket() { return joint(Racket); }

    Pose() {
        neckBase().configure(0, 150, .015f, .4);
        centerPelvis().configure(-5.f, 5.f, 1.0f, .25);
        leftPelvis().configure(-45.f, 45.f, .04, .4);
        rightPelvis().configure(-45.f, 45.f, .04, .4);

        leftShoulder().configure(-110, 110, .04f, .4);
        leftElbow().configure(0, 145.f, .01f, .6);
        leftWrist().configure(-80, 80, .015f, .8);
        leftKnee().configure(0, 150, .03f, .6);
        leftAnkle().configure(-15, 15, .04, .85);

        rightShoulder().configure(-110, 110, .04f, .4);
        rightElbow().configure(0, 145.f, .01f, .6);
        rightWrist().configure(-80, 80, .015f, .8);
        rightKnee().configure(0, 150, .03f, .6);
        rightAnkle().configure(-15, 15, .04, .85);

        racket().configure(-60, 60, 0.002f, 1);
        bone(PoseBoneID::LeftUpperArm).configure(LeftShoulder, LeftElbow, -0.08f, 0.03f);
        bone(PoseBoneID::LeftLowerArm).configure(LeftElbow, LeftWrist, -0.05f, 0.01f);
        bone(PoseBoneID::LeftPelvisBone).configure(CenterPelvis, LeftPelvis, -0.01f, 0.02f);
        bone(PoseBoneID::LeftUpperLeg).configure(LeftPelvis, LeftKnee, -0.10f, 0.02f);
        bone(PoseBoneID::LeftLowerLeg).configure(LeftKnee, LeftAnkle, -0.07f, 0.01f);
        bone(PoseBoneID::LeftShoulder).configure(NeckBase, LeftShoulder, -0.25f, 0.15f);

        bone(PoseBoneID::RightUpperArm).configure(RightShoulder, RightElbow, -0.08f, 0.03f);
        bone(PoseBoneID::RightPelvisBone).configure(CenterPelvis, RightPelvis, -0.01f, 0.02f);
        bone(PoseBoneID::RightLowerArm).configure(RightElbow, RightWrist, -0.05f, 0.01f);
        bone(PoseBoneID::RightUpperLeg).configure(RightPelvis, RightKnee, -0.10f, 0.02f);
        bone(PoseBoneID::RightLowerLeg).configure(RightKnee, RightAnkle, -0.07f, 0.01f);
        bone(PoseBoneID::RightShoulder).configure(NeckBase, RightShoulder, -0.25f, 0.15f);

        bone(PoseBoneID::Spine).configure(CenterPelvis, NeckBase, -0.05f, 0.07f);

        bone(PoseBoneID::RacketHand).configure(LeftWrist, Racket, -.02, .02);



    }
    template <typename Fn>
    void forEachJoint(Fn&& fn) {
        for (int i = 0; i < JointCount; ++i)
            fn(joints[i], static_cast<PoseJointID>(i));
    }

    template <typename Fn>
    void forEachJoint(Fn&& fn) const {
        for (int i = 0; i < JointCount; ++i)
            fn(joints[i], static_cast<PoseJointID>(i));
    }
    template <typename Fn>
    void forEachBone(Fn&& fn) {
        for (int i = 0; i < (int)PoseBoneID::BoneCount; ++i)
            fn(bones[i], static_cast<PoseBoneID>(i));
    }

    template <typename Fn>
    void forEachBone(Fn&& fn) const {
        for (int i = 0; i < (int)PoseBoneID::BoneCount; ++i)
            fn(bones[i], static_cast<PoseBoneID>(i));
    }
    PoseJoint& jointFromStart(const PoseBone& bone) {
        return joint(bone.joint1);
    }

    PoseJoint& jointFromEnd(const PoseBone& bone) {
        return joint(bone.joint2);
    }


};

