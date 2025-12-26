#include "RacketHandleSystem.hpp"
#include "components/Components.hpp"
#include "math/Constants.hpp"
SystemExec RacketHandleSystem::update(GameContext* context) {
    auto* ePlayer = context->registry.getEntity("player");
    auto [cAuthorization, cPlayerHandle] = context->registry.getComponents<CAuthorization, CRacketHandle>(*ePlayer);

    auto eRacket = cPlayerHandle->racketEntity;
    auto [cRacketRacketPhysical, cRacketRacketRotation] = context->registry.getComponents<CRacketPhysical, CRotation3D>(eRacket);

    // Map J2 to Euler angles for high skill ceiling
    float pitch = cAuthorization->vec2Map["BladeOrientation"].x * 70.f; // Tilt down/up
    float yaw = cAuthorization->vec2Map["BladeOrientatioin"].y * 50.f; // Angling cross-court

    cRacketRacketRotation->euler_deg = Vec3(pitch, yaw, 0.f);

    // Convert to Physical Normal for CollisionSystem
    float p = pitch * DEG2RAD;
    float y = yaw * DEG2RAD;

    cRacketRacketPhysical->normal = Vec3(
        std::sin(y) * std::cos(p),
        -std::sin(p),
        std::cos(y) * std::cos(p)
    ).normalized();

    return { SystemExecResult::Ran };
}