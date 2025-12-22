#include "RenderLayerSystem.hpp"
#include "components/Components.hpp"
#include "game/utils/GameContext.hpp"
#include "debug/Debug.hpp"

SystemExec RenderLayerSystem::update(GameContext* context) {
    Entity* eNet = context->registry.getEntity("net");
    if (!eNet) {
        return { SystemExecResult::EarlyExit, "Net entity not found!" };
    }

    auto* cNetTransform3D = context->registry.getComponent<CTransform3D>(*eNet);
    if (!cNetTransform3D) {
        return { SystemExecResult::EarlyExit, "Net missing CTransform3D!" };
    }

    float netZ = cNetTransform3D->pos_m.z;

    for (auto eBall : context->registry.getEntitiesWith<CRenderLayer, CBall, CTransform3D>()) {
        auto [cBallBall, cBallRenderLayer, cBallTransform3D] = context->registry.getComponents<CBall, CRenderLayer, CTransform3D>(eBall);
        auto eBallShadow = cBallBall->ballShadow;
        auto cBallShadowRenderLayer = context->registry.getComponent<CRenderLayer>(eBallShadow);
        cBallRenderLayer->layer = 80;
        //if ball is above net on Z axis, render the net after the ball
        if (cBallTransform3D->pos_m.z > netZ) {
            cBallRenderLayer->layer = 50;
        }
        //if ball is off the table (towards the opponent) render it after the table.
        if (cBallTransform3D->pos_m.z > context->tableParameters.tableLength) {
            cBallRenderLayer->layer = 20;
        }
        cBallShadowRenderLayer->layer = cBallRenderLayer->layer - 1;
    }
    return { SystemExecResult::Ran };
}
