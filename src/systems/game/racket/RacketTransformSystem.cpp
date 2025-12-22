#include "RacketTransformSystem.hpp"
#include "components/Components.hpp"
#include "game/utils/GameContext.hpp"
SystemExec RacketTransformSystem::update(GameContext* context) {

    for (auto eRacket : context->registry.getEntitiesWith<CRacketPhysical, CTransform, CTransform3D>()) {
        auto [cRacketBoundingBox3D, cRacketTransform, cRacketTransform3D] = context->registry.getComponents<CBoundingBox3D, CTransform, CTransform3D>(eRacket);
        cRacketBoundingBox3D->setBoundingBox(cRacketTransform3D->pos_m);
    }
    return { SystemExecResult::Ran };
}
