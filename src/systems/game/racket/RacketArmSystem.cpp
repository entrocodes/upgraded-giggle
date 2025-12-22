#include "RacketArmSystem.hpp"
#include "components/Components.hpp"
#include "game/utils/GameContext.hpp"

SystemExec RacketArmSystem::update(GameContext* context) {
    if (context->frameStats.dt <= 0.f) return { SystemExecResult::EarlyExit };

    auto* ePlayer = context->registry.getEntity("player");
    if (!ePlayer) return { SystemExecResult::EarlyExit, "player entity not found" };

    auto [cPlayerInput, cPlayerArm, cPlayerRacketHandle, cPlayerPos] =
        context->registry.getComponents<CInput, CArm, CRacketHandle, CTransform3D>(*ePlayer);
    if (!cPlayerInput || !cPlayerArm || !cPlayerRacketHandle || !cPlayerPos) return { SystemExecResult::EarlyExit, "player entity missing a component" };

    Entity eRacket = cPlayerRacketHandle->racketEntity;
    auto [cRacketTransform3D, cRacketVelocity3D] =
        context->registry.getComponents<CTransform3D, CVelocity3D>(eRacket);
    if (!cRacketTransform3D || !cRacketVelocity3D) return { SystemExecResult::EarlyExit, "racket entity missing components" };

    Vec3 lastPos = cRacketTransform3D->pos_m;

    // --------------------------------------------------
    // 1) Shoulder follows player (THIS WAS THE MISSING LINK)
    // --------------------------------------------------
    cPlayerArm->shoulderPos_m =
        cPlayerPos->pos_m + Vec3(0.f, 0.45f, 0.f); // tune later

    // --------------------------------------------------
    // 2) Free racket placement (ONLY when not swinging)
    // --------------------------------------------------
    if (cPlayerRacketHandle->strokeWeight < 0.01f) {
        float aimX = cPlayerInput->axes["AimX"];
        float aimY = cPlayerInput->axes["AimY"];

        Vec3 delta(aimX, aimY, 0.f);
        cPlayerRacketHandle->freeOffset_m += delta * FreeMoveSpeed * context->frameStats.dt;
    }

    // --------------------------------------------------
    // 3) Arm reach constraint (relative to shoulder)
    // --------------------------------------------------
    Vec3 toTarget = cPlayerRacketHandle->freeOffset_m;
    float len = toTarget.length();
    if (len > cPlayerArm->maxReach_m) {
        toTarget *= cPlayerArm->maxReach_m / len;
        cPlayerRacketHandle->freeOffset_m = toTarget; // keep it clamped
    }

    Vec3 placementPos =
        cPlayerArm->shoulderPos_m + toTarget;

    // --------------------------------------------------
    // 4) Stroke blending
    // --------------------------------------------------
    Vec3 strokePos = placementPos + cPlayerRacketHandle->swingOffset_m;

    Vec3 finalPos =
        placementPos * (1.f - cPlayerRacketHandle->strokeWeight) +
        strokePos * cPlayerRacketHandle->strokeWeight;

    // --------------------------------------------------
    // 5) Commit transform + velocity
    // --------------------------------------------------
    // 5) Commit transform + velocity
    cRacketTransform3D->lastPos_m = lastPos;
    cRacketVelocity3D->vel_mps = (finalPos - lastPos) / context->frameStats.dt;
    cRacketTransform3D->pos_m = finalPos;
    return { SystemExecResult::Ran };
}
