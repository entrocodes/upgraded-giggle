#pragma once
#include "ecs/system/ISystem.hpp"
#include "game/utils/GameContext.hpp"
class BoundingBoxUpdateSystem : public ISystem {
public:
    SystemExec update(GameContext* context) override;

};
