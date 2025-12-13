#pragma once
#include "../game/utils/GameContext.hpp"
#include "../ecs/system/ISystem.hpp"
class GlobalActionSystem : public ISystem {
public:
    SystemExec update(GameContext* context);

};
