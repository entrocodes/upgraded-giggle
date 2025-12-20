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

    auto [input, transform3D, state] = context->registry.getComponents<CInput, CTransform3D, CState>(*player);

    if (input->actions["AttackDown"])  state->state = "backswing";
    if (input->actions["ReleaseAttack"]) state->state = "stand";
    if (input->actions["MoveLeft"]) {
        auto& cFootworkIntent = context->registry.addComponent<CFootworkIntent>(*player);
        cFootworkIntent.direction = { -1, 0, 0 };
        context->playerMovement.moveTriggered = true;
        cFootworkIntent.heldFrames = input->holdTime["MoveLeft"];
    }
    if (input->actions["MoveRight"]) {
        auto& cFootworkIntent = context->registry.addComponent<CFootworkIntent>(*player);
        cFootworkIntent.direction = { 1, 0, 0 };
        context->playerMovement.moveTriggered = true;
        cFootworkIntent.heldFrames = input->holdTime["MoveRight"];

    }
    return { SystemExecResult::Ran };

};
