#pragma once
#include "ecs/Component.hpp"
#include "math/Vec3.hpp"
#include "ecs/Entity.hpp"

struct CRacketHandle : public Component {
    Entity racketEntity;

    // Offsets RELATIVE to shoulder
    Vec3 pushOffset_m = { 0.f, 0.f, 0.f };
    Vec3 freeOffset_m = { 0.f, 0.f, 0.f };   // driven by Aim stick
    Vec3 resolvedOffset_m;
    CRacketHandle(){}
    CRacketHandle(Vec3 pFreeOffset_m) 
        : freeOffset_m(pFreeOffset_m) { }
};
