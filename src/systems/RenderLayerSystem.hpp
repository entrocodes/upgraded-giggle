#pragma once
#include "game/utils/GameContext.hpp"
#include "ecs/system/ISystem.hpp"

class RenderLayerSystem : public ISystem{
public:
    SystemExec update(GameContext* context);
};
