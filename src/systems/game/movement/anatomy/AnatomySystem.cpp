#include "AnatomySystem.hpp"
#include "debug/Debug.hpp"
#include "components/Components.hpp"
#include <algorithm>
#include <cmath>

SystemExec AnatomySystem::update(GameContext* context) {
    auto entities = context->registry.getEntitiesWith<CArm, CRacketHandle, CTransform3D>();

    for (auto e : entities) {
        auto [cArm, cRacketHandle, cTransform3D, cAuthorization] = context->registry.getComponents<CArm, CRacketHandle, CTransform3D, CAuthorization>(e);

        Entity eRacket = cRacketHandle->racketEntity;
        auto [cRacketSwing] = context->registry.getComponents<CRacketSwing>(eRacket);

        float twistLoad = (cAuthorization->floatMap["TorsoRightLoad"] - cAuthorization->floatMap["TorsoLeftLoad"]) * 0.005f;
        float twistAngle = twistLoad * context->playerMovement.bodyMovement.twistFactor;

        float shoulderSideMult = cArm->isLeftHanded ? -1.0f : 1.0f;
        float shoulderRadius = 0.2f;

        Vec3 shoulderLocal(
            std::cos(twistAngle) * (shoulderRadius * shoulderSideMult),
            0.45f,
            std::sin(twistAngle) * (shoulderRadius * -shoulderSideMult)
        );

        Vec3 baseOffset = cRacketHandle->freeOffset_m + cRacketHandle->pushOffset_m;
        float w = std::clamp(cRacketHandle->strokeWeight, 0.0f, 1.0f);
        Vec3 reachVec = baseOffset + cRacketSwing->swingDelta_m * w;

        Vec3 bodyLean = reachVec * context->playerMovement.bodyMovement.leanIntensity;

        float maxLeanDistance = 0.15f;
        if (bodyLean.length() > maxLeanDistance) bodyLean = bodyLean.normalized() * maxLeanDistance;

        cArm->shoulderPos_m = cTransform3D->pos_m + shoulderLocal + bodyLean;

        if (reachVec.y < 0.0f) {
            float rawCrouch = reachVec.y * context->playerMovement.bodyMovement.crouchIntensity;
            float minHeight = -0.35f;
            float crouch = std::max(rawCrouch, minHeight);
            cArm->shoulderPos_m.y += crouch;
        }

        auto cRacketT3D = context->registry.getComponent<CTransform3D>(cRacketHandle->racketEntity);
        Vec3 racketPos = cRacketT3D ? cRacketT3D->pos_m : Vec3{ 0,0,0 };

        Vec3 towardRacket = racketPos - cArm->shoulderPos_m;
        float dist = towardRacket.length();

        float totalLength = cArm->upperArmLength + cArm->lowerArmLength;
        if (dist > totalLength * 0.98f) {
            dist = totalLength * 0.98f;
            towardRacket = towardRacket.normalized() * dist;
        }

        float a = cArm->upperArmLength;
        float b = cArm->lowerArmLength;
        float c = dist;

        float cosAlpha = (a * a + c * c - b * b) / (2.0f * a * c);
        float alpha = std::acos(std::clamp(cosAlpha, -1.0f, 1.0f));

        Vec3 ribsOut = cArm->isLeftHanded ? Vec3(-1, -0.5f, 0) : Vec3(1, -0.5f, 0);

        Vec3 binormal = towardRacket.cross(ribsOut).normalized();
        Vec3 hinge = binormal.cross(towardRacket).normalized();

        cArm->elbowPos_m = cArm->shoulderPos_m + (towardRacket.normalized() * std::cos(alpha) * a) + (hinge * std::sin(alpha) * a);

        if (context->renderSettings.debugDrawPlayerBody) {
            Debug::queueSphere3D(cArm->shoulderPos_m, 0.04f, sf::Color::Green);
            Debug::queueSphere3D(cArm->elbowPos_m, 0.03f, sf::Color::Yellow);
            Debug::queueLine3D(cArm->shoulderPos_m, cArm->elbowPos_m, sf::Color::Green);
            Debug::queueLine3D(cArm->elbowPos_m, racketPos, sf::Color::Green);
            Debug::queueSphere3D(cTransform3D->pos_m, 0.04f, sf::Color::Blue);
            Debug::queueLine3D(cTransform3D->pos_m, cArm->shoulderPos_m, sf::Color(150, 150, 150));
        }
    }

    return { SystemExecResult::Ran };
}
