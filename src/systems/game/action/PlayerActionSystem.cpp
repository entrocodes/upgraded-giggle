#include "PlayerActionSystem.hpp"
#include "../components/Components.hpp"
#include "../debug/Debug.hpp"
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
    if (input->actions["StepLeft"])  transform3D->pos_m.x -= .25f;
    if (input->actions["StepRight"]) transform3D->pos_m.x += 1.5f;
    return { SystemExecResult::Ran };

};
