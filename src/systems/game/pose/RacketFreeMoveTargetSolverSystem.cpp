#include "RacketFreeMoveTargetSolverSystem.hpp"
#include "components/Components.hpp"
#include "math/MathHelpers.hpp"
#include "debug/Debug.hpp"

SystemExec RacketFreeMoveTargetSolverSystem::update(GameContext* context) {
    for (auto [e, cPose, cRacketHandle] : context->registry.getEntitiesWithComponents<CPose, CRacketHandle>()) {
        auto& pose = cPose->pose;
        auto& wrist = pose.leftWrist();
        auto& pelvis = pose.centerPelvis();

        Entity eRacket = cRacketHandle->racketEntity;
        auto cRacketSwing = context->registry.getComponent<CRacketSwing>(eRacket);
        if (!cRacketSwing) continue;

        StrokeState& strokeState = cRacketSwing->strokeState;
        if (strokeState == StrokeState::Swing ||
            strokeState == StrokeState::SwingRecovery ||
            strokeState == StrokeState::Backswing ||
            strokeState == StrokeState::BrakedBackswing) continue;

        wrist.targetOffsetFromBind += cRacketHandle->freeOffset_m;

        // Clamp total
        auto& off = wrist.targetOffsetFromBind;

        float posConstraintY = context->physicsDebug.freeMoveConstraints.posConstraintY;
        float negConstraintY = context->physicsDebug.freeMoveConstraints.negConstraintY;
        float posConstraintX = context->physicsDebug.freeMoveConstraints.posConstraintX;
        float negConstraintX = context->physicsDebug.freeMoveConstraints.negConstraintX;
        float posConstraintZ = context->physicsDebug.freeMoveConstraints.posConstraintZ;
        float negConstraintZ = context->physicsDebug.freeMoveConstraints.negConstraintZ;

        if (off.x < negConstraintX) off.x = negConstraintX;
        else if (off.x > posConstraintX) off.x = posConstraintX;
        if (off.y < negConstraintY) off.y = negConstraintY;
        else if (off.y > posConstraintY) off.y = posConstraintY;
        if (off.z < negConstraintZ) off.z = negConstraintZ;
        else if (off.z > posConstraintZ) off.z = posConstraintZ;

        // Persist clamped offset for next frame
        cRacketHandle->freeOffset_m = off;

        // Compute stable world target anchored to PELVIS, not to arm bind positions.
        // This means ikTargetWorldPos moves with the player's body but is completely
        wrist.ikTargetWorldPos = pelvis.pos_m +
            MathHelpers::rotateByEuler(off, pelvis.rotWorld_rad);

        wrist.ikTargetActive = true;

        Debug::event(Debug::Channel::Pose, "Desired Racket Offset", {}, {
            { "Offset",           off },
            { "ikTargetWorldPos", wrist.ikTargetWorldPos },
            { "Racket Pos",       wrist.pos_m }
            });
    }
    return { SystemExecResult::Ran };
}