#include "RacketTransformSystem.hpp"
#include "../components/Components.hpp"
#include "../game/utils/GameContext.hpp"
SystemExec RacketTransformSystem::update(GameContext* context) {

    for (auto racket : context->registry.getEntitiesWith<CRacketPhysical, CTransform, CTransform3D>()) {
        auto [cBoundingBox3D, cTransform, cTransform3D] = context->registry.getComponents<CBoundingBox3D, CTransform, CTransform3D>(racket);
        cTransform->pos = context->camera.homography.worldToImage(cTransform3D->pos_m);
        cBoundingBox3D->setBoundingBox(cTransform3D->pos_m);
    }
    return { SystemExecResult::Ran };
}
