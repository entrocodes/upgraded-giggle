#pragma once
#include "ecs/Component.hpp"
#include "math/Vec3.hpp"
#include "ecs/Entity.hpp"

struct CRacketHandle : public Component {
    Entity racketEntity;

    // Offsets RELATIVE to shoulder
    Vec3 freeOffset_m = { 0.f, 0.f, 0.f };   // driven by Aim stick
    Vec3 swingOffset_m = { 0.f, 0.f, 0.f };  // driven by swing system
    Vec3  arcStartPoint = { 0,0,0 }; // Snapshot at RT release
    Vec3  steerVector = { 0,0,0 }; // Captured J1 during Commit Window
    float strokeTime_ms = 0.0f;    // Elapsed time since release

    float strokeWeight = 0.f; // 0 = free placement, 1 = full stroke
    float currentStrokeQuality = 1.0f; //for debug visualization, NOT FOR game output
    CRacketHandle(){}
    CRacketHandle(Vec3 pFreeOffset_m) 
        : freeOffset_m(pFreeOffset_m) { }
};
