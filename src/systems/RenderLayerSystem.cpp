#include "RenderLayerSystem.hpp"
#include "../components/Components.hpp"
#include "../game/utils/GameContext.hpp"
void RenderLayerSystem::update(GameContext* context) {
    float netZ = context->tableParameters.netPos_m.z;
    for (auto e : context->registry.getEntitiesWith<CRenderLayer, CBall>()) {
        auto [cLayer, cBall] = context->registry.getComponents<CRenderLayer, CBall>(e);
        auto shadowEntity = cBall->ballShadow;
        auto cShadowLayer = context->registry.getComponent<CRenderLayer>(shadowEntity);
        cLayer->layer = 4;
        if (cBall->pos_m.z > netZ) {
            cLayer->layer = 7;
        }
        if (cBall->pos_m.z > context->tableParameters.tableLength) {
            cLayer->layer = 2;
        }
        cShadowLayer->layer = cLayer->layer - 1;
    }
}
