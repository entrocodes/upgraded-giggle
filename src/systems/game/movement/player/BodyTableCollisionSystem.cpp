#include "BodyTableCollisionSystem.hpp"
#include "math/physics/collision/ObjectIntersection.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"
SystemExec BodyTableCollisionSystem::update(GameContext* context)
{
    const float tableZ = 0.0f;
    const float tableLeft = 0.0f;
    const float tableRight = context->tableParameters.tableWidth;
    Entity* eTable = context->registry.getEntity("table");
    auto [cTableBox] = context->registry.getComponents<CBoundingBox3D>(*eTable);
    for (auto eBody : context->registry.getEntitiesWith<CFootworkState, CBoundingBox3D, CTransform3D>())
    {
        auto [cBodyBox, cBodyTransfrom3D] = context->registry.getComponents<CBoundingBox3D, CTransform3D>(eBody);

        float penetrationZ = ObjectIntersection::calculatePenetration(cBodyBox->box, cTableBox->box).z;
        auto& cCollision =
            context->registry.addComponent<CBodyTableCollision>(eBody);

        cCollision.overlap = penetrationZ;


        // Positional correction (Z only)
        cBodyTransfrom3D->pos_m.z -= penetrationZ;
    }

    return { SystemExecResult::Ran };
}
