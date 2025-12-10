#pragma once
#include "../game/utils/GameContext.hpp"
#include "../math/Bounds3D.hpp"
#include "../ecs/system/ISystem.hpp"
class NetCollisionSystem : public ISystem{
public:
    SystemExec update(GameContext* context);
private:
    void resolveNetContact(GameContext* context, Entity ballEntity);
};
