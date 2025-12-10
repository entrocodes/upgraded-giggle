#pragma once
#include "../game/utils/GameContext.hpp"
#include "../ecs/system/ISystem.hpp"
class PlayerActionSystem : public ISystem {
public:
    SystemExec update(GameContext* context);

};
