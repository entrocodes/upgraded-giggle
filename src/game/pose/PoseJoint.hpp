#pragma once

#include "PoseIDs.hpp"
struct PoseJoint {
    Vec3 pos_m;           // world or model space
    float rot;          // yaw / sprite rotation

    // Joint constraints (relative to parent)
    float minRot;       // radians or degrees
    float maxRot;

    // Optional positional slack (small!)
    float maxOffset;    // meters (visual jiggle)

    void configure(float pMinRot, float pMaxRot, float pMaxOffset) {
        minRot = pMinRot;
        maxRot = pMaxRot;
        maxOffset = pMaxOffset;
    }
};
