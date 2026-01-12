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
                float prevT = float(cFootworkState->frame - 1) / cFootworkState->current.totalFrames;
                float currT = float(cFootworkState->frame) / cFootworkState->current.totalFrames;

                float currW = std::sin(currT * PI * 0.5f); // fast start
                float prevW = std::sin(prevT * PI * 0.5f);

                float stride = (currW - prevW) * cFootworkState->current.maxStride_m;

                if (cFootworkState->current.kind == StepKind::Tap) {
                    bool stepRight = (cFootworkState->current.dominantFoot == DominantFoot::Right);
                    PoseJointID swingAnkle = stepRight ? PoseJointID::RightAnkle : PoseJointID::LeftAnkle;
                    PoseJointID hip = stepRight ? PoseJointID::RightPelvis : PoseJointID::LeftPelvis;

                    // Pelvis leads (small)
                    cPoseIntentBuffer->intents.push_back({
                        PoseJointID::CenterPelvis, PoseIntentPhase::Commit, 0, PoseIntentType::ShiftBody,
                        cFootworkState->direction * stride * 0.6f, 1.0f, 10.f
                        });

                    //// Tiny hip open
                    //float yaw = stepRight ? 0.15f : -0.15f;
                    //cPoseIntentBuffer->intents.push_back({
                    //    hip, PoseIntentType::Rotate, Vec3{0.f,yaw,0.f}, 0.8f, 9.f
                    //    });

                    //// Swing foot
                    //cPoseIntentBuffer->intents.push_back({
                    //    swingAnkle, PoseIntentType::Translate,
                    //    cFootworkState->direction * stride, 0.8f, 7.f
                    //    });
                }

                else if (cFootworkState->current.kind == StepKind::Hop) {
                    // Strong pelvis shift
                    cPoseIntentBuffer->intents.push_back({
                        PoseJointID::CenterPelvis, PoseIntentPhase::Commit, 0, PoseIntentType::ShiftBody,
                        cFootworkState->direction * stride, 1.0f, 10.f
                        });

                    //// Both feet follow
                    //cPoseIntentBuffer->intents.push_back({
                    //    PoseJointID::LeftAnkle, PoseIntentType::Translate,
                    //    cFootworkState->direction * stride * 0.7f, 0.7f, 7.f
                    //    });

                    //cPoseIntentBuffer->intents.push_back({
                    //    PoseJointID::RightAnkle, PoseIntentType::Translate,
                    //    cFootworkState->direction * stride * 0.7f, 0.7f, 7.f
                    //    });

                    //// Optional tiny hip yaw to prevent inward collapse
                    //float yaw = (cFootworkState->direction.x > 0) ? 0.1f : -0.1f;
                    //cPoseIntentBuffer->intents.push_back({
                    //    PoseJointID::CenterPelvis, PoseIntentType::Rotate,
                    //    Vec3{0.f,yaw,0.f}, 0.5f, 9.f
                    //    });
                }

                else if (cFootworkState->current.kind == StepKind::Reach) {
                    bool reachRight = (cFootworkState->direction.x > 0);

                    PoseJointID reachPelvis = reachRight ? PoseJointID::RightPelvis : PoseJointID::LeftPelvis;
                    PoseJointID reachUpperLeg = reachRight ? PoseJointID::RightKnee : PoseJointID::LeftKnee; // femur flexion proxy
                    PoseJointID reachAnkle = reachRight ? PoseJointID::RightAnkle : PoseJointID::LeftAnkle;
                    PoseJointID trailAnkle = reachRight ? PoseJointID::LeftAnkle : PoseJointID::RightAnkle;

                    //// 1) Pull center of mass first (THIS is what makes it feel like a save)
                    cPoseIntentBuffer->intents.push_back({
                        PoseJointID::CenterPelvis,PoseIntentPhase::Commit, 0, PoseIntentType::ShiftBody,cFootworkState->direction * stride,1.2f,1.0f, 10.f});

                    //// 2) Small pelvis yaw ONLY for balance, not reach
                    //float pelvisYaw = reachRight ? +0.15f : -0.15f;
                    //cPoseIntentBuffer->intents.push_back({
                    //    reachPelvis,
                    //    PoseIntentType::Rotate,Vec3{ 0.f, pelvisYaw, 0.f },0.6f,8.f});

                    // 3) Hip flexion: THIS drives the leg forward
                    cPoseIntentBuffer->intents.push_back({
                        PoseJointID::CenterPelvis, PoseIntentPhase::Load, 1, PoseIntentType::LoadBody, Vec3{ 0.f, 0.f, 0.f }, 1.0f,2.0f, .08});

                    // 4) Reach foot slides forward to catch
                    cPoseIntentBuffer->intents.push_back({
                        reachAnkle,PoseIntentPhase::Recover, 0, PoseIntentType::ShiftBody,cFootworkState->direction * stride ,1.0f,3.0f, 1.f});

                    //// 5) Trail foot drags slightly (don’t let it stick)
                    //cPoseIntentBuffer->intents.push_back({
                    //    trailAnkle, PoseIntentType::Translate,cFootworkState->direction * stride ,0.3f * .15,6.f});
                }

                else { // LEAP
                    cPoseIntentBuffer->intents.push_back({
                        PoseJointID::CenterPelvis,  PoseIntentPhase::Translate, 0, PoseIntentType::Translate,
                        cFootworkState->direction * stride * 1.2f, 1.0f, 10.f
                        });

                    cPoseIntentBuffer->intents.push_back({
                        PoseJointID::LeftAnkle, PoseIntentPhase::Translate, 0, PoseIntentType::Translate,
                        cFootworkState->direction * stride, 0.8f, 7.f
                        });

                    cPoseIntentBuffer->intents.push_back({
                        PoseJointID::RightAnkle, PoseIntentPhase::Translate, 0, PoseIntentType::Translate,
                        cFootworkState->direction * stride, 0.8f, 7.f
                        });
                }

                cFootworkState->recentStepKind = cFootworkState->current.kind;
                cFootworkState->recentDirection = cFootworkState->direction;

            }
            cFootworkState->frame += 1;
            if (cFootworkState->frame >= cFootworkState->current.totalFrames + cFootworkState->current.recoveryFrames) { 
                //cFootworkState->recentStepKind = StepKind::None;
                //cFootworkState->recentDirection = { 0,0,0 };
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