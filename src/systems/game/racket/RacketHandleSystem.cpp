#include "RacketHandleSystem.hpp"
#include "../components/Components.hpp"
#include "../debug/Debug.hpp"

SystemExec RacketHandleSystem::update(GameContext* context) {
    auto* player = context->registry.getEntity("player");
    if (!player) return { SystemExecResult::EarlyExit, "player entity not found" };

    auto [playerPos, handle] =
        context->registry.getComponents<
        CTransform3D,
        CRacketHandle
        >(*player);

    if (!playerPos || !handle) return { SystemExecResult::EarlyExit, "necessary player components not found" };

    // --- Debug visualization only ---
    if (context->physicsDebug.debugRacketAttach) {
        auto racket = handle->racketEntity;
        auto* racketPos =
            context->registry.getComponent<CTransform3D>(racket);

        if (racketPos) {
            Debug::queueArrow3D(
                playerPos->pos_m,
                racketPos->pos_m,
                sf::Color::Cyan
            );
        }
    }

    // Nothing else!
    return { SystemExecResult::Ran };
}
