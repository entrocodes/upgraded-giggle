#pragma once

#include "../game/utils/GameContext.hpp"
#include "../ecs/Entity.hpp"
#include "../components/Components.hpp"
#include "../math/Vec3.hpp"

class ApplyFriction {
public:
    void applyFriction(GameContext* context, Entity ball, float dt) {
        auto [ballComp, cBallVelocity3D] = context->registry.getComponents<CBall, CVelocity3D>(ball);
        if (!ballComp || !cBallVelocity3D) return;
        const float frictionCoefficient = context->tableParameters.tableFrictionCoefficient;  // Example: coefficient of friction between ball and table
        float normalForce = ballComp->mass * 9.8f; // Gravity force (assuming mass is in kg)
        Vec3 frictionVec;
        frictionVec.x = frictionCoefficient * normalForce * (cBallVelocity3D->vel_mps.x < 0 ? -1 : 1);
        frictionVec.z = frictionCoefficient * normalForce * (cBallVelocity3D->vel_mps.z < 0 ? -1 : 1);
        cBallVelocity3D->vel_mps.x -= frictionVec.x * dt;
        cBallVelocity3D->vel_mps.z -= frictionVec.z * dt;

        ballComp->spin.x -= frictionVec.x * dt * context->tableParameters.tableSpinDecayRate;
        ballComp->spin.z -= frictionVec.z * dt * context->tableParameters.tableSpinDecayRate;
        ballComp->spin.x = std::max(0.0f, ballComp->spin.x);
        ballComp->spin.z = std::max(0.0f, ballComp->spin.z);
    }
};
