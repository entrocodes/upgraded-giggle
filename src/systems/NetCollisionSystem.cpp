#include "NetCollisionSystem.hpp"
#include "../math/Vec2.hpp"
#include "../math/Vec3.hpp"
#include "../math/physics/collision/BallObjectIntersection.hpp"
#include "../components/Components.hpp"
#include "../ecs/Entity.hpp"
#include "../debug/Debug.hpp"



void NetCollisionSystem::resolve(GameContext* context, Entity ballEntity) {
    auto [cBall, cBoundingBox3D] = context->registry.getComponents<CBall, CBoundingBox3D>(ballEntity);
    const Vec3 ballPos_m = cBall->pos_m;
    Entity* net = context->registry.getEntity("net");
    auto cNetBoundingBox3D = context->registry.getComponent<CBoundingBox3D>(*net);
    if (BallObjectIntersection::intersects(cBoundingBox3D->box, cNetBoundingBox3D->box)) {
        Bounds3D intersection = BallObjectIntersection::calculateIntersection(cBoundingBox3D->box, cNetBoundingBox3D->box);
        Debug::debugPrint("Net Collision Occured!");
        Debug::debugPrint("Intersection Min:", intersection.min);
        Debug::debugPrint("Intersection Max:", intersection.max);

    }
}
