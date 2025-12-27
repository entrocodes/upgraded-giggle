#include "RacketOrientationSystem.hpp"

#include "math/Constants.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"

SystemExec RacketOrientationSystem::update(GameContext* context) {
    for (auto e : context->registry.getEntitiesWith<CAuthorization>()) {
        auto [cRacketHandle, cAuthorization] =
            context->registry.getComponents<CRacketHandle,CAuthorization>(e);
        Entity eRacket = cRacketHandle->racketEntity;
        auto [cRacketPhysical, cRotation3D, cTransform3D] =
            context->registry.getComponents<CRacketPhysical, CRotation3D, CTransform3D>(eRacket);
        auto it = cAuthorization->vec2Map.find("BladeOrientation");
        if (it != cAuthorization->vec2Map.end() || cAuthorization->vec2Map["BladeOrientation"].length() == 0) {
            cRotation3D->euler_deg.y = cAuthorization->vec2Map["BladeOrientation"].x;
            cRotation3D->euler_deg.x = cAuthorization->vec2Map["BladeOrientation"].y;
            float pitch = cRotation3D->euler_deg.x * DEG2RAD;
            float yaw = cRotation3D->euler_deg.y * DEG2RAD;
            float roll = cRotation3D->euler_deg.z * DEG2RAD;

            // --- Rotate local blade normal ¡ú world ---
            Vec3 normal = cRacketPhysical->localNormal;

            // Yaw (Y)
            normal = {
                std::sin(yaw) * normal.z + std::cos(yaw) * normal.x,
                normal.y,
                std::cos(yaw) * normal.z - std::sin(yaw) * normal.x
            };

            // Pitch (X)
            normal = {
                normal.x,
                normal.y * std::cos(pitch) - normal.z * std::sin(pitch),
                normal.y * std::sin(pitch) + normal.z * std::cos(pitch)
            };
            cRacketPhysical->worldNormal = normal.normalized();

        }
        else { cRacketPhysical->worldNormal = cRacketPhysical->localNormal; }


        // --- DEBUG DRAW ---
        if (context->renderSettings.debugDrawBladeNormal) {
            Vec3 origin = cTransform3D->pos_m;
            float len = 0.30f;

            // Local normal (unrotated reference)
            Debug::queueArrow3D(
                origin,
                origin + cRacketPhysical->localNormal.normalized() * len,
                sf::Color::Yellow // Blue = local
            );

            // World normal (actual blade facing)
            Debug::queueArrow3D(
                origin,
                origin + cRacketPhysical->worldNormal * len,
                sf::Color::Cyan // Cyan = world
            );
        }
    }

    return { SystemExecResult::Ran };
}
