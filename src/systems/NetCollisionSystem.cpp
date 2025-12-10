#include "NetCollisionSystem.hpp"
#include "../math/Vec2.hpp"
#include "../math/Vec3.hpp"
#include "../math/physics/collision/BallObjectIntersection.hpp"
#include "../components/Components.hpp"
#include "../ecs/Entity.hpp"
#include "../debug/Debug.hpp"
#include "../math/Random.hpp"


SystemExec NetCollisionSystem::update(GameContext* context) {
    Entity* net = context->registry.getEntity("net");
    if (!net) return {SystemExecResult::EarlyExit};

    auto* cNetBounds =
        context->registry.getComponent<CBoundingBox3D>(*net);

    if (!cNetBounds) return {SystemExecResult::EarlyExit};

    for (auto ball : context->registry.getEntitiesWith<CBall, CTransform3D, CBoundingBox3D, CVelocity3D>()) {

        auto [cBall, cTransform3D, cBoundingBox3D, cVelocity3D] = context->registry.getComponents<CBall, CTransform3D, CBoundingBox3D, CVelocity3D>(ball);

        if (!cBall || !cBoundingBox3D || !cVelocity3D) continue;

        resolveNetContact(context, ball);
    }
    return {SystemExecResult::Ran};
}


void NetCollisionSystem::resolveNetContact(GameContext* context, Entity ballEntity)
{
    auto [cBall, cTransform3D, cBoundingBox3D, cVelocity3D] = context->registry.getComponents<CBall, CTransform3D, CBoundingBox3D, CVelocity3D>(ballEntity);

    if (!cBall || !cTransform3D || !cBoundingBox3D || !cVelocity3D) return;

    Vec3& pos = cTransform3D->pos_m;
    Vec3& vel = cVelocity3D->vel_mps;
    Vec3& spin = cBall->spin;

    Entity* net = context->registry.getEntity("net");
    auto cNetBounds = context->registry.getComponent<CBoundingBox3D>(*net);

    if (!BallObjectIntersection::intersects(cBoundingBox3D->box, cNetBounds->box))
    {
        cBall->hitNet = false; // reset when clear
        return;
    }

    if (cBall->hitNet)
        return; // avoid constant flipping

    cBall->hitNet = true;
    vel.z *= -(1.f - context->tableParameters.netDamping);   // reverse and reduce forward motion

    // Spin to trajectory (filthy net clips)

    // Topspin: ball dives downward after net
    vel.y -= spin.x * context->tableParameters.netSpinKick;

    // Sidespin: curve after grazing net
    vel.x += spin.z * context->tableParameters.netSpinKick;
    // Spin reduction — net grabs rotation
    const float spinLoss = 0.35f;
    spin *= (1.f - context->tableParameters.netSpinLoss);
    // Randomized cloth chaos 
    // detect grazing
    float penetrationDepth = (cBoundingBox3D->box.max.z - cNetBounds->box.min.z);

    if (penetrationDepth < 0.01f) // Net graze only
    {
        float chaos = context->tableParameters.netRandomChaos;
        vel.x += Math::Random::range(-chaos, chaos);
        vel.z += Math::Random::range(-chaos, chaos * 0.33f); // less in Z
        Debug::debugPrint("Chaos Triggered", penetrationDepth);
    }

    // Vertical pop if hit net top
    float netTop = cNetBounds->box.max.y;
    if (pos.y > netTop - 0.01f)
        Debug::debugPrint("Hit net top", pos.y);
        vel.y = std::abs(vel.y) + 0.3f; // slight upward deflection

    // Debug arrows (optional)
    if (context->physicsDebug.debugSpinArrows) {
        Debug::queueArrow3D(pos, pos + vel.normalized() * 0.2f, sf::Color::Red);
    }
}

