#include "NetCollisionSystem.hpp"
#include "../math/Vec2.hpp"
#include "../math/Vec3.hpp"
#include "../math/physics/collision/BallObjectIntersection.hpp"
#include "../components/Components.hpp"
#include "../ecs/Entity.hpp"
#include "../debug/Debug.hpp"



void NetCollisionSystem::resolve(GameContext* context, Entity ballEntity) {
    float netRestitution = 0.70f;
    auto [cBall, cTransform3D, cBoundingBox3D, cVelocity3D] = context->registry.getComponents<CBall, CTransform3D, CBoundingBox3D, CVelocity3D>(ballEntity);
    const Vec3 ballPos_m = cTransform3D->pos_m;
    const Vec3 ballLastPos_m = cTransform3D->lastPos_m;
    Entity* net = context->registry.getEntity("net");
    auto cNetBoundingBox3D = context->registry.getComponent<CBoundingBox3D>(*net);
    if (BallObjectIntersection::intersects(cBoundingBox3D->box, cNetBoundingBox3D->box) && !cBall->hitNet) {
        cBall->hitNet = true;
        Bounds3D intersection = BallObjectIntersection::calculateIntersection(cBoundingBox3D->box, cNetBoundingBox3D->box);
        cVelocity3D->vel_mps.z = -cVelocity3D->vel_mps.z * netRestitution;
        Debug::debugPrint("Net Collision Occured!");
        Debug::debugPrint("Intersection Min:", intersection.min);
        Debug::debugPrint("Intersection Max:", intersection.max);

    }
}
