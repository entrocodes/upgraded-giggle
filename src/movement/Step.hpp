#pragma once

#include "math/Vec3.hpp"
enum class StepKind { None, Tap, Hop, Leap, Reach };
enum class DominantFoot { None, Left, Right };
struct StepRaw {
    StepKind kind;
    float strength;

};
struct StepProfile {
    StepKind kind;
    DominantFoot dominantFoot = DominantFoot::None;
    int totalFrames;
    int recoveryFrames;
    float maxStride_m;     // replaces maxSpeed
    float staminaCost;

    int shiftEndFrame = 0;   // body shift completes here
    int squatStartFrame = 6; // squat begins here
    int lockFrame = 6;       // foot lock
    int recoverFrame = 10;

};


struct ActiveStep {
    const StepProfile* profile;
    int frame = 0;
    Vec3 direction;
};
