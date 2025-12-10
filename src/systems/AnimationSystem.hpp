#pragma once
#include "../ecs/system/ISystem.hpp"

class AnimationSystem : public ISystem {
public:
    SystemExec update(GameContext* context) override;

private:
    SystemExec updatePlayer(GameContext* context);
};
