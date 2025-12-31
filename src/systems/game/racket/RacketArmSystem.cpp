#include "RacketArmSystem.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"
#include <cmath>
#include <algorithm>

SystemExec RacketArmSystem::update(GameContext* context) {
    for (auto entity : context->registry.getEntitiesWith<CAuthorization, CArm, CRacketHandle, CTransform3D, CRacketSwing>()) {
        auto [cAuth, cArm, cHandle, cPos, cSwing] = context->registry.getComponents<CAuthorization, CArm, CRacketHandle, CTransform3D, CRacketSwing>(entity);

        auto eRacket = cHandle->racketEntity;
        auto [cRacketPhysical, cRacketTransform3D, cRacketVel, cRacketBoundingBox3D] = context->registry.getComponents<CRacketPhysical, CTransform3D, CVelocity3D, CBoundingBox3D>(eRacket);

        float dt = context->frameStats.dt;
        Vec3 lastPos = cRacketTransform3D->pos_m;

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
            (cHandle->freeOffset_m * (1.0f - cHandle->strokeWeight)) + cHandle->pushOffset_m; //consider reworking this logic later

        // Final Hard Constraint: Arm cannot physically leave the socket
        if (targetOffset.length() > cArm->maxReach_m) {
            targetOffset = targetOffset.normalized() * cArm->maxReach_m;
        }

        Vec3 finalPos = cArm->shoulderPos_m + targetOffset;

        // --- 4. PHYSICS COMMIT ---
        cRacketTransform3D->lastPos_m = lastPos;
        cRacketTransform3D->pos_m = finalPos;

        // Velocity calculation (Smoothed for collision detection)
        Vec3 instantVel = (finalPos - lastPos) / dt;
        cRacketVel->vel_mps = (instantVel * 0.5f) + (cRacketVel->vel_mps * 0.5f);


        if (cSwing->strokeState == StrokeState::Swing) {
            Debug::queueLine3D(cArm->shoulderPos_m, finalPos, sf::Color::Yellow);
        }
        // Shadow
        auto eTable = context->registry.getEntity("table");
        auto cTableBox3D = context->registry.getComponent<CBoundingBox3D>(*eTable);
        // Check if the racket is within the table boundaries (X and Z plane)
        bool xOverlap = (cRacketBoundingBox3D->box.min.x <= cTableBox3D->box.max.x) &&
            (cRacketBoundingBox3D->box.max.x >= cTableBox3D->box.min.x);

        bool zOverlap = (cRacketBoundingBox3D->box.min.z <= cTableBox3D->box.max.z) &&
            (cRacketBoundingBox3D->box.max.z >= cTableBox3D->box.min.z);

        // If it overlaps both X and Z, it is OVER the table. 
        // If it doesn't, it is OFF the table.
        cRacketPhysical->offTable = !(xOverlap && zOverlap);
        // --- SHADOW ---

        auto tp = context->tableParameters;
        auto p = cRacketTransform3D->pos_m;
        Entity eRacketShadow = cRacketPhysical->racketShadow;
        auto [cRacketShadowTransform, cRacketShadowTransform3D] =
            context->registry.getComponents<CTransform, CTransform3D>(eRacketShadow);

        if (cRacketShadowTransform && cRacketShadowTransform3D) {
            cRacketShadowTransform3D->lastPos_m = cRacketShadowTransform3D->pos_m;
            cRacketShadowTransform3D->lastScale_m = cRacketShadowTransform3D->scale_m;

            float shadowY = cRacketPhysical->offTable ? tp.floorY : tp.tableY;
            cRacketShadowTransform3D->pos_m = Vec3(p.x, shadowY, p.z);

            float heightAboveSurface = p.y - shadowY;
            float scale = std::max(0.5f, 1.5f - 0.2f * heightAboveSurface);
            cRacketShadowTransform3D->scale_m = { scale, 1.f, scale };
        }
    }
        


    return { SystemExecResult::Ran };
}