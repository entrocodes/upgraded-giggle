#pragma once

#include "game/utils/GameContext.hpp"
#include "ecs/system/ISystem.hpp"

class RacketReachSystem : public ISystem {
public:
    SystemExec update(GameContext* context);

private:
    static constexpr float FreeMoveSpeed = 1.2f; // m/s
};
