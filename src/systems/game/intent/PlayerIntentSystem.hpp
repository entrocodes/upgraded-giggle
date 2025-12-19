#pragma once

#include "ecs/system/ISystem.hpp"
class PlayerIntentSystem : public ISystem{
public:
    SystemExec update(GameContext* context);
};
