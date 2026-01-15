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

    int shiftEndFrame;   // body shift completes here
    int squatStartFrame; // squat begins here
    int lockFrame;       // foot lock
    int recoverFrame;

};


struct ActiveStep {
    const StepProfile* profile;
    int frame = 0;
    Vec3 direction;
};
