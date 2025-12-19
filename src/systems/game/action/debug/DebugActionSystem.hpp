#pragma once

#include "ecs/system/ISystem.hpp"


class DebugActionSystem : public ISystem {
public:
    SystemExec update(GameContext* context);
};
