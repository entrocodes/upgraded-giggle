#include "RenderLayerSystem.hpp"
#include "../components/Components.hpp"
#include "../game/utils/GameContext.hpp"
#include "../debug/Debug.hpp"
void RenderLayerSystem::update(GameContext* context) {
    Entity* net = context->registry.getEntity("net");
    if (!net) {
        Debug::debugPrint("Net entity not found!");
        return;
    }

    auto* netTransform3D = context->registry.getComponent<CTransform3D>(*net);
    if (!netTransform3D) {
        Debug::debugPrint("Net missing CTransform3D!");
        return;
    }

    float netZ = netTransform3D->pos_m.z;

    for (auto e : context->registry.getEntitiesWith<CRenderLayer, CBall>()) {
        auto [cLayer, cBall] = context->registry.getComponents<CRenderLayer, CBall>(e);
        auto shadowEntity = cBall->ballShadow;
        auto cShadowLayer = context->registry.getComponent<CRenderLayer>(shadowEntity);
        cLayer->layer = 8;
        //if ball is above net on Z axis, render the net after the ball
        if (cBall->pos_m.z > netZ) {
            cLayer->layer = 5;
        }
        //if ball is off the table (towards the opponent) render it after the table.
        if (cBall->pos_m.z > context->tableParameters.tableLength) {
            cLayer->layer = 2;
        }
        cShadowLayer->layer = cLayer->layer - 1;
    }
}
