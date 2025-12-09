#include "RacketHandleSystem.hpp"
#include "../components/Components.hpp"
#include "../debug/Debug.hpp"

void RacketHandleSystem::update(GameContext* context) {
    auto* player = context->registry.getEntity("player");
    if (!player) return;

    auto [playerPos, handle] =
        context->registry.getComponents<
        CTransform3D,
        CRacketHandle
        >(*player);

    if (!playerPos || !handle) return;

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
}
