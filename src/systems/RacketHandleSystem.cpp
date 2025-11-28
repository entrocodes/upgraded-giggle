#include "RacketHandleSystem.hpp"
#include "../ecs/Component.hpp"
#include "../ecs/Entity.hpp"
#include "../components/Components.hpp"
#include "../ecs/Registry.hpp"
#include "../math/Vec3.hpp"
#include "../debug/Debug.hpp"
void RacketHandleSystem::update(GameContext* context, float dt) {
    auto* player = context->registry.getEntity("player");
    if (!player) return;

    auto [playerPos, handle, input] =
        context->registry.getComponents<CTransform3D, CRacketHandle, InputComponent>(*player);

    if (!playerPos || !handle || !input) return;

    Entity racket = handle->racketEntity;

    auto [cRacketPos, cRacketVel, cBox3D, cRacketPhys] =
        context->registry.getComponents<CTransform3D, CVelocity3D, CBoundingBox3D, CRacketPhysical>(racket);
    if (!cRacketPos || !cRacketVel || !cBox3D || !cRacketPhys)
        return;

    // Cache previous position for velocity
    Vec3 lastPos = cRacketPos->pos_m;

    // Basic follow (replace later w/ real swing logic)
    cRacketPos->pos_m = playerPos->pos_m + handle->localOffset_m;

    // Compute velocity
    cRacketVel->vel_mps = (cRacketPos->pos_m - lastPos) / dt;
    float aimX = input->axes["AimX"];
    float aimY = input->axes["AimY"];

    if (fabs(aimX) > 0.1f || fabs(aimY) > 0.1f) {
        cRacketPhys->normal.x = -aimX * 0.5f;
        cRacketPhys->normal.y = -aimY * 0.5f;
        cRacketPhys->normal.z = 1.0f;
        cRacketPhys->normal = cRacketPhys->normal.normalized();
    }

    // After updating position:
    Vec3 halfSize = (cBox3D->box.max - cBox3D->box.min) * 0.5f;
    cBox3D->box.min = cRacketPos->pos_m - halfSize;
    cBox3D->box.max = cRacketPos->pos_m + halfSize;

    if (context->physicsDebug.debugSpinArrows) {
        Debug::queueArrow3D(
            cRacketPos->pos_m,
            cRacketPos->pos_m + cRacketPhys->normal * 0.2f,
            sf::Color::Cyan
        );
    }
}
