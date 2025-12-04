#pragma once
#include "../ecs/Component.hpp"
#include "../ecs/Entity.hpp"
#include "../math/Vec3.hpp"

struct CRacketHandle : public Component {
    Entity racketEntity;
    Vec3 localOffset_m = { -0.25f, 0.90f, -0.30f }; // from player's hand pivot
    Vec3 swingOffset_m = { 0,0,0 }; // offset from swing

    // Future: grip rotation, tension input, etc.
};
