#pragma once

enum PoseJointID {
    NeckBase,
    CenterPelvis,
    LeftHip,
    RightHip,
    RightShoulder,
    RightElbow,
    RightWrist,
    LeftShoulder,
    LeftElbow,
    LeftWrist,
    LeftKnee,
    LeftAnkle,
    RightKnee,
    RightAnkle,
    Racket,
    JointCount
};
static constexpr const char* PoseJointIDNames[JointCount] = {
    "NeckBase",
    "CenterPelvis",
    "LeftHip",
    "RightHip",
    "RightShoulder",
    "RightElbow",
    "RightWrist",
    "LeftShoulder",
    "LeftElbow",
    "LeftWrist",
    "LeftKnee",
    "LeftAnkle",
    "RightKnee",
    "RightAnkle",
    "Racket"
};
enum class PoseBoneID {
    LeftUpperArm,
    LeftLowerArm,
    LeftUpperLeg,
    LeftLowerLeg,
    LeftShoulder,
    LeftHipBone,
    RightUpperArm,
    RightLowerArm,
    RightUpperLeg,
    RightLowerLeg,
    RightShoulder,
    RightHipBone,
    Spine,
    RacketHand,
    BoneCount
};
static constexpr const char* PoseBoneIDNames[
    static_cast<size_t>(PoseBoneID::BoneCount)
] = {
    "LeftUpperArm",
    "LeftLowerArm",
    "LeftUpperLeg",
    "LeftLowerLeg",
    "LeftShoulder",
    "LeftHipBone",
    "RightUpperArm",
    "RightLowerArm",
    "RightUpperLeg",
    "RightLowerLeg",
    "RightShoulder",
    "RightHipBone",
    "Spine",
    "RacketHand"
};


static PoseBoneID poseSolveOrder[] = {
    PoseBoneID::LeftLowerLeg,
    PoseBoneID::LeftUpperLeg,
    PoseBoneID::LeftHipBone,
    PoseBoneID::RightLowerLeg,
    PoseBoneID::RightUpperLeg,
    PoseBoneID::RightHipBone,
    PoseBoneID::RacketHand,
    PoseBoneID::LeftLowerArm,
    PoseBoneID::LeftUpperArm,
    PoseBoneID::LeftShoulder,
    PoseBoneID::RightLowerArm,
    PoseBoneID::RightUpperArm,
    PoseBoneID::RightShoulder,
    PoseBoneID::Spine

};