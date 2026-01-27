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
        if (cFootworkIntent) {

            StepRaw raw{};
            raw.kind = (cFootworkIntent->heldFrames < context->playerMovement.footworkMovement.tapFrameLimit) ? StepKind::Tap : (cFootworkIntent->heldFrames < context->playerMovement.footworkMovement.hopFrameLimit) ? StepKind::Hop : StepKind::Leap;
            raw.strength = cFootworkIntent->directionalStrength * (cFootworkIntent->heldFrames / 60.f);

            if (!cFootworkState->active) {
                startStep(context, eCharacter, *cFootworkState, *cPoseIntentBuffer, raw, cFootworkIntent->direction);
            }
            else {
                if (raw.kind == StepKind::Tap) raw.kind = StepKind::Reach;
                cFootworkState->buffered = true;
                cFootworkState->bufferedStep = raw;
                cFootworkState->bufferedDirection = cFootworkIntent->direction;
            }

            context->registry.removeComponent<CFootworkIntent>(eCharacter);
        }

        if (cFootworkState->active) {
            if (cFootworkState->frame <= cFootworkState->current.totalFrames) {
                float prevT = float(cFootworkState->frame - 1) / cFootworkState->current.shiftEndFrame;
                float currT = float(cFootworkState->frame) / cFootworkState->current.shiftEndFrame;

                float currW = std::sin(currT * PI * 0.5f); // fast start
                float prevW = std::sin(prevT * PI * 0.5f);

                float stride = (currW - prevW) * cFootworkState->current.maxStride_m;
                if (cFootworkState->current.kind == StepKind::Tap) {
                    bool stepRight = (cFootworkState->current.dominantFoot == DominantFoot::Right);
                    PoseJointID swingAnkle = stepRight ? PoseJointID::RightAnkle : PoseJointID::LeftAnkle;
                    PoseJointID hip = stepRight ? PoseJointID::RightHip : PoseJointID::LeftHip;

                    cPoseIntentBuffer->intents.push_back({ PoseJointID::CenterPelvis,PoseIntentPhase::Support, 0, PoseIntentType::ShiftBody });
                    // Pelvis leads (small)
                    Vec3 shiftAmount = cFootworkState->direction * stride * .6f;
                    cPoseIntentBuffer->intents.push_back({PoseJointID::CenterPelvis, PoseIntentPhase::Translate, 0, PoseIntentType::Translate, shiftAmount});
                    // Swing foot
                    cPoseIntentBuffer->intents.push_back({swingAnkle, PoseIntentPhase::Translate, 1, PoseIntentType::Translate, cFootworkState->direction * stride * 0.15f});
                }

                else if (cFootworkState->current.kind == StepKind::Hop) {
                    // unlock feet by changing support mode
                    cPoseIntentBuffer->intents.push_back({ PoseJointID::CenterPelvis,PoseIntentPhase::Support, 0, PoseIntentType::ShiftBody});
                    // Strong pelvis shift
                    Vec3 shiftAmount = cFootworkState->direction * stride * 1.2f;
                    cPoseIntentBuffer->intents.push_back({PoseJointID::CenterPelvis, PoseIntentPhase::Translate, 0, PoseIntentType::Translate, shiftAmount});
                }

                else if (cFootworkState->current.kind == StepKind::Reach) {
                    bool reachRight = (cFootworkState->direction.x > 0);

                    PoseJointID reachPelvis = reachRight ? PoseJointID::RightHip : PoseJointID::LeftHip;
                    PoseJointID reachUpperLeg = reachRight ? PoseJointID::RightKnee : PoseJointID::LeftKnee; // femur flexion proxy
                    PoseJointID reachAnkle = reachRight ? PoseJointID::RightAnkle : PoseJointID::LeftAnkle;
                    PoseJointID trailAnkle = reachRight ? PoseJointID::LeftAnkle : PoseJointID::RightAnkle;

                    // 1) Pull center of mass first (THIS is what makes it feel like a save)
                    if (cFootworkState->frame == 0) {
                        cPoseIntentBuffer->intents.push_back({PoseJointID::CenterPelvis,PoseIntentPhase::Translate, 0, PoseIntentType::Translate});
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

                else if (cFootworkState->current.kind == StepKind::Leap) { // LEAP
                    //if (cFootworkState->frame == 0) {
                    //    float preload = 0.05f;
                    //    cPoseIntentBuffer->intents.push_back({ PoseJointID::CenterPelvis,PoseIntentPhase::Support,0,PoseIntentType::LoadBody });
                    //    cPoseIntentBuffer->intents.push_back({ PoseJointID::CenterPelvis,PoseIntentPhase::Translate,0,PoseIntentType::LoadBody,Vec3(0.f, preload, 0.f) });
                    //}
                    if (cFootworkState->frame < cFootworkState->current.shiftEndFrame) {
                        //shift body
                        cPoseIntentBuffer->intents.push_back({ PoseJointID::CenterPelvis,PoseIntentPhase::Support, 1, PoseIntentType::ShiftBody }); //unlocks feet
                        cPoseIntentBuffer->intents.push_back({PoseJointID::CenterPelvis, PoseIntentPhase::Translate,1,PoseIntentType::Translate,cFootworkState->direction * stride * 0.5f});
                    }
                    if (cFootworkState->frame >= cFootworkState->current.shiftEndFrame && cFootworkState->frame < cFootworkState->current.shiftEndFrame + 3) {
                        float absorb = 0.03f;
                        cPoseIntentBuffer->intents.push_back({PoseJointID::CenterPelvis,PoseIntentPhase::Recover,2, PoseIntentType::LoadBody,Vec3(0.f, absorb, 0.f)});
                    }
                    //float yaw = (cFootworkState->direction.x > 0 ? 1.f : -1.f) * 0.15f;
                    //cPoseIntentBuffer->intents.push_back({PoseJointID::RightAnkle, PoseIntentPhase::Translate, 1, PoseIntentType::Rotate,Vec3(0.f, yaw, 0.f)});
                    //cPoseIntentBuffer->intents.push_back({ PoseJointID::LeftAnkle, PoseIntentPhase::Translate, 1, PoseIntentType::Rotate, Vec3(0.f, -yaw, 0.f) });      
                }
                cFootworkState->recentStepKind = cFootworkState->current.kind;
                cFootworkState->recentDirection = cFootworkState->direction;
            }
            cFootworkState->frame += 1;
            //need to make reach only trigger directly after previous input
            if (cFootworkState->frame >= cFootworkState->current.totalFrames + cFootworkState->current.recoveryFrames) {

                cFootworkState->active = false;
                cFootworkState->frame = 0;

                if (cFootworkState->buffered) {
                    cFootworkState->buffered = false;
                    startStep(context, eCharacter, *cFootworkState, *cPoseIntentBuffer, cFootworkState->bufferedStep, cFootworkState->bufferedDirection);
                }
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
    else if (rawStep.kind == StepKind::Leap) {
        step.totalFrames = 9;
        step.shiftEndFrame = 6;
        step.recoveryFrames = 6;
        step.maxStride_m = rawStep.strength * context->playerMovement.footworkMovement.leapStrength;
        step.staminaCost = 20;
    }
    if (step.shiftEndFrame == 0) {
        step.shiftEndFrame = step.totalFrames;
    }
    return step;
}

void FootworkMovementSystem::startStep(GameContext* context, Entity eCharacter, CFootworkState& state, CPoseIntentBuffer& poseBuffer, StepRaw raw, const Vec3& direction) {
    // Reach logic will live here later
    state.current = convertStepFromRaw(context, raw);
    state.direction = direction;

    // Dominant foot logic
    if (direction.x > 0) {
        state.current.dominantFoot = DominantFoot::Right;
    }
    else {
        state.current.dominantFoot = DominantFoot::Left;
    }

    state.frame = 0;
    state.active = true;

    // History
    state.recentStepKind = state.current.kind;
    state.recentDirection = direction;
}
