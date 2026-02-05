#include "RacketFreeMoveTargetSolverSystem.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"

SystemExec RacketFreeMoveTargetSolverSystem::update(GameContext* context) {
    for (auto [e, cPose] : context->registry.getEntitiesWithComponents<CPose>()) {
        auto& pose = cPose->pose;
        auto& wrist = pose.leftWrist();
        auto& desiredOffset = wrist.desiredDeltaOffset_m;
        float constraintY = .25;
        float constraintX = .75;
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
        Debug::debugPrint("Desired Offset", desiredOffset);
    }
    return { SystemExecResult::Ran };
}
