#pragma once
#include "game/utils/GameContext.hpp"
#include "ecs/system/ISystem.hpp"
class BallRemovalSystem : public ISystem {
public:
    SystemExec update(GameContext* context);
    void removeAll(GameContext* context);
    void keepOnlyXMostRecent(GameContext* context, int countToKeep);
private:
    bool didWork = true;
};
