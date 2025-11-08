#include "PlayerInputSystem.hpp"
#include "../components/InputComponent.hpp"
#include "../components/Player.hpp"

void PlayerInputSystem::update(GameContext* context) {
    for (auto e : context->registry.getEntitiesWith<Player, InputComponent>()) {
        auto comp = context->registry.getComponent<InputComponent>(e);
        if (!comp) continue;

        // Map raw input to logical actions
        comp->actions["MoveUp"] = context->rawInput.isKeyDown(sf::Keyboard::W);
        comp->actions["MoveDown"] = context->rawInput.isKeyDown(sf::Keyboard::S);
        comp->actions["MoveLeft"] = context->rawInput.isKeyDown(sf::Keyboard::A);
        comp->actions["MoveRight"] = context->rawInput.isKeyDown(sf::Keyboard::D);
        comp->actions["Quit"] = context->rawInput.isKeyDown(sf::Keyboard::Escape);
    }
}
