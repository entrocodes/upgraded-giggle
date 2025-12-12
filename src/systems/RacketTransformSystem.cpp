#include "RacketTransformSystem.hpp"
#include "../components/Components.hpp"
#include "../game/utils/GameContext.hpp"
SystemExec RacketTransformSystem::update(GameContext* context) {

    for (auto racket : context->registry.getEntitiesWith<CRacket, CTransform, CTransform3D>()) {
        auto [cTransform, cTransform3D] = context->registry.getComponents<CTransform, CTransform3D>(racket);
        cTransform->pos = context->camera.homography.WorldToImage(cTransform3D->pos_m);

    }
    return { SystemExecResult::Ran };
}
