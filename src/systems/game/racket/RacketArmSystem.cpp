#include "RacketArmSystem.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"
#include <cmath>
#include <algorithm>

SystemExec RacketArmSystem::update(GameContext* context) {
    auto* ePlayer = context->registry.getEntity("player");
    auto [cAuth, cArm, cHandle, cPos, cSwing] =
        context->registry.getComponents<CAuthorization, CArm, CRacketHandle, CTransform3D, CRacketSwing>(*ePlayer);

    auto eRacket = cHandle->racketEntity;
    auto [cRacketTransform, cRacketVel] = context->registry.getComponents<CTransform3D, CVelocity3D>(eRacket);

    float dt = context->frameStats.dt;
    Vec3 lastPos = cRacketTransform->pos_m;

    // Body Lean: Moves the anchor based on where the player is reaching
    Vec3 reachVec = (cSwing->swingTriggered) ? cHandle->swingOffset_m : cHandle->freeOffset_m;
    Vec3 bodyLean = reachVec * context->playerMovement.bodyMovement.leanIntensity;

    // --- 2. REACH & QUALITY CONSTRAINTS ---
    float currentReachDist = reachVec.length();
    float reachRatio = std::clamp(currentReachDist / cArm->maxReach_m, 0.0f, 1.1f);

    // Quality mapping: Penalize power/control at the edge of reach
    if (reachRatio < 0.85f) {
        cHandle->currentStrokeQuality = 1.0f;
    }
    else {
        float penalty = (reachRatio - 0.85f) / 0.15f;
        cHandle->currentStrokeQuality = std::clamp(1.0f - (penalty * 0.9f), 0.1f, 1.0f);
    }

    // --- FINAL POSITIONING ---
    Vec3 targetOffset = (cHandle->swingOffset_m * cHandle->strokeWeight) +
        (cHandle->freeOffset_m * (1.0f - cHandle->strokeWeight));

    // Final Hard Constraint: Arm cannot physically leave the socket
    if (targetOffset.length() > cArm->maxReach_m) {
        targetOffset = targetOffset.normalized() * cArm->maxReach_m;
    }

    Vec3 finalPos = cArm->shoulderPos_m + targetOffset;

    // --- 4. PHYSICS COMMIT ---
    cRacketTransform->lastPos_m = lastPos;
    cRacketTransform->pos_m = finalPos;

    // Velocity calculation (Smoothed for collision detection)
    Vec3 instantVel = (finalPos - lastPos) / dt;
    cRacketVel->vel_mps = (instantVel * 0.5f) + (cRacketVel->vel_mps * 0.5f);


    if (cSwing->isCharging || cSwing->swingTriggered) {
        Debug::queueLine3D(cArm->shoulderPos_m, finalPos, sf::Color::Yellow);
    }


    return { SystemExecResult::Ran };
}