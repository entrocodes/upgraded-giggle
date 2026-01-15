// PoseIntentConsumerSystem.cpp
#include "PoseIntentConsumerSystem.hpp"
#include "game/pose/PoseIntentPhase.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"
#include <algorithm>

static PoseJointID kneeFromAnkle(PoseJointID ankle) {
    return (ankle == PoseJointID::LeftAnkle) ? PoseJointID::LeftKnee : PoseJointID::RightKnee;
}
static PoseJointID pelvisFromAnkle(PoseJointID ankle) {
    return (ankle == PoseJointID::LeftAnkle) ? PoseJointID::LeftHip : PoseJointID::RightHip;
}
static void applyIntent(const PoseIntent& intent, Pose& pose) {
        if (intent.type == PoseIntentType::Translate) {
            PoseJoint& j = pose.joint(intent.joint);
            j.deltaOffset_m += intent.desiredDelta_m;
            Debug::debugPrint("transform intent consumed", j.deltaOffset_m);
            return;
        }
        if (intent.type == PoseIntentType::ShiftBody) {
            if (intent.phase == PoseIntentPhase::Support) {
                pose.supportMode = SupportMode::Airborne;
            }
            else if (intent.phase == PoseIntentPhase::Translate) {
                PoseJoint& j = pose.joint(intent.joint);
                j.deltaOffset_m += intent.desiredDelta_m;
                Debug::debugPrint("shift body translate intent consumed", j.deltaOffset_m);
            }
            return;
        }

        if (intent.type == PoseIntentType::Rotate) {
            PoseJoint& j = pose.joint(intent.joint);
            j.deltaRotation_rad += intent.desiredDelta_m;
            return;
        }

        //// New: "LoadBody" means "compress down into the floor" (squat/lunge driver)
        if (intent.type == PoseIntentType::LoadBody) {
            if (intent.phase == PoseIntentPhase::Support) {
                pose.supportMode = SupportMode::Grounded;
            }
            if (intent.phase == PoseIntentPhase::Translate) {
                pose.requestedSquat = intent.desiredDelta_m.y;
                Debug::debugPrint("squat amount", pose.requestedSquat);
            }
            return;
        }
}
SystemExec PoseIntentConsumerSystem::update(GameContext* context) {
    int stage = context->poseRuntime.currentStage;

    for (auto [e, cPose, cBuffer] :
        context->registry.getEntitiesWithComponents<CPose, CPoseIntentBuffer>()) {

        Pose& pose = cPose->pose;

        static constexpr PoseIntentPhase kPhaseOrder[] = {
            PoseIntentPhase::Support,
            PoseIntentPhase::Load,
            PoseIntentPhase::Commit,
            PoseIntentPhase::Translate,
            PoseIntentPhase::Recover
        };

        for (PoseIntentPhase ph : kPhaseOrder) {
            for (const PoseIntent& it : cBuffer->intents) {
                if (it.stage != stage) continue;
                if (it.phase != ph) continue;
                applyIntent(it, pose);
            }
        }
        // Remove intents that were consumed in this stage
        auto& v = cBuffer->intents;
        v.erase(std::remove_if(v.begin(), v.end(),
            [stage](const PoseIntent& it) {
                return it.stage == stage;
            }), v.end());


    }

    return { SystemExecResult::Ran };
}
