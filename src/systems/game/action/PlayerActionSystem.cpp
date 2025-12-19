#include "PlayerActionSystem.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"
#include "game/utils/GameContext.hpp"
#include <cmath>
#include <algorithm>

SystemExec PlayerActionSystem::update(GameContext* context) {
    Entity* player = context->registry.getEntity("player");
    if (!player) {
        Debug::debugPrint("Player Entity not found.");
        return { SystemExecResult::EarlyExit };
    }

    auto [input, vel3D, transform3D, state] = context->registry.getComponents<CInput, CVelocity3D, CTransform3D, CState>(*player);

    vel3D->vel_mps = { 0.f, 0.f, 0.f };

    if (input->actions["AttackDown"])  state->state = "backswing";
    if (input->actions["ReleaseAttack"]) state->state = "stand";
    if (input->actions["MoveForward"])  vel3D->vel_mps.z += .5f;
    if (input->actions["MoveBackward"])  vel3D->vel_mps.z -= .5f;
    if (input->actions["MoveLeft"]) {
        float moveDistance = (context->playerMovement.maxStrength * (1.0f - std::exp(-context->playerMovement.speedFactor * input->holdTime["MoveLeft"]))) * context->playerMovement.scale;
        vel3D->vel_mps.x -= moveDistance;
        context->playerMovement.moveDistance.x = moveDistance;
        context->playerMovement.moveTriggered = true;
    }
    if (input->actions["MoveRight"]) {
        float moveDistance = (context->playerMovement.maxStrength * (1.0f - std::exp(-context->playerMovement.speedFactor * input->holdTime["MoveRight"]))) * context->playerMovement.scale;
        vel3D->vel_mps.x += moveDistance;
        context->playerMovement.moveDistance.y = moveDistance;
        context->playerMovement.moveTriggered = true;
    }
    return { SystemExecResult::Ran };

};
