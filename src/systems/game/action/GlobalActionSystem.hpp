#pragma once

#include "ecs/system/ISystem.hpp"
class GlobalActionSystem : public ISystem {
public:
    SystemExec update(GameContext* context);

};
