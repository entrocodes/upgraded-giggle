#include "RacketArmSystem.hpp"
#include "../components/Components.hpp"
#include "../game/utils/GameContext.hpp"

void RacketArmSystem::update(GameContext* context) {
    float dt = context->frameStats.dt;
    if (dt <= 0.f) return;

    auto* player = context->registry.getEntity("player");
    if (!player) return;

    auto [input, arm, handle] =
        context->registry.getComponents<CInput, CArm, CRacketHandle>(*player);
    if (!input || !arm || !handle) return;

    auto* playerPos =
        context->registry.getComponent<CTransform3D>(*player);
    if (!playerPos) return;

    Entity racket = handle->racketEntity;
    auto [cPos, cVel] =
        context->registry.getComponents<CTransform3D, CVelocity3D>(racket);
    if (!cPos || !cVel) return;

    Vec3 lastPos = cPos->pos_m;

    // --------------------------------------------------
    // 1) Shoulder follows player (THIS WAS THE MISSING LINK)
    // --------------------------------------------------
    arm->shoulderPos_m =
        playerPos->pos_m + Vec3(0.f, 0.45f, 0.f); // tune later

    // --------------------------------------------------
    // 2) Free racket placement (ONLY when not swinging)
    // --------------------------------------------------
    if (handle->strokeWeight < 0.01f) {
        float aimX = input->axes["AimX"];
        float aimY = input->axes["AimY"];

        Vec3 delta(aimX, aimY, 0.f);
        handle->freeOffset_m += delta * FreeMoveSpeed * dt;
    }

    // --------------------------------------------------
    // 3) Arm reach constraint (relative to shoulder)
    // --------------------------------------------------
    Vec3 toTarget = handle->freeOffset_m;
    float len = toTarget.length();
    if (len > arm->maxReach_m) {
        toTarget *= arm->maxReach_m / len;
        handle->freeOffset_m = toTarget; // keep it clamped
    }

    Vec3 placementPos =
        arm->shoulderPos_m + toTarget;

    // --------------------------------------------------
    // 4) Stroke blending
    // --------------------------------------------------
    Vec3 strokePos = placementPos + handle->swingOffset_m;

    Vec3 finalPos =
        placementPos * (1.f - handle->strokeWeight) +
        strokePos * handle->strokeWeight;

    // --------------------------------------------------
    // 5) Commit transform + velocity
    // --------------------------------------------------
    cVel->vel_mps = (finalPos - lastPos) / dt;
    cPos->pos_m = finalPos;
}
