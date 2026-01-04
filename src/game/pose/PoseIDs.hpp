#pragma once

enum PoseJointID {
    NeckBase,
    CenterPelvis,
    LeftPelvis,
    RightPelvis,
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
    "LeftPelvis",
    "RightPelvis",
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
    LeftPelvisBone,
    RightUpperArm,
    RightLowerArm,
    RightUpperLeg,
    RightLowerLeg,
    RightShoulder,
    RightPelvisBone,
    Spine,
    RacketHand,
    BoneCount
};

static PoseBoneID poseSolveOrder[] = {
    PoseBoneID::LeftLowerLeg,
    PoseBoneID::LeftUpperLeg,
    PoseBoneID::LeftPelvisBone,
    PoseBoneID::RightLowerLeg,
    PoseBoneID::RightUpperLeg,
    PoseBoneID::RightPelvisBone,
    PoseBoneID::RacketHand,
    PoseBoneID::LeftLowerArm,
    PoseBoneID::LeftUpperArm,
    PoseBoneID::LeftShoulder,
    PoseBoneID::RightLowerArm,
    PoseBoneID::RightUpperArm,
    PoseBoneID::RightShoulder,
    PoseBoneID::Spine

};