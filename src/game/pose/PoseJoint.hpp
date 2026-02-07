// PoseJoint.hpp
#pragma once
#include "math/Vec3.hpp"
#include <algorithm>

struct PoseJoint {
    bool ikWasActiveLastFrame = false;
    bool nearStraightLatched = false; // optional but recommended

    Vec3 lockBendNormalW = { 0,0,1 };
    Vec3 lockBendDirW = { 0,1,0 };
    bool lockBendValid = false;

    bool rotClampedThisFrame = false;
    Vec3 rotWorld_rad = { 0,0,0 };
    Vec3 ikTargetWorldPos = { 0,0,0};
    bool ikTargetActive = false;
    float lockRefPelvisY = 0.f;
    float lockRefPelvisZ = 0.f;
    bool  lockRefPelvisValid = false;
    float lockRefDistanceYZ = -1;
    float lockRefDzSign = -1;
    bool wasClampedLastFrame = true;
    // --- Driver state (persistent) ---
    Vec3 baseOffset_m{ 0,0,0 };        // bind/local socket offset from parent (pre-scale)
    Vec3 restOffset_m{ 0,0,0 };        // persistent local translation offset (pre-scale)
    Vec3 restRotation_rad{ 0,0,0 };    // persistent local euler rotation (radians), applied to baseOffset_m

    // --- Per-frame deltas (cleared each frame) ---
    Vec3 deltaOffset_m{ 0,0,0 };
    Vec3 lastTargetOffset{ 0,0,0 };
    Vec3 lastTargetOffsetFromBind{ 0,0,0 };
    Vec3 lastWristWorldPos{ 0,0,0 };
    Vec3 targetOffsetFromBind{ 0,0,0 };
    Vec3 deltaRotation_rad{ 0,0,0 };

    // --- Solver outputs (computed) ---
    Vec3 offset_m{ 0,0,0 };            // baseOffset_m + restOffset_m (pre-scale)
    Vec3 pos_m{ 0,0,0 };               // world position
    Vec3 overflow_m{ 0,0,0 };          // used by joint maxOffset clamp

    // --- Constraints ---
    float maxOffset = 0.015f;        // meters (pre-scale)
    float overflowTransfer = 1.0f;   // how much child overflow propagates to parent deltaOffset

    Vec3 minRot{ -3.1415926f, -3.1415926f, -3.1415926f };
    Vec3 maxRot{ +3.1415926f, +3.1415926f, +3.1415926f };

    bool disablerotationCalc = false;

    // --- Contact/locking (constraint phase) ---
    bool locked = false;
    Vec3 lockedWorldPos_m{ 0,0,0 };
    float lockWeight = 1.0f;         // 0..1 strength

    Vec3 lastPos_m = { 0,0,0 };
    // --- Debug helpers (optional) ---
    Vec3 trueRestOffset_m{ 0,0,0 };    // world delta from parent minus bind (for debug)
    Vec3 tempTrueRestOffset_m{ 0,0,0 };

    void configure(Vec3 pMinRot, Vec3 pMaxRot, float pMaxOffset, float pOverflowTransfer) {
        minRot = pMinRot;
        maxRot = pMaxRot;
        maxOffset = pMaxOffset;
        overflowTransfer = pOverflowTransfer;
    }

    inline void clampRotationInPlace() {
        restRotation_rad.x = std::clamp(restRotation_rad.x, minRot.x, maxRot.x);
        restRotation_rad.y = std::clamp(restRotation_rad.y, minRot.y, maxRot.y);
        restRotation_rad.z = std::clamp(restRotation_rad.z, minRot.z, maxRot.z);
    }
};
