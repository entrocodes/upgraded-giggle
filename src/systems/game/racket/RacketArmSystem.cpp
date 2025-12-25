#include "RacketArmSystem.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"
#include <cmath>
#include <algorithm>

SystemExec RacketArmSystem::update(GameContext* context) {
    auto* ePlayer = context->registry.getEntity("player");
    auto [cPlayerInput, cPlayerArm, cPlayerHandle, cPlayerPos, cPlayerSwing] =
        context->registry.getComponents<CInput, CArm, CRacketHandle, CTransform3D, CRacketSwing>(*ePlayer);

    auto eRacket = cPlayerHandle->racketEntity;
    auto [cRacketTransform3D, cRacketVelocity3D] = context->registry.getComponents<CTransform3D, CVelocity3D>(eRacket);

    float dt = context->frameStats.dt;
    Vec3 lastPos = cRacketTransform3D->pos_m;

    // --- 1. SHOULDER & BODY KINEMATICS ---
    // Reactive Body Lean: Shoulder follows player + Torso Twist
    float twist = cPlayerSwing->torsoLeftLoad * -context->playerMovement.bodyMovement.twistFactor;

    // Base shoulder position (Locking to the left side for your current setup)
    Vec3 baseShoulderLocal(std::cos(twist) * -0.2f, 0.45f, -std::sin(twist) * -0.2f);

    // Body Lean: Contributes to reach by moving the shoulder anchor toward J1 intent
    Vec3 bodyLean = cPlayerHandle->freeOffset_m * context->playerMovement.bodyMovement.leanIntensity;
    float crouch = (cPlayerHandle->freeOffset_m.y < 0) ?
        cPlayerHandle->freeOffset_m.y * context->playerMovement.bodyMovement.crouchIntensity : 0.f;

    cPlayerArm->shoulderPos_m = cPlayerPos->pos_m + baseShoulderLocal + bodyLean;
    cPlayerArm->shoulderPos_m.y += crouch;

    // --- 2. OFFSET MANAGEMENT (J1) ---
    // Handle Free Move (Neutral state)
    if (cPlayerHandle->strokeWeight < 0.99f) {
        Vec3 delta(cPlayerInput->axes["J1X"], -cPlayerInput->axes["J1Y"], 0.f);
        cPlayerHandle->freeOffset_m += delta * 3.0f * dt;
    }

    // Reach Constraint: Keep the arm from detaching from the shoulder
    if (cPlayerHandle->freeOffset_m.length() > cPlayerArm->maxReach_m) {
        cPlayerHandle->freeOffset_m = cPlayerHandle->freeOffset_m.normalized() * cPlayerArm->maxReach_m;
    }

    // --- 3. THE ABILITY INTERPRETER (Stroke Quality) ---
    // Calculate how "stretched" the arm is to penalize power
    float currentExtension = (cPlayerSwing->swingTriggered) ?
        cPlayerHandle->swingOffset_m.length() :
        cPlayerHandle->freeOffset_m.length();

    float reachRatio = currentExtension / cPlayerArm->maxReach_m;

    // Quality mapping: 1.0 (perfect) until 85% reach, then drops to 0.1 at 100% reach
    if (reachRatio < 0.85f) {
        cPlayerHandle->currentStrokeQuality = 1.0f;
    }
    else {
        float penalty = (reachRatio - 0.85f) / 0.15f;
        cPlayerHandle->currentStrokeQuality = std::clamp(1.0f - (penalty * 0.9f), 0.1f, 1.0f);
    }

    // --- 4. FINAL POSITION BLENDING ---
    // anchorPos is where the racket is in "Neutral/Preparation"
    Vec3 anchorPos = cPlayerArm->shoulderPos_m + cPlayerHandle->freeOffset_m;

    // finalPos blends the ballistic swing offset into the anchor
    Vec3 finalPos = anchorPos + (cPlayerHandle->swingOffset_m * cPlayerHandle->strokeWeight);

    // --- 5. PHYSICS COMMIT ---
    cRacketTransform3D->lastPos_m = lastPos;
    cRacketTransform3D->pos_m = finalPos;

    // Emergent Velocity: Calculated from movement, smoothed for the Collision System
    Vec3 instantVel = (finalPos - lastPos) / dt;
    cRacketVelocity3D->vel_mps = (instantVel * 0.4f) + (cRacketVelocity3D->vel_mps * 0.6f);

    // --- 6. DEBUG VISUALS ---
    if (context->renderSettings.debugDrawPlayerBody) {
        // Body Structure (Blue base, Green shoulder, Grey connector)
        Debug::queueSphere3D(cPlayerPos->pos_m, 0.05f, sf::Color::Blue);
        Debug::queueSphere3D(cPlayerArm->shoulderPos_m, 0.05f, sf::Color::Green);
        Debug::queueLine3D(cPlayerPos->pos_m, cPlayerArm->shoulderPos_m, sf::Color(100, 100, 100));

        // Arm Quality Visualization (Green -> Red)
        sf::Color qualityColor(
            static_cast<uint8_t>((1.0f - cPlayerHandle->currentStrokeQuality) * 255),
            static_cast<uint8_t>(cPlayerHandle->currentStrokeQuality * 255),
            0, 255
        );
        Debug::queueLine3D(cPlayerArm->shoulderPos_m, cRacketTransform3D->pos_m, qualityColor);
    }

    // Blade Normal (J2 Orientation)
    if (context->renderSettings.debugDrawBladeNormal) {
        auto* cRacketPhys = context->registry.getComponent<CRacketPhysical>(eRacket);
        if (cRacketPhys) {
            Debug::queueArrow3D(cRacketTransform3D->pos_m,
                cRacketTransform3D->pos_m + cRacketPhys->normal * 0.3f,
                sf::Color::Red);
        }
    }

    // Predicted Arc Path (Cyan)
    if (context->renderSettings.debugDrawPlayerBody && (cPlayerSwing->isCharging || cPlayerSwing->swingTriggered)) {
        Vec3 prevStep = cPlayerArm->shoulderPos_m + cPlayerHandle->arcStartPoint;
        Vec3 j1Steer(cPlayerInput->axes["J1X"], -cPlayerInput->axes["J1Y"], 0.f);

        for (int i = 1; i <= 10; ++i) {
            float simMs = i * 25.0f;
            Vec3 simVel(0, 0, cPlayerSwing->swingSpeed);
            if (simMs < 80.f) simVel += j1Steer * 4.0f;
            else if (simMs < 180.f) simVel += j1Steer * 0.8f;

            Vec3 nextStep = prevStep + (simVel * 0.025f);
            Debug::queueLine3D(prevStep, nextStep, sf::Color(0, 255, 255, 150));
            prevStep = nextStep;
        }
    }

    return { SystemExecResult::Ran };
}