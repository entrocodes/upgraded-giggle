#include "PlayerActionSystem.hpp"
#include "../components/InputComponent.hpp"
#include "../components/Velocity.hpp"

void PlayerActionSystem::update(GameContext* context) {
    Entity* player = context->registry.getEntity("player");
    auto [input, vel, state] = context->registry.getComponents<InputComponent, Velocity, CState>(player);
    if (!input || !vel) continue;

    vel->velocity = { 0.f, 0.f };

    //if (input->actions["MoveUp"])    vel->velocity.y -= 200.f;
    if (input->actions["MoveDown"])  state = "running";
    if (input->actions["MoveLeft"])  vel->velocity.x -= 200.f;
    if (input->actions["MoveRight"]) vel->velocity.x += 200.f;
}
