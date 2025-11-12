#include "PlayerActionSystem.hpp"
#include "../components/Components.hpp"
#include "../debug/Debug.hpp"
void PlayerActionSystem::update(GameContext* context) {
    Entity* player = context->registry.getEntity("player");
    if (!player) {
        Debug::debugPrint("Player Entity not found.");
        return;
    }

    auto [input, vel, state] = context->registry.getComponents<InputComponent, Velocity, CState>(*player);
    if (!input || !vel || !state) {
        if (!input) {
            Debug::debugPrint("Missing input component from player.");
        }
        if (!vel) {
            Debug::debugPrint("Missing velocity component from player.");
        }
        if (!state) {
            Debug::debugPrint("Missing state component from player.");
        }
        return;
    }
    vel->velocity = { 0.f, 0.f };

    //if (input->actions["MoveUp"])    vel->velocity.y -= 200.f;
    if (input->actions["PressDown"])  state->state = "backswing";
    if (input->actions["ReleaseDown"]) state->state = "stand";
    if (input->actions["MoveLeft"])  vel->velocity.x -= 200.f;
    if (input->actions["MoveRight"]) vel->velocity.x += 200.f;
};
