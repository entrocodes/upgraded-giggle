#include "RacketHandleSystem.hpp"
#include "../ecs/Component.hpp"
#include "../components/Components.hpp"
#include "../debug/Debug.hpp"
#include <cmath>

void RacketHandleSystem::update(GameContext* context, float dt) {
    if (dt <= 0.f) return;

    auto* player = context->registry.getEntity("player");
    if (!player) return;

    auto [playerPos, handle, input, swing] =
        context->registry.getComponents<
        CTransform3D,
        CRacketHandle,
        InputComponent,
        CRacketSwing
        >(*player);

    if (!playerPos || !handle || !input || !swing) return;

    Entity racket = handle->racketEntity;
    auto [cPos, cVel, cBox3D, cPhys] =
        context->registry.getComponents<
        CTransform3D,
        CVelocity3D,
        CBoundingBox3D,
        CRacketPhysical
        >(racket);

    if (!cPos || !cVel || !cBox3D || !cPhys) return;

    Vec3 lastPos = cPos->pos_m;

    // Base position = player position + local mount offset
    Vec3 basePos = playerPos->pos_m + handle->localOffset_m;

    // Apply swing offset (backswing, etc.)
    Vec3 targetPos = basePos + handle->swingOffset_m;

    // Option C: combine offset + one-frame forward burst when swing fires
    if (swing->swingTriggered) {
        // forward burst along +Z in racket local / world frame
        targetPos.z += swing->swingSpeed * dt;
        swing->swingTriggered = false; // consumed this frame
    }

    // Compute velocity from position delta
    cVel->vel_mps = (targetPos - lastPos) / dt;

    // Commit final world position
    cPos->pos_m = targetPos;

    // Update bounding box around racket center
    Vec3 halfSize = (cBox3D->box.max - cBox3D->box.min) * 0.5f;
    cBox3D->box.min = cPos->pos_m - halfSize;
    cBox3D->box.max = cPos->pos_m + halfSize;

    // Optional debug: visualize attachment vs player
    if (context->physicsDebug.debugRacketAttach) {
        Debug::queueArrow3D(
            playerPos->pos_m,
            cPos->pos_m,
            sf::Color::Cyan
        );
    }
}
