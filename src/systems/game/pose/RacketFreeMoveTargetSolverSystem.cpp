#include "RacketFreeMoveTargetSolverSystem.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"

SystemExec RacketFreeMoveTargetSolverSystem::update(GameContext* context) {
    for (auto [e, cPose] : context->registry.getEntitiesWithComponents<CPose>()) {
        auto& pose = cPose->pose;
        auto& wrist = pose.leftWrist();
        auto& desiredOffset = wrist.targetOffsetFromBind;
        float constraintY = .2;
        float constraintX = .3;
        float constraintZ = 3;
        float negConstraintZ = -.05; //IMPORTANT:: This is what determines how far back the desired wrist offset can be
        if (desiredOffset.x < -constraintX) {
            desiredOffset.x = -constraintX;
        }
        else if (desiredOffset.x > constraintX) {
            desiredOffset.x = constraintX;
        }
        if (desiredOffset.y < -constraintY) {
            desiredOffset.y = -constraintY;
        }
        else if (desiredOffset.y > constraintY) {
            desiredOffset.y = constraintY;
        }
        if (desiredOffset.z < negConstraintZ) {
            desiredOffset.z = negConstraintZ;
        }
        else if (desiredOffset.z > constraintZ) {
            desiredOffset.z = constraintZ;
        }
        Debug::event(Debug::Channel::Pose, "Desired Racket Offset", {}, { { "Offset", desiredOffset }, {"Current Racket Position", wrist.pos_m} });
    }
    return { SystemExecResult::Ran };
}
