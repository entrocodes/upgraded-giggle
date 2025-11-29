#include "RacketHandleSystem.hpp"
#include "../debug/Debug.hpp"
#include <cmath>

void RacketHandleSystem::update(GameContext* context, float dt) {
    auto* player = context->registry.getEntity("player");
    if (!player) return;

    auto [playerPos, handle, input, swing] =
        context->registry.getComponents<
        CTransform3D, CRacketHandle, InputComponent, CRacketSwing
        >(*player);

    if (!playerPos || !handle || !input || !swing) return;

    Entity racket = handle->racketEntity;
    auto [cPos, cVel, cBox3D, cPhys] =
        context->registry.getComponents<
        CTransform3D, CVelocity3D, CBoundingBox3D, CRacketPhysical
        >(racket);

    if (!cPos || !cVel || !cBox3D || !cPhys) return;

    Vec3 lastPos = cPos->pos_m;

    // Follow player at base offset
    cPos->pos_m = playerPos->pos_m + handle->localOffset_m;

    // Backswing movement (racket moves backward)
    if (swing->isCharging) {
        cPos->pos_m.z -= swing->backswingDistance * dt;
    }

    // Forward swing impulse (only once)
    if (swing->swingTriggered) {
        cVel->vel_mps.z += swing->swingSpeed;
        swing->swingTriggered = false;
    }

    // Recompute racket velocity after any movement
    cVel->vel_mps = (cPos->pos_m - lastPos) / dt;

    // Update bounding box
    Vec3 halfSize = (cBox3D->box.max - cBox3D->box.min) * 0.5f;
    cBox3D->box.min = cPos->pos_m - halfSize;
    cBox3D->box.max = cPos->pos_m + halfSize;
}
