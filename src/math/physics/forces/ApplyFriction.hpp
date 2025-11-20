#pragma once

#include "../game/utils/GameContext.hpp"
#include "../ecs/Entity.hpp"
#include "../components/Components.hpp"
#include "../math/Vec3.hpp"
#include "../debug/Debug.hpp"
#include <algorithm>
class ApplyFriction {
public:
    void applyFriction(GameContext* context, Entity& ball, float dt) {
        auto [ballComp, cBallVelocity3D] = context->registry.getComponents<CBall, CVelocity3D>(ball);
        if (!ballComp || !cBallVelocity3D) return;
        const float frictionCoefficient = context->tableParameters.tableFrictionCoefficient;  // Example: coefficient of friction between ball and table
        float normalForce = ballComp->mass * 9.8f; // Gravity force (assuming mass is in kg)
        ballComp->bForces.friction.x = frictionCoefficient * normalForce;
        ballComp->bForces.friction.z = frictionCoefficient * normalForce;
        if (ballComp->bForces.friction.z < 0 || ballComp->bForces.friction.x < 0) {
            Debug::debugPrint("Something Wrong Here", ballComp->bForces.friction);
        }
        if (cBallVelocity3D->vel_mps.x > 0) {
            cBallVelocity3D->vel_mps.x -= ballComp->bForces.friction.x * dt;
        }
        else if (cBallVelocity3D.vel_mps.x < 0) {
            cBallVelocity3D->vel_mps.x += ballComp->bForces.friction.x * dt;
        }
        if (cBallVelocity3D->vel_mps.z > 0) {
            cBallVelocity3D->vel_mps.z -= ballComp->bForces.friction.z * dt;
        }
        else if (cBallVelocity3D->vel_mps.z < 0) {
            cBallVelocity3D->vel_mps.z += ballComp->bForces.friction.z * dt;
        }

        if (ballComp->spin.x < 0) {
            ballComp->spin.x += ballComp->bForces.friction.x * dt * context->tableParameters.tableSpinDecayRate;
        }
        else if (ballComp->spin.x > 0) {
            ballComp->spin.x -= ballComp->bForces.friction.x * dt * context->tableParameters.tableSpinDecayRate;
        }
        if (ballComp->spin.z < 0) {
            ballComp->spin.z += ballComp->bForces.friction.z * dt * context->tableParameters.tableSpinDecayRate;
        }
        else if (ballComp->spin.z > 0) {
            ballComp->spin.z -= ballComp->bForces.friction.z * dt * context->tableParameters.tableSpinDecayRate;
        }

        // clamp using sign — allow both positive and negative spin to decay
        if (std::abs(ballComp->spin.x) < 0.0001f) ballComp->spin.x = 0.0f;
        if (std::abs(ballComp->spin.z) < 0.0001f) ballComp->spin.z = 0.0f;
    }
};
