#pragma once

#include "ecs/system/ISystem.hpp"


class DebugIntentSystem : public ISystem {
public:
    SystemExec update(GameContext* context);
};
