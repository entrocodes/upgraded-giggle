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
    return (ankle == PoseJointID::LeftAnkle) ? PoseJointID::LeftPelvis : PoseJointID::RightPelvis;
}
static void applyIntent(PoseIntent intent, Pose& pose) {
        if (intent.type == PoseIntentType::Translate) {
            PoseJoint& j = pose.joint(intent.joint);
            j.deltaOffset_m += intent.desiredDelta_m * intent.weight;
            Debug::debugPrint("transform intent consumed", j.deltaOffset_m);
            return;
        }
        //if (intent.type == PoseIntentType::ShiftBody) {
        //    PoseJoint& j = pose.joint(intent.joint);
        //    pose.supportMode = SupportMode::Airborne;
        //    j.deltaOffset_m += intent.desiredDelta_m * intent.weight;
        //    Debug::debugPrint("shift body intent consumed", j.deltaOffset_m);
        //    return;
        //}

        if (intent.type == PoseIntentType::Rotate) {
            PoseJoint& j = pose.joint(intent.joint);
            j.deltaRotation_rad += intent.desiredDelta_m * intent.weight;
            return;
        }

        //// New: "LoadBody" means "compress down into the floor" (squat/lunge driver)
        //if (intent.type == PoseIntentType::LoadBody) {
        //    const float load = intent.magnitude * intent.weight; // meters of requested "body drop"

        //    // (A) Pelvis drop driver (this will be resolved by IK so feet don't move)
        //    pose.centerPelvis().deltaOffset_m.y -= load;

        //    // (B) Mild counterbalance: pelvis goes slightly backward (helps "squat", not elevator)
        //    pose.centerPelvis().deltaOffset_m.z -= load * 0.20f;

        //    // (C) Lock both ankles at current world position (captured pre-drop, from last FK)
        //    pose.supportMode = SupportMode::Grounded;

        //    return;
        //}
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
    }

    return { SystemExecResult::Ran };
}
