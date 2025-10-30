#include "PlayerActionSystem.hpp"
#include "../components/InputComponent.hpp"
#include "../components/Velocity.hpp"

void PlayerActionSystem::update(Registry& registry) {
    for (auto e : registry.getEntitiesWith<InputComponent, Velocity>()) {
        auto* input = registry.getComponent<InputComponent>(e);
        auto* vel = registry.getComponent<Velocity>(e);
        if (!input || !vel) continue;

        vel->velocity = { 0.f, 0.f };

        //if (input->actions["MoveUp"])    vel->velocity.y -= 200.f;
        //if (input->actions["MoveDown"])  vel->velocity.y += 200.f;
        if (input->actions["MoveLeft"])  vel->velocity.x -= 200.f;
        if (input->actions["MoveRight"]) vel->velocity.x += 200.f;
    }
}
