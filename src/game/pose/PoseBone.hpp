#pragma once
#include "PoseJoint.hpp"
#include "PoseIDs.hpp"
struct PoseBone {
    PoseJointID joint1;
    PoseJointID joint2;

    float baseLength;
    float restStretch;
    float deltaStretch;
    float maxStretch = 0.25f;
    float maxCompression = -0.25f;
    PoseBone() = default;

    PoseBone(PoseJointID j1, PoseJointID j2, float maxStretch = 0.25f, float maxCompression = -0.25f)
        : joint1(j1), joint2(j2), maxStretch(maxStretch), maxCompression(maxCompression) {
    }

    void setJoints(PoseJointID j1, PoseJointID j2) {
        joint1 = j1;
        joint2 = j2;
    }

    void setMaxStretch(float v) { maxStretch = v; }
    void setMaxCompression(float v) { maxCompression = v; }

    void configure(PoseJointID j1, PoseJointID j2,
        float pMaxCompression,
        float pMaxStretch)
    {
        joint1 = j1;
        joint2 = j2;
        maxStretch = pMaxStretch;
        maxCompression = pMaxCompression;
    }
};
