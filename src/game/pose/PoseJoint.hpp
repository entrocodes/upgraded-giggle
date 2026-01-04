#pragma once

#include "PoseIDs.hpp"
struct PoseJoint {
    Vec3 offset_m;
    Vec3 deltaOffset_m = { 0,0,0 };
    Vec3 baseOffset_m;
    Vec3 restOffset_m;
    Vec3 overflow_m; //offset past max offset
    Vec3 pos_m;           // world or model space
    float rot;          // yaw / sprite rotation
    float overflowTransfer = 1.0f;
    // Joint constraints (relative to parent)
    float minRot;       // radians or degrees
    float maxRot;

    // Optional positional slack (small!)
    float maxOffset;    // meters (visual jiggle)

    void configure(float pMinRot, float pMaxRot, float pMaxOffset, float pOverflowTransfer) {
        minRot = pMinRot;
        maxRot = pMaxRot;
        maxOffset = pMaxOffset;
        overflowTransfer = pOverflowTransfer;
    }
};
