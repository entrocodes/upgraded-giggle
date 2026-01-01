#include "NetCollisionSystem.hpp"

#include "math/Vec3.hpp"
#include "components/Components.hpp"
#include "ecs/Entity.hpp"
#include "debug/Debug.hpp"
#include "math/Random.hpp"
#include <algorithm>
#include <cmath>
#include "math/Bounds3D.hpp"
#include "math/physics/collision/ObjectIntersection.hpp"
// System update
SystemExec NetCollisionSystem::update(GameContext* context)
{
    Entity* eNet = context->registry.getEntity("net");
    if (!eNet)
        return { SystemExecResult::EarlyExit };

    auto* cNetBoundingBox3D =
        context->registry.getComponent<CBoundingBox3D>(*eNet);

    if (!cNetBoundingBox3D)
        return { SystemExecResult::EarlyExit };

    for (auto eBall : context->registry.getEntitiesWith<CBall>())
    {
        resolveNetContact(context, eBall);
    }

    return { SystemExecResult::Ran };
}

// ------------------------------------------------------------
// Net collision resolution (CCD)
// ------------------------------------------------------------
void NetCollisionSystem::resolveNetContact(GameContext* context, Entity eBall)
{
    auto [cBall, cBallTransform3D, cBallBoundingBox3D, cBallVelocity3D] = context->registry.getComponents<CBall, CTransform3D, CBoundingBox3D, CVelocity3D>(eBall);

    if (!cBall || !cBallTransform3D || !cBallBoundingBox3D || !cBallVelocity3D) return;
    // Only react when ball is moving toward the net
    if (cBallVelocity3D->vel_mps.z <= 0.0f) {
        cBall->hitNet = false;
        return;
    }

    Entity* eNet = context->registry.getEntity("net");
    auto* cNetBoundingBox3D = context->registry.getComponent<CBoundingBox3D>(*eNet);

    if (!cNetBoundingBox3D) return;

    if (!ObjectIntersection::intersects(cBallBoundingBox3D->box, cNetBoundingBox3D->box)) {cBall->hitNet = false;}

    Vec3 bStart = cBallTransform3D->lastPos_m;
    Vec3 bEnd = cBallTransform3D->pos_m;

    // Expand net AABB by ball radius
    Bounds3D expandedNet = cNetBoundingBox3D->box;
    expandedNet.min -= cBall->radius_m;
    expandedNet.max += cBall->radius_m;

    float tHit;
    if (!ObjectIntersection::sweepSegmentAABB(bStart, bEnd, expandedNet, tHit))
    {
        cBall->hitNet = false;
        return;
    }

    if (cBall->hitNet)
        return;

    cBall->hitNet = true;

    // Temporal snap (prevents deep penetration)
    float snapT = std::clamp(tHit - 0.01f, 0.0f, 1.0f);
    cBallTransform3D->pos_m = bStart + (bEnd - bStart) * snapT;

    Vec3& vel_mps = cBallVelocity3D->vel_mps;
    Vec3& spin = cBall->spin;

    // ------------------------------------------------------------
    // Net response (your existing behavior)
    // ------------------------------------------------------------

    // Reverse & damp forward motion
    vel_mps.z *= -(1.f - context->tableParameters.netDamping);

    // Spin → trajectory coupling
    vel_mps.y -= spin.x * context->tableParameters.netSpinKick; // topspin dive
    vel_mps.x += spin.z * context->tableParameters.netSpinKick; // sidespin curve

    // Spin loss from cloth grab
    spin *= (1.f - context->tableParameters.netSpinLoss);

    // Grazing chaos
    float penetrationDepth =
        expandedNet.max.z - cBallTransform3D->pos_m.z;

    if (penetrationDepth < 0.01f)
    {
        float chaos = context->tableParameters.netRandomChaos;
        vel_mps.x += Math::Random::range(-chaos, chaos);
        vel_mps.z += Math::Random::range(-chaos, chaos * 0.33f);
    }

    // Tape pop
    float netTop = cNetBoundingBox3D->box.max.y;
    if (cBallTransform3D->pos_m.y > netTop - 0.01f){
        vel_mps.y = std::abs(vel_mps.y) + 0.3f;
    }

    // Debug visualization
    if (context->physicsDebug.debugShapes){
        Debug::queueArrow3D(cBallTransform3D->pos_m, cBallTransform3D->pos_m + vel_mps.normalized() * 0.2f, sf::Color::Red);
    }
}
