#include "BodyTableCollisionSystem.hpp"

#include "components/Components.hpp"

SystemExec BodyTableCollisionSystem::update(GameContext* context)
{
    const float tableZ = 0.0f;
    const float tableLeft = 0.0f;
    const float tableRight = context->tableParameters.tableWidth;

    for (auto eBody : context->registry.getEntitiesWith<CFootworkState, CBoundingBox3D, CTransform3D>())
    {
        auto [cBox, cBodyTransfrom3D] = context->registry.getComponents<CBoundingBox3D, CTransform3D>(eBody);

        // AABB extents
        float frontZ = cBodyTransfrom3D->pos_m.z + cBox->halfSize.z;
        float leftX = cBodyTransfrom3D->pos_m.x - cBox->halfSize.x;
        float rightX = cBodyTransfrom3D->pos_m.x + cBox->halfSize.x;

        // Overlapping table region?
        bool overlapsX = (rightX > tableLeft) && (leftX < tableRight);
        bool penetratesZ = frontZ > tableZ;

        if (!overlapsX || !penetratesZ)
            continue;

        float overlap = frontZ - tableZ;
        if (overlap <= 0.f)
            continue;

        if (!context->registry.hasComponent<CBodyTableCollision>(eBody)) {
            auto& cCollision =
                context->registry.addComponent<CBodyTableCollision>(eBody);

            cCollision.overlap = overlap;
        }

        // Positional correction (Z only)
        cBodyTransfrom3D->pos_m.z -= overlap;
    }

    return { SystemExecResult::Ran };
}
