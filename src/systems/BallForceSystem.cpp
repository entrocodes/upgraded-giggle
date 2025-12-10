#include "BallForceSystem.hpp"
#include "../math/Vec2.hpp"
#include "../math/Bounds3D.hpp"
#include "../components/Components.hpp"
#include "../ecs/Entity.hpp"
#include "../debug/Debug.hpp"
#include "../math/GridTransform.hpp"

#include <algorithm>
#include <iostream>

SystemExec BallForceSystem::update(GameContext* context) {
    for (auto ball : context->registry.getEntitiesWith<CBall, CTransform>()) {
        auto [ballComp, cBallVelocity3D, cBallTransform3D] = context->registry.getComponents<CBall, CVelocity3D, CTransform3D>(ball);
        if (!ballComp || !cBallVelocity3D || !cBallTransform3D) continue;

        ballComp->bForces.forceGravity = calcBallGrav.calculateForceGravity(ballComp->mass);
        ballComp->bForces.forceMagnus = calcMagnus.calculateForceMagnus(ballComp->spin, cBallVelocity3D->vel_mps, context->physicsDebug.debugKMagnus);
        ballComp->bForces.forceDrag = calcBallDrag.calculateForceDrag(cBallVelocity3D->vel_mps);

        ballComp->bForces.totalForces = ballComp->bForces.forceGravity + ballComp->bForces.forceMagnus + ballComp->bForces.forceDrag;
        ballComp->bForces.acceleration = ballComp->bForces.totalForces / ballComp->mass;

    }
    return {SystemExecResult::Ran};
}