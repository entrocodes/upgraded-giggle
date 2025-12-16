#pragma once
#include "../ecs/system/ISystem.hpp"
#include "../math/Vec3.hpp"
#include "../game/utils/GameContext.hpp"
class RacketCollisionSystem : public ISystem{
public:
    SystemExec update(GameContext* context);
};
