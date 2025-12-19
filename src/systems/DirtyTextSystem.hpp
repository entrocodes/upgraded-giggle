#pragma once
#include "components/Components.hpp"
#include "game/utils/GameContext.hpp"
#include "ecs/system/ISystem.hpp"
class DirtyTextSystem : public ISystem {
public:
    SystemExec update(GameContext* context);
};
