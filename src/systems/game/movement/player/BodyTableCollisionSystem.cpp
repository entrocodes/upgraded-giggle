#include "BodyTableCollisionSystem.hpp"
#include "math/physics/collision/ObjectIntersection.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"
#include "systems/game/imgui/GameImGuiConsole.hpp"
SystemExec BodyTableCollisionSystem::update(GameContext* context)
{
    Entity* eTable = context->registry.getEntity("table");
    auto [cTableBox] = context->registry.getComponents<CBoundingBox3D>(*eTable);

    for (auto eBody : context->registry.getEntitiesWith<CFootworkState, CBoundingBox3D, CTransform3D>())
    {
        auto [cBodyBox, cBodyTransfrom3D] = context->registry.getComponents<CBoundingBox3D, CTransform3D>(eBody);

        // 1. Only proceed if there is an actual 3D intersection
        if (ObjectIntersection::intersects(cBodyBox->box, cTableBox->box)) {

            // Calculate potential penetration for both X and Z
            // For X (Sides)
            float pushRight = cTableBox->box.max.x - cBodyBox->box.min.x;
            float pushLeft = cBodyBox->box.max.x - cTableBox->box.min.x;
            float penX = (pushRight < pushLeft) ? pushRight : -pushLeft;

            // For Z (Front/Back)
            float pushForward = cTableBox->box.max.z - cBodyBox->box.min.z;
            float pushBack = cBodyBox->box.max.z - cTableBox->box.min.z;
            float penZ = (pushForward < pushBack) ? pushForward : -pushBack;

            Vec3 correction(0, 0, 0);

            // 2. Resolve only the SHORTEST axis (Minimum Translation Vector)
            // This prevents the player from "warping" to the wrong side of the table
            if (std::abs(penX) < std::abs(penZ)) {
                // Hitting the side of the table
                cBodyTransfrom3D->pos_m.x += penX; // Note: Use += to apply the push
                correction.x = penX;
            }
            else {
                // Hitting the front or back of the table
                cBodyTransfrom3D->pos_m.z += penZ;
                correction.z = -penZ;
            }

            // 3. Add/Update collision component
            context->registry.getOrAddComponent<CBodyTableCollision>(eBody, correction);


        }
    }

    return { SystemExecResult::Ran };
}
