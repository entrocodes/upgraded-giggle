#include "PlayerInputSystem.hpp"
#include "../ecs/Registry.hpp"
#include "../components/Player.hpp"
#include "../components/Velocity.hpp"
#include "../components/Transform.hpp"
#include <SFML/Window/Keyboard.hpp>

void PlayerInputSystem::update(Registry& registry, float dt) {
    for (auto e : registry.getEntitiesWith<Player, Velocity>()) {
        auto* vel = registry.getComponent<Velocity>(e);
        auto* transform = registry.getComponent<Transform>(e);
        if (!vel || !transform) continue;

        vel->velocity = { 0.f, 0.f };
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) vel->velocity.y -= 200.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) vel->velocity.y += 200.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) vel->velocity.x -= 200.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) vel->velocity.x += 200.f;

        // Optionally, update rotation/facing based on input
    }
}
