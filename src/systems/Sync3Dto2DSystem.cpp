#include "Sync3Dto2DSystem.hpp"
#include "components/Components.hpp"

SystemExec Sync3Dto2DSystem::update(GameContext* context) {
    for (auto e : context->registry.getEntitiesWith<CTransform3D, CTransform>()) {
        auto [t3d, t2d] =
            context->registry.getComponents<CTransform3D, CTransform>(e);

        t2d->lastPos =
            context->camera.homography.worldToImage(t3d->lastPos_m);

        t2d->pos =
            context->camera.homography.worldToImage(t3d->pos_m);
    }

    return { SystemExecResult::Ran };
}
