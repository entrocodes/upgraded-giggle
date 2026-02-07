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
            elbow.configure(Vec3(-0.12f, -0.12f, -0.6f), Vec3(.12f, 0.12f, 1.8f), 0.01f, 0.6f);
            shoulder.configure(Vec3(-0.15f, -0.8f, -0.15f), Vec3(.25f, 0.8f, 1.2f), 0.04f, 0.4f);
        }
        else if (armState == ArmState::Push) {
            elbow.configure(Vec3(-0.15f, -0.4f, -0.6f), Vec3(.12f, 0.4f, 0.12f), 0.01f, 0.6f);
            shoulder.configure(Vec3(-0.15f, -0.8f, -0.6f), Vec3(.25f, 0.8f, 0.9f), 0.04f, 0.4f);
        }
        else if (armState == ArmState::Backswing) {
            elbow.configure(Vec3(-1.2f, -1.2f, -1.2f), Vec3(1.2f, 1.2f, 1.2f), 0.01f, 0.6f);
            shoulder.configure(Vec3(-1.2f, -1.2f, -1.2f), Vec3(1.2f, 1.2f, 1.2f), 0.04f, 0.4f);
        }
    }
    return { SystemExecResult::Ran };
}
