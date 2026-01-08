#include "PoseIntentConsumerSystem.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"

static PoseJointID kneeFromAnkle(PoseJointID ankle) {
    return (ankle == PoseJointID::LeftAnkle)
        ? PoseJointID::LeftKnee
        : PoseJointID::RightKnee;
}

static PoseJointID pelvisFromAnkle(PoseJointID ankle) {
    return (ankle == PoseJointID::LeftAnkle)
        ? PoseJointID::LeftPelvis
        : PoseJointID::RightPelvis;
}

static PoseBoneID upperLegFromAnkle(PoseJointID ankle) {
    return (ankle == PoseJointID::LeftAnkle)
        ? PoseBoneID::LeftUpperLeg
        : PoseBoneID::RightUpperLeg;
}

static PoseBoneID lowerLegFromAnkle(PoseJointID ankle) {
    return (ankle == PoseJointID::LeftAnkle)
        ? PoseBoneID::LeftLowerLeg
        : PoseBoneID::RightLowerLeg;
}

SystemExec PoseIntentConsumerSystem::update(GameContext* context) {
    for (auto [e, cPose, cBuffer] :
        context->registry.getEntitiesWithComponents<CPose, CPoseIntentBuffer>()) {

        Pose& pose = cPose->pose;


        // 2) Sort intents by priority (high → low)
        std::sort(cBuffer->intents.begin(), cBuffer->intents.end(),
            [](const PoseIntent& a, const PoseIntent& b) {
                return a.priority > b.priority;
            });

        // 3) Apply intents
        for (const PoseIntent& intent : cBuffer->intents) {

            if (intent.type == PoseIntentType::Translate) {
                PoseJoint& j = pose.joint(intent.joint);
                j.deltaOffset_m += intent.desiredDelta_m * intent.weight;
                if(intent.joint == PoseJointID::CenterPelvis) Debug::debugPrint("Pelvis Translate Consumed!", j.deltaOffset_m);
                continue;
            }

            if (intent.type == PoseIntentType::Rotate) {
                PoseJoint& j = pose.joint(intent.joint);
                j.deltaRotation_rad += intent.desiredDelta_m * intent.weight;
                continue;
            }

            if (intent.type == PoseIntentType::LoadAnkle) {
                PoseJointID ankle = intent.joint;
                PoseJointID knee = kneeFromAnkle(ankle);
                PoseJointID pelvis = pelvisFromAnkle(ankle);
                
                PoseBone& upperLeg = pose.bone(upperLegFromAnkle(ankle));
                PoseBone& lowerLeg = pose.bone(lowerLegFromAnkle(ankle));

                float load = intent.magnitude * intent.weight; // meters of compression request

                // Bone compression (negative stretch = shortening)
                upperLeg.deltaStretch -= load * 0.6f;
                lowerLeg.deltaStretch -= load * 0.3f;

                // Pelvis drops (this is the key!)
                pose.centerPelvis().deltaOffset_m.y -= load;

                pose.forEachAnkle([](PoseJoint& fAnkle) {
                    fAnkle.lockPosition = true;
                    fAnkle.lockedWorldPos_m = fAnkle.pos_m;
                    fAnkle.lockWeight = 1.0f;
                    });


            }
        }

        // 4) Clear buffer
        cBuffer->intents.clear();
    }

    return { SystemExecResult::Ran };
}
