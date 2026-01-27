#pragma once

#include "math/Vec3.hpp"
enum class StrokeKind { None, Push, Block, Drive, Loop };

struct StrokeProfile {
    StrokeKind kind;
    int totalFrames = 20;
    int recoveryFrames = 0;
    float staminaCost;

    int shiftEndFrame = 0;   // body shift completes here
    int recoverFrame = 10;

    float strokeTime = 0.f;
    Vec3  strokeAccum = { 0,0,0 };   
    Vec3  strokeDirW = { 0,0,1 };     
    bool  strokePrimed = false;

};


struct ActiveStroke {
    const StrokeProfile* profile;
    int frame = 0;
};
