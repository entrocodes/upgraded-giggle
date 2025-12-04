#include "PlayerActionSystem.hpp"
#include "../components/Components.hpp"
#include "../debug/Debug.hpp"
void PlayerActionSystem::update(GameContext* context) {
    Entity* player = context->registry.getEntity("player");
    if (!player) {
        Debug::debugPrint("Player Entity not found.");
        return;
    }

    auto [input, vel3D, state] = context->registry.getComponents<InputComponent, CVelocity3D, CState>(*player);

    vel3D->vel_mps = { 0.f, 0.f, 0.f };

    if (input->actions["AttackDown"])  state->state = "backswing";
    if (input->actions["ReleaseAttack"]) state->state = "stand";
    if (input->actions["MoveForward"])  vel3D->vel_mps.z += .5f;
    if (input->actions["MoveBackward"])  vel3D->vel_mps.z -= .5f;
    if (input->actions["MoveLeft"])  vel3D->vel_mps.x -= 1.5f;
    if (input->actions["MoveRight"]) vel3D->vel_mps.x += 1.5f;
    vel3D->vel_mps.x += input->axes["MoveX"] * 2.0f;
    vel3D->vel_mps.z += input->axes["MoveZ"] * 1.0f;


};
