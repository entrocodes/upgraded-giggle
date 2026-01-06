// PoseBone.hpp
#pragma once
#include "PoseIDs.hpp"

struct PoseBone {
    PoseJointID joint1{};
    PoseJointID joint2{};

    // Length model (in LOCAL/pre-scale space)
    float baseLength = 0.f;      // |bind vector|, stable reference
    float restStretch = 0.f;     // persistent additive delta to baseLength
    float deltaStretch = 0.f;    // per-frame stretch delta (cleared each frame)

    // Allowed range relative to baseLength (local units)
    float maxStretch = 0.25f;        // + meters
    float maxCompression = -0.25f;   // - meters

    PoseBone() = default;

    PoseBone(PoseJointID j1, PoseJointID j2, float pMaxStretch = 0.25f, float pMaxCompression = -0.25f)
        : joint1(j1), joint2(j2), maxStretch(pMaxStretch), maxCompression(pMaxCompression) {
    }

    void configure(PoseJointID j1, PoseJointID j2, float pMaxCompression, float pMaxStretch) {
        joint1 = j1;
        joint2 = j2;
        maxStretch = pMaxStretch;
        maxCompression = pMaxCompression;
    }

    inline float minLenLocal() const { return baseLength + maxCompression; }
    inline float maxLenLocal() const { return baseLength + maxStretch; }
    inline float targetLenLocal() const { return baseLength + restStretch; }
};

