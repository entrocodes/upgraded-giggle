// PoseIntentConsumerSystem.cpp
#include "PoseIntentConsumerSystem.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"
#include <algorithm>

static PoseJointID kneeFromAnkle(PoseJointID ankle) {
    return (ankle == PoseJointID::LeftAnkle) ? PoseJointID::LeftKnee : PoseJointID::RightKnee;
}
static PoseJointID pelvisFromAnkle(PoseJointID ankle) {
    return (ankle == PoseJointID::LeftAnkle) ? PoseJointID::LeftPelvis : PoseJointID::RightPelvis;
}

SystemExec PoseIntentConsumerSystem::update(GameContext* context) {
    for (auto [e, cPose, cBuffer] :
        context->registry.getEntitiesWithComponents<CPose, CPoseIntentBuffer>()) {

        Pose& pose = cPose->pose;


        // 2) Sort intents by order (low -> high)
        std::sort(cBuffer->intents.begin(), cBuffer->intents.end(),
            [](const PoseIntent& a, const PoseIntent& b) {
                return a.order < b.order;
            });

        // 3) Apply intents
        for (const PoseIntent& intent : cBuffer->intents) {

            if (intent.type == PoseIntentType::Translate) {
                PoseJoint& j = pose.joint(intent.joint);
                j.deltaOffset_m += intent.desiredDelta_m * intent.weight;
                Debug::debugPrint("transform intent consumed", j.deltaOffset_m);
                continue;
            }
            if (intent.type == PoseIntentType::ShiftBody) {
                PoseJoint& j = pose.joint(intent.joint);
                pose.supportMode = SupportMode::Airborne;
                j.deltaOffset_m += intent.desiredDelta_m * intent.weight;
                Debug::debugPrint("shift body intent consumed", j.deltaOffset_m);
                continue;
            }

            if (intent.type == PoseIntentType::Rotate) {
                PoseJoint& j = pose.joint(intent.joint);
                j.deltaRotation_rad += intent.desiredDelta_m * intent.weight;
                continue;
            }

            // New: "LoadBody" means "compress down into the floor" (squat/lunge driver)
            if (intent.type == PoseIntentType::LoadBody) {
                const float load = intent.magnitude * intent.weight; // meters of requested "body drop"

                // (A) Pelvis drop driver (this will be resolved by IK so feet don't move)
                pose.centerPelvis().deltaOffset_m.y -= load;

                // (B) Mild counterbalance: pelvis goes slightly backward (helps "squat", not elevator)
                pose.centerPelvis().deltaOffset_m.z -= load * 0.20f;

                // (C) Lock both ankles at current world position (captured pre-drop, from last FK)
                pose.supportMode = SupportMode::Grounded;

                continue;
            }
        }

        // 4) Clear buffer
        cBuffer->intents.clear();
    }

    return { SystemExecResult::Ran };
}
