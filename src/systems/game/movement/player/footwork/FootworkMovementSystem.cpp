#include "FootworkMovementSystem.hpp"
#include "math/Vec3.hpp"
#include "game/pose/PoseIntent.hpp"
#include "components/Components.hpp"
#include "math/Constants.hpp"
#include "debug/Debug.hpp"
#include <cmath>

SystemExec FootworkMovementSystem::update(GameContext* context) {
    for (auto [eCharacter, cFootworkState, cPoseIntentBuffer] : context->registry.getEntitiesWithComponents<CFootworkState, CPoseIntentBuffer>()) {
        auto [cFootworkIntent] = context->registry.getComponents<CFootworkIntent>(eCharacter);
        if (cFootworkIntent && !cFootworkState->active) {
            StepRaw raw{};

            raw.kind = (cFootworkIntent->heldFrames < context->playerMovement.footworkMovement.tapFrameLimit) ? StepKind::Tap :
                (cFootworkIntent->heldFrames < context->playerMovement.footworkMovement.hopFrameLimit) ? StepKind::Hop : StepKind::Leap;

            bool isReach = (raw.kind == StepKind::Tap && cFootworkState->recentStepKind == StepKind::Tap);

            if (isReach) {
                raw.kind = StepKind::Reach;
            }

            raw.strength = cFootworkIntent->directionalStrength * (cFootworkIntent->heldFrames / 60.f);
            cFootworkState->current = convertStepFromRaw(context, raw);
            cFootworkState->direction = cFootworkIntent->direction;
            Debug::debugPrint("direction", cFootworkState->direction);
            if (cFootworkState->direction.x > 0) { //LEFTY Logic Only
                cFootworkState->current.dominantFoot = DominantFoot::Right;
            }
            else {
                cFootworkState->current.dominantFoot = DominantFoot::Left;
            }
            cFootworkState->frame = 0;
            cFootworkState->active = true;
            context->registry.removeComponent<CFootworkIntent>(eCharacter);
        }

        if (cFootworkState->active) {
            if (cFootworkState->frame < cFootworkState->current.totalFrames) {
                float prevT = float(cFootworkState->frame - 1) / cFootworkState->current.shiftEndFrame;
                float currT = float(cFootworkState->frame) / cFootworkState->current.shiftEndFrame;

                float currW = std::sin(currT * PI * 0.5f); // fast start
                float prevW = std::sin(prevT * PI * 0.5f);

                float stride = (currW - prevW) * cFootworkState->current.maxStride_m;
                Debug::debugPrint("footwork stride", stride);
                if (cFootworkState->current.kind == StepKind::Tap) {
                    bool stepRight = (cFootworkState->current.dominantFoot == DominantFoot::Right);
                    PoseJointID swingAnkle = stepRight ? PoseJointID::RightAnkle : PoseJointID::LeftAnkle;
                    PoseJointID hip = stepRight ? PoseJointID::RightHip : PoseJointID::LeftHip;

                    // Pelvis leads (small)
                    Vec3 shiftAmount = cFootworkState->direction * stride * .6f;
                    Debug::debugPrint("footwork tap shift amount", shiftAmount);
                    cPoseIntentBuffer->intents.push_back({PoseJointID::CenterPelvis, PoseIntentPhase::Translate, 0, PoseIntentType::ShiftBody, shiftAmount});
                    // Swing foot
                    cPoseIntentBuffer->intents.push_back({swingAnkle, PoseIntentPhase::Translate, 1, PoseIntentType::ShiftBody, cFootworkState->direction * stride * 0.15f});
                }

                else if (cFootworkState->current.kind == StepKind::Hop) {
                    // Strong pelvis shift
                    Vec3 shiftAmount = cFootworkState->direction * stride * 1.2f;
                    Debug::debugPrint("footwork hop shift amount", shiftAmount);
                    cPoseIntentBuffer->intents.push_back({PoseJointID::CenterPelvis, PoseIntentPhase::Translate, 0, PoseIntentType::ShiftBody, shiftAmount});
                }

                else if (cFootworkState->current.kind == StepKind::Reach) {
                    bool reachRight = (cFootworkState->direction.x > 0);

                    PoseJointID reachPelvis = reachRight ? PoseJointID::RightHip : PoseJointID::LeftHip;
                    PoseJointID reachUpperLeg = reachRight ? PoseJointID::RightKnee : PoseJointID::LeftKnee; // femur flexion proxy
                    PoseJointID reachAnkle = reachRight ? PoseJointID::RightAnkle : PoseJointID::LeftAnkle;
                    PoseJointID trailAnkle = reachRight ? PoseJointID::LeftAnkle : PoseJointID::RightAnkle;

                    //// 1) Pull center of mass first (THIS is what makes it feel like a save)
                    if (cFootworkState->frame == 0) {
                        cPoseIntentBuffer->intents.push_back({PoseJointID::CenterPelvis,PoseIntentPhase::Translate, 0, PoseIntentType::ShiftBody,cFootworkState->direction * stride * .5f });
                    }
                    if (cFootworkState->frame <= cFootworkState->current.shiftEndFrame) {
                        cPoseIntentBuffer->intents.push_back({ PoseJointID::CenterPelvis,PoseIntentPhase::Support, 0, PoseIntentType::ShiftBody,cFootworkState->direction * stride * .5f });
                    }

                    if (cFootworkState->frame == cFootworkState->current.shiftEndFrame + 1) {
                        float squatAmount = .08f;
                        cPoseIntentBuffer->intents.push_back({
                            PoseJointID::CenterPelvis, PoseIntentPhase::Support, 2, PoseIntentType::LoadBody });
                        cPoseIntentBuffer->intents.push_back({
                            PoseJointID::CenterPelvis, PoseIntentPhase::Translate, 2, PoseIntentType::LoadBody, Vec3( 0.f, squatAmount, 0.f )});
                    }

                    // 4) Reach foot slides forward
                    cPoseIntentBuffer->intents.push_back({
                        reachAnkle,PoseIntentPhase::Recover, 3, PoseIntentType::ShiftBody,cFootworkState->direction * stride * .3});
                    cPoseIntentBuffer->intents.push_back({
                        reachAnkle,PoseIntentPhase::Recover, 3, PoseIntentType::ShiftBody,cFootworkState->direction * stride * .3});

                }

                else { // LEAP
                    cPoseIntentBuffer->intents.push_back({
                        PoseJointID::CenterPelvis,  PoseIntentPhase::Translate, 0, PoseIntentType::ShiftBody,
                        cFootworkState->direction * stride * 3.f});
                }

                cFootworkState->recentStepKind = cFootworkState->current.kind;
                cFootworkState->recentDirection = cFootworkState->direction;

            }
            cFootworkState->frame += 1;
            //need to make reach only trigger directly after previous input
            if (cFootworkState->frame >= cFootworkState->current.totalFrames + cFootworkState->current.recoveryFrames) { 
                cFootworkState->active = false;
            }
        }
    }
    return{ SystemExecResult::Ran };
}


StepProfile FootworkMovementSystem::convertStepFromRaw(GameContext* context, const StepRaw& rawStep) {
    StepProfile step{}; step.kind = rawStep.kind;
    if (rawStep.kind == StepKind::Tap) {
        step.totalFrames = 5;
        step.recoveryFrames = 2;
        step.maxStride_m = rawStep.strength * context->playerMovement.footworkMovement.tapStrength;
        step.staminaCost = 5; }
    if (rawStep.kind == StepKind::Reach) {
        step.totalFrames = 5;
        step.shiftEndFrame = 3;
        step.recoveryFrames = 2;
        step.maxStride_m = rawStep.strength * context->playerMovement.footworkMovement.reachStrength;
        step.staminaCost = 5; }
    else if (rawStep.kind == StepKind::Hop) {
        step.totalFrames = 6;
        step.recoveryFrames = 5;
        step.maxStride_m = rawStep.strength * context->playerMovement.footworkMovement.hopStrength;
        step.staminaCost = 12;
    }
    else {
        step.totalFrames = 10;
        step.recoveryFrames = 12;
        step.maxStride_m = rawStep.strength * context->playerMovement.footworkMovement.leapStrength;
        step.staminaCost = 20; }
    return step;
}