#include "RacketHandleSystem.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"

SystemExec RacketHandleSystem::update(GameContext* context) {
    auto* ePlayer = context->registry.getEntity("player");
    if (!ePlayer) return { SystemExecResult::EarlyExit, "player entity not found" };

    auto [cPlayerTransform3D, cPlayerRacketHandle] =
        context->registry.getComponents<
        CTransform3D,
        CRacketHandle
        >(*ePlayer);

    if (!cPlayerTransform3D || !cPlayerRacketHandle) return { SystemExecResult::EarlyExit, "necessary player components not found" };

    // --- Debug visualization only ---
    if (context->physicsDebug.debugRacketAttach) {
        auto eRacket = cPlayerRacketHandle->racketEntity;
        auto* cRacketTransform3D =
            context->registry.getComponent<CTransform3D>(eRacket);

        if (cRacketTransform3D) {
            Debug::queueArrow3D(
                cPlayerTransform3D->pos_m,
                cRacketTransform3D->pos_m,
                sf::Color::Cyan
            );
        }
    }

    // Nothing else!
    return { SystemExecResult::Ran };
}
