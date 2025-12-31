#include "BallSpawnDebugSystem.hpp"
#include "systems/game/imgui/GameImGuiConsole.hpp"
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
    auto& debug = context->ballSpawnDebug;
    const float speed = debug.feedSpeed;
    // Mode 1 ?Toward Racket
    Entity eRacket;

    // Mode: Toward racket (player or opponent)
    if (debug.mode == BallSpawnMode::TowardRacket ||
        debug.mode == BallSpawnMode::TowardOpponentRacket)
    {
        if (debug.mode == BallSpawnMode::TowardRacket) {
            auto* ePlayer = context->registry.getEntity("player");
            if (!ePlayer) return;

            auto* handle = context->registry.getComponent<CRacketHandle>(*ePlayer);
            if (!handle) return;

            eRacket = handle->racketEntity;
        }
        else {
            auto* eOpponent = context->registry.getEntity("opponent");
            if (!eOpponent) return;

            auto* handle = context->registry.getComponent<CRacketHandle>(*eOpponent);
            if (!handle) return;

            eRacket = handle->racketEntity;
        }

        auto* cRacketPhysical = context->registry.getComponent<CRacketPhysical>(eRacket);
        auto* cRacketTransform3D = context->registry.getComponent<CTransform3D>(eRacket);
        if (!cRacketPhysical || !cRacketTransform3D) return;

        // Invariants
        Vec3 outgoing = cRacketPhysical->worldNormal;   // post-contact
        Vec3 incoming = -outgoing;           // pre-contact

        Vec3 out = cRacketPhysical->worldNormal.normalized();

        float offset = .4f; // always positive
        Vec3 spawnPos = cRacketTransform3D->pos_m + out * offset;
        spawnPos.y += speed * context->physicsDebug.towardsRacketHeightDebugFactor;
        Vec3 vel = -out * speed;

        Entity ball = context->entityFactory.createBall(spawnPos, vel);
        //if (auto* v = context->registry.getComponent<CVelocity3D>(ball)) {
        //    ImGuiConsoleQueue(
        //        "[Spawn] vel = " +
        //        std::to_string(v->vel_mps.x) + ", " +
        //        std::to_string(v->vel_mps.y) + ", " +
        //        std::to_string(v->vel_mps.z)
        //    );
        //    ImGuiConsoleQueue("[Spawn] out = " + std::to_string(out.x) + ", " + std::to_string(out.y) + ", " + std::to_string(out.z));
        //    ImGuiConsoleQueue("[Spawn] speed = " + std::to_string(speed));
        //}
    }


    // Mode 2 ?Fixed Position
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

    // Mode 3 ?Alternate L / R
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

        Entity ball = context->entityFactory.createBall(
            spawnPos,
            context->physicsDebug.debugBallVelocity,
            context->physicsDebug.debugBallSpin
        );

        return;
    }


}