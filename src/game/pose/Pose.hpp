#pragma once
#include "PoseJoint.hpp"
#include "PoseBone.hpp"
#include "PoseIDs.hpp"
#include "footwork/SupportMode.hpp"
#include "math/Vec3.hpp"

struct Pose {
    float requestedSquat = 0.f;
    float requestedRaise = 0.f;
    SupportMode prevSupportMode = SupportMode::Airborne;
    SupportMode supportMode = SupportMode::Airborne;
    PoseJoint joints[JointCount];
    PoseBone  bones[(int)PoseBoneID::BoneCount];
    Vec3      scale = { 1, 1, 1 };
    bool      resetRest = false;

    PoseJoint& joint(PoseJointID id) { return joints[id]; }
    PoseBone& bone(PoseBoneID id) { return bones[(int)id]; }

    PoseJoint& neckBase() { return joint(NeckBase); }
    PoseJoint& centerPelvis() { return joint(CenterPelvis); }
    PoseJoint& leftHip() { return joint(LeftHip); }
    PoseJoint& rightHip() { return joint(RightHip); }
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
        Vec3 testMaxRot = { 3.14, 3.14, 3.14 };
        Vec3 testMinRot = { -3.14, -3.14, -3.14 };
        // Spine / root
        neckBase().configure(Vec3(-0.4f, -0.6f, -0.3f), Vec3(0.6f, 0.6f, 0.3f), 0.015f, 0.4f);
        centerPelvis().configure(Vec3(-0.3f, -0.4f, -0.2f), Vec3(0.4f, 0.4f, 0.2f), 0, 0.25f);

        // Pelvis (hips) ?yaw dominant, small pitch
        leftHip().configure(Vec3(-1.0f, -0.6f, 0.0f), Vec3(1.0f, 0.6f, 0.0f), 0.3f, 0.4f);
        rightHip().configure(Vec3(-1.0f, -0.6f, 0.0f), Vec3(1.0f, 0.6f, 0.0f), 0.3f, 0.4f);

        // Shoulders ?reach with limited twist
        leftShoulder().configure(Vec3(-0.9f, -0.8f, -0.9f), Vec3(1.2f, 0.8f, 0.9f), 0.04f, 0.4f);
        rightShoulder().configure(Vec3(-0.6f, -0.8f, -0.4f), Vec3(1.0f, 0.8f, 0.4f), 0.04f, 0.4f);

        leftElbow().configure(Vec3(0.0f, -0.12f, -0.12f), Vec3(2.5f, 0.12f, 0.12f), 0.01f, 0.6f);
        rightElbow().configure(Vec3(0.0f, -0.12f, -0.12f), Vec3(2.5f, 0.12f, 0.12f), 0.01f, 0.6f);

        leftWrist().configure(Vec3(-0.9f, -0.25f, -0.6f), Vec3(0.9f, 0.25f, 0.6f), 0.005f, 0.45f);
        rightWrist().configure(Vec3(-0.9f, -0.25f, -0.15f), Vec3(0.9f, 0.25f, 0.15f), 0.005f, 0.45f);
        //leftWrist().configure(testMinRot, testMaxRot, 4., 0.8f);
        //rightWrist().configure(testMinRot, testMaxRot, 4., 0.8f);

        // Knees strict hinge (forward only)
        leftKnee().configure(Vec3(-2.4f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f), 0.08f, 0.6f);
        rightKnee().configure(Vec3(-2.4f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f), 0.08f, 0.6f);

        // Ankles flex and roll, no yaw
        leftAnkle().configure(Vec3(-0.8f, 0.0f, -0.3f), Vec3(0.8f, 0.0f, 0.3f), 0.04f, 0.85f);
        rightAnkle().configure(Vec3(-0.8f, 0.0f, -0.3f), Vec3(0.8f, 0.0f, 0.3f), 0.04f, 0.85f);

        // Racket very tight rotation
        racket().configure(Vec3(-0.4f, -0.4f, -0.2f), Vec3(0.4f, 0.4f, 0.2f), 0.002f, 1.0f);

        bone(PoseBoneID::LeftUpperArm).configure(LeftShoulder, LeftElbow, -0.08f, 0.03f);
        bone(PoseBoneID::LeftLowerArm).configure(LeftElbow, LeftWrist, -0.05f, 0.01f);
        bone(PoseBoneID::LeftHipBone).configure(CenterPelvis, LeftHip, -0.01f, 0.02f);
        bone(PoseBoneID::LeftUpperLeg).configure(LeftHip, LeftKnee, -0.10f, 0.02f);
        bone(PoseBoneID::LeftLowerLeg).configure(LeftKnee, LeftAnkle, -0.07f, 0.01f);
        bone(PoseBoneID::LeftShoulder).configure(NeckBase, LeftShoulder, -0.25f, 0.15f);

        bone(PoseBoneID::RightUpperArm).configure(RightShoulder, RightElbow, -0.08f, 0.03f);
        bone(PoseBoneID::RightHipBone).configure(CenterPelvis, RightHip, -0.01f, 0.02f);
        bone(PoseBoneID::RightLowerArm).configure(RightElbow, RightWrist, -0.05f, 0.01f);
        bone(PoseBoneID::RightUpperLeg).configure(RightHip, RightKnee, -0.10f, 0.02f);
        bone(PoseBoneID::RightLowerLeg).configure(RightKnee, RightAnkle, -0.07f, 0.01f);
        bone(PoseBoneID::RightShoulder).configure(NeckBase, RightShoulder, -0.25f, 0.15f);

        bone(PoseBoneID::Spine).configure(CenterPelvis, NeckBase, -0.05f, 0.07f);

        bone(PoseBoneID::RacketHand).configure(LeftWrist, Racket, -.02, .02);

    }
    template <typename Fn>
    void forEachAnkle(Fn&& fn) {
        fn(joints[PoseJointID::LeftAnkle], PoseJointID::LeftAnkle);
        fn(joints[PoseJointID::RightAnkle], PoseJointID::RightAnkle);
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
    void computeBindLengths() {
        forEachBone([&](PoseBone& b, PoseBoneID) {
            PoseJoint& child = joint(b.joint2);
            b.baseLength = child.baseOffset_m.length(); // local pre-scale
            });
    }


};

