#pragma once
#include "ecs/system/ISystem.hpp"
#include "math/Vec3.hpp"
#include "game/utils/GameContext.hpp"
#include "components/CTransform3D.hpp"
class RacketCollisionSystem : public ISystem{
public:
    SystemExec update(GameContext* context);
private:
    void checkIntersection(GameContext* context, const CTransform3D* cRacketTransform3D, const CTransform3D* cBallTransform3D);
};
