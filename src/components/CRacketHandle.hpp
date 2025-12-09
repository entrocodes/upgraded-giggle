#pragma once
#include "../ecs/Component.hpp"
#include "../math/Vec3.hpp"
#include "../ecs/Entity.hpp"

struct CRacketHandle : public Component {
    Entity racketEntity;

    // Offsets RELATIVE to shoulder
    Vec3 freeOffset_m = { 0.f, 0.f, 0.f };   // driven by Aim stick
    Vec3 swingOffset_m = { 0.f, 0.f, 0.f };  // driven by swing system

    float strokeWeight = 0.f; // 0 = free placement, 1 = full stroke
};
