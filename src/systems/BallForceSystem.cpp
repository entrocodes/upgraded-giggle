#include "BallForceSystem.hpp"
#include "math/Vec2.hpp"
#include "math/Bounds3D.hpp"
#include "components/Components.hpp"
#include "ecs/Entity.hpp"
#include "debug/Debug.hpp"
#include "math/GridTransform.hpp"

#include <algorithm>
#include <iostream>

SystemExec BallForceSystem::update(GameContext* context) {
    for (auto eBall : context->registry.getEntitiesWith<CBall, CTransform>()) {
        auto [cBallBall, cBallVelocity3D, cBallTransform3D] = context->registry.getComponents<CBall, CVelocity3D, CTransform3D>(eBall);
        if (!cBallBall || !cBallVelocity3D || !cBallTransform3D) continue;

        cBallBall->bForces.forceGravity = calcBallGrav.calculateForceGravity(cBallBall->mass);
        cBallBall->bForces.forceMagnus = calcMagnus.calculateForceMagnus(cBallBall->spin, cBallVelocity3D->vel_mps, context->physicsDebug.debugKMagnus);
        cBallBall->bForces.forceDrag = calcBallDrag.calculateForceDrag(cBallVelocity3D->vel_mps);

        cBallBall->bForces.totalForces = cBallBall->bForces.forceGravity + cBallBall->bForces.forceMagnus + cBallBall->bForces.forceDrag;
        cBallBall->bForces.acceleration = cBallBall->bForces.totalForces / cBallBall->mass;

    }
    return {SystemExecResult::Ran};
}