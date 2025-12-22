#include "BallSpawnDebugSystem.hpp"

SystemExec BallSpawnDebugSystem::update(GameContext * context) {
    // --- Auto ball spawn ---
    auto& debug = context->ballSpawnDebug;
    if (debug.autoSpawn) {
        debug.timer += context->frameStats.dt;
        if (debug.timer >= debug.interval) {
            debug.timer = 0.f;
            spawnDebugBall(context);
        }
    }
    return { SystemExecResult::Ran };
   
}
void BallSpawnDebugSystem::spawnDebugBall(GameContext* context) {
    auto* ePlayer = context->registry.getEntity("player");
    if (!ePlayer) return;

    auto* cPlayerRacketHandle = context->registry.getComponent<CRacketHandle>(*ePlayer);
    if (!cPlayerRacketHandle) return;

    Entity eRacket = cPlayerRacketHandle->racketEntity;

    auto* cRacketRacketPhysical = context->registry.getComponent<CRacketPhysical>(eRacket);
    auto* cRacketTransform3D = context->registry.getComponent<CTransform3D>(eRacket);

    if (!cRacketRacketPhysical || !cRacketTransform3D) return;

    auto& debug = context->ballSpawnDebug;
    const float speed = debug.feedSpeed;
    // Mode 1 — Toward Racket
    if (debug.mode == BallSpawnMode::TowardRacket) {
        Vec3 spawnPos = cRacketTransform3D->pos_m + cRacketRacketPhysical->normal * 0.20f;
        Vec3 vel = -cRacketRacketPhysical->normal * speed;

        context->entityFactory.createBall(spawnPos, vel);
        return;
    }

    // Mode 2 — Fixed Position
    if (debug.mode == BallSpawnMode::FixedPosition) {
        Vec3 spawnPos(
            debug.fixedPosLeft.x,
            context->physicsDebug.debugBallHeight,
            debug.fixedPosLeft.z
        );

        context->entityFactory.createBall(
            spawnPos,
            context->physicsDebug.debugBallVelocity,
            context->physicsDebug.debugBallSpin
        );
        return;
    }

    // Mode 3 — Alternate L / R
    if (debug.mode == BallSpawnMode::AlternateLeftRight) {
        debug.spawnLeftLast = !debug.spawnLeftLast;

        const Vec3& src = debug.spawnLeftLast
            ? debug.fixedPosLeft
            : debug.fixedPosRight;

        Vec3 spawnPos(
            src.x,
            context->physicsDebug.debugBallHeight,
            src.z
        );

        context->entityFactory.createBall(
            spawnPos,
            context->physicsDebug.debugBallVelocity,
            context->physicsDebug.debugBallSpin
        );
        return;
    }
}