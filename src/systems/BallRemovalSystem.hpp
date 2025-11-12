#pragma once
#include "../game/utils/GameContext.hpp"
class BallRemovalSystem {
public:
    void update(GameContext* context);
    void removeAll(GameContext* context);
};
