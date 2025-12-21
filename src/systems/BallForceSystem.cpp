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
    for (auto ball : context->registry.getEntitiesWith<CBall, CTransform>()) {
        auto [cBall, cBallVelocity3D, cBallTransform3D] = context->registry.getComponents<CBall, CVelocity3D, CTransform3D>(ball);
        if (!cBall || !cBallVelocity3D || !cBallTransform3D) continue;

        cBall->bForces.forceGravity = calcBallGrav.calculateForceGravity(cBall->mass);
        cBall->bForces.forceMagnus = calcMagnus.calculateForceMagnus(cBall->spin, cBallVelocity3D->vel_mps, context->physicsDebug.debugKMagnus);
        cBall->bForces.forceDrag = calcBallDrag.calculateForceDrag(cBallVelocity3D->vel_mps);

        cBall->bForces.totalForces = cBall->bForces.forceGravity + cBall->bForces.forceMagnus + cBall->bForces.forceDrag;
        cBall->bForces.acceleration = cBall->bForces.totalForces / cBall->mass;

    }
    return {SystemExecResult::Ran};
}