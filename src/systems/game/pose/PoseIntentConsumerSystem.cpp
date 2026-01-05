#include "PoseIntentConsumerSystem.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"

SystemExec PoseIntentConsumerSystem::update(GameContext* context) {
    for (auto [e, cPose, cBuffer] : context->registry.getEntitiesWithComponents<CPose, CPoseIntentBuffer>()) {
        Pose& pose = cPose->pose;

        // 1. Clear deltas
        pose.forEachJoint([](PoseJoint& j, PoseJointID) {
            j.deltaOffset_m = { 0,0,0 };
            j.deltaRotation_rad = { 0,0,0 };
            });

        // 2. Sort intents by priority (high → low)
        std::sort(cBuffer->intents.begin(), cBuffer->intents.end(),
            [](const PoseIntent& a, const PoseIntent& b) {
                return a.priority > b.priority;
            });

        // 3. Apply intents
        for (const PoseIntent& intent : cBuffer->intents) {
            PoseJoint& j = pose.joint(intent.joint);
            Vec3 contribution = intent.desiredDelta_m * intent.weight;
            if (intent.type == PoseIntentType::Translate) {
                j.deltaOffset_m += contribution;
            }
            else {
                j.deltaRotation_rad += contribution;
            }
            
        }

        // 4. Clear buffer
        cBuffer->intents.clear();
    }
    return { SystemExecResult::Ran };
}

