#include "PoseArmConstraintSetterSystem.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"

SystemExec PoseArmConstraintSetterSystem::update(GameContext* context) {
    for (auto [e, cPose] : context->registry.getEntitiesWithComponents<CPose>()) {
        auto& pose = cPose->pose;
        auto armState = pose.armState;
        //TODO::Righty settings
        auto& elbow = pose.leftElbow();
        auto& shoulder = pose.leftShoulder();
        if (armState == ArmState::FreeMove) {
            elbow.configure(Vec3(-0.12f, -0.12f, -0.12f), Vec3(.12f, 0.12f, 0.12f), 0.01f, 0.6f);
            shoulder.configure(Vec3(-0.15f, -0.8f, -0.9f), Vec3(.25f, 0.8f, 0.9f), 0.04f, 0.4f);
        }
    }
    return { SystemExecResult::Ran };
}
