#include "RacketSwingIKTargetSystem.hpp"
#include "components/Components.hpp"
#include "math/MathHelpers.hpp"
#include "debug/Debug.hpp"
#include <algorithm>

SystemExec RacketSwingIKTargetSystem::update(GameContext* context) {
    for (auto [entity, cPose, cRacketHandle] : context->registry.getEntitiesWithComponents<CPose, CRacketHandle>()) {
        Entity eRacket = cRacketHandle->racketEntity;
        auto cRacketSwing = context->registry.getComponent<CRacketSwing>(eRacket);
        if (!cRacketSwing) continue;

        StrokeState& strokeState = cRacketSwing->strokeState;
        if (strokeState != StrokeState::Swing &&
            strokeState != StrokeState::SwingRecovery &&
            strokeState != StrokeState::Backswing &&
            strokeState != StrokeState::BrakedBackswing) continue;

        Pose& pose = cPose->pose;
        PoseJoint& sh = pose.leftShoulder();
        PoseJoint& wr = pose.leftWrist();
        PoseJoint& pelvis = pose.centerPelvis();

        float progress = 0.0f;
        // Shoulder-local offsets — tune for stroke feel.
        // Distances from shoulder at backswing ~0.18m, contact ~0.25m.
        Vec3 backswingShLocal = { -0.1f, -0.16f, -0.08f };
        Vec3 contactShLocal = { 0.05f,  0.08f,  0.2f };
        Vec3 targetShLocal = {
            backswingShLocal.x + (contactShLocal.x - backswingShLocal.x) * progress,
            backswingShLocal.y + (contactShLocal.y - backswingShLocal.y) * progress,
            backswingShLocal.z + (contactShLocal.z - backswingShLocal.z) * progress,
        };
        if (strokeState == StrokeState::Swing) {
            float finish = -cRacketSwing->backswingTorsoRotation * 1.35f;
            if (finish > 1e-6f)
                progress = std::clamp(cRacketSwing->forwardTorsoRotation / finish, 0.0f, 1.0f);
        }
        else if (strokeState == StrokeState::SwingRecovery) {
            // Don't retrace through backswing — blend from contact to neutral ready pose
            float finishRot = -cRacketSwing->backswingTorsoRotation * 1.35f;
            float unwindProgress = (finishRot > 1e-6f)
                ? std::clamp(cRacketSwing->extraTorsoRotation / finishRot, 0.0f, 1.0f)
                : 0.0f;

            // Neutral ready: arm relaxed in front, racket up
            Vec3 readyShLocal = { -0.05f, -0.05f, 0.08f };

            // Blend from contact toward ready as torso unwinds
            targetShLocal = {
                contactShLocal.x + (readyShLocal.x - contactShLocal.x) * (1.0f - unwindProgress),
                contactShLocal.y + (readyShLocal.y - contactShLocal.y) * (1.0f - unwindProgress),
                contactShLocal.z + (readyShLocal.z - contactShLocal.z) * (1.0f - unwindProgress),
            };
        }


        // Rotate by pelvis (not shoulder) so the target tracks torso rotation
        // but is independent of arm joint rest accumulation.
        Vec3 targetWorld = MathHelpers::rotateByEuler(targetShLocal, pelvis.rotWorld_rad);
        targetWorld += sh.pos_m;

        // Store as stable world target
        wr.ikTargetWorldPos = targetWorld;
        wr.ikTargetActive = true;

        Debug::queueSphere3D(targetWorld, 0.04f, sf::Color::Green);
        Debug::queueSphere3D(sh.pos_m, 0.02f, sf::Color::Cyan);
        Debug::queueSphere3D(
            sh.pos_m + MathHelpers::rotateByEuler(contactShLocal, pelvis.rotWorld_rad),
            0.04f, sf::Color::Blue);
        Debug::queueSphere3D(
            sh.pos_m + MathHelpers::rotateByEuler(backswingShLocal, pelvis.rotWorld_rad),
            0.04f, sf::Color::Yellow);
    }
    return { SystemExecResult::Ran };
}