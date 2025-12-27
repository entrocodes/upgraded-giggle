#include "FootworkMovementSystem.hpp"
#include "math/Vec3.hpp"
#include "components/Components.hpp"
#include "math/Constants.hpp"
#include <cmath>

SystemExec FootworkMovementSystem::update(GameContext* context) {
	for (auto entity : context->registry.getEntitiesWith<CFootworkState, CVelocity3D>()) {
		auto [cFootworkIntent, cFootworkState, cVelocity3D] = context->registry.getComponents<CFootworkIntent, CFootworkState, CVelocity3D>(entity);
		if (cFootworkIntent) {
			StepRaw rawStep{};
			float s = cFootworkIntent->directionalStrength;
			if (cFootworkIntent->heldFrames < context->playerMovement.footworkMovement.tapFrameLimit) {
				rawStep.kind = StepKind::Tap;
				rawStep.strength = context->playerMovement.footworkMovement.tapStrength;
			}
			else if (cFootworkIntent->heldFrames < context->playerMovement.footworkMovement.hopFrameLimit) { //change to double tap later
				rawStep.kind = StepKind::Hop;
				rawStep.strength = (cFootworkIntent->heldFrames / context->playerMovement.footworkMovement.hopFrameFactor) * context->playerMovement.footworkMovement.hopStrength;
			}
			else {
				rawStep.kind = StepKind::Leap;
				rawStep.strength = (std::min(context->playerMovement.footworkMovement.maxLeapStrength, cFootworkIntent->heldFrames / context->playerMovement.footworkMovement.leapFrameFactor)) * context->playerMovement.footworkMovement.leapStrength;
			}
			rawStep.strength *= cFootworkIntent->directionalStrength;
			StepProfile step = convertStepFromRaw(rawStep);
			if (!cFootworkState->active) {
				cFootworkState->active = true;
				cFootworkState->frame = 0;
				cFootworkState->current = step;
				cFootworkState->direction = cFootworkIntent->direction;


			}
			else {
				// buffer one step only
				cFootworkState->buffered = true;
				cFootworkState->bufferedStep = rawStep;
				cFootworkState->bufferedDirection = cFootworkIntent->direction;
			}
			context->registry.removeComponent<CFootworkIntent>(entity);
		}
		if (cFootworkState->active) {
			if (cFootworkState->frame < cFootworkState->current.totalFrames) {

				float t = float(cFootworkState->frame) /
					float(cFootworkState->current.totalFrames);

				float speed =
					cFootworkState->current.maxSpeed_mps *
					std::sin(t * PI);

				cVelocity3D->vel_mps =
					cFootworkState->direction * speed;
			}
			else {
				// recovery frames → no movement
				cVelocity3D->vel_mps = { 0.f, 0.f, 0.f };
			}

			cFootworkState->frame++;

			if (cFootworkState->frame >=
				cFootworkState->current.totalFrames +
				cFootworkState->current.recoveryFrames) {

				if (cFootworkState->buffered) {
					cFootworkState->current =
						convertStepFromRaw(cFootworkState->bufferedStep);
					cFootworkState->direction = cFootworkState->bufferedDirection;
					cFootworkState->frame = 0;
					cFootworkState->buffered = false;
				}
				else {
					cFootworkState->active = false;
				}
			}
		}
		else {
			cVelocity3D->vel_mps = { 0.f, 0.f, 0.f };
		}

		return { SystemExecResult::Ran };
	}
	
}

StepProfile FootworkMovementSystem::convertStepFromRaw(const StepRaw& rawStep) {

	StepProfile stepProfile{};
	stepProfile.kind = rawStep.kind;
	if (rawStep.kind == StepKind::Tap) {
		stepProfile.totalFrames = 5;
		stepProfile.recoveryFrames = 7;
		stepProfile.maxSpeed_mps = rawStep.strength;
		stepProfile.staminaCost = 5;
	}
	else if (rawStep.kind == StepKind::Hop) {
		stepProfile.totalFrames = 10;
		stepProfile.recoveryFrames = 15;
		stepProfile.maxSpeed_mps = rawStep.strength;
		stepProfile.staminaCost = 12;
	}
	else {
		stepProfile.totalFrames = 18;
		stepProfile.recoveryFrames = 24;
		stepProfile.maxSpeed_mps = rawStep.strength;
		stepProfile.staminaCost = 20;
	}
	return stepProfile;
}