#pragma once
#include "../game/utils/GameContext.hpp"
#include "../math/Bounds3D.hpp"

class NetCollisionSystem {
public:
    void resolve(GameContext* context, Entity ballEntity);
    Bounds3D netBounds3D;
};
