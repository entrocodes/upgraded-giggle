#pragma once
#include "components/Components.hpp"
#include "game/utils/GameContext.hpp"
#include "ecs/system/ISystem.hpp"
class BallContactSystem : public ISystem {
public:
    SystemExec update(GameContext* context);
private:
    void handleContact(GameContext* context, Entity& ball);

};
