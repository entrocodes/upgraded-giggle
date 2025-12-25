#include "NetCollisionSystem.hpp"
#include "math/Vec2.hpp"
#include "math/Vec3.hpp"
#include "math/physics/collision/BallObjectIntersection.hpp"
#include "components/Components.hpp"
#include "ecs/Entity.hpp"
#include "debug/Debug.hpp"
#include "math/Random.hpp"


SystemExec NetCollisionSystem::update(GameContext* context) {
    Entity* eNet = context->registry.getEntity("net");
    if (!eNet) return {SystemExecResult::EarlyExit};

    auto* cNetBoundingBox3D =
        context->registry.getComponent<CBoundingBox3D>(*eNet);

    if (!cNetBoundingBox3D) return {SystemExecResult::EarlyExit};

    for (auto eBall : context->registry.getEntitiesWith<CBall>()) {
        resolveNetContact(context, eBall);
    }
    return {SystemExecResult::Ran};
}


void NetCollisionSystem::resolveNetContact(GameContext* context, Entity eBall)
{
    auto [cBallBall, cBallTransform3D, cBallBoundingBox3D, cBallVelocity3D] = context->registry.getComponents<CBall, CTransform3D, CBoundingBox3D, CVelocity3D>(eBall);

    if (!cBallBall || !cBallTransform3D || !cBallBoundingBox3D || !cBallVelocity3D) return;

    Vec3& pos_m = cBallTransform3D->pos_m;
    Vec3& vel_mps = cBallVelocity3D->vel_mps;
    Vec3& spin = cBallBall->spin;

    Entity* eNet = context->registry.getEntity("net");
    auto cNetBoundingBox3D = context->registry.getComponent<CBoundingBox3D>(*eNet);

    if (!BallObjectIntersection::intersects(cBallBoundingBox3D->box, cNetBoundingBox3D->box))
    {
        cBallBall->hitNet = false; // reset when clear
        return;
    }

    if (cBallBall->hitNet)
        return; // avoid constant flipping

    cBallBall->hitNet = true;
    vel_mps.z *= -(1.f - context->tableParameters.netDamping);   // reverse and reduce forward motion

    // Spin to trajectory (filthy net clips)

    // Topspin: ball dives downward after net
    vel_mps.y -= spin.x * context->tableParameters.netSpinKick;

    // Sidespin: curve after grazing net
    vel_mps.x += spin.z * context->tableParameters.netSpinKick;
    // Spin reduction — net grabs rotation
    const float spinLoss = 0.35f;
    spin *= (1.f - context->tableParameters.netSpinLoss);
    // Randomized cloth chaos 
    // detect grazing
    float penetrationDepth = (cBallBoundingBox3D->box.max.z - cNetBoundingBox3D->box.min.z);

    if (penetrationDepth < 0.01f) // Net graze only
    {
        float chaos = context->tableParameters.netRandomChaos;
        vel_mps.x += Math::Random::range(-chaos, chaos);
        vel_mps.z += Math::Random::range(-chaos, chaos * 0.33f); // less in Z
        Debug::debugPrint("Chaos Triggered", penetrationDepth);
    }

    // Vertical pop if hit net top
    float netTop = cNetBoundingBox3D->box.max.y;
    if (pos_m.y > netTop - 0.01f)
        Debug::debugPrint("Hit net top", pos_m.y);
        vel_mps.y = std::abs(vel_mps.y) + 0.3f; // slight upward deflection

    // Debug arrows (optional)
    if (context->physicsDebug.debugArrows) {
        Debug::queueArrow3D(pos_m, pos_m + vel_mps.normalized() * 0.2f, sf::Color::Red);
    }
}

