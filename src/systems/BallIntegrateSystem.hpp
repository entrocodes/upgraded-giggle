#pragma once
#include "ecs/system/ISystem.hpp"
#include "ecs/Entity.hpp"
#include "game/utils/GameContext.hpp"
class BallIntegrateSystem : public ISystem {
public:
    SystemExec update(GameContext* context) override;

private:
    void updateOffTable(GameContext* context);
    void handleTableContact(GameContext* context, Entity& ball);
    void handleFloorContact(GameContext* context, Entity& ball);
};
