#include "FootworkMovementSystem.hpp"
#include "math/Vec3.hpp"
#include "components/Components.hpp"
#include "math/Constants.hpp"
#include <cmath>

SystemExec FootworkMovementSystem::update(GameContext* context) {
	Entity* ePlayer = context->registry.getEntity("player");
	auto [cPlayerFootworkIntent, cPlayerFootworkState, cPlayerVelocity3D] = context->registry.getComponents<CFootworkIntent, CFootworkState, CVelocity3D>(*ePlayer);
	if (!cPlayerVelocity3D || !cPlayerFootworkState) return { SystemExecResult::EarlyExit, "Missing one or more components in player" };
	if (cPlayerFootworkIntent) {
		StepRaw rawStep{};
		float s = cPlayerFootworkIntent->directionalStrength;
		if (cPlayerFootworkIntent->heldFrames < context->playerMovement.footworkMovement.tapFrameLimit) {
			rawStep.kind = StepKind::Tap;
			rawStep.strength = context->playerMovement.footworkMovement.tapStrength;
		}
		else if (cPlayerFootworkIntent->heldFrames < context->playerMovement.footworkMovement.hopFrameLimit) { //change to double tap later
			rawStep.kind = StepKind::Hop;
			rawStep.strength = (cPlayerFootworkIntent->heldFrames / context->playerMovement.footworkMovement.hopFrameFactor) * context->playerMovement.footworkMovement.hopStrength;
		}
		else {
			rawStep.kind = StepKind::Leap;
			rawStep.strength = (std::min(context->playerMovement.footworkMovement.maxLeapStrength, cPlayerFootworkIntent->heldFrames / context->playerMovement.footworkMovement.leapFrameFactor)) * context->playerMovement.footworkMovement.leapStrength;
		}
		rawStep.strength *= cPlayerFootworkIntent->directionalStrength;
		StepProfile step = convertStepFromRaw(rawStep);
		if (!cPlayerFootworkState->active) {
			cPlayerFootworkState->active = true;
			cPlayerFootworkState->frame = 0;
			cPlayerFootworkState->current = step;
			cPlayerFootworkState->direction = cPlayerFootworkIntent->direction;


		}
		else {
			// buffer one step only
			cPlayerFootworkState->buffered = true;
			cPlayerFootworkState->bufferedStep = rawStep;
			cPlayerFootworkState->bufferedDirection = cPlayerFootworkIntent->direction;
		}
		context->registry.removeComponent<CFootworkIntent>(*ePlayer);
	}
	if (cPlayerFootworkState->active) {
		if (cPlayerFootworkState->frame < cPlayerFootworkState->current.totalFrames) {

			float t = float(cPlayerFootworkState->frame) /
				float(cPlayerFootworkState->current.totalFrames);

			float speed =
				cPlayerFootworkState->current.maxSpeed_mps *
				std::sin(t * PI);

			cPlayerVelocity3D->vel_mps =
				cPlayerFootworkState->direction * speed;
		}
		else {
			// recovery frames → no movement
			cPlayerVelocity3D->vel_mps = { 0.f, 0.f, 0.f };
		}

		cPlayerFootworkState->frame++;

		if (cPlayerFootworkState->frame >=
			cPlayerFootworkState->current.totalFrames +
			cPlayerFootworkState->current.recoveryFrames) {

			if (cPlayerFootworkState->buffered) {
				cPlayerFootworkState->current =
					convertStepFromRaw(cPlayerFootworkState->bufferedStep);
				cPlayerFootworkState->direction = cPlayerFootworkState->bufferedDirection;
				cPlayerFootworkState->frame = 0;
				cPlayerFootworkState->buffered = false;
			}
			else {
				cPlayerFootworkState->active = false;
			}
		}
	}
	else {
		cPlayerVelocity3D->vel_mps = { 0.f, 0.f, 0.f };
	}

	return { SystemExecResult::Ran };
}

StepProfile FootworkMovementSystem::convertStepFromRaw(const StepRaw& rawStep) {

	StepProfile stepProfile{};
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