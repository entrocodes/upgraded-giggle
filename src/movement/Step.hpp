#pragma once

#include "math/Vec3.hpp"
enum class StepKind { Tap, Hop, Leap };

struct StepRaw {
    StepKind kind;
    float strength;

};
struct StepProfile {
    int totalFrames;
    int recoveryFrames;
    float maxSpeed_mps;
    float staminaCost;
};

struct ActiveStep {
    const StepProfile* profile;
    int frame = 0;
    Vec3 direction;
};
