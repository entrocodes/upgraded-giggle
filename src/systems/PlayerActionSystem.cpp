#include "PlayerActionSystem.hpp"
#include "../components/InputComponent.hpp"
#include "../components/Velocity.hpp"

void PlayerActionSystem::update(GameContext* context) {
    for (auto e : context->registry.getEntitiesWith<InputComponent, Velocity>()) {
        auto [input, vel] = context->registry.getComponents<InputComponent, Velocity>(e);
        if (!input || !vel) continue;

        vel->velocity = { 0.f, 0.f };

        //if (input->actions["MoveUp"])    vel->velocity.y -= 200.f;
        //if (input->actions["MoveDown"])  vel->velocity.y += 200.f;
        if (input->actions["MoveLeft"])  vel->velocity.x -= 200.f;
        if (input->actions["MoveRight"]) vel->velocity.x += 200.f;
    }
}
