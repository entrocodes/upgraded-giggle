#include "PlayerActionSystem.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"
#include "game/utils/GameContext.hpp"
#include <cmath>
#include <algorithm>

SystemExec PlayerActionSystem::update(GameContext* context) {
    Entity* ePlayer = context->registry.getEntity("player");
    if (!ePlayer) {
        Debug::debugPrint("Player Entity not found.");
        return { SystemExecResult::EarlyExit };
    }

    auto [cPlayerInput, cPlayerTransform3D, cPlayerState] = context->registry.getComponents<CInput, CTransform3D, CState>(*ePlayer);

    if (cPlayerInput->actions["StartAttack"])  cPlayerState->state = "backswing";
    if (cPlayerInput->actions["ReleaseAttack"]) cPlayerState->state = "stand";
    if (cPlayerInput->actions["MoveLeft"]) {
        auto& cFootworkIntent = context->registry.addComponent<CFootworkIntent>(*ePlayer);
        cFootworkIntent.direction = { -1, 0, 0 };
        context->playerMovement.moveTriggered = true;
        cFootworkIntent.heldFrames = cPlayerInput->holdTime["MoveLeft"];
    }
    if (cPlayerInput->actions["MoveRight"]) {
        auto& cFootworkIntent = context->registry.addComponent<CFootworkIntent>(*ePlayer);
        cFootworkIntent.direction = { 1, 0, 0 };
        context->playerMovement.moveTriggered = true;
        cFootworkIntent.heldFrames = cPlayerInput->holdTime["MoveRight"];

    }
    if (cPlayerInput->actions["MoveForward"]) {
        auto& cFootworkIntent = context->registry.addComponent<CFootworkIntent>(*ePlayer);
        cFootworkIntent.direction = { 0, 0, 1 };
        cFootworkIntent.directionalStrength = .25;
        context->playerMovement.moveTriggered = true;
        cFootworkIntent.heldFrames = cPlayerInput->holdTime["MoveForward"];
    }
    if (cPlayerInput->actions["MoveBackward"]) {
        auto& cFootworkIntent = context->registry.addComponent<CFootworkIntent>(*ePlayer);
        cFootworkIntent.direction = { 0, 0, -1 };
        cFootworkIntent.directionalStrength = .25;
        context->playerMovement.moveTriggered = true;
        cFootworkIntent.heldFrames = cPlayerInput->holdTime["MoveBackward"];

    }
    return { SystemExecResult::Ran };

};
