#pragma once

#include "../game/utils/GameContext.hpp"
#include "../ecs/Entity.hpp"
#include "../components/Components.hpp"
#include "../math/Vec3.hpp"
#include "../debug/Debug.hpp"
#include <algorithm>
class ApplyFriction {
public:
    void applyFriction(GameContext* context, Entity ball, float dt) {
        auto [ballComp, vel] =
            context->registry.getComponents<CBall, CVelocity3D>(ball);
        if (!ballComp || !vel) return;

        float mu = context->tableParameters.tableFrictionCoefficient;
        float Fn = ballComp->mass * 9.8f;        // Normal force
        float dS = mu * Fn * dt;                 // Max slowdown during dt

        // 1️ Apply friction opposing X motion
        if (vel->vel_mps.x > 0) vel->vel_mps.x = std::max(0.f, vel->vel_mps.x - dS);
        else if (vel->vel_mps.x < 0) vel->vel_mps.x = std::min(0.f, vel->vel_mps.x + dS);

        // 2️ Apply friction opposing Z motion
        if (vel->vel_mps.z > 0) vel->vel_mps.z = std::max(0.f, vel->vel_mps.z - dS);
        else if (vel->vel_mps.z < 0) vel->vel_mps.z = std::min(0.f, vel->vel_mps.z + dS);
        // 3️ Spin decay → oppose current spin rotation
        float decayAmount = dS * context->tableParameters.tableSpinDecayRate;

        if (ballComp->spin.x > 0) ballComp->spin.x = std::max(0.f, ballComp->spin.x - decayAmount);
        else if (ballComp->spin.x < 0) ballComp->spin.x = std::min(0.f, ballComp->spin.x + decayAmount);

        if (ballComp->spin.z > 0) ballComp->spin.z = std::max(0.f, ballComp->spin.z - decayAmount);
        else if (ballComp->spin.z < 0) ballComp->spin.z = std::min(0.f, ballComp->spin.z + decayAmount);
    }

};
