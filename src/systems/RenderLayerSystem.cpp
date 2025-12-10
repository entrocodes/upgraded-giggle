#include "RenderLayerSystem.hpp"
#include "../components/Components.hpp"
#include "../game/utils/GameContext.hpp"
#include "../debug/Debug.hpp"

SystemExec RenderLayerSystem::update(GameContext* context) {
    Entity* net = context->registry.getEntity("net");
    if (!net) {
        return { SystemExecResult::EarlyExit, "Net entity not found!" };
    }

    auto* netTransform3D = context->registry.getComponent<CTransform3D>(*net);
    if (!netTransform3D) {
        return { SystemExecResult::EarlyExit, "Net missing CTransform3D!" };
    }

    float netZ = netTransform3D->pos_m.z;

    for (auto e : context->registry.getEntitiesWith<CRenderLayer, CBall, CTransform3D>()) {
        auto [cBall, cLayer, cTransform3D] = context->registry.getComponents<CBall, CRenderLayer, CTransform3D>(e);
        auto shadowEntity = cBall->ballShadow;
        auto cShadowLayer = context->registry.getComponent<CRenderLayer>(shadowEntity);
        cLayer->layer = 8;
        //if ball is above net on Z axis, render the net after the ball
        if (cTransform3D->pos_m.z > netZ) {
            cLayer->layer = 5;
        }
        //if ball is off the table (towards the opponent) render it after the table.
        if (cTransform3D->pos_m.z > context->tableParameters.tableLength) {
            cLayer->layer = 2;
        }
        cShadowLayer->layer = cLayer->layer - 1;
    }
    return { SystemExecResult::Ran };
}
