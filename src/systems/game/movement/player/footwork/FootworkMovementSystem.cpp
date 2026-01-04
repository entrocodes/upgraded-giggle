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
            raw.strength = cFootworkIntent->directionalStrength * (cFootworkIntent->heldFrames / 120.0f);
            cFootworkState->current = convertStepFromRaw(raw);
            cFootworkState->direction = cFootworkIntent->direction;
            if (cFootworkState->direction.x > 0 && cFootworkState->lastFootMovedAlone != DominantFoot::Right) { //LEFTY Logic Only
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
            Debug::debugPrint("Footwork Active", cFootworkState->frame);
            if (cFootworkState->frame < cFootworkState->current.totalFrames) {
                float prevT = float(cFootworkState->frame - 1) / cFootworkState->current.totalFrames;
                float currT = float(cFootworkState->frame) / cFootworkState->current.totalFrames;

                float prevW = (cFootworkState->frame == 0)
                    ? 0.0f
                    : (0.5f - 0.5f * std::cos(prevT * PI));

                float currW = 0.5f - 0.5f * std::cos(currT * PI);
                float stride = (currW - prevW) * cFootworkState->current.maxStride_m;


                if (cFootworkState->current.kind != StepKind::Leap) {
                    if (cFootworkState->current.dominantFoot == DominantFoot::Left) {
                        cPoseIntentBuffer->intents.push_back(
                            { PoseJointID::LeftAnkle, PoseIntentType::Translate,cFootworkState->direction * stride * 50, 1.0f, 10.f }
                        );
                        cFootworkState->lastFootMovedAlone = DominantFoot::Left;
                    }
                    else if (cFootworkState->current.dominantFoot == DominantFoot::Right) {
                        cPoseIntentBuffer->intents.push_back(
                            { PoseJointID::RightAnkle, PoseIntentType::Translate,cFootworkState->direction * stride * 50, 1.0f, 10.f }
                        );
                        cFootworkState->lastFootMovedAlone = DominantFoot::Right;
                    }
                }
                else {
                    cPoseIntentBuffer->intents.push_back(
                        { PoseJointID::CenterPelvis, PoseIntentType::Translate,cFootworkState->direction * stride, 1.0f, 10.f }
                    );
                    cFootworkState->lastFootMovedAlone = DominantFoot::None;
                }

            }
            cFootworkState->frame += 1;
            if (cFootworkState->frame >= cFootworkState->current.totalFrames + cFootworkState->current.recoveryFrames) { 
                cFootworkState->lastFootMovedAlone = DominantFoot::None;
                cFootworkState->active = false;
            }
        }
    }
    return{ SystemExecResult::Ran };
}


StepProfile FootworkMovementSystem::convertStepFromRaw(const StepRaw& rawStep) {
    StepProfile step{}; step.kind = rawStep.kind;
    if (rawStep.kind == StepKind::Tap) {
        step.totalFrames = 5;
        step.recoveryFrames = 2;
        step.maxStride_m = rawStep.strength;
        step.staminaCost = 5; }
    else if (rawStep.kind == StepKind::Hop) {
        step.totalFrames = 10;
        step.recoveryFrames = 15;
        step.maxStride_m = rawStep.strength;
        step.staminaCost = 12;
    }
    else {
        step.totalFrames = 18;
        step.recoveryFrames = 24;
        step.maxStride_m = rawStep.strength;
        step.staminaCost = 20; }
    return step;
}