#pragma once
#include "../ecs/System.hpp"
#include "../math/Vec3.hpp"
#include "../game/utils/GameContext.hpp"
class RacketCollisionSystem {
public:
    void update(GameContext* context, float dt);
};
