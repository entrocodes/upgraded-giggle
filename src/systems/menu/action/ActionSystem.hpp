#pragma once
#include "ecs/system/ISystem.hpp"
class ActionSystem : public ISystem {
public:
    SystemExec update(GameContext* context);
};
