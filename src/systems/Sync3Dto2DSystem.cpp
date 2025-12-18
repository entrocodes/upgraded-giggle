#include "Sync3Dto2DSystem.hpp"

SystemExec Sync3Dto2DSystem::update(GameContext* context) {

    for (auto e : context->registry.getEntitiesWith<CTransform3D, CTransform>()) {
        auto [t3d, t2d] = context->registry.getComponents<CTransform3D, CTransform>(e);
        // 1. Calculate the new 2D position from 3D
        Vec2 newScreenPos = context->camera.homography.worldToImage(t3d->pos_m);
        if (t2d->lastPos.x == 0.f && t2d->lastPos.y == 0.f) {
            t2d->lastPos = newScreenPos;
        }
        // 2. Calculate new screen position from 3D world
        t2d->pos = newScreenPos;
    }
    return { SystemExecResult::Ran };
}