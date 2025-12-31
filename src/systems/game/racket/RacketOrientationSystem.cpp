#include "RacketOrientationSystem.hpp"

#include "math/Constants.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"

SystemExec RacketOrientationSystem::update(GameContext* context) {
    for (auto e : context->registry.getEntitiesWith<CAuthorization>()) {
        auto [cRacketHandle, cAuthorization] =
            context->registry.getComponents<CRacketHandle, CAuthorization>(e);

        Entity eRacket = cRacketHandle->racketEntity;

        auto [cRacketPhysical, cRotation3D, cTransform3D] =
            context->registry.getComponents<CRacketPhysical, CRotation3D, CTransform3D>(eRacket);

        auto it = cAuthorization->vec2Map.find("BladeOrientation");

        
        // Original logic: it checks if the map entry exists OR if the length is 0 (which was likely a bug)
        if (it != cAuthorization->vec2Map.end() || cAuthorization->vec2Map["BladeOrientation"].length() == 0) {
            
            // Map raw inputs directly to Euler angles
            cRotation3D->euler_deg.y = (cAuthorization->vec2Map["BladeOrientation"].x / 100) * context->physicsDebug.racketOrientation.max_euler_x; 
            cRotation3D->euler_deg.x = (cAuthorization->vec2Map["BladeOrientation"].y / 100) * context->physicsDebug.racketOrientation.max_euler_y;

            cRotation3D->pitch = cRotation3D->euler_deg.x * DEG2RAD;
            cRotation3D->yaw = cRotation3D->euler_deg.y * DEG2RAD;
            cRotation3D->roll = cRotation3D->euler_deg.z * DEG2RAD;


            // --- Rotate local blade normal -> world ---
            Vec3 normal = cRacketPhysical->localNormal;

            // Pass 1: cRotation3D->yaw (Y-axis rotation)
            normal = {
                std::sin(cRotation3D->yaw) * normal.z + std::cos(cRotation3D->yaw) * normal.x,
                normal.y,
                std::cos(cRotation3D->yaw) * normal.z - std::sin(cRotation3D->yaw) * normal.x
            };

            // Pass 2: cRotation3D->pitch (X-axis rotation)
            normal = {
                normal.x,
                normal.y * std::cos(cRotation3D->pitch) - normal.z * std::sin(cRotation3D->pitch),
                normal.y * std::sin(cRotation3D->pitch) + normal.z * std::cos(cRotation3D->pitch)
            };

            cRacketPhysical->worldNormal = normal.normalized();

        }
        else {
            // Default to local normal if no input
            cRacketPhysical->worldNormal = cRacketPhysical->localNormal;
        }

        // --- DEBUG DRAW ---
        if (context->renderSettings.debugDrawBladeNormal) {
            Vec3 origin = cTransform3D->pos_m;
            float len = 0.30f;

            // Yellow arrow = Local/Default orientation
            Debug::queueArrow3D(
                origin,
                origin + cRacketPhysical->localNormal.normalized() * len,
                sf::Color::Yellow
            );

            // Cyan arrow = The calculated worldNormal being used for physics
            Debug::queueArrow3D(
                origin,
                origin + cRacketPhysical->worldNormal * len,
                sf::Color::Cyan
            );
        }
    }

    return { SystemExecResult::Ran };
}