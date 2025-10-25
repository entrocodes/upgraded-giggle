#include "PlayerInputSystem.hpp"

void PlayerInputSystem::update(Registry& registry) {
    for (auto e : registry.getEntitiesWith<InputComponent>()) {
        auto* input = registry.getComponent<InputComponent>(e);
        if (!input) continue;

        // Reset actions each frame
        input->actions["MoveUp"] = false;
        input->actions["MoveDown"] = false;
        input->actions["MoveLeft"] = false;
        input->actions["MoveRight"] = false;
        input->actions["Quit"] = false;

        // Register actions based on keys
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) input->actions["MoveUp"] = true;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) input->actions["MoveDown"] = true;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) input->actions["MoveLeft"] = true;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) input->actions["MoveRight"] = true;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) input->actions["Quit"] = true;
    }
}
