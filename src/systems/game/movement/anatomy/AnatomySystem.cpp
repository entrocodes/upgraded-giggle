#include "AnatomySystem.hpp"
#include "debug/Debug.hpp"
#include "components/Components.hpp"
SystemExec AnatomySystem::update(GameContext* context) {
    
    auto entities = context->registry.getEntitiesWith<CArm, CRacketHandle, CTransform3D>();

    for (auto e : entities) {
        auto [cArm, cRacketHandle, cTransform3D, cAuthorization, cRacketSwing] = context->registry.getComponents<CArm, CRacketHandle, CTransform3D, CAuthorization, CRacketSwing>(e);

        // --- 1. CALCULATE ACTUAL SHOULDER (With Twist & Crouch) ---
        float twistLoad = (cAuthorization->floatMap["TorsoRightLoad"] - cAuthorization->floatMap["TorsoLeftLoad"]) * 0.005f;
        float twistAngle = twistLoad * context->playerMovement.bodyMovement.twistFactor;

        float shoulderSideMult = cArm->isLeftHanded ? -1.0f : 1.0f;
        float shoulderRadius = 0.2f;

        Vec3 shoulderLocal(
            std::cos(twistAngle) * (shoulderRadius * shoulderSideMult),
            0.45f,
            std::sin(twistAngle) * (shoulderRadius * -shoulderSideMult) // Swapped for correct rotation
        );

        float maxLeanDistance = .15f;

        Vec3 reachVec = (cRacketSwing->swingTriggered) ? cRacketHandle->swingOffset_m : cRacketHandle->freeOffset_m;
        Vec3 bodyLean = reachVec * context->playerMovement.bodyMovement.leanIntensity;
        // CLAMP THE LEAN: Prevents "infinite" stretching
        if (bodyLean.length() > maxLeanDistance) {
            bodyLean = bodyLean.normalized() * maxLeanDistance;
        }
        if (reachVec.y < 0) {
            float rawCrouch = reachVec.y * context->playerMovement.bodyMovement.crouchIntensity;
            float minHeight = -0.35f;
            float crouch = std::max(rawCrouch, minHeight);

            cArm->shoulderPos_m = cTransform3D->pos_m + shoulderLocal + bodyLean;
            cArm->shoulderPos_m.y += crouch;
        }
        else {
            // Don't forget to set the position if they aren't crouching!
            cArm->shoulderPos_m = cTransform3D->pos_m + shoulderLocal + bodyLean;
        }
        // --- 2. SOLVE IK (Use the shared position) ---
        Vec3 racketPos = context->registry.getComponent<CTransform3D>(cRacketHandle->racketEntity)->pos_m;
        Vec3 towardRacket = racketPos - cArm->shoulderPos_m;
        float dist = towardRacket.length();

        // Clamp to arm length
        float totalLength = cArm->upperArmLength + cArm->lowerArmLength;
        if (dist > totalLength * 0.98f) {
            dist = totalLength * 0.98f;
            towardRacket = towardRacket.normalized() * dist;
        }

        // Law of Cosines
        float a = cArm->upperArmLength;
        float b = cArm->lowerArmLength;
        float c = dist;
        float cosAlpha = (a * a + c * c - b * b) / (2.0f * a * c);
        float alpha = std::acos(std::clamp(cosAlpha, -1.0f, 1.0f));

        // --- 3. STABLE ELBOW HINGE ---
        // Instead of cross(up), use a vector that points "out" from the ribs
        Vec3 ribsOut = cArm->isLeftHanded ? Vec3(-1, -0.5f, 0) : Vec3(1, -0.5f, 0);

        // Orthogonalize ribsOut to towardRacket to find the true hinge
        Vec3 binormal = towardRacket.cross(ribsOut).normalized();
        Vec3 hinge = binormal.cross(towardRacket).normalized();

        // Position Elbow
        cArm->elbowPos_m = cArm->shoulderPos_m +
            (towardRacket.normalized() * std::cos(alpha) * a) +
            (hinge * std::sin(alpha) * a);

        // --- DEBUG RENDERING ---
        if (context->renderSettings.debugDrawPlayerBody) {
            Debug::queueSphere3D(cArm->shoulderPos_m, 0.04f, sf::Color::Green);
            Debug::queueSphere3D(cArm->elbowPos_m, 0.03f, sf::Color::Yellow);
            Debug::queueLine3D(cArm->shoulderPos_m, cArm->elbowPos_m, sf::Color::Green);
            Debug::queueLine3D(cArm->elbowPos_m, racketPos, sf::Color::Green);
            Debug::queueSphere3D(cTransform3D->pos_m, 0.04f, sf::Color::Blue); // body core
            Debug::queueLine3D(cTransform3D->pos_m, cArm->shoulderPos_m, sf::Color(150, 150, 150)); //core to shoulder
        }
    }
    return { SystemExecResult::Ran };
}