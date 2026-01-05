#pragma once

#include "PoseIDs.hpp"
struct PoseJoint {
    Vec3 offset_m;
    Vec3 deltaOffset_m = { 0,0,0 };
    Vec3 baseOffset_m;
    Vec3 restOffset_m;
    Vec3 trueRestOffset_m;
    Vec3 overflow_m; //offset past max offset
    Vec3 pos_m;           // world or model space
    float rotation_rad;          // yaw / sprite rotation
    Vec3 restRotation_rad;
    Vec3 deltaRotation_rad = { 0,0,0 };  // per-frame, cleared every update

    float overflowTransfer = 1.0f;
    // Joint constraints (relative to parent)
    Vec3 minRot;       // radians or degrees
    Vec3 maxRot;

    // Optional positional slack (small!)
    float maxOffset;    // meters (visual jiggle)

    bool disablerotationCalc = false;
    void configure(Vec3 pMinRot, Vec3 pMaxRot, float pMaxOffset, float pOverflowTransfer) {
        minRot = pMinRot;
        maxRot = pMaxRot;
        maxOffset = pMaxOffset;
        overflowTransfer = pOverflowTransfer;
    }
};
