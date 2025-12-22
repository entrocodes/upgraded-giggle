#include "ProceduralAnimationSystem.hpp"

#include "components/Components.hpp"
#include "math/Constants.hpp"
SystemExec ProceduralAnimationSystem::update(GameContext* context) {
    auto ePlayer = context->registry.getEntity("player");
    auto [cPlayerRotation, cPlayerTransform, cPlayerRacketHandle] = context->registry.getComponents<CRotation3D, CTransform, CRacketHandle>(*ePlayer);

    //if (cPlayerRotation && cPlayerTransform) {
    //    cPlayerTransform->rotation = cPlayerRotation->euler_deg.y;

    //    float twistRad = cPlayerRotation->euler_deg.y * DEG2RAD;
    //    cPlayerTransform->scale.x = std::cos(twistRad);

    //}
    auto eRacket = cPlayerRacketHandle->racketEntity;
    auto* cRacketRotation = context->registry.getComponent<CRotation3D>(eRacket);
    auto* cRacketTransform = context->registry.getComponent<CTransform>(eRacket);

    if (cRacketRotation && cRacketTransform) {
        // 1. J2 X (Yaw) rotates the sprite normally
        cRacketTransform->rotation = cRacketRotation->euler_deg.y;

        // 2. J2 Y (Pitch/Tilt) "squashes" the sprite vertically.
        // If the blade is perfectly closed (flat), the height should be near 0.
        float pitchRad = cRacketRotation->euler_deg.x * DEG2RAD;
        float heightScale = std::abs(std::cos(pitchRad));

        // Clamp so it doesn't disappear entirely
        cRacketTransform->scale.y = std::max(heightScale, 0.15f);

        // 3. Flip the sprite if the pitch goes past 90 (showing the back of the rubber)
        if (std::cos(pitchRad) < 0) {
            cRacketTransform->scale.y *= -1.0f; // Shows the "back" side
        }
    }
    return { SystemExecResult::Ran };
}