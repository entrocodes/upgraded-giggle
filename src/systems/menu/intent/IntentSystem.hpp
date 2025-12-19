#pragma once
#include "ecs/system/ISystem.hpp"
class IntentSystem : public ISystem{
public:
    SystemExec update(GameContext* context);
};
