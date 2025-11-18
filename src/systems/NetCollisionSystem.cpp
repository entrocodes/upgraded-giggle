#include "NetCollisionSystem.hpp"
#include "../math/Vec2.hpp"
#include "../math/Vec3.hpp"
#include "../math/physics/collision/BallObjectIntersection.hpp"
#include "../components/Components.hpp"
#include "../ecs/Entity.hpp"
#include "../debug/Debug.hpp"



void NetCollisionSystem::resolve(GameContext* context, Entity ballEntity) {
    const Vec3 netPos_m = context->tableParameters.netPos_m;
    const Vec3 netSize_m = context->tableParameters.netSize_m;
    netBounds3D.setBounds(netPos_m - (netSize_m / 2), netPos_m + (netSize_m / 2));
    auto [cBall] = context->registry.getComponents<CBall>(ballEntity);
    const Vec3 ballPos_m = cBall->pos_m;

    if (BallObjectIntersection::intersects(cBall->ballBounds3D, netBounds3D)) {
        Debug::debugPrint("Net Collision Occured! Ball Position:", ballPos_m);

    }
}
