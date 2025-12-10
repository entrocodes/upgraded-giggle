#pragma once
#include "../game/utils/GameContext.hpp"
#include "../ecs/system/ISystem.hpp"
class BallRemovalSystem : public ISystem {
public:
    SystemExec update(GameContext* context);
    void removeAll(GameContext* context);
private:
    bool didWork = true;
};
