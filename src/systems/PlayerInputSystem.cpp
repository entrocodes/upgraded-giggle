#include "PlayerInputSystem.hpp"
#include "../components/InputComponent.hpp"
#include "../components/Player.hpp"

void PlayerInputSystem::update(Registry& registry, const RawInputState& input) {
    for (auto e : registry.getEntitiesWith<Player, InputComponent>()) {
        auto* comp = registry.getComponent<InputComponent>(e);
        if (!comp) continue;

        // Map raw input to logical actions
        comp->actions["MoveUp"] = input.isKeyDown(sf::Keyboard::W);
        comp->actions["MoveDown"] = input.isKeyDown(sf::Keyboard::S);
        comp->actions["MoveLeft"] = input.isKeyDown(sf::Keyboard::A);
        comp->actions["MoveRight"] = input.isKeyDown(sf::Keyboard::D);
        comp->actions["Quit"] = input.isKeyDown(sf::Keyboard::Escape);
    }
}
