#include "RacketSwingIKTargetSystem.hpp"
#include "components/Components.hpp"
#include "math/MathHelpers.hpp"
#include "debug/Debug.hpp"

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
        PoseJoint& el = pose.leftElbow();
        PoseJoint& wr = pose.leftWrist();
        PoseJoint& pelvis = pose.centerPelvis();

        float progress = 0.0f;
        if (strokeState == StrokeState::Swing) {
            float finish = -cRacketSwing->backswingTorsoRotation * 1.35f;
            if (finish > 1e-6f)
                progress = std::clamp(cRacketSwing->forwardTorsoRotation / finish, 0.0f, 1.0f);
        }
        else if (strokeState == StrokeState::SwingRecovery) {
            progress = 1.0f; // hold at contact position
        }

        Vec3 backswingLocal = { -0.3f,  0.0f, -0.2f };
        Vec3 contactLocal = { 0.3f,  1.0f,  0.5f };

        Vec3 targetLocal = {
            backswingLocal.x + (contactLocal.x - backswingLocal.x) * progress,
            backswingLocal.y + (contactLocal.y - backswingLocal.y) * progress,
            backswingLocal.z + (contactLocal.z - backswingLocal.z) * progress,
        };

        Vec3 targetWorld = MathHelpers::rotateByEuler(targetLocal, pelvis.rotWorld_rad);
        targetWorld += pelvis.pos_m;

        Vec3 wristBindWorldPos = el.lastPos_m +
            MathHelpers::compMul(wr.baseOffset_m + wr.restOffset_m, pose.scale);

        wr.targetOffsetFromBind = targetWorld - wristBindWorldPos;
        wr.ikTargetActive = true;

        Debug::queueSphere3D(targetWorld, 0.04f, sf::Color::Green);
    }
    return { SystemExecResult::Ran };
}