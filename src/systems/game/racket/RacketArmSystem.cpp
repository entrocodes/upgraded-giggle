#include "RacketArmSystem.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"
SystemExec RacketArmSystem::update(GameContext* context) {
    auto* ePlayer = context->registry.getEntity("player");
    auto [cPlayerInput, cPlayerArm, cPlayerHandle, cPlayerPos, cPlayerSwing] =
        context->registry.getComponents<CInput, CArm, CRacketHandle, CTransform3D, CRacketSwing>(*ePlayer);

    auto eRacket = cPlayerHandle->racketEntity;
    auto [cRacketTransform3D, cRacketVelocity3D] = context->registry.getComponents<CTransform3D, CVelocity3D>(eRacket);

    float dt = context->frameStats.dt;
    Vec3 lastPos = cRacketTransform3D->pos_m;

    // 1. Shoulder follows player + Waist Rotation (RT)
    // As RT increases, shoulder pulls back and slightly right
    float twist = cPlayerSwing->torsoLoad * 0.7f; // ~40 degrees
    Vec3 localShoulderOffset(std::cos(twist) * 0.2f, 0.45f, -std::sin(twist) * 0.2f);
    cPlayerArm->shoulderPos_m = cPlayerPos->pos_m + localShoulderOffset;

    // 2. Free Move (J1)
    if (cPlayerHandle->strokeWeight < 0.99f) {
        // SDL: J1Y is negative when pushing stick UP. 
        Vec3 delta(cPlayerInput->axes["J1X"], -cPlayerInput->axes["J1Y"], 0.f);
        cPlayerHandle->freeOffset_m += delta * 3.0f * dt;
    }

    // 3. Reach Constraint
    if (cPlayerHandle->freeOffset_m.length() > cPlayerArm->maxReach_m) {
        cPlayerHandle->freeOffset_m = cPlayerHandle->freeOffset_m.normalized() * cPlayerArm->maxReach_m;
    }

    // 4. Refined Stroke Blending
    // We blend the POSITION, but add the swing offset on top
    Vec3 anchorPos = cPlayerArm->shoulderPos_m + cPlayerHandle->freeOffset_m;

    // finalPos starts at the anchor, then follows the swingOffset arc
    Vec3 finalPos = anchorPos + (cPlayerHandle->swingOffset_m * cPlayerHandle->strokeWeight);

    // 5. Physics Commit
    cRacketTransform3D->lastPos_m = lastPos;
    cRacketTransform3D->pos_m = finalPos;
    // Inside RacketArmSystem.cpp - Step 5
    Vec3 instantVel = (finalPos - lastPos) / dt;
    // Blend 40% new velocity, 60% old velocity to smooth out input noise
    cRacketVelocity3D->vel_mps = (instantVel * 0.4f) + (cRacketVelocity3D->vel_mps * 0.6f);
    
    // --- DEBUG VISUALIZATION SYNC ---
    if (context->renderSettings.debugDrawArmLine) {
        Debug::queueLine3D(cPlayerArm->shoulderPos_m, cRacketTransform3D->pos_m, sf::Color::White);
    }

    if (context->renderSettings.debugDrawBladeNormal) {
        auto* cRacketPhys = context->registry.getComponent<CRacketPhysical>(eRacket);
        if (cRacketPhys) {
            Debug::queueArrow3D(cRacketTransform3D->pos_m,
                cRacketTransform3D->pos_m + cRacketPhys->normal * 0.3f,
                sf::Color::Yellow);
        }
    }

    if (context->renderSettings.debugDrawTorsoIndicator) {
        if (cPlayerSwing->torsoLoad > 0.01f) {
            sf::Color loadColor = sf::Color(255, 255 - (uint8_t)(cPlayerSwing->torsoLoad * 255), 0);
            Debug::queueSphere3D(cPlayerPos->pos_m + Vec3(0, 1.0f, 0),
                cPlayerSwing->torsoLoad * 0.15f, loadColor);
        }
    }
    return { SystemExecResult::Ran };
}