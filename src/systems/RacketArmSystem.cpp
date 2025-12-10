#include "RacketArmSystem.hpp"
#include "../components/Components.hpp"
#include "../game/utils/GameContext.hpp"

SystemExec RacketArmSystem::update(GameContext* context) {
    if (context->frameStats.dt <= 0.f) return { SystemExecResult::EarlyExit };

    auto* player = context->registry.getEntity("player");
    if (!player) return { SystemExecResult::EarlyExit, "player entity not found" };

    auto [input, arm, handle, playerPos] =
        context->registry.getComponents<CInput, CArm, CRacketHandle, CTransform3D>(*player);
    if (!input || !arm || !handle || !playerPos) return { SystemExecResult::EarlyExit, "player entity missing a component" };

    Entity racket = handle->racketEntity;
    auto [cPos, cVel] =
        context->registry.getComponents<CTransform3D, CVelocity3D>(racket);
    if (!cPos || !cVel) return { SystemExecResult::EarlyExit, "racket entity missing components" };

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
        handle->freeOffset_m += delta * FreeMoveSpeed * context->frameStats.dt;
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
    cVel->vel_mps = (finalPos - lastPos) / context->frameStats.dt;
    cPos->pos_m = finalPos;
    return { SystemExecResult::Ran };
}
