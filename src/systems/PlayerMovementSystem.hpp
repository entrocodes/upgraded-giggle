#pragma once

#include "../ecs/system/ISystem.hpp"
#include "../game/utils/GameContext.hpp"


class PlayerMovementSystem : public ISystem {
public:
    SystemExec update(GameContext* context) override {
        Entity* player = context->registry.getEntity("player");
        if (!player) return { SystemExecResult::EarlyExit, "player entity not found" };

        auto [cTransform, cTransform3D, cVelocity3D] =
            context->registry.getComponents<CTransform, CTransform3D, CVelocity3D>(*player);

        cTransform3D->pos_m += cVelocity3D->vel_mps * context->frameStats.dt;
        cTransform->pos =
            context->camera.homography.worldToImage(cTransform3D->pos_m);
        return { SystemExecResult::Ran };
    }
};
