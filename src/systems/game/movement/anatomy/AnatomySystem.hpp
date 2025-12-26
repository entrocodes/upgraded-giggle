#pragma once
#include "ecs/system/ISystem.hpp"
class AnatomySystem : public ISystem {
public:
    SystemExec update(GameContext* context);

};
